/*
 * application.h
 *
 *  Created on: Apr 22, 2015
 *      Author: John
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

/*
 *   For now only forward declarations included here
 */
void readDoorState();
void checkSensor();


void onMessageReceived(String topic, String message);
void sensorInterruptHandler();

void publishDoorState();


#endif /* APPLICATION_H_ */
