/*This project is under Apache-2.0 license and is owned by adimrsingh (a.k.a Aditya Kumar)
and is open sourced to all at Github (https://github.com/adimrsingh/standalone-uvc-disinfectant-android-app-controlled)*/

#include "BluetoothSerial.h" 
#include <AceButton.h>
using namespace ace_button;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run
`make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 900000; // Safety 2 : Auto Off in 15 minutes


#define RelayPin1 23  //D23 UV Lamp Pin
#define RelayPin2 22  //D22 Safety 1: Buzzer Pin
#define RelayPin3 21  //D22 Blue LED Pin
#define sensor 25     //D24 Safety 3 : PIR Sensor
#define SwitchPin1 13  //D13 Manual Switching (for prototyping)
#define SwitchPin2 12  //D12 Manual Switching (for prototyping)
#define SwitchPin3 14  //D14 Manual Switching (for prototyping)

int toggleState_1 = 1; //Define integer to remember the toggle state for relay
int toggleState_2 = 1; //Define integer to remember the toggle state for buzzer
int toggleState_3 = 1; //Define integer to remember the toggle state for led

char bt_data;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);

void all_Switch_ON(){
  digitalWrite(RelayPin1, LOW); toggleState_1 = 0; delay(100);
  digitalWrite(RelayPin2, LOW); toggleState_2 = 0; delay(100);
  digitalWrite(RelayPin3, LOW); toggleState_3 = 0; delay(100);
}

void all_Switch_OFF(){
  digitalWrite(RelayPin1, HIGH); toggleState_1 = 1; delay(100);
  digitalWrite(RelayPin2, HIGH); toggleState_2 = 1; delay(100);
  digitalWrite(RelayPin3, HIGH); toggleState_3 = 1; delay(100);
}

void Bluetooth_handle()
{
  bt_data = SerialBT.read();
//  Serial.println(bt_data);
  delay(20);

  switch(bt_data)
      {
        case 'A': digitalWrite(RelayPin1, LOW);  toggleState_1 = 0; digitalWrite(RelayPin2, LOW);  toggleState_2 = 0; previousMillis = currentMillis; break; // if 'A' received Turn on Relay
        case 'a': digitalWrite(RelayPin1, HIGH); toggleState_1 = 1; digitalWrite(RelayPin2, HIGH); toggleState_2 = 1; break; // if 'a' received Turn off Relay
        case 'Z': all_Switch_ON(); break;  // if 'Z' received Turn on all Relays
        case 'z': all_Switch_OFF(); break; // if 'z' received Turn off all Relays
        default : break;
      }
}

void setup()
{
  Serial.begin(9600);

  btStart();  //Serial.println("Bluetooth On");
  
  SerialBT.begin("UV_Disinfectant_v2");
  Serial.println("The device is started, now you can pair it with bluetooth!");
  delay(5000);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(sensor, INPUT);


  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);
  digitalWrite(RelayPin3, toggleState_3);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);

  button1.init(SwitchPin1);
  button2.init(SwitchPin2);
  button3.init(SwitchPin3);

  delay(200);
}

void loop()
{  
  currentMillis = millis();
  long state = digitalRead(sensor);

  if(state == HIGH){
    Serial.println("Motion Detected!");
    all_Switch_OFF();
  }

  if(currentMillis - previousMillis > interval) {
     previousMillis = currentMillis;  
     all_Switch_OFF();
  }

  if (SerialBT.available()){
   Bluetooth_handle();
 }

  button1.check();
  button2.check();
  button3.check();
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_1 = 0;
      digitalWrite(RelayPin1, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_1 = 1;
      digitalWrite(RelayPin1, HIGH);
      break;
  }
}

void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_2 = 0;
      digitalWrite(RelayPin2, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_2 = 1;
      digitalWrite(RelayPin2, HIGH);
      break;
  }
}

void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      toggleState_3 = 0;
      digitalWrite(RelayPin3, LOW);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      toggleState_3 = 1;
      digitalWrite(RelayPin3, HIGH);
      break;
  }
}

