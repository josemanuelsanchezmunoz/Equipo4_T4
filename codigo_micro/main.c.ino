#include "bsp.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define LED_PIN 5
#define BTN_PIN 18 // boton de encendido
const int adcPin1 = 34; // ADC1
const int adcPin2 = 35; // ADC2
const int adcPin3 = 32; // ADC3
const int adc_button_1 = 19; // pines de botones
const int adc_button_2 = 33;
const int adc_button_3 = 25;
volatile bool ADC_pin1_active = true; // banderas para saber si estan activos los botones
volatile bool ADC_pin2_active = true;
volatile bool ADC_pin3_active = true;
volatile int pressedButton = -1; // Identifica el botón presionado
volatile bool buttonPressed = false;

TaskHandle_t Handle_ADC_Tarea_1 = NULL; // Handle de las tareas para poder suspenderlas y resumirlas
TaskHandle_t Handle_ADC_Tarea_2 = NULL;
TaskHandle_t Handle_ADC_Tarea_3 = NULL;

void setup() {
 
  gpio_config(LED_PIN, OUTPUT); // Configura el pin del LED como salida
  gpio_config(BTN_PIN, INPUT_PULLUP); // Configura el pin del botón como entrada
  gpio_config(adc_button_1, INPUT_PULLUP); // entrada a botones para desactivar sensores de temp con pullup
  gpio_config(adc_button_2, INPUT_PULLUP);
  gpio_config(adc_button_3, INPUT_PULLUP);
  gpio_config(adcPin1, INPUT); // entrada de los sensores
  gpio_config(adcPin2, INPUT);
  gpio_config(adcPin3, INPUT);
  serial_init(115200);    // Inicializa la comunicación serial
  adc_init(); // Inicializa el ADC
  
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), ISR_button1, FALLING); // interrupcion para boton de encendido
  attachInterrupt(digitalPinToInterrupt(adc_button_1), ISR_button2, FALLING); // interrupciones para desactivar sensores de temp
  attachInterrupt(digitalPinToInterrupt(adc_button_2), ISR_button3, FALLING);
  attachInterrupt(digitalPinToInterrupt(adc_button_3), ISR_button4, FALLING);

  xTaskCreate(led_task, "LED Task", 1000, NULL, 1, NULL);
  xTaskCreate(leerADC, "Tarea ADC1", 2048, NULL, 2, &Handle_ADC_Tarea_1); // instanciar 3 tareas para leer ADC
  xTaskCreate(leerADC, "Tarea ADC2", 2048, NULL, 2, &Handle_ADC_Tarea_2);
  xTaskCreate(leerADC, "Tarea ADC3", 2048, NULL, 2, &Handle_ADC_Tarea_3);
 
}

void loop() { }

void leerADC(void *parameter) {
  String lectura;
  int retardo = 100;

  while(true){
    if(buttonPressed){
      if (ADC_pin1_active) { // si esta activo el ADC 1 
        lectura = String(map(adc_read(adcPin1), 0, 4095, 0, 50)) + " " + "1"; // mandar lectura de temp y de pantalla
        serial_write(lectura.c_str());
      }
      else { serial_write("100 1"); } // si no esta activo el sensor mandar codigo 100 para indicar no activo y numero de sensor no activo
      vTaskDelay(pdMS_TO_TICKS(retardo));

      if (ADC_pin2_active ) { // si esta activo el ADC 2
        lectura = String(map(adc_read(adcPin2), 0, 4095, 0, 50)) + " " + "2"; // mandar lectura de temp y de pantalla
        serial_write(lectura.c_str());
      }
      else { serial_write("100 2"); } // si no esta activo el sensor mandar codigo 100 para indicar no activo y numero de sensor no activo
      vTaskDelay(pdMS_TO_TICKS(retardo));

      if (ADC_pin3_active ) { // si esta activo el ADC 3
        lectura = String(map(adc_read(adcPin3), 0, 4095, 0, 50)) + " " + "3"; // mandar lectura de temp y de pantalla
        serial_write(lectura.c_str());
      }
      else { serial_write("100 3"); } // si no esta activo el sensor mandar codigo 100 para indicar no activo y numero de sensor no activo
      vTaskDelay(pdMS_TO_TICKS(retardo));
    }
    else{ serial_write("100 0");} // si no esta activo ningun sensor mandar codigo 100 para indicar no activo y numero 0 para todos los sensores

    vTaskDelay(pdMS_TO_TICKS(retardo)); // Delay 
  }
}

void led_task(void *pvParameters) {
  bool ADC_1_suspended = false;
  bool ADC_2_suspended = false;
  bool ADC_3_suspended = false;

  for (;;) {
    if(buttonPressed){ gpio_write(LED_PIN, HIGH); } 
    else{ gpio_write(LED_PIN, LOW); }

    if(ADC_pin1_active && ADC_1_suspended){ vTaskResume(Handle_ADC_Tarea_1); ADC_1_suspended = false;} // Resumir la tarea suspendida
    else if(!ADC_pin1_active){ vTaskSuspend(Handle_ADC_Tarea_1); ADC_1_suspended = true;} // bloquear tarea 1

    if(ADC_pin2_active && ADC_2_suspended){ vTaskResume(Handle_ADC_Tarea_2); ADC_2_suspended = false;} // Resumir la tarea suspendida
    else if(!ADC_pin2_active){ vTaskSuspend(Handle_ADC_Tarea_2); ADC_2_suspended = true;} // bloquear tarea 2
     
    if(ADC_pin3_active && ADC_3_suspended){ vTaskResume(Handle_ADC_Tarea_3); ADC_3_suspended = false;} // Resumir la tarea suspendida
    else if(!ADC_pin3_active){ vTaskSuspend(Handle_ADC_Tarea_3); ADC_3_suspended = true;} // bloquear tarea 3

    vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100 ms
  }
}

// ISR para el botón 1 (BTN_PIN)
void ISR_button1() {
  pressedButton = 0;
  handleButtonPress();
}

// ISR para el botón adc_button_1
void ISR_button2() {
  pressedButton = 1;
  handleButtonPress();
}

// ISR para el botón adc_button_2
void ISR_button3() {
  pressedButton = 2;
  handleButtonPress();
}

// ISR para el botón adc_button_3
void ISR_button4() {
  pressedButton = 3;
  handleButtonPress();
}

void handleButtonPress() { // rutina para evitar rebotes en todos los botones
  static unsigned long lastPressTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime > 200) {
    switch (pressedButton) {
      case 0: buttonPressed = !buttonPressed; break;
      case 1: ADC_pin1_active = !ADC_pin1_active; break;
      case 2: ADC_pin2_active = !ADC_pin2_active; break;
      case 3: ADC_pin3_active = !ADC_pin3_active; break;
    }
    lastPressTime = currentTime;
  }
}





