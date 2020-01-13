/* (C) T.Kovacik
    http://github.com/tomaskovacik/

    example of using VAGFISREader.h on STM32, arduino untested, yet

*/

#include <VAGFISReader.h>
//arduino
#define RADIO_CLK 3
#define RADIO_ENA 2
#define RADIO_DATA 4
//stm32
//#define RADIO_CLK PB0 //on `EXTI0
//#define RADIO_ENA PB1 //on EXTI1
//#define RADIO_DATA PA1 //no interrupt attached to this pin only using digitalRead here

VAGFISReader radio_read(RADIO_CLK, RADIO_DATA, RADIO_ENA);
long last_update = 0;
void setup() {
  radio_read.begin();
  Serial.begin(115200);
}

void loop() {
  //  Serial.println(radio_read.hasNewMsg());
  if (radio_read.hasNewMsg()) {
    if (radio_read.msgIsNavi()) {
      if (radio_read.msgIsRadioText()) {
        //radio msg(upper 2lines) in navi mode
        for (uint8_t i = 3; i < radio_read.getSize() - 1; i++) { //1st byte is msg ID, second one is packet size,3th is second msg id (sort of) last is checksumm so we skip them
          Serial.write(radio_read.readData(i));
        }
        Serial.println();
      }
      if (radio_read.msgIsText()) {
        //universal text msg in navi mode
        for (uint8_t i = 5; i < radio_read.getSize() - 1; i++) { //1st byte is msg ID, second one is packet size,3th is second msg id (sort of) last is checksumm so we skip them
          Serial.write(radio_read.readData(i));
        }
        Serial.println();
      }
      //      if (radio_read.msgIsKeepAlive()) {
      //        Serial.println("KeepAlive");
      //      }
      //      if (radio_read.msgIsInit()) {
      //        Serial.println("Init/Reset of screen");
      //      }
      if (radio_read.msgIsGraphics()) {
        //graphics in navi mode, ignoring sizes and starting coordinates
        for (uint8_t i = 5; i < radio_read.getSize() - 1; i++) { //1st byte is msg ID, second one is packet size,3th is second msg id (sort of) last is checksumm so we skip them
          //Y=88 X=64max
          uint8_t tmp=radio_read.readData(i);
          for (int i = 7; i >= 0; i--)  {
            Serial.print(((tmp >> i) & 1) == 1 ? "#" : " ");
          }
        }
        Serial.println();
      }
    } else {
      //pure radio mode
      for (uint8_t i = 1; i < radio_read.getSize() - 1; i++) { //1st byte is msg ID, last is checksumm
        Serial.write(radio_read.readData(i));
      }
      Serial.println();
    }
    radio_read.clearNewMsgFlag();

       last_update = millis();
  }

    if ((millis() - last_update) > 100) {
      radio_read.ACK();
      last_update = millis();
    }
}
