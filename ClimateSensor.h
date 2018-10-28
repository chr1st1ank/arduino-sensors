#ifndef CLIMATESENSOR_H
#define CLIMATESENSOR_H

//////////////////////////////////////////////////////////////////////////
// Class to manage temperature / humidity sensors ////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <DHT.h>          // For temp / humidity sensors
class ClimateSensor {
  protected:
    int _pin; // 0 means: Empty slot
    unsigned _interval; // Measurement interval in seconds; Valid range: >= 0;
    const char* _name;//[(SENSOR_NAME_LENGTH+1)];
    DHT* _dht;
    float _temp;
    float _hum;
    unsigned long _lastMeasurement;
    
  public:
    ClimateSensor() : _pin(0), _interval(0), _name(nullptr), _dht(nullptr), _temp(-100), _hum(-100), _lastMeasurement(0)
    {
    }

    ~ClimateSensor()
    {
      if(_dht != nullptr)
        delete _dht;
    }

    // Reset state and assign new pin and delay
    void init(int pin, int interval, const char* name)
    {
      _pin = pin;
      _interval = interval;
      this->reset();
      
      _name = name;

      this->update(true);
    }

    // Reset state and create new DHT object
    void reset()
    {
      _temp = -100.0;
      _hum = -100.0;
      
      if(_dht != nullptr){
        delete _dht;
        _dht = nullptr;
      }
        
      if(_pin != 0){
        _dht = new DHT(_pin, DHT22);
        _dht->begin();
      }
    }

    void setName(const char* name)
    {
      _name = name;
    }

    const char* name()
    {
      return _name;
    }

    int pin()
    {
      return _pin;
    }

    bool isActive(){
      return ((pin() != 0) && (_dht != nullptr));
    }

    bool update(bool force=false) 
    {
      // Sensor inactive
      if(pin() == 0 || _dht == nullptr)
      {
        return false;
      }

      // Interval passed? If not report no change.
      if((millis()/1000  < _lastMeasurement/1000 + _interval) && (!force))
      {
        return false;
      }

      // Read temp and humidity
      float h = _dht->readHumidity();
      float t = _dht->readTemperature();
  
      // Check if any reads failed and exit early (don't update _lastMeasurement to try again).
      if (isnan(h) || isnan(t)) {
        DEBUG(F("Failed to read from DHT sensor!"));
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

    float temperature() 
    {
      return _temp;
    }

    float humidity() 
    {
      return _hum;
    }
};

#endif
