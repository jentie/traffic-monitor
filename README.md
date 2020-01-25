# traffic-monitor

![cloud caseing](https://github.com/jentie/traffic-monitor/blob/master/media/cloud-active.jpg)

## WLAN man-in-the-middle traffic monitor

This monitor device captures traffic from a WLAN user, based on an ESP8266 NAT Router. Most important traffic flows are assigned to LED indicators to demonstrate current smartphone activities. 
Additionally packet headers / IP addresses are send via serial interface to a host/PC for further evaluation (e.g. decoding server names) or storage.

## Basic Idea

The device can be used to demonstrate smartphone access to different important service providers. Each service provider (or group) is represented by one LED, which is triggered for a short period of time. 
The user needs to connect via the WLAN of the device to the Internet. Subsequently the traffic from the smartphone toward the Internet will be evaluated. A small ad hoc evaluation can already be done on the device and drive the LED. More sophisticated evaluations can be done on a PC, using packet information received via serial interface. 

Current proof-of-concept with 5 LED shows traffic flows to Akamai, Amazon, Facebook, Google and all other flows on the last LED. 

## Usage

* WLAN SSID and password are known to a smartphone user
* the build-in LED indicates one and only one user in the WLAN

* different modi
- stand-alone device: device is powered via USB
- capturing/evaluating packet information: device is connected to PC

set serial data rate, disable flow control

> bash> stty -F /dev/ttyS5 115200

> bash> stty -F /dev/ttyS5  -crtscts -ixon

capture traffic, show and store in file

> bash> cat /dev/ttyS5 | tee sample.txt

capture addresses and lookup host names

> bash> cat /dev/ttyS5 | ./gethostbyaddr.py

### notes about unix command line filtering
* PC with Windows10 can easily use Ubuntu subsystem to use command line filters
* cut only selected column of dumped output (original traffic decoder, code currently disabled)
> bash> cat dump.txt | grep 'out' | cut -d' ' -f5 | cut -d'>' -f2 > addresses.txt

* sort addresses and remove / count duplicates  
> bash> more addresses.txt | sort | uniq -c | sort -n > add.sort.num.txt

> bash> more add.sort.num.txt

> bash> more addresses.txt | sort -n | uniq > add.sort.txt

> bash> more add.sort.txt


pictures | [short video](/jentie/traffic-monitor/blob/master/media/cloud-video.mp4)
---------|---------
![device](https://github.com/jentie/traffic-monitor/blob/master/media/cloud.jpg) Traffic Monitor Device | ![WLAN connect](https://github.com/jentie/traffic-monitor/blob/master/media/cloud-1STA.jpg) WLAN client connected
![traffic](https://github.com/jentie/traffic-monitor/blob/master/media/cloud-active.jpg) Traffic Flows | ![back](https://github.com/jentie/traffic-monitor/blob/master/media/cloud-back.jpg) Back with Wemos D1 mini
