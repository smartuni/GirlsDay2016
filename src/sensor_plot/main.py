#!/usr/bin/env python3

# coap stuff
from aiocoap import *
import asyncio
import time
# plot stuff
import matplotlib as mpl
mpl.use("agg")
import matplotlib.pyplot as plt
import numpy
from matplotlib.ticker import FormatStrFormatter

max_samples = 20
host_uri = "[fe80::f353:4e59:71ba:600a%lowpan0]"
yFormatter = FormatStrFormatter('%.2f')
app_samples = True

@asyncio.coroutine
def main():
    protocol = yield from Context.create_client_context()
    samples = dict()
    samples['temperature'] = list()
    samples['humidity'] = list()
    samples['airquality'] = list()
    if not app_samples:
        for i in range(0,max_samples):
            samples['temperature'].append(0)
            samples['humidity'].append(0)
            samples['airquality'].append(0)

    pos = 0
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
            if not app_samples:
                samples['temperature'][pos] = t_temp
                samples['humidity'][pos] = t_humi
                samples['airquality'][pos] = t_airq
            else:
                samples['temperature'].append(t_temp)
                samples['humidity'].append(t_humi)
                samples['airquality'].append(t_airq)

            print('Temperatur: %2.2f, Humitdy: %2.2f, AirQuality: %2.2f' %(t_temp, t_humi, t_airq))
            pos = (pos + 1) % max_samples
            if pos == 0:
                fig = plt.figure(figsize=(12,7))
                ax = fig.add_subplot(311)
                ax.plot(samples['temperature'])
                ax.set_title('Sensor data')
                ax.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
                ax.set_ylabel('temperature')

                ay = fig.add_subplot(312)
                ay.plot(samples['humidity'])
                ay.set_ylabel('humidity')
                ay.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

                az = fig.add_subplot(313)
                az.plot(samples['airquality'])
                az.set_xlabel('time (s)')
                az.set_ylabel('AirQuality')
                az.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

                plt.savefig('sensordata.png')
        time.sleep(1)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
