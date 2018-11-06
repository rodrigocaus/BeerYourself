
#include <SPI.h>
#include <MFRC522.h> // See example at https://www.filipeflop.com/blog/controle-acesso-leitor-rfid-arduino/
#include <LiquidCrystal.h>

#include "flow.hpp"
#include "solenoid.hpp"

#define SDA_PIN 10
#define RST_PIN 9
MFRC522 rfid(SDA_PIN, RST_PIN);

#define RS_LCD_PIN 6
#define EN_LCD_PIN 7
#define DB4_PIN 5
#define DB5_PIN 4
#define DB6_PIN 3
#define DB7_PIN 2
LiquidCrystal lcd(RS_LCD_PIN, EN_LCD_PIN, DB4_PIN, DB5_PIN, DB6_PIN, DB7_PIN);

#define SOLENOID_PIN 8
Solenoid s(SOLENOID_PIN);

#define FLOW_PIN A0
Flow sf(FLOW_PIN);


/* TESTING FLOW SENSOR */
void setup() {
	Serial.begin(9600);
	lcd.begin(16,2);

	SPI.begin();
	rfid.PCD_Init();
	delay(10);
}

void loop() {
	static int count = 0;
	bool state = sf.get_state();
	if(state)
		count++;

	String s = String(state)+" "+String(count);
	Serial.println(s);
	lcd.print(s.c_str());
	lcd.setCursor(0,0);
	delay(100);
}
