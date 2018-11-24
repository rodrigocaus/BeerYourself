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

const float volume_pulse_ratio = 9.896e-06;

void setup() {
	com.init();
	lcd.begin(16,2);

	SPI.begin();
	rfid.PCD_Init();
	mensageminicial();
	delay(10);
}

char machine_state = 's';

void loop() {

	if (com.inMaintenance()) {
		machine_state = 'm';
		lcd.clear();
		lcd.print(F("Em manutencao..."));
	}
	else if (com.addingUser()) {
    lcd.clear();
    lcd.print(F("Cadastrando..."));
		machine_state = 'a';
	}
	else {
		machine_state = 's';
	}

  String conteudo = "";
	switch (machine_state) {
		case 'm':
		case 'M':
			valve.open();
			lcd.clear();
			lcd.print(F("Pressione para"));
			lcd.setCursor(0,1);
			lcd.print(F("encerrar manutencao"));
			while (digitalRead(BUTTON_PIN) != HIGH);
			valve.close();
			machine_state = 's';
			mensageminicial();
			break;

		case 'a':
		case 'A':
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

			for (byte i = 0; i < rfid.uid.size; i++)
			{
				conteudo.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
				conteudo.concat(String(rfid.uid.uidByte[i], HEX));
			}
			conteudo.toUpperCase();
			com.sendRawUUID(conteudo.substring(1));

      mensageminicial();

      break;

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

			for (byte i = 0; i < rfid.uid.size; i++)
			{
			 	conteudo.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
			 	conteudo.concat(String(rfid.uid.uidByte[i], HEX));
			}
			conteudo.toUpperCase();
			if (com.authenticate(conteudo.substring(1)))
			{
				lcd.clear();
				lcd.print(F("Cartao valido!"));
				delay(2000);
				lcd.clear();
				lcd.print(F("Mantenha o botao"));
        lcd.setCursor(0,1);
        lcd.print(F("pressionado"));
				// Espera o botao ser pressionado
        while (digitalRead(BUTTON_PIN) != HIGH);
				lcd.setCursor(0,0);
				lcd.clear();
				lcd.print(F("Consumo: "));
				valve.open();
        unsigned long before;
        String vol_str = "0.0ml";
        float vol = 0.0;
				while (digitalRead(BUTTON_PIN) == HIGH) {
          before = micros();
					sensor_flow.wait_pulse();
          lcd.setCursor(0, 1);
					lcd.print(vol_str.c_str());
          vol += volume_pulse_ratio*(micros()-before) + 53.7e-06;
          vol_str = String(vol) + "ml";
				}
				valve.close();
				com.addConsumed(conteudo.substring(1), vol);
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
