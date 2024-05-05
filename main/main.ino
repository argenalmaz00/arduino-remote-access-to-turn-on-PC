#include <ESP8266WiFi.h>
#include <ESP8266TelegramBOT.h>
#include <WakeOnLan.h>

// config
// wifi config
static String ssid = "#";
static String password = "#";
// telebot config
static String token = "#";
static String nameBot = "#";
static String userBot = "#";
// admin id user telegram
static int idAdminUser = 0;
// telebot init
static TelegramBOT Tbot (token,nameBot,userBot);
// wifi udp
static WiFiUDP udp;
// wake on lan 
static WakeOnLan onLan (udp);
// status wifi led
static int pinLedStatusWiFi = 2; // D4
// pin power (rele)
static int pinPowerPC = 16; // D0
static int pinStatusPc = 4; // D1
static int pinResetPC = 5; // D2
// mac address desktop2
static String macAddres = "22:35:4D:10:3F:B1";
//init pin function
void initPin(){
  // init pin OUTPUT
  pinMode(pinLedStatusWiFi, OUTPUT);
  pinMode(pinPowerPC,OUTPUT);
  pinMode(pinResetPC,OUTPUT);
  // init pin INPUT
  pinMode(pinStatusPc,INPUT);
}
// init wake On Lan
void initWakeOnLan(){
  onLan.setRepeat(1,100);
  onLan.calculateBroadcastAddress(WiFi.localIP(),WiFi.subnetMask());
}
void setup(){
    //init tty speed
    Serial.begin(9600);
    initPin();
    WiFi.begin(ssid,password);
    Serial.print("WiFi : " + ssid + "Connect");
    while (!WiFi.isConnected()){
        digitalWrite(pinLedStatusWiFi,HIGH);
        delay(500);
        digitalWrite(pinLedStatusWiFi,LOW);
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println("Connectd");
    Serial.println(WiFi.localIP());
    initWakeOnLan();
    Tbot.begin();
}

void Bot_ExecMessages() {
  for (int i = 1; i < Tbot.message[0][0].toInt() + 1; i++){
      Serial.println("user : " + Tbot.message[i][2] + "\nmessage : " + Tbot.message[i][5]);

    if (Tbot.message[i][1].toInt() == idAdminUser){
      Tbot.message[i][5].substring(1,Tbot.message[i][5].length());

      if (Tbot.message[i][5] == "/power"){
        digitalWrite(pinPowerPC,HIGH);
        delay(1000);
        digitalWrite(pinPowerPC,LOW);
        String ms = "clicked power";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
      }
      else if (Tbot.message[i][5] == "/reset"){
        digitalWrite(pinResetPC,HIGH);
        delay(1500);
        digitalWrite(pinResetPC,LOW);
        String ms = "click restart";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
      }
      else if (Tbot.message[i][5] == "/status"){
        if (digitalRead(pinStatusPc) == HIGH){
          String ms = "Turn ON";
          Tbot.sendMessage(Tbot.message[i][4],ms,"");
        }
        else {
          String ms = "Turn OFF";
          Tbot.sendMessage(Tbot.message[i][4],ms,"");
        }
      }
      else if (Tbot.message[i][5] == "/power_2"){
        onLan.sendMagicPacket(macAddres);
        String ms = "Desktop wake on lan run";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
      }

      else {
        String ms = "Desktop";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
        ms = "Button power /power";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
        ms = "Hold button /reset";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
        ms = "Desktop2";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
        ms = "wake on lan /power_2";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
        ms = "status pc /status";
        Tbot.sendMessage(Tbot.message[i][4],ms,"");
      }
    }
    // other user
    else {
      String ms = "hello user " + Tbot.message[i][2] + " i'm not working,you id : " + Tbot.message[i][1];
      Tbot.sendMessage(Tbot.message[i][4],ms,"");
    }
    
  }
  Tbot.message[0][0] = "";
}


int Bot_mtbs = 1000; 
long Bot_lasttime;

void  loop(){
  if (millis() > Bot_lasttime + Bot_mtbs){
    // Led status reconnect
    while (!WiFi.isConnected()){
      digitalWrite(pinLedStatusWiFi,HIGH);
      delay(500);
      digitalWrite(pinLedStatusWiFi,LOW);
      delay(500);
    }
    Tbot.getUpdates(Tbot.message[0][1]);
    if (Tbot.message[0][0].toInt() != 0 ){
      Bot_ExecMessages();
    }
    Bot_lasttime = millis();
  }
}   
