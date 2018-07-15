// script du circuit de gestion du poulailler


#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;

#include <OneWire.h>
//OneWire  ds(8);

// PINS
/*
  A0 - IN_BTN
  A1 -
  A2 - ANALOG_IN_LIGHT
  A3 -
  3  -
  4  - DIGITAL_IN_LIGHT
  5  - PIN_MOTOR_1
  6  - PIN_MOTOR_2
  7  - BTN_PORTE_HAUT
  8  - BTN_PORTE_BAS
  9  - OUT_RED_LED
  10 - OUT_GREEN_LED

*/

//input
const int IN_BTN = A0;
const int ANALOG_IN_LIGHT = A2;
const int DIGITAL_IN_LIGHT = 4;
const int BTN_PORTE_HAUT = 7;
const int BTN_PORTE_BAS = 8;

// output
const int OUT_RED_LED = 10;
const int OUT_GREEN_LED = 9;
const int PIN_MOTOR_1 = 5; // PWM
const int PIN_MOTOR_2 = 6; // PWM

// variables globales
boolean isOpen = false;
boolean isLight = false;

unsigned long flashTime = 0;

void setup() {
  // config pins inputs
  pinMode( IN_BTN, INPUT );
  pinMode( ANALOG_IN_LIGHT, INPUT );
  pinMode( DIGITAL_IN_LIGHT, INPUT );

  // config pins leds
  pinMode( OUT_RED_LED, OUTPUT );
  pinMode( OUT_GREEN_LED, OUTPUT );

  // config pins capteurs porte
  pinMode( BTN_PORTE_HAUT, INPUT );
  pinMode( BTN_PORTE_BAS, INPUT );

  pinMode( PIN_MOTOR_1, OUTPUT );
  pinMode( PIN_MOTOR_2, OUTPUT );

  // liaison sÃ©rie / bluetooth
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print(F("Domo poules - "));
  Serial.println(F(__DATE__));

  // tÃ©moin init
  for ( byte i = 0; i < 3; i++ ) {
    digitalWrite( OUT_RED_LED, HIGH );
    digitalWrite( OUT_GREEN_LED, HIGH );
    delay( 300 );
    digitalWrite( OUT_RED_LED, LOW );
    digitalWrite( OUT_GREEN_LED, LOW );
    delay( 300 );
  }

  // initialisation etat porte
  isOpen = isPorteOpen();
  isLight = isOpen;
  if( !isOpen && isPorteClose() ) {
    // la porte est blquÃ©e entre les deux
    openPorte();
  }
}

void loop() {
  // gestion des entrÃ©es
  int lightVal = analogRead( ANALOG_IN_LIGHT );
  boolean isLightLoop = digitalRead( DIGITAL_IN_LIGHT );

  boolean btnPorte = digitalRead( IN_BTN );

  Serial.print(F("porte ouvertre : " ));
  Serial.print( isOpen );
  Serial.print(" | Light : ");
  Serial.print( lightVal );
  Serial.print(" -> ");
  Serial.println( isLight );
  delay(1000);

  if ( isLight != isLightLoop ) {
    // plus de lumiÃ¨re et porte ouverte -> on ferme
    // de lumiere et porte fermÃ©e  -> on ouvre
    unsigned long startTime = millis();
    boolean chgt = true;
    while( (millis() - startTime) < 18000 ) {
      if( isLightLoop !=  digitalRead( DIGITAL_IN_LIGHT ) ) {
        chgt = false;
      }
    }
    if( chgt ) {
      isOpen = isLightLoop;
      if (isOpen) openPorte();
      else closePorte();
      isLight = isLightLoop;
    }
  }


  //btn user UX
  if( btnPorte ) {
    isOpen = !isOpen;
    if (isOpen) openPorte();
    else closePorte();
  }

  // flash led pour l'état de la poste
  if( (millis() - flashTime) > 2000) {
    if( isOpen ) {
      digitalWrite( OUT_GREEN_LED, HIGH );
      delay( 300 );
      digitalWrite( OUT_GREEN_LED, LOW );
    } else {
      digitalWrite( OUT_RED_LED, HIGH );
      delay( 300 );
      digitalWrite( OUT_RED_LED, LOW );
    }
    delay(300);
    flashTime = millis();
  }
}

void openPorte() {
  Serial.println(F("Ouverture de la porte"));
  digitalWrite( OUT_GREEN_LED, HIGH );
  digitalWrite( PIN_MOTOR_1, LOW );
  digitalWrite( PIN_MOTOR_2, HIGH );
  delay( 5100 );
  digitalWrite( PIN_MOTOR_2, LOW );
  digitalWrite( OUT_GREEN_LED, LOW );
}

void closePorte() {
  Serial.println(F("Fermeture de la porte"));
  digitalWrite( OUT_GREEN_LED, LOW );
  digitalWrite( PIN_MOTOR_2, LOW );
  digitalWrite( PIN_MOTOR_1, HIGH );
  delay( 6000 );
  digitalWrite( PIN_MOTOR_1, LOW );
  digitalWrite( OUT_RED_LED, LOW );
}

boolean isPorteOpen() {
  return isOpen;
}
boolean isPorteClose() {
  return !isOpen;
}
