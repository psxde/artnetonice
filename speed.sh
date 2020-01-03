speed=$(curl -s https://iceportal.de/api1/rs/status | jq -r .speed)
maxspeed=250.0
dots=60
color="0,255,0"
color_off="100,0,0"

result=$(echo "scale=2; $speed/$maxspeed*$dots" | bc | xargs printf %.0f)
offdots=$(expr $dots - $result)

echo Speed: $speed
echo Dots: $result
echo OffDots: $offdots


echo -n "">temp.dmx

while [ "$result" -gt 0 ]; do
	echo -n $color >>temp.dmx
        echo -n "," >>temp.dmx
	result=$(( result - 1 ))
done


while [ "$offdots" -gt 0 ]; do
	echo -n $color_off >>temp.dmx
        echo -n "," >>temp.dmx
	offdots=$(( offdots - 1 ))
done


exec ola_set_dmx -u 0 -d $(cat temp.dmx)