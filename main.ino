#include "NMEA.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define LEN(arr) ((int)(sizeof(arr) / sizeof(arr)[0]))

// Config GPS
union {
  char bytes[4];
  float valor;
} velocidadeGPS;

float latitude;
float longitude;

NMEA gps(GPRMC);

// Config LED Bar
const int analogPin = A0;
const int ledCount = 10;

int ledPins[] = {
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13
};

// Config Display OLED
const byte SCREEN_WIDTH = 128;
const byte SCREEN_HEIGHT = 64;

const int OLED_RESET = -1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Iniciando comunicação serial
  Serial.begin(9600);
  Serial1.begin(9600);  // GPS

  // Iniciando display OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Definindo todos os leds como saída
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
}

void loop() {
  // Simulação do sensor de nível de bateria no led bar
  int sensorReading = analogRead(analogPin);
  int ledLevel = map(sensorReading, 0, 1023, 0, ledCount);
  int porcentLevel = map(sensorReading, 0, 1023, 0, 100);

  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    if (thisLed < ledLevel) {
      digitalWrite(ledPins[thisLed], HIGH);
    } else {
      digitalWrite(ledPins[thisLed], LOW);
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Bateria: ");
  display.print(porcentLevel);
  display.println("%");

  while (Serial1.available()) {
    char serialData = Serial1.read();       // Recebendo dados GPS da porta serial

    if (gps.decode(serialData)) {           // Checks if the GPS sentence is valid
      if (gps.gprmc_status() == 'A') {      // Checks if GPS status is 'A'
        velocidadeGPS.valor = gps.gprmc_speed(KMPH);        // Recebendo dados de velocidade em Km/h
      } else {
        velocidadeGPS.valor = 0;
      }

      latitude  = gps.gprmc_latitude();
      longitude = gps.gprmc_longitude();
    }
  }
  display.setCursor(0, 12);
  display.println("Latitude: ");
  display.setCursor(0, 21);
  display.println(latitude, 8);

  display.setCursor(0, 33);
  display.println("Longitude: ");
  display.setCursor(0, 42);
  display.println(longitude, 8);

  display.setCursor(0, 54);
  display.print("Velocidade: ");
  display.print(velocidadeGPS.valor, 2);
  display.println("Km/h");
  display.display();
}
