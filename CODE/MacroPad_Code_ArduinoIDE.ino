#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>

//FUNC PROTOTYPES
void configEncoder();
void ejecutarMacro(char key);
void pulsar(uint8_t keycode);
void combo(uint8_t modificador, uint8_t tecla);
void escribir(const char* texto);
void abrirApp(const char* comando);

//id del teclado
enum {
  RID_KEYBOARD = 1,
  RID_CONSUMER_CONTROL,
};

//configuraciónes tecnicas del HID
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(RID_CONSUMER_CONTROL) )
};

Adafruit_USBD_HID usb_hid;

#define LED_PIN   D10 //pin de los leds
#define NUM_LEDS  12 //numero de leds

//configuraciónes de los leds RGB

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
RotaryEncoder encoder(D8, D9, RotaryEncoder::LatchMode::TWO03);
#define ENC_BTN D7 //pin boton del encoder

const byte FILAS = 3;
const byte COLUMNAS = 4;

char keys[FILAS][COLUMNAS] = { //arreglo 2D para switches
  {'1', '2','3','4'},
  {'5','6','7','8'},
  {'9','0','A','B'},
};

byte rowPins[FILAS] = {D6, D5, D4}; //PINES FILAS
byte colPins[COLUMNAS] = {D0, D1, D2, D3  }; //PINES COLUMNAS

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, FILAS, COLUMNAS); //ASIGNACIÓN DE PINES A IDENTIFICADOR

int lastEncPos = 0; //ESTADO ANTERIOR

void pulsar(uint8_t keycode){
  usb_hid.keyboardPress(0, keycode); // 0 significa "sin Ctrl ni Alt"
  delay(5); 
  usb_hid.keyboardRelease(0); // ¡Importante soltar!
}

void combo(uint8_t modificador, uint8_t tecla) {
  usb_hid.keyboardPress(modificador, tecla);
  delay(5);
  usb_hid.keyboardRelease(0);

}

void escribir(const char* texto) {

  for(int i=0; i<strlen(texto); i++) {
     char c = texto[i];
     // Convertimos caracteres ASCII a códigos HID (Simplificado)
     if (c >= 'a' && c <= 'z') pulsar(HID_KEY_A + (c - 'a'));
     else if (c >= 'A' && c <= 'Z') combo(KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_A + (c - 'A'));
     else if (c >= '1' && c <= '9') pulsar(HID_KEY_1 + (c - '1'));
     else if (c == '0') pulsar(HID_KEY_0);
     else if (c == ' ') pulsar(HID_KEY_SPACE);
     else if (c == '.') pulsar(HID_KEY_PERIOD);
     else if (c == '@') combo(KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_2);
  }
}

//Abrir Aplicación
void abrirApp(const char* comando) {
  usb_hid.keyboardPress(KEYBOARD_MODIFIER_LEFTGUI, HID_KEY_R);
  delay(20);
  usb_hid.keyboardRelease(0);
  delay(300);
  escribir(comando);
  pulsar(HID_KEY_ENTER);
}

void setup() {
// 1. Configurar USB
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();
  
  // 2. Esperar a que el PC nos reconozca (Importante)
  while( !TinyUSBDevice.mounted() ) delay(1);

  // 3. Iniciar Hardware
  strip.begin();
  strip.setBrightness(40);
  strip.show(); // Apagar leds al inicio
  
  pinMode(ENC_BTN, INPUT_PULLUP);

}

void loop() {
  
  if ( !usb_hid.ready() ) return; // Si se desconecta el USB, no hacer nada
  configEncoder();

  // --- B. GESTIÓN DE LA MATRIZ ---
  char key = kpd.getKey(); 
  if (key) {
    ejecutarMacro(key); 
    
   //flash de luces
    strip.setPixelColor(0, 0, 0, 255); strip.show(); //AQUI CONFIGURAR COLOR DEL FLASHEO
    delay(50); strip.setPixelColor(0, 0); strip.show();
  }
}
//configuración de encoder:

void configEncoder(){
  
  encoder.tick(); // Leemos el hardware
  int newPos = encoder.getPosition();
  
  if (lastEncPos != newPos) {
    if (newPos > lastEncPos) {
      // Giro Derecha
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_INCREMENT);
    } else {
      // Giro Izquierda
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
    }
    // Siempre hay que enviar "0" después para soltar el botón de volumen
    usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
    lastEncPos = newPos;
  }
}
//COMPLETAR CON MACROS
void ejecutarMacro(char key) {
  switch(key) {
    case '1': 

      break;
      
    case '2': 

      break;
      
    case '3': 

      break;
      
    case '4': 

      break;
      
    case 'A':
      // Play / Pause Música
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_PLAY_PAUSE);
      usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      break;

    case 'B':
      usb_hid.keyboardPress(KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_ESCAPE);
      delay(10);
      usb_hid.keyboardRelease(0);
      break;

  }
}
