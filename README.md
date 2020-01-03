# artnetonice
Using an LED strip over Art-Net to display status information from the WifiOnICE API

## Requirements
### Hardware
* Arduino Nano oder Klon
* Ethernet Shield mit ENC28J60. W5100 wäre noch viel besser, aber ich habe dafür kein einfach steckbares Shield gefunden.
* 5V, GND und Datenpin auf Kontaktleiste oder per Kabel auf Buchse herausführen, um die LED-Strip verbinden zu können

Ich habe die Strip softwareseitig auf 300mA beschränkt (mit ENC und Atmega sollten es etwa 500mA sein, aber Achtung: Muss nicht funktionieren) und betreibe den Strip direkt am USB-Port bzw. über ein USB-Netzteil.

Wird der Strip nicht per Art-Net angesprochen, läuft eine Default-Animation. Für diesen Zweck wäre eine Alternative auch, den Strip per Seriell über USB anzusteuern, Art-Net war jedoch flexibler nutzbar.

### Software
* jq
* ola (muss auf Universe 0 erfolgreich ausgeben), ggf. über die GUI unter http://localhost:9090
* Netzwerkverbindung zum ICE-WLAN mit erfolgter Authentisierung
* Netzwerkverbindung zum Art-Net-Node
#### speed.sh
Dieses Script zeigt die aktuelle Geschwindigkeit des ICEs auf der LED-Strip an. Beim ICE 4 wurde die Api etwa alle 5 Sekunden aktualisiert, jedoch wohl die Geschwindigkeit lediglich anhand der GPS-Position ermittelt und nicht aus den Fahrzeugdaten ausgelesen, denn ohne GPS-Signal war die Geschwindigkeit=0.

## API Samples
Unter samples sind ein paar Beispiel für API-Responses. Die URLs sind:
* https://iceportal.de/api1/rs/status
* https://iceportal.de/api1/rs/tripInfo/trip
* https://iceportal.de/api1/rs/configs

## Weitere Ideen
* Anzeige der Unterwegsbahnhöfe
* Anzeige der Verspätung
* Anzeige der aktuellen Position
* Überlagern der jeweiligen Anzeigen