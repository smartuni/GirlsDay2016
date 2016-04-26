#!/usr/bin/env python3

# coap stuff
from aiocoap import *
import asyncio
import time
# plot stuff
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy

max_samples = 32
host_uri = "[fe80::f353:4e59:71ba:600a%lowpan0]"
pp = PdfPages('sensordata.pdf')

@asyncio.coroutine
def main():
    protocol = yield from Context.create_client_context()
    samples = dict()
    samples['temperature'] = list()
    samples['humidity'] = list()
    samples['airquality'] = list()
    for i in range(0,max_samples):
        samples['temperature'].append(0)
        samples['humidity'].append(0)
        samples['airquality'].append(0)

    pos = 0
    cnt = 0
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
            t_temp = float(res_temperature.payload.decode('utf-8'))
            t_humi = float(res_humidity.payload.decode('utf-8'))
            t_airq = float(res_airquality.payload.decode('utf-8'))
            samples['temperature'][pos] = t_temp
            samples['humidity'][pos] = t_humi
            samples['airquality'][pos] = t_airq
            print('Temperatur: %2.2f, Humitdy: %2.2f, AirQuality: %2.2f' %(t_temp, t_humi, t_airq))
            pos = (pos + 1) % max_samples
            if pos == 0:
                plt.subplot(3, 1, 1)
                plt.plot(samples['temperature'])
                plt.title('Sensor data')
                plt.ylabel('temperature')
                plt.ylim([0,50])

                plt.subplot(3, 1, 2)
                plt.plot(samples['humidity'])
                plt.ylabel('humidity')
                plt.ylim([0,100])

                plt.subplot(3, 1, 3)
                plt.plot(samples['airquality'])
                plt.xlabel('time (s)')
                plt.ylabel('AirQuality')
                plt.ylim([0,100])
                plt.savefig('sensordata_'+cnt+'.pdf')
                cnt += 1
        time.sleep(1)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
