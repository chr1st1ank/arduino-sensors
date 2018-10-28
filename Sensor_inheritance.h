#ifndef SENSOR_H
#define SENSOR_H

//////////////////////////////////////////////////////////////////////////
// Base class that only manages naming ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class AbstractSensor {
  private:
    int _pin; // 0 means: Empty slot
    const char* _name;
    
  public:

    AbstractSensor() : _pin(0), _name(nullptr)
    {
    }

    // Reset state and assign new pin and delay
    void init(int pin, const char* name)
    {
      _pin = pin;
      _name = name;
    }

    virtual void reset()
    {
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

    virtual bool isActive(){
      return (pin() != 0);
    }
    
//    int delay()
//    {
//      return _delay;
//    }
//
//    int& pinRef()
//    {
//      return _pin;
//    }
//
//    int& delayRef()
//    {
//      return _delay;
//    }

    virtual bool update(bool force) = 0;

//    int state() 
//    {
//      return _state;
//    }
};


//////////////////////////////////////////////////////////////////////////
// Class to manage binary input pins /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class BinarySensor : public AbstractSensor {
  protected:
    bool _use_pullup; // Use the internal pullup resistor?
    int _delay; // Delay for reporting inactivity in seconds; Valid range: >= 0;
    int _state;
    unsigned long _lastActivity;
    
  public:

    BinarySensor() : AbstractSensor(), _use_pullup(false), _delay(0), _state(LOW), _lastActivity(0)
    {
    }

    // Reset state and assign new pin and delay
    void init(int pin, int delay, const char* name, bool use_pullup = false)
    {
      AbstractSensor::init(pin, name);
      _use_pullup = use_pullup;
      _delay = delay;
      
      this->reset();
      this->update();
    }

    // Reset state but don't change pin and delay
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
    
    int delay()
    {
      return _delay;
    }

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
      if(s == LOW && (millis() < _lastActivity + (delay()*1000)))
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

    int state() 
    {
      return _state;
    }
};


//////////////////////////////////////////////////////////////////////////
// Class to manage temperature / humidity sensors ////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <DHT.h>          // For temp / humidity sensors
class ClimateSensor : public AbstractSensor {
  protected:
    unsigned _interval; // Measurement interval in seconds; Valid range: >= 0;
    DHT* _dht;
    float _temp;
    float _hum;
    unsigned long _lastMeasurement;
    
  public:

    ClimateSensor() : AbstractSensor(), _interval(0), _dht(nullptr), _temp(-100), _hum(-100), _lastMeasurement(0)
    {
    }

    ~ClimateSensor()
    {
      if(_dht != nullptr)
        delete _dht;
    }

    // Reset state and assign new pin and delay
    virtual void init(int pin, int interval, const char* name)
    {
      AbstractSensor::init(pin, name);
      _interval = interval;
      
      this->reset();
      this->update(true);
    }

    // Reset state and create new DHT object
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

    virtual bool isActive(){
      return ((pin() != 0) && (_dht != nullptr));
    }

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
