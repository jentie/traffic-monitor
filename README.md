# WLAN Traffic Monitor

![cloud caseing](https://github.com/jentie/traffic-monitor/blob/master/media/cloud-active.jpg)

## WLAN man-in-the-middle Traffic Monitor

This monitor device captures traffic from a WLAN user, based on an ESP8266 NAT Router. Most important traffic flows are assigned to LED indicators to demonstrate current smartphone activities. 
Additionally packet headers / IP addresses are send via serial interface to a host/PC for further evaluation (e.g. decoding server names) or storage.

## Basic Idea

The device can be used to demonstrate smartphone access to different important service providers. Each service provider (or group) is represented by one LED, which is triggered for a short period of time. 
The user needs to connect via the WLAN of the device to the Internet. Subsequently the traffic from the smartphone toward the Internet will be evaluated. A small ad hoc evaluation can already be done on the device and drive the LED. More sophisticated evaluations can be done on a PC, using packet information received via serial interface. 

Current proof-of-concept with 5 LED shows traffic flows to Akamai, Amazon, Facebook, Google and all other flows on the last LED.

We can observe some difference between traffic from Apps and Web Browsing, some web pages are highly linked to serveral servers (tracking, advertisment). Apps / web pages from public broadcasters / public government use typically only small number of additional service providers.

## Security and Data Proctection Considerations

This software / device is intended to increase the awareness of smart phone activities, usage of services via Internet or Internet tracking (based on smart phone itself, apps and/or mobile web browsing). Nevertheless we need still discuss possible security and data proctection issues:
* current implementation processes only IP addresses, one direction of traffic flow
  * just needed information is captured, separation with USB / serial interface increases transparency of process(es)
* other output includes full IP/TCP header decoding, similar to tcpdump
* traffic monitoring can reveal user data in packets, typically most traffic is already encrypted (e.g. using https on port 443)

## Usage

* WLAN SSID and password are known to a smartphone user
* the build-in LED indicates one and only one user in the WLAN

* different modi:
  - stand-alone device: device is powered via USB
  - capturing/evaluating packet information: device is connected to PC
  - (sending flags from PC to control traffic monitor output format)

set serial data rate, disable flow control

> bash> stty -F /dev/ttyS5 115200

> bash> stty -F /dev/ttyS5  -crtscts -ixon

capture traffic, show and store in file

> bash> cat /dev/ttyS5 | tee sample.txt

capture addresses and lookup host names

> bash> cat /dev/ttyS5 | ./gethostbyaddr.py


### current limitations and future ideas

* on-device evaluation (running on EPS8266) is currently quite simple, just evaluation of first byte of Internet address. Possibly this assignment to service providers (akamai, amazon, facebook, google) is only valid for Berlin. 
  * larger look-up tables are restricted by memory, esp. because there are NAT tables needed
  * initialization of functions will fail, see debug outputs on serial monitor
* maybe an algorithm can use a hash function, at least a table with 1024 entries will work
* 'gethostbyaddr' will quite often fail (e.g. for twitter), maybe a table can be constructed on other IP address information, e.g. from routing information (table needs to be created offline, possibly from LIR data bases or service like https://ipinfo.io/AS13414)
* traffic evaluation on host PC might be much more complex, e.g. visualize geographic distribution of servers, based on a world map

### notes about unix command line filtering
* PC with Windows10 can easily use Ubuntu subsystem to use command line filters
* cut only selected column of dumped output (original traffic decoder, code currently disabled)
> bash> cat dump.txt | grep 'out' | cut -d' ' -f5 | cut -d'>' -f2 > addresses.txt

* sort addresses and remove / count duplicates  
> bash> more addresses.txt | sort | uniq -c | sort -n > add.sort.num.txt

> bash> more add.sort.num.txt

> bash> more addresses.txt | sort -n | uniq > add.sort.txt

> bash> more add.sort.txt


   pictures                                        |    :film_strip:[short video](/media/cloud-video.mp4)
---------------------------------------------------|------------------------------------------------------------
![device](/media/cloud.jpg) Traffic Monitor Device | ![WLAN connect](/media/cloud-1STA.jpg) WLAN client connected
![traffic](/media/cloud-active.jpg) Traffic Flows  | ![back](/media/cloud-back.jpg) Back with Wemos D1 mini
