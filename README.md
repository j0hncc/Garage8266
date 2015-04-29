# Garage8266
Garage door sensor/controller via MQTT

This is exploratory project w.r.t. both ESP2688, Sming, GIT, and Github.

The primary purpose of the Garage controller is to enable an action which will close the door every night when/if I leave it about every week or two.  

MQTT is chosen as the messaging middlewary, so this device only needs to be an MQTT client, and no need for any UI here.

Current intention/implementation has node-red as the scheduling of the above "nightly close" feature.

A secondary benefit is to allow control of the door "at will".  For the time being any general purpose MQTT client is used, such as www.hivemq.com/demos/websocket-client/, when suitably tunnelled to internal server "mqbroker".  Also Android smartphone works when local to home network, or VPN'd.

Further UI ideas TBD.

