# Garage8266
Garage door sensor/controller via MQTT

This is exploratory project w.r.t. both ESP2688, Sming, MQTT, GIT, and Github.

The primary purpose of the Garage controller is to enable an action which will close the door every night when/if I leave it open like I tend to do about every week or two.  

MQTT is chosen as the messaging middlewary, so this device only needs to be an MQTT client, and no need for any UI here.

Current intention/implementation has node-red as the scheduling of the above "nightly close" feature.

A secondary benefit is to allow control of the door "at will".  For the time being any general purpose MQTT client is used, such as www.hivemq.com/demos/websocket-client/, when suitably tunnelled to internal server "mqbroker".  Also Android smartphone works when local to home network, or VPN'd.

Further UI ideas TBD.

Hardware interface is 
* a reed switch for sensor.  
* The existing door button of this garage door opener happens to have 24vDC across it when the switch is open, and when pressed passes 30ma.  Therefore I am using a simple PNP transistor driver on GPIO in a never-seen-before-by-me configuration of isolated ground but common Vss (3.3v controller to 24v door opener).  Therefore the button press is "active low" and doesn't suffer from nuances of ESP-01 GPIO0/GPIO2 boot-time signals.
