## IPv6 addresses

The following IPv6 link local addresses are used by sensor nodes:
- fe80::4be3:4e73:718d:600a
- fe80::f8e3:4e62:71ba:600a
- fe80::e0d3:4e71:71a8:600a
- fe80::b993:4e72:7187:600a
- fe80::f353:4e59:71ba:600a
- fe80::5bb3:4e48:6fdc:6002

## Test with libcoap

To test the sensor node by triggering its RGB LED run the following command in
shell. It uses `coap-client` provided by _libcoap_, so make sure the latter is
installed on your system. Replace `<IPADDR>` matching respective sensor node.

```
for i in r 0 g 0 b 0 1 0; do
    echo $i | coap-client -m put coap://[<IPADRR>%lowpan0]/led -f -
    sleep 1
done
```

This loop will toggle the RGB LED in a sequence of (red, off, green, off, blue,
    off white, off).
