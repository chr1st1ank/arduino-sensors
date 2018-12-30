//////////////////////////////////////////////////////////////////////////
// Class to manage temperature / humidity sensors ////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifndef ASCLIMATESENSOR_H
#define ASCLIMATESENSOR_H

#include "_as_private_sensor_utils.h"
#include "asAbstractSensor.h"

#include <DHT.h>          // For temp / humidity sensors of type DHT22

namespace as{
    
  /**
   * @brief Class to manage temperature / humidity sensors of type DHT11 or DHT22
   * 
   * To interface the hardware the DHT sensor library by Adafruit is used.
   * 
   */
  class ClimateSensor : public AbstractSensor {
    protected:
      unsigned _interval; // Measurement interval in seconds; Valid range: >= 0;
      DHT* _dht;
      float _temp;
      float _hum;
      unsigned long _lastMeasurement;

    public:

      /**
       * @brief Construct a new uninitialized ClimateSensor object.
       * 
       */
      ClimateSensor() : AbstractSensor(), _interval(0), _dht(nullptr), _temp(-100), _hum(-100), _lastMeasurement(0)
      {
      }

      /**
       * @brief Destroy the Climate Sensor object and free the internal resources.
       * 
       */
      ~ClimateSensor()
      {
        if(_dht != nullptr)
          delete _dht;
      }

      /**
       * @brief Reset state and assign new pin, update interval and name
       * 
       * @param pin The associated I/O pin of the microcontroller.
       * @param interval The update interval of the sensor in seconds.
       * @param name The name of the sensor. This needs to be a pointer to a char array. The string is not copied but just referenced to save memory, so make sure the pointer stays valid.
       */
      virtual void init(int pin, int interval, const char* name)
      {
        AbstractSensor::init(pin, name);
        _interval = interval;

        this->reset();
        this->update(true);
      }

      /**
       * @brief Reset state and create new internal DHT object.
       * 
       */
      virtual void reset()
      {
        _temp = -100.0;
        _hum = -100.0;

        if(_dht != nullptr){
          delete _dht;
          _dht = nullptr;
        }

        if(pin() != 0){
          _dht = new DHT(pin(), DHT22);
          _dht->begin();
        }
      }

      /**
       * @brief Returns true if the sensor is active, i.e. has a pin assigned and an internal DHT object created.
       * 
       */
      virtual bool isActive(){
        return ((pin() != 0) && (_dht != nullptr));
      }

      /**
       * @brief Update the sensor values.
       * 
       * @param force If set to true, the values are updated even if before the defined update interval.
       * @return true The value has changed.
       * @return false The value is unchanged.
       */
      virtual bool update(bool force=false)
      {
        // Sensor inactive
        if(pin() == 0 || _dht == nullptr)
        {
          return false;
        }

        if((millis()/1000  < _lastMeasurement/1000 + _interval) && (!force))
        {
          return false;
        }

        // Read temp and humidity
        float h = _dht->readHumidity();
        float t = _dht->readTemperature();

        // Check if any reads failed and exit early (don't update _lastMeasurement to try again).
        if (isnan(h) || isnan(t)) {
          return false;
        }

        // Update last measurement time
        _lastMeasurement = millis();

        // Update stored values and return wether there was a change
        if(t != _temp || h != _hum) {
          _temp = t;
          _hum = h;
          return true;
        }
        return false;
      }

      /**
       * @brief Last measured temperatur in degrees celsius.
       * 
       */
      float temperature()
      {
        return _temp;
      }

      /**
       * @brief Relative air moisture in %.
       * 
       */
      float humidity()
      {
        return _hum;
      }
  };
  
} // namespace as

#endif
