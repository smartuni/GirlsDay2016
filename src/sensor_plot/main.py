#!/usr/bin/env python3

from aiocoap import *
import asyncio
import time

host_uri = "[fe80::f353:4e59:71ba:600a%lowpan0]"

@asyncio.coroutine
def main():
    protocol = yield from Context.create_client_context()

    while True:
        req_temperature = Message(code=GET)
        req_humidity = Message(code=GET)
        req_airquality = Message(code=GET)
        req_temperature.set_request_uri('coap://'+host_uri+'/temperature')
        req_humidity.set_request_uri('coap://'+host_uri+'/humidity')
        req_airquality.set_request_uri('coap://'+host_uri+'/airquality')

        try:
            res_temperature = yield from protocol.request(req_temperature).response
            res_humidity = yield from protocol.request(req_humidity).response
            res_airquality = yield from protocol.request(req_airquality).response
        except Exception as e:
            print('Failed to fetch resource:')
            print(e)
        else:
            print('Temperatur: %s, Humitdy: %s, AirQuality: %s' %(res_temperature.payload.decode('utf-8'), res_humidity.payload.decode('utf-8'), res_airquality.payload.decode('utf-8')))
        time.sleep(1)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
