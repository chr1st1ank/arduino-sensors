//////////////////////////////////////////////////////////////////////////
// Base class that only manages sensor naming ////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef ASABSTRACTSENSOR_H
#define ASABSTRACTSENSOR_H

#include "_as_private_sensor_utils.h"

namespace as{

  /**
   * @brief Base class for all sensors in the arduino-sensors library.
   * 
   * This class manages only the naming and holds the associated pin.
   * 
   */
  class AbstractSensor {
    private:
      int _pin; // 0 means: Empty slot
      const char* _name;

    protected:
      /**
       * @brief Reset state and assign new pin, name and delay
       * 
       * @param pin The pin to assign to this sensor object.
       * @param name The name of the sensor. This needs to be a pointer to a char array. The string is not copied but just referenced to save memory, so make sure the pointer stays valid.
       */
      void init(int pin, const char* name)
      {
        _pin = pin;
        _name = name;
      }

    public:

      /**
       * @brief Construct a new uninitialized AbstractSensor object.
       * 
       */
      AbstractSensor() : _pin(0), _name(nullptr)
      {
      }

      /**
       * @brief Reset state but don't change any settings
       * 
       */
      virtual void reset()
      {
      }

      /**
       * @brief Set the sensor's name
       * 
       * @param name Pointer to a char array with the new name. The string is not copied but just referenced to save memory, so make sure the pointer stays valid.
       */
      void setName(const char* name)
      {
        _name = name;
      }

      /**
       * @brief The sensor's name
       * 
       * @return const char* Pointer to a char array with the sensor's name.
       */
      const char* name()
      {
        return _name;
      }

      /**
       * @brief Returns the connected pin
       * 
       */
      int pin()
      {
        return _pin;
      }

      /**
       * @brief Whether the sensor is "active", i.e. whether it was initialized.
       * 
       * @return true The sensor was initialized (with the init() function) and is ready to work.
       * @return false The sensor is uninitialized and won't report any states.
       */
      virtual bool isActive(){
        return (pin() != 0);
      }

      virtual bool update(bool force) = 0;
  };
  
} // namespace as

#endif
