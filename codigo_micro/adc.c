#ifdef ARDUINO_BSP

  #include <wiring_private.h>
  #include <pins_arduino.h>

  void adc_write_arduino(uint8_t pin, int val);
  int adc_read_arduino(uint8_t pin);
  #define adc_write(pin, val) adc_write_arduino(pin, val)
  #define adc_read(pin) adc_read_arduino(pin)


  uint8_t analog_reference = DEFAULT;

  void analogReference(uint8_t mode)
  {
    // can't actually set the register here because the default setting
    // will connect AVCC and the AREF pin, which would cause a short if
    // there's something connected to AREF.
    analog_reference = mode;
  }

  int adc_read_arduino(uint8_t pin)
  {

  #if defined(analogPinToChannel)
  #if defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
  #endif
    pin = analogPinToChannel(pin);
  #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
  #elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
  #elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
  #else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
  #endif

  #if defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
  #endif
    
    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
  #if defined(ADMUX)
  #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = (analog_reference << 4) | (pin & 0x07);
  #else
    ADMUX = (analog_reference << 6) | (pin & 0x07);
  #endif
  #endif

    // without a delay, we seem to read from the wrong channel
    //delay(1);

  #if defined(ADCSRA) && defined(ADC)
    // start the conversion
    sbi(ADCSRA, ADSC);

    // ADSC is cleared when the conversion finishes
    while (bit_is_set(ADCSRA, ADSC));

    // ADC macro takes care of reading ADC register.
    // avr-gcc implements the proper reading order: ADCL is read first.
    return ADC;
  #else
    // we dont have an ADC, return 0
    return 0;
  #endif
  }

  // Right now, PWM output only works on the pins with
  // hardware support.  These are defined in the appropriate
  // pins_*.c file.  For the rest of the pins, we default
  // to digital output.
  void adc_write_arduino(uint8_t pin, int val)
  {
    // We need to make sure the PWM output is enabled for those pins
    // that support it, as we turn it off when digitally reading or
    // writing with them.  Also, make sure the pin is in output mode
    // for consistenty with Wiring, which doesn't require a pinMode
    // call for the analog output pins.
    pinMode(pin, OUTPUT);
    if (val == 0)
    {
      digitalWrite(pin, LOW);
    }
    else if (val == 255)
    {
      digitalWrite(pin, HIGH);
    }
    else
    {
      switch(digitalPinToTimer(pin))
      {
        // XXX fix needed for atmega8
        #if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
        case TIMER0A:
          // connect pwm to pin on timer 0
          sbi(TCCR0, COM00);
          OCR0 = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR0A) && defined(COM0A1)
        case TIMER0A:
          // connect pwm to pin on timer 0, channel A
          sbi(TCCR0A, COM0A1);
          OCR0A = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR0A) && defined(COM0B1)
        case TIMER0B:
          // connect pwm to pin on timer 0, channel B
          sbi(TCCR0A, COM0B1);
          OCR0B = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR1A) && defined(COM1A1)
        case TIMER1A:
          // connect pwm to pin on timer 1, channel A
          sbi(TCCR1A, COM1A1);
          OCR1A = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR1A) && defined(COM1B1)
        case TIMER1B:
          // connect pwm to pin on timer 1, channel B
          sbi(TCCR1A, COM1B1);
          OCR1B = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR1A) && defined(COM1C1)
        case TIMER1C:
          // connect pwm to pin on timer 1, channel C
          sbi(TCCR1A, COM1C1);
          OCR1C = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR2) && defined(COM21)
        case TIMER2:
          // connect pwm to pin on timer 2
          sbi(TCCR2, COM21);
          OCR2 = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR2A) && defined(COM2A1)
        case TIMER2A:
          // connect pwm to pin on timer 2, channel A
          sbi(TCCR2A, COM2A1);
          OCR2A = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR2A) && defined(COM2B1)
        case TIMER2B:
          // connect pwm to pin on timer 2, channel B
          sbi(TCCR2A, COM2B1);
          OCR2B = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR3A) && defined(COM3A1)
        case TIMER3A:
          // connect pwm to pin on timer 3, channel A
          sbi(TCCR3A, COM3A1);
          OCR3A = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR3A) && defined(COM3B1)
        case TIMER3B:
          // connect pwm to pin on timer 3, channel B
          sbi(TCCR3A, COM3B1);
          OCR3B = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR3A) && defined(COM3C1)
        case TIMER3C:
          // connect pwm to pin on timer 3, channel C
          sbi(TCCR3A, COM3C1);
          OCR3C = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR4A)
        case TIMER4A:
          //connect pwm to pin on timer 4, channel A
          sbi(TCCR4A, COM4A1);
          #if defined(COM4A0)		// only used on 32U4
          cbi(TCCR4A, COM4A0);
          #endif
          OCR4A = val;	// set pwm duty
          break;
        #endif
        
        #if defined(TCCR4A) && defined(COM4B1)
        case TIMER4B:
          // connect pwm to pin on timer 4, channel B
          sbi(TCCR4A, COM4B1);
          OCR4B = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR4A) && defined(COM4C1)
        case TIMER4C:
          // connect pwm to pin on timer 4, channel C
          sbi(TCCR4A, COM4C1);
          OCR4C = val; // set pwm duty
          break;
        #endif
          
        #if defined(TCCR4C) && defined(COM4D1)
        case TIMER4D:				
          // connect pwm to pin on timer 4, channel D
          sbi(TCCR4C, COM4D1);
          #if defined(COM4D0)		// only used on 32U4
          cbi(TCCR4C, COM4D0);
          #endif
          OCR4D = val;	// set pwm duty
          break;
        #endif

                
        #if defined(TCCR5A) && defined(COM5A1)
        case TIMER5A:
          // connect pwm to pin on timer 5, channel A
          sbi(TCCR5A, COM5A1);
          OCR5A = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR5A) && defined(COM5B1)
        case TIMER5B:
          // connect pwm to pin on timer 5, channel B
          sbi(TCCR5A, COM5B1);
          OCR5B = val; // set pwm duty
          break;
        #endif

        #if defined(TCCR5A) && defined(COM5C1)
        case TIMER5C:
          // connect pwm to pin on timer 5, channel C
          sbi(TCCR5A, COM5C1);
          OCR5C = val; // set pwm duty
          break;
        #endif

        case NOT_ON_TIMER:
        default:
          if (val < 128) {
            digitalWrite(pin, LOW);
          } else {
            digitalWrite(pin, HIGH);
          }
      }
    }
  }

