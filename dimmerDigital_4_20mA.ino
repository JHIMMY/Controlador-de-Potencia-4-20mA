/**********************************************************************************
 * TITULO: Dimmer Digital controlado por corriente 4-20mA
 * AUTOR: Romer Z. & Jhimmy A.
 * DESCRIPCION: Controla el circuito por medio de la entradad de corriente de 4 a 20mA
 * CANAL YOUTUBE: https://www.youtube.com/c/jadsatv
 * © 2022
 * *******************************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
// #include <Adafruit_SSD1306.h>
#include <Wire.h>

// oled
const uint8_t ancho = 128; //px
const uint8_t alto = 64; // 32px
Adafruit_SH1106G oled = Adafruit_SH1106G(ancho, alto, &Wire);
// Adafruit_SSD1306 oled(ancho, alto, &Wire);

// constantes
const uint8_t pinPasoPorCero = 2; // entrada
const uint8_t pinDisparo = 9;
const uint16_t pot = A0;              // entrada potenciometro
const uint16_t duracionDisparo = 800; // 50hz -> 20ms  1 semiciclo 10ms (10000 us)

// variables globales
uint16_t lecturaLm358Actual;
uint16_t lecturaLm358Anterior;
uint16_t duracionSemicicloSinCorte;
uint8_t porcentajeDimmer;

void setup()
{
  Serial.begin(9600);
  // configuracion de pines e interrupciones
  pinMode(pinDisparo, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pinPasoPorCero), crucePorCero, RISING);

  // Config volate de referencia (3.3v)
  //analogReference(EXTERNAL);

  // oled init
  oled.begin(0x3C, true); // inicializa pantalla con direccion 0x3C
  oled.clearDisplay();    // limpia pantalla
  //oled.setTextColor(SH110X_WHITE); // establece color al unico disponible (pantalla monocromo)
  oled.setTextColor(WHITE); // establece color al unico disponible (pantalla monocromo)

  // Mensaje de Inicio
  oled.display();
  oled.setCursor(25, 0);              // ubica cursor en coordenas 10,30
  oled.setTextSize(1);                // establece tamano de texto en 2
  oled.print("Jadsa Tv & Zurco"); // escribe texto
  oled.display();
  delay(2000);
  oled.clearDisplay();
  oled.display();
}

void loop()
{

  lecturaLm358Actual = analogRead(A0);
  
  // solamente actualizar valores si cambia la corriente
 if (lecturaLm358Actual != lecturaLm358Anterior){
    // 4mA -> 0.65v => 202
    lecturaLm358Actual = constrain(lecturaLm358Actual, 202, 1023);
    duracionSemicicloSinCorte = map(lecturaLm358Actual, 202, 1023, 9050, 2500);

    // calculo porcentaje prueba y error  
    porcentajeDimmer = map(lecturaLm358Actual, 202, 1023, 0, 100);       
    
    lecturaLm358Anterior = lecturaLm358Actual;
    actualizarOled();
 }

} // eof loop


// ISR
void crucePorCero()
{
  delayMicroseconds(duracionSemicicloSinCorte);
  digitalWrite(pinDisparo, HIGH); // disparo
  delayMicroseconds(duracionDisparo);
  digitalWrite(pinDisparo, LOW);
}


void actualizarOled()
{ 
  //oled.fillRect(0, 0, 128, 64, SH110X_BLACK);
  oled.clearDisplay();
  oled.setCursor(40, 0);             // ubica cursor en inicio de coordenadas 0,0
  oled.setTextSize(3);              // establece tamano de texto en 1-21 2-10 3-7 4-5 caracteres
  oled.print(porcentajeDimmer); // escribe texto
  oled.print("%");
  oled.display();
}
