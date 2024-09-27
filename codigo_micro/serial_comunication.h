#ifdef ARDUINO_BSP

  #ifndef HardwareSerial_h
  #define HardwareSerial_h

  #include <inttypes.h>

  #include "Stream.h"

  // Define constants and variables for buffering incoming serial data.  We're
  // using a ring buffer (I think), in which head is the index of the location
  // to which to write the next incoming character and tail is the index of the
  // location from which to read.
  // NOTE: a "power of 2" buffer size is recommended to dramatically
  //       optimize all the modulo operations for ring buffers.
  // WARNING: When buffer sizes are increased to > 256, the buffer index
  // variables are automatically increased in size, but the extra
  // atomicity guards needed for that are not implemented. This will
  // often work, but occasionally a race condition can occur that makes
  // Serial behave erratically. See https://github.com/arduino/Arduino/issues/2405
  #if !defined(SERIAL_TX_BUFFER_SIZE)
  #if ((RAMEND - RAMSTART) < 1023)
  #define SERIAL_TX_BUFFER_SIZE 16
  #else
  #define SERIAL_TX_BUFFER_SIZE 64
  #endif
  #endif
  #if !defined(SERIAL_RX_BUFFER_SIZE)
  #if ((RAMEND - RAMSTART) < 1023)
  #define SERIAL_RX_BUFFER_SIZE 16
  #else
  #define SERIAL_RX_BUFFER_SIZE 64
  #endif
  #endif
  #if (SERIAL_TX_BUFFER_SIZE>256)
  typedef uint16_t tx_buffer_index_t;
  #else
  typedef uint8_t tx_buffer_index_t;
  #endif
  #if  (SERIAL_RX_BUFFER_SIZE>256)
  typedef uint16_t rx_buffer_index_t;
  #else
  typedef uint8_t rx_buffer_index_t;
  #endif

  // Define config for Serial.begin(baud, config);
  #define SERIAL_5N1 0x00
  #define SERIAL_6N1 0x02
  #define SERIAL_7N1 0x04
  #define SERIAL_8N1 0x06
  #define SERIAL_5N2 0x08
  #define SERIAL_6N2 0x0A
  #define SERIAL_7N2 0x0C
  #define SERIAL_8N2 0x0E
  #define SERIAL_5E1 0x20
  #define SERIAL_6E1 0x22
  #define SERIAL_7E1 0x24
  #define SERIAL_8E1 0x26
  #define SERIAL_5E2 0x28
  #define SERIAL_6E2 0x2A
  #define SERIAL_7E2 0x2C
  #define SERIAL_8E2 0x2E
  #define SERIAL_5O1 0x30
  #define SERIAL_6O1 0x32
  #define SERIAL_7O1 0x34
  #define SERIAL_8O1 0x36
  #define SERIAL_5O2 0x38
  #define SERIAL_6O2 0x3A
  #define SERIAL_7O2 0x3C
  #define SERIAL_8O2 0x3E

  class HardwareSerial : public Stream
  {
    protected:
      volatile uint8_t * const _ubrrh;
      volatile uint8_t * const _ubrrl;
      volatile uint8_t * const _ucsra;
      volatile uint8_t * const _ucsrb;
      volatile uint8_t * const _ucsrc;
      volatile uint8_t * const _udr;
      // Has any byte been written to the UART since begin()
      bool _written;

      volatile rx_buffer_index_t _rx_buffer_head;
      volatile rx_buffer_index_t _rx_buffer_tail;
      volatile tx_buffer_index_t _tx_buffer_head;
      volatile tx_buffer_index_t _tx_buffer_tail;

      // Don't put any members after these buffers, since only the first
      // 32 bytes of this struct can be accessed quickly using the ldd
      // instruction.
      unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
      unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

    public:
      inline HardwareSerial(
        volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
        volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
        volatile uint8_t *ucsrc, volatile uint8_t *udr);
      void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
      void begin(unsigned long, uint8_t);
      void end();
      virtual int available(void);
      virtual int peek(void);
      virtual int read(void);
      virtual int availableForWrite(void);
      virtual void flush(void);
      virtual size_t write(uint8_t);
      inline size_t write(unsigned long n) { return write((uint8_t)n); }
      inline size_t write(long n) { return write((uint8_t)n); }
      inline size_t write(unsigned int n) { return write((uint8_t)n); }
      inline size_t write(int n) { return write((uint8_t)n); }
      using Print::write; // pull in write(str) and write(buf, size) from Print
      operator bool() { return true; }

      // Interrupt handlers - Not intended to be called externally
      inline void _rx_complete_irq(void);
      void _tx_udr_empty_irq(void);
  };

  #if defined(UBRRH) || defined(UBRR0H)
    extern HardwareSerial Serial;
    #define HAVE_HWSERIAL0
  #endif
  #if defined(UBRR1H)
    extern HardwareSerial Serial1;
    #define HAVE_HWSERIAL1
  #endif
  #if defined(UBRR2H)
    extern HardwareSerial Serial2;
    #define HAVE_HWSERIAL2
  #endif
  #if defined(UBRR3H)
    extern HardwareSerial Serial3;
    #define HAVE_HWSERIAL3
  #endif

  extern void serialEventRun(void) __attribute__((weak));

  #endif

  //****************************print de arduino************************************

  #define serial_init(baud) Serial.begin(baud)
  #define serial_write(message) Serial.println(message)

#endif

#ifdef ESP_BSP

  #include "driver/uart.h"

  // Configuración de los buffers de UART
  #define UART_TX_BUFFER_SIZE 256
  #define UART_RX_BUFFER_SIZE 256

  // Configuración de los parámetros de UART
  #define UART_BAUD_RATE 115200

  // Configura el UART0 
  #define UART_NUM UART_NUM_0
  #define UART_TX_PIN 17
  #define UART_RX_PIN 16

  // Función para inicializar la UART
  void custom_serial_init(uint32_t baud_rate = UART_BAUD_RATE) {
      uart_config_t uart_config = {
          .baud_rate = baud_rate,
          .data_bits = UART_DATA_8_BITS,
          .parity = UART_PARITY_DISABLE,
          .stop_bits = UART_STOP_BITS_1,
          .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
          .source_clk = UART_SCLK_APB
      };
      
      // Configura el puerto UART
      uart_param_config(UART_NUM, &uart_config);
      uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
      uart_driver_install(UART_NUM, UART_RX_BUFFER_SIZE * 2, UART_TX_BUFFER_SIZE * 2, 0, NULL, 0);
  }

  // Función para enviar un mensaje a través de UART con salto de línea
  void custom_serial_write(const char* message) {
    uart_write_bytes(UART_NUM, message, strlen(message));
    uart_write_bytes(UART_NUM, "\r\n", 2); // Agrega un salto de línea (CR LF)
  }

  // Función para leer un mensaje desde UART
  int custom_serial_read(uint8_t* buffer, size_t length) {
      return uart_read_bytes(UART_NUM, buffer, length, 100 / portTICK_RATE_MS);
  }

  // Macros para facilitar el uso
  #define serial_init(baud) custom_serial_init(baud)
  #define serial_write(message) custom_serial_write(message)

#endif


