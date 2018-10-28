# arduino-sensors
Some classes for Arduino sensors. 

There is:
  * **BinarySensor**: A class which watches a pin, immediately reports activity and reports inactivity after a given delay. This was written for PIR sensors. This is in "BinarySensor.h".
  * **ClimateSensor**: This is a lean wrapper around the DHT type temperature and humidity sensors. The class makes sure that a (time-consuming) update is only done every x milliseconds. But it always provides access to the last measured values. This is in "ClimateSensor.h".
  
Although a little redundant, there is a second version of the above mentioned classes in "Sensors_inheritance.h". There, both classes inherit from **AbstractSensor**. This is the cleaner implementation. But it costs some additional space on the sparse memory of the Arduino.
