#include <SPI.h>
#include <MFRC522.h> // See example at https://www.filipeflop.com/blog/controle-acesso-leitor-rfid-arduino/
#include <LiquidCrystal.h>

#include "flow.hpp"
#include "solenoid.hpp"
#include "communicator.hpp"

#define SDA_PIN 10
#define RST_PIN 9
MFRC522 rfid(SDA_PIN, RST_PIN);

#define RS_LCD_PIN 7
#define EN_LCD_PIN 6
#define DB4_PIN 5
#define DB5_PIN 4
#define DB6_PIN 3
#define DB7_PIN 2
LiquidCrystal lcd(RS_LCD_PIN, EN_LCD_PIN, DB4_PIN, DB5_PIN, DB6_PIN, DB7_PIN);

#define SOLENOID_PIN 8
Solenoid valve(SOLENOID_PIN);

#define FLOW_PIN A1
Flow sensor_flow(FLOW_PIN);

#define BUTTON_PIN A0

Communicator com;

const float volume_pulse_ratio = 1.0;

void setup() {
	com.init();
	lcd.begin(16,2);

	SPI.begin();
	rfid.PCD_Init();
	mensageminicial();
	delay(10);
}

/* TESTING FLOW SENSOR */
/*
void loop() {
	valve.open();
	while (1) {
		static int count = 0;
		bool state = sensor_flow.get_state();
		if(state)
			count++;

		String s = String(state)+" "+String(count);
		Serial.println(s);
		lcd.print(s.c_str());
		lcd.setCursor(0,0);
		delay(100);
	}
}
*/


char machine_state = 's';

void loop() {

	if (com.inMantenance()) {
		machine_state = 'm';
		lcd.clear();
		lcd.print(F("Em manutencao..."));
	}
	else if (com.addingUser()) {
		machine_state = 'a';
	}
	else {
		machine_state = 's';
	}


	switch (machine_state) {
		case 'm':
		case 'M':
			valve.open();
			lcd.clear();
			lcd.print(F("Pressione para"));
			lcd.setCursor(0,1);
			lcd.print(F("encerrar manutencao"));
			pulseIn(BUTTON_PIN, HIGH);
			valve.close();
			machine_state = 's';
			mensageminicial();
			break;

		case 'a':
		case 'A':
			// Le o codigo do cartao
			while ( ! rfid.PICC_ReadCardSerial());
			String conteudo= "";
			for (byte i = 0; i < rfid.uid.size; i++)
			{
				conteudo.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
				conteudo.concat(String(rfid.uid.uidByte[i], HEX));
			}
			conteudo.toUpperCase();
			com.sendRawUUID(conteudo.substring(1));

			/*
			lcd.clear();
			lcd.print(F("Pressione para"));
			lcd.setCursor(0,1);
			lcd.print(F("encerrar cadastro"));
			pulseIn(BUTTON_PIN, HIGH);
			*/

		case 's':
		case 'S':
		default:
			// Verifica se ha cartao
			if ( ! rfid.PICC_IsNewCardPresent())
			{
				return;
			}
			// Le o codigo do cartao
			if ( ! rfid.PICC_ReadCardSerial())
			{
				return;
			}

			String conteudo= "";
			for (byte i = 0; i < rfid.uid.size; i++)
			{
			 	conteudo.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
			 	conteudo.concat(String(rfid.uid.uidByte[i], HEX));
			}
			conteudo.toUpperCase();
			if (com.authenticate(conteudo.substring(1))) //UID 1 - Chaveiro
			{
				lcd.setCursor(0,0);
				lcd.print(F("Acesso liberado!"));
				delay(1000);
				lcd.setCursor(0,0);
				lcd.print(F("Pressione o botao"));
				// Espera o botao ser pressionado
				pulseIn(BUTTON_PIN, HIGH);
				lcd.setCursor(0,0);
				lcd.clear();
				lcd.print(F("Consumo: "));
				lcd.setCursor(0, 1);
				unsigned int count_pulses = 0;
				valve.open();
				while (digitalRead(BUTTON_PIN) == HIGH) {
					if (sensor_flow.get_state()) {
						count_pulses++;
					}
					String vol = String(volume_pulse_ratio*count_pulses) + "ml";
					lcd.print(vol.c_str);
					// Debouncing delay
					delay(100);
				}
				valve.close();
				com.addConsumed(conteudo.substring(1), volume_pulse_ratio*count_pulses);
				delay(3000);
				mensageminicial();
			}

			break;
	}
}

void mensageminicial()
{
  lcd.clear();
  lcd.print(F("Insira o seu"));
  lcd.setCursor(0,1);
  lcd.print(F("cartao no leitor"));
}
