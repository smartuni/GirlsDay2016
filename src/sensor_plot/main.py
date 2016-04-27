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

max_samples = 120
fig_samples = 10
uri_sensorA = "[fe80::f353:4e59:71ba:600a%lowpan0]"
uri_sensorB = "[fe80::5bb3:4e48:6fdc:6002%lowpan0]"
yFormatter = FormatStrFormatter('%.2f')
app_samples = False
use_sensorB = True

@asyncio.coroutine
def main():
    protocol = yield from Context.create_client_context()
    samples = dict()
    samples['temperatureA'] = list()
    samples['humidityA'] = list()
    samples['airqualityA'] = list()
    samples['temperatureB'] = list()
    samples['humidityB'] = list()
    samples['airqualityB'] = list()
    if not app_samples:
        for i in range(0,max_samples):
            samples['temperatureA'].append(0)
            samples['humidityA'].append(0)
            samples['airqualityA'].append(0)
            samples['temperatureB'].append(0)
            samples['humidityB'].append(0)
            samples['airqualityB'].append(0)
    if max_samples < 1:
        return
    pos = max_samples - 1
    while True:
        req_temperatureA = Message(code=GET)
        req_humidityA = Message(code=GET)
        req_airqualityA = Message(code=GET)
        req_temperatureA.set_request_uri('coap://'+uri_sensorA+'/temperature')
        req_humidityA.set_request_uri('coap://'+uri_sensorA+'/humidity')
        req_airqualityA.set_request_uri('coap://'+uri_sensorA+'/airquality')
        req_temperatureB = Message(code=GET)
        req_humidityB = Message(code=GET)
        req_airqualityB = Message(code=GET)
        req_temperatureB.set_request_uri('coap://'+uri_sensorB+'/temperature')
        req_humidityB.set_request_uri('coap://'+uri_sensorB+'/humidity')
        req_airqualityB.set_request_uri('coap://'+uri_sensorB+'/airquality')
        try:
            res_temperatureA = yield from protocol.request(req_temperatureA).response
            res_humidityA = yield from protocol.request(req_humidityA).response
            res_airqualityA = yield from protocol.request(req_airqualityA).response
        except Exception as e:
            print('Failed to fetch resource:')
            print(e)
        else:
            t_temp = float(res_temperatureA.payload.decode('utf-8'))
            t_humi = float(res_humidityA.payload.decode('utf-8'))
            t_airq = float(res_airqualityA.payload.decode('utf-8'))
            if not app_samples:
                samples['temperatureA'][pos] = t_temp
                samples['humidityA'][pos] = t_humi
                samples['airqualityA'][pos] = t_airq
            else:
                samples['temperatureA'].append(t_temp)
                samples['humidityA'].append(t_humi)
                samples['airqualityA'].append(t_airq)
            print('SensorA -- Temperatur: %2.2f, Humitdy: %2.2f, AirQuality: %2.2f' %(t_temp, t_humi, t_airq))
            if use_sensorB:
                try:
                    res_temperatureB = yield from protocol.request(req_temperatureB).response
                    res_humidityB = yield from protocol.request(req_humidityB).response
                    res_airqualityB = yield from protocol.request(req_airqualityB).response
                except Exception as e:
                    print('Failed to fetch resource:')
                    print(e)
                else:
                    t_temp = float(res_temperatureB.payload.decode('utf-8'))
                    t_humi = float(res_humidityB.payload.decode('utf-8'))
                    t_airq = float(res_airqualityB.payload.decode('utf-8'))
                    if not app_samples:
                        samples['temperatureB'][pos] = t_temp
                        samples['humidityB'][pos] = t_humi
                        samples['airqualityB'][pos] = t_airq
                    else:
                        samples['temperatureB'].append(t_temp)
                        samples['humidityB'].append(t_humi)
                        samples['airqualityB'].append(t_airq)

                    print('SensorB -- Temperatur: %2.2f, Humitdy: %2.2f, AirQuality: %2.2f' %(t_temp, t_humi, t_airq))
        pos = (pos - 1) % max_samples
        if (pos % fig_samples) == 0:
            fig = plt.figure(figsize=(12,7))
            ax = fig.add_subplot(311)
            ax.plot(samples['temperatureA'])
            if use_sensorB:
                ax.plot(samples['temperatureB'])
            ax.set_title('Sensor data')
            ax.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
            ax.set_ylabel('Temperature')

            ay = fig.add_subplot(312)
            ay.plot(samples['humidityA'])
            if use_sensorB:
                ay.plot(samples['humidityB'])
            ay.set_ylabel('Humidity')
            ay.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

            az = fig.add_subplot(313)
            az.plot(samples['airqualityA'])
            if use_sensorB:
                az.plot(samples['airqualityB'])
            az.set_xlabel('time (s)')
            az.set_ylabel('Pollution')
            az.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
            if use_sensorB:
                plt.legend(['SensorA', 'SensorB'], loc="best")
            plt.savefig('sensordata.png')
        time.sleep(1)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