#endif

#ifdef ESP_BSP

  #include "driver/adc.h"

  // Define la configuración del ADC
  #define ADC_WIDTH    ADC_WIDTH_BIT_12  // 12 bits de resolución
  #define ADC_ATTEN    ADC_ATTEN_DB_0    // Atenuación de 0 dB
  #define ADC_CHANNEL ADC1_CHANNEL_0    // Canal ADC (puede cambiar según el pin)

  void custom_adc_init() {
      // Configura la resolución del ADC y la atenuación
      adc1_config_width(ADC_WIDTH);
      adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
  }

  int custom_adc_read(uint8_t pin) {
    // Selecciona el canal ADC
    adc1_channel_t channel1;
    adc2_channel_t channel2;
    bool is_adc1 = true;

    switch (pin) {
        // ADC1 Channels
        case 34: channel1 = ADC1_CHANNEL_6; break; // Pin 34
        case 35: channel1 = ADC1_CHANNEL_7; break; // Pin 35
        case 32: channel1 = ADC1_CHANNEL_4; break; // Pin 32
        case 33: channel1 = ADC1_CHANNEL_5; break; // Pin 33
        case 36: channel1 = ADC1_CHANNEL_0; break; // Pin 36
        case 39: channel1 = ADC1_CHANNEL_3; break; // Pin 39
        case 0:  channel1 = ADC1_CHANNEL_0; break; // Pin 0
        case 2:  channel1 = ADC1_CHANNEL_1; break; // Pin 2
        case 4:  channel1 = ADC1_CHANNEL_2; break; // Pin 4
        case 5:  channel1 = ADC1_CHANNEL_3; break; // Pin 5

        // ADC2 Channels
        case 12: channel2 = ADC2_CHANNEL_0; is_adc1 = false; break; // Pin 12
        case 13: channel2 = ADC2_CHANNEL_1; is_adc1 = false; break; // Pin 13
        case 14: channel2 = ADC2_CHANNEL_2; is_adc1 = false; break; // Pin 14
        case 15: channel2 = ADC2_CHANNEL_3; is_adc1 = false; break; // Pin 15
        case 25: channel2 = ADC2_CHANNEL_8; is_adc1 = false; break; // Pin 25
        case 26: channel2 = ADC2_CHANNEL_9; is_adc1 = false; break; // Pin 26
        case 27: channel2 = ADC2_CHANNEL_7; is_adc1 = false; break; // Pin 27

        default: return -1; // Valor no válido
    }

    // Lee el valor del ADC según el tipo de canal
    if (is_adc1) {
        return adc1_get_raw(channel1);
    } else {
        int adc_val;
        // Usa adc2_get_raw para leer los canales de ADC2
        if (adc2_get_raw(channel2, ADC_WIDTH, &adc_val) == ESP_OK) {
            return adc_val;
        } else {
            return -1; // Error en la lectura
        }
    }
  }

  void custom_adc_write(uint8_t pin, int val) {
      //Ver función de Jose
  }

  #define adc_init() custom_adc_init()
  #define adc_read(pin) custom_adc_read(pin)
  #define adc_write(pin, val) custom_adc_write(pin) // No funcional

#endif

