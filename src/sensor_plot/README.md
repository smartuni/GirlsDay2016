## Introduction

This simple demo queries one or two sensor nodes retrieving sensor values for
temperature, humidity, and air quality (aka pollution). The python code in
`main.py` uses COAP to query sensor nodes and _matplotlib_ to generate a graph
showing sensor value which is saved as a PNG file. This image is served by a
simple website `index.html` using Python buildin _SimpleHTTPServer_.

## Sensor data

```
$ cd </path/to/sensor_plot>
$ python3 main.py
```

## Web View

```
$ cd <path/to/sensor_plot>
$ python -m SimpleHTTPServer 8000
```
