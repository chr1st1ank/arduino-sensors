//////////////////////////////////////////////////////////////////////////
// Class to manage binary input pins /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef ASBINARYSENSOR_H
#define ASBINARYSENSOR_H

#include "_as_private_sensor_utils.h"
#include "asAbstractSensor.h"

namespace as{

  /**
   * @brief Class for binary sensors which are directly connected to an I/O pin.
   * 
   * Examples of sensors which work this way are reed switches or PIR sensors. 
   * In both cases consider using the internal pullup resistors.
   * 
   */
  class BinarySensor : public AbstractSensor {
    protected:
      bool _use_pullup; // Use the internal pullup resistor?
      int _delay; // Delay for reporting inactivity in seconds; Valid range: >= 0;
      int _state;
      unsigned long _lastActivity;

    public:

      /**
       * @brief Construct a new uninitialized BinarySensor object
       * 
       */
      BinarySensor() : AbstractSensor(), _use_pullup(false), _delay(0), _state(STATE_UNKNOWN), _lastActivity(0)
      {
      }

      /**
       * @brief Reset state and assign new pin and delay
       * 
       * @param pin The pin the sensor is connected to
       * @param delay The delay in seconds until a change from HIGH to LOW is reported
       * @param name The name of the sensor. This needs to be a pointer to a char array. The string is not copied but just referenced to save memory, so make sure the pointer stays valid.
       * @param use_pullup Activate the built-in pull-up resistor?
       */
      void init(int pin, int delay, const char* name, bool use_pullup = false)
      {
        AbstractSensor::init(pin, name);
        _use_pullup = use_pullup;
        _delay = delay;

        this->reset();
        this->update();
      }

      /**
       * @brief Reset state but don't change pin, delay or name
       * 
       */
      virtual void reset()
      {
        _state = LOW;
        _lastActivity = 0;
        if(pin() != 0){
          if(_use_pullup){
            pinMode(pin(), INPUT_PULLUP);
          }else{
            pinMode(pin(), INPUT);
          }
        }
      }

      /**
       * @brief The value of the delay property
       * 
       */
      int delay()
      {
        return _delay;
      }

      /**
       * @brief Update the state of the sensor.
       * 
       * @return true The status has changed.
       * @return false The status is unchanged.
       */
      virtual bool update(bool force=false)
      {
        // Sensor inactive
        if(pin() == 0)
        {
          return false;
        }

        // Read state and update _lastActivity
        int s = digitalRead(pin());
        if(s == HIGH){
          _lastActivity = millis();
        }

        // Return LOW only after delay() seconds of inactivity
        if(s == LOW && _state == HIGH && (millis() < _lastActivity + (delay()*1000)))
        {
          // Delay not yet over: Forget about "LOW"
          s = HIGH;
        }

        if(s != _state) {
          _state = s;
          return true;
        }
        return false;
      }

      /**
       * @brief Returns the current status of the sensor
       * 
       * @return int Possible values: HIGH, LOW or STATE_UNKNOWN
       */
      int state()
      {
        return _state;
      }
  };
  
} // namespace as

#endif
