#ifdef ARDUINO_BSP

  #include <wiring_private.h>
  #include <pins_arduino.h>

  void gpio_config_arduino(uint8_t pin, uint8_t mode);
  void gpio_write_arduino(uint8_t pin, uint8_t val);
  int gpio_read_arduino(uint8_t pin);
  #define gpio_config(pin, mode) gpio_config_arduino(pin, mode)
  #define gpio_write(pin, val) gpio_write_arduino(pin, val)
  #define gpio_read(pin) gpio_read_arduino(pin)


  void gpio_config_arduino(uint8_t pin, uint8_t mode)
  {
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *reg, *out;

    if (port == NOT_A_PIN) return;

    // JWS: can I let the optimizer do this?
    reg = portModeRegister(port);
    out = portOutputRegister(port);

    if (mode == INPUT) { 
      uint8_t oldSREG = SREG;
                  cli();
      *reg &= ~bit;
      *out &= ~bit;
      SREG = oldSREG;
    } else if (mode == INPUT_PULLUP) {
      uint8_t oldSREG = SREG;
                  cli();
      *reg &= ~bit;
      *out |= bit;
      SREG = oldSREG;
    } else {
      uint8_t oldSREG = SREG;
                  cli();
      *reg |= bit;
      SREG = oldSREG;
    }
  }

  // Forcing this inline keeps the callers from having to push their own stuff
  // on the stack. It is a good performance win and only takes 1 more byte per
  // user than calling. (It will take more bytes on the 168.)
  //
  // But shouldn't this be moved into pinMode? Seems silly to check and do on
  // each digitalread or write.
  //
  // Mark Sproul:
  // - Removed inline. Save 170 bytes on atmega1280
  // - changed to a switch statement; added 32 bytes but much easier to read and maintain.
  // - Added more #ifdefs, now compiles for atmega645
  //
  //static inline void turnOffPWM(uint8_t timer) __attribute__ ((always_inline));
  //static inline void turnOffPWM(uint8_t timer)
  static void turnOffPWM(uint8_t timer)
  {
    switch (timer)
    {
      #if defined(TCCR1A) && defined(COM1A1)
      case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
      #endif
      #if defined(TCCR1A) && defined(COM1B1)
      case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
      #endif
      #if defined(TCCR1A) && defined(COM1C1)
      case TIMER1C:   cbi(TCCR1A, COM1C1);    break;
      #endif
      
      #if defined(TCCR2) && defined(COM21)
      case  TIMER2:   cbi(TCCR2, COM21);      break;
      #endif
      
      #if defined(TCCR0A) && defined(COM0A1)
      case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
      #endif
      
      #if defined(TCCR0A) && defined(COM0B1)
      case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
      #endif
      #if defined(TCCR2A) && defined(COM2A1)
      case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
      #endif
      #if defined(TCCR2A) && defined(COM2B1)
      case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
      #endif
      
      #if defined(TCCR3A) && defined(COM3A1)
      case  TIMER3A:  cbi(TCCR3A, COM3A1);    break;
      #endif
      #if defined(TCCR3A) && defined(COM3B1)
      case  TIMER3B:  cbi(TCCR3A, COM3B1);    break;
      #endif
      #if defined(TCCR3A) && defined(COM3C1)
      case  TIMER3C:  cbi(TCCR3A, COM3C1);    break;
      #endif

      #if defined(TCCR4A) && defined(COM4A1)
      case  TIMER4A:  cbi(TCCR4A, COM4A1);    break;
      #endif					
      #if defined(TCCR4A) && defined(COM4B1)
      case  TIMER4B:  cbi(TCCR4A, COM4B1);    break;
      #endif
      #if defined(TCCR4A) && defined(COM4C1)
      case  TIMER4C:  cbi(TCCR4A, COM4C1);    break;
      #endif			
      #if defined(TCCR4C) && defined(COM4D1)
      case TIMER4D:	cbi(TCCR4C, COM4D1);	break;
      #endif			
        
      #if defined(TCCR5A)
      case  TIMER5A:  cbi(TCCR5A, COM5A1);    break;
      case  TIMER5B:  cbi(TCCR5A, COM5B1);    break;
      case  TIMER5C:  cbi(TCCR5A, COM5C1);    break;
      #endif
    }
  }

  void gpio_write_arduino(uint8_t pin, uint8_t val)
  {
    uint8_t timer = digitalPinToTimer(pin);
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *out;

    if (port == NOT_A_PIN) return;

    // If the pin that support PWM output, we need to turn it off
    // before doing a digital write.
    if (timer != NOT_ON_TIMER) turnOffPWM(timer);

    out = portOutputRegister(port);

    uint8_t oldSREG = SREG;
    cli();

    if (val == LOW) {
      *out &= ~bit;
    } else {
      *out |= bit;
    }

    SREG = oldSREG;
  }

  int gpio_read_arduino(uint8_t pin)
  {
    uint8_t timer = digitalPinToTimer(pin);
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);

    if (port == NOT_A_PIN) return LOW;

    // If the pin that support PWM output, we need to turn it off
    // before getting a digital reading.
    if (timer != NOT_ON_TIMER) turnOffPWM(timer);

    if (*portInputRegister(port) & bit) return HIGH;
    return LOW;
  }

#endif

#ifdef ESP_BSP
  #include "driver/gpio.h"
  // Define funciones y macros personalizadas
  void custom_gpio_config(uint8_t pin, uint8_t mode);
  void custom_gpio_write(uint8_t pin, uint8_t val);
  int custom_gpio_read(uint8_t pin);

  #define gpio_config(pin, mode) custom_gpio_config(pin, mode)
  #define gpio_write(pin, val) custom_gpio_write(pin, val)
  #define gpio_read(pin) custom_gpio_read(pin)

  // Función para configurar el GPIO en ESP32
  void custom_gpio_config(uint8_t pin, uint8_t mode)
  {
    gpio_config_t io_conf = {};  // Inicializa la estructura de configuración del GPIO
    io_conf.pin_bit_mask = (1ULL << pin);  // Configura el pin específico
    // Configura el modo del pin
    if (mode == INPUT) {
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    } else if (mode == INPUT_PULLUP) {
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    } else if (mode == OUTPUT) {
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    // Deshabilita temporalmente la macro para llamar a la función nativa
    #undef gpio_config
    gpio_config(&io_conf); // Llama a la función nativa de ESP-IDF
    // Vuelve a definir la macro después de la llamada
    #define gpio_config(pin, mode) custom_gpio_config(pin, mode)
  }

  // Función para escribir en el GPIO de la ESP32
  void custom_gpio_write(uint8_t pin, uint8_t val)
  {
    gpio_set_level((gpio_num_t)pin, val);
  }

  // Función para leer el GPIO de la ESP32
  int custom_gpio_read(uint8_t pin)
  {
    return gpio_get_level((gpio_num_t)pin);
  }
#endif



