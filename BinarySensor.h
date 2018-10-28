#ifndef BINARYSENSOR_H
#define BINARYSENSOR_H

//////////////////////////////////////////////////////////////////////////
// Class to manage binary input pins /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class BinarySensor {
  protected:
    int _pin; // 0 means: Empty slot
    bool _use_pullup; // Use the internal pullup resistor?
    int _delay; // Delay for reporting inactivity in seconds; Valid range: >= 0;
    int _state;
    unsigned long _lastActivity;
    const char* _name;
    
  public:
    BinarySensor() : _pin(0), _use_pullup(false), _delay(0), _state(LOW), _lastActivity(0), _name(nullptr)
    {
    }

    // Reset state and assign new pin and delay
    void init(int pin, int delay, const char* name, bool use_pullup = false)
    {
      _pin = pin;
      _use_pullup = use_pullup;
      _delay = delay;
      _name = name;

      this->reset();
      this->update();
    }

    // Reset state but don't change pin and delay
    void reset()
    {
      _state = LOW;
      _lastActivity = 0;
      if(_pin != 0){
        if(_use_pullup){
          pinMode(_pin, INPUT_PULLUP); 
        }else{
          pinMode(_pin, INPUT); 
        }
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
      return (pin() != 0);
    }
    
    int delay()
    {
      return _delay;
    }

    bool update() 
    {
      // Sensor inactive
      if(pin() == 0)
      {
        return false;
      }

      // Read state and update _lastActivity
      int s = digitalRead(_pin);      
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

#endif
