/*
 *  Garage door
 *
 *  4/10/2015   v.0.8
 *  4/25        v.0.10	lowercase all commands
 *  			v.0.11  Validate incoming cmd.  Add query cmd.
 *				v.0.12  Tweaked and installed
 *	4/28        v.0.14  Boot publish, installed
 *	4/29        v.0.16  Switch to use GPIO3 (RXD), LWT implemented in libemqtt, installed
 *	test commit
 */
#define VERSION "v0.0.16"

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "application.h"

/*
/ * WIFI
#define WIFI_SSID "MY SSID"
#define WIFI_PWD "MY PW"
*/
#include "mywifi.h"

/*
 *  Hardware
 */
// use gpio2 and gpio3 for esp01, 14 and 4 for esp12
#define CMDPIN 2    // gpio2. garage door button GPIOx.  LOW==0==press HIGH==1==release
#define PRESS LOW
#define RELEASE HIGH
#define PRESS_MS 500

#define SENSPIN 3   // gpio3. magnetic reed switch GPIOx. HIGH==0==switchopen==doorclosed
#define DOOROPEN LOW
#define DOORCLOSED HIGH

Timer cmdTimer, sensTimer, bootTimer;
int currentDoorState=DOORCLOSED;  // DOOROPEN or DOORCLOSED
String doorMsg;

void initHardware()
{
	// try to ensure that it doesn't start up "PRESS"ed
	pullup( CMDPIN);
	pullup( SENSPIN);
	digitalWrite( CMDPIN, RELEASE);

	pinMode( CMDPIN, OUTPUT);
	pinMode( SENSPIN, INPUT);
	attachInterrupt(SENSPIN, sensorInterruptHandler, CHANGE);
}


/*
 * MQTT
 */
#define SUBCMDTOPIC  "pv/garage/door/cmd"
#define PUBSENSTOPIC "pv/garage/door/state"
#define PUBLWTTOPIC "pv/garage/door/lwt"
#define MQTTSERVER "mqbroker.clonts.org"  // e.g. test.mosquitto.org or mqbroker.clonts.org
MqttClient mqtt("mqbroker", 1883, onMessageReceived );


/*
 *  	CMD BUTTON
 */
void endPress()
{
	digitalWrite( CMDPIN, RELEASE);
}

void commandDoor( String message)
{
	if ( message == "query" ) { publishDoorState(); return ; }
	if ( message != "open" && message != "close") return;

	readDoorState();
	if ( doorMsg != message )
	{
		Serial.print( "Pressing to to change state to: ");
		Serial.println( message);
		// do the press
		digitalWrite( CMDPIN, PRESS);
		// schedule the release in 1/2 second or so
		cmdTimer.initializeMs( PRESS_MS, endPress).startOnce();
		// TODO:  need to ignore requests 12 seconds until the door has finished
	}
}

/**
 *		DOORSENSOR
 */

void readDoorState()
{
	currentDoorState=digitalRead(SENSPIN);
	debugf( "=============== Reading Door State : %s\n", doorMsg.c_str());
	doorMsg= ( currentDoorState ? "close" : "open");
	if ( currentDoorState == DOORCLOSED )
		doorMsg="close";
	else
		doorMsg="open";
}


void publishDoorState()
{
	readDoorState();
	debugf("----------------- Publishing %s\n", doorMsg.c_str());
	mqtt.publish( PUBSENSTOPIC, doorMsg, true);
}

/*  This interrupt is called when the SENSPIN (reed switch) changes state */
void IRAM_ATTR sensorInterruptHandler()
{
	// debounce by testing it after a second
	sensTimer.initializeMs( 1000, publishDoorState).startOnce();
}

// Callback for messages arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	Serial.print(topic); Serial.print(":\t"); Serial.println(message);

	topic.toLowerCase();
	message.toLowerCase();
	if ( topic == SUBCMDTOPIC ) commandDoor( message);
	else Serial.print(" not understood");

	Serial.println();
}

void onStartMqtt()
{
	// publish that I am starting
	mqtt.publish( "pv/garage/door/boot", VERSION, false);
	mqtt.publish( PUBLWTTOPIC, "online", true );  // LWT now supported in my copy of SmingFramework
	Serial.println( "publishing Startup");
}

// Callback for when WiFi station was connected to AP
void wifiConnectOk()
{
	Serial.println("I'm CONNECTED to wifi");

	// Run MQTT client
	mqtt.connect("esp8266-gdoor2","","",PUBLWTTOPIC,"offline");	// last will and testament
	mqtt.subscribe( SUBCMDTOPIC);
	// doesn't seem to allow direct publish here, so:
	bootTimer.initializeMs( 10000, onStartMqtt).startOnce();
}

// Callback for when WiFi station timeout was reached
void wifiConnectFail()
{
	Serial.println("I'm NOT CONNECTED to wifi. Need help :(");
}

/**
 *      "MAIN"
 */
void init()
{
	//Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Debug output to serial

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	initHardware();

	WifiStation.waitConnection(wifiConnectOk, 25, wifiConnectFail); // We recommend 20+ seconds for connection timeout at start

}
