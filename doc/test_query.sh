#!/bin/bash
TS=$(date +%s)
LOG=/home/pi/climate.log
COAPCLIENT=/usr/local/bin/coap-client
for j in "temperature" "humidity" "airquality"; do
  for i in "fe80::f353:4e59:71ba:600a" "fe80::5bb3:4e48:6fdc:6002"; do
    VAL=$($COAPCLIENT -m get coap://[$i%lowpan0]/$j -N -B 5 -v 1 2>/dev/null | grep -v GET )
    [ -z "$VAL" ] && { VAL="NA"; }
    echo "$TS $i $j $VAL" >> $LOG
  done
done
