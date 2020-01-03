#include <Arduino.h>
#include <FastLED.h>
#include <UIPEthernet.h>
#include <ArtNode.h>

////////////////////////////////////////////////////////////
ArtConfig config = {
  .mac =  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}, // MAC
  .ip =   {10, 0, 0, 85},                         // IP
  .mask = {255, 255, 255, 0},                       // Subnet mask
  .udpPort = 0x1936,
  .dhcp = false,
  .net = 0, // Net (0-127)
  .subnet = 0,  // Subnet (0-15)
  "NanoNode", // Short name
  "NanoNode", // Long name
  .numPorts = 1,
  .portTypes = {
    PortTypeDmx | PortTypeOutput},
  .portAddrIn = {0}, // Port input universes (0-15)
  .portAddrOut = {0}, // Port output universes (0-15)
  .verHi = 0,
  .verLo = 1
};
////////////////////////////////////////////////////////////
IPAddress gateway(config.ip[0], config.ip[1], config.ip[2], 1);
EthernetUDP udp;
byte buffer[530];
ArtNode node = ArtNode(config, sizeof(buffer), buffer);

// How many leds are in the strip?
#define NUM_LEDS 60

// Data pin that led data will be written out over
#define DATA_PIN 2

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

// Standby fade
uint8_t hue = 0;
uint8_t standby = 0; // seconds to change to standby lighting

// Standby fade functions
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, hue, 7);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( hue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( hue, 255, 192);
}


// Modify for DHCP
/*volatile unsigned char localip[4];
ArtPollReply * ArtNode::createPollReply() {
    ArtPollReply *reply = (ArtPollReply*)buffer;
    memset(buffer, 0, sizeof(ArtPollReply));
    
    setPacketHeader();
    reply->OpCode = OpPollReply;
    memcpy(reply->BoxAddr.IP, localip, 4);
    reply->BoxAddr.Port = config->udpPort;
    
    reply->VersionInfoHi = config->verHi;
    reply->VersionInfoLo = config->verLo;
    
    reply->NetSwitch = config->net;
    reply->SubSwitch = config->subnet;

    strcpy((char*)reply->ShortName, config->shortName);
    strcpy((char*)reply->LongName, config->longName);
    
    reply->NumPortsLo = config->numPorts;
    memcpy(reply->PortTypes, config->portTypes, 4);
    memset(reply->GoodInput, 0x8, config->numPorts); // Input disabled
    memset(reply->GoodOutput, 0x80, config->numPorts); // Very important for MadMapper!
    memcpy(reply->SwIn, config->portAddrIn, 4);
    memcpy(reply->SwOut, config->portAddrOut, 4);
    reply->Style = StyleNode;
    memcpy(reply->Mac, config->mac, 6);
    reply->Status2 = 0x8; // Supports 15bit address (ArtNet 3)
    memcpy(reply->BindIp, config->ip, 4);

	packetSize = sizeof(ArtPollReply);
	return reply;
}*/

void setup() {
  // Wait a little bit for recovery
  delay(3000);

  // Init WS2812 out and limit current
  //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
  FastLED.setBrightness(0);// blackout
  set_max_power_in_volts_and_milliamps(5, 300);

  // Init Art-Net
  // DHCP?
  Ethernet.begin(config.mac, config.ip, gateway, gateway, config.mask);
  //Ethernet.begin(config.mac);

  udp.begin(config.udpPort);
} // setup


// loop through the rainbow colors 
void loop() {

  // Test DHCP localip
  //localip = Ethernet.localIP();

  // send the 'leds' array out to the actual LED strip
  //FastLED.delay(1000/120); 

  FastLED.show();  

  // Standby Animation
  if (standby == 0) {

    // insert a delay to keep the framerate modest
    FastLED.delay(1000/120); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 50 ) { 
      
      // slowly cycle the "base color" through the rainbow
      hue++; 
      
      // Fade in
      if(FastLED.getBrightness() < 100) {
        FastLED.setBrightness((FastLED.getBrightness() + 1));
      }

    } 

    rainbow();
    //rainbowWithGlitter();
    //sinelon();
    //confetti();

  } else {

    // Countdown to standby
    EVERY_N_SECONDS(1) {
      standby--;
    }

  }


  while (udp.parsePacket()) {

    standby = 10;
    if(FastLED.getBrightness() < 100) {
      FastLED.setBrightness(100);
    }

    int n = udp.read(buffer, min(udp.available(), sizeof(buffer)));
    if (n >= sizeof(ArtHeader) && node.isPacketValid()) {

      // Package Op-Code determines type of packet
      switch (node.getOpCode()) {

        // Poll packet. Send poll reply.
        /*case OpPoll: {
          //ArtPoll* poll = (ArtPoll*)buffer;
          node.createPollReply();

          udp.beginPacket(node.broadcastIP(), config.udpPort);
          udp.write(buffer, sizeof(ArtPollReply));
          udp.endPacket();
        } break;*/

        // DMX packet
        case OpDmx: {

          ArtDmx* dmx = (ArtDmx*)buffer;
          //int port = node.getPort(dmx->Net, dmx->SubUni);
          //int len = dmx->getLength();
          byte *data = dmx->Data;
          //if (port == 0 ) { //&& len >= (NUM_LEDS * 3)

            //FastLED.setBrightness(100);// global brightness

            // read recent DMX values and set levels
            for(int LedDot = 0; LedDot < NUM_LEDS; LedDot++) {
              int DMXStartAddress = LedDot * 3;

              leds[LedDot] = CRGB(data[DMXStartAddress],data[(DMXStartAddress + 1)],data[(DMXStartAddress + 2)]);
            }
          
            // Brightness set on Channel 512
            // ToDo
          //}
        } break;

        default:
          break;
      }
    }
  }

} // loop
