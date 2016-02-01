# sensor_node

## introduction

This code provides a simple sensor node that will response to COAP request send
via 6lowpan using IEEE 802.15.4. Currently it supports COAP requests for:
* GET /temperature ; in degree Celsius, with factor 100
* GET /humidity ; in percent, with factor 100
* PUT /led {0|1} ; 1 = on, 0 or anything else = off

## build

The sensor nodes are based on the RIOT-OS and currently support only the Phytec
board, namely `pba-d-01-kw2x`. To compile the source code set `RIOTBASE`, `PORT`,
and `PATH` environment variables according to your system and run `make` afterwards:

```
export RIOTBASE=<path/to/riot>
export PORT=/dev/tty<usbserial>
export PATH=<path/to/arm-none-eabi-gcc>:$PATH
make
make flash
make term
```
