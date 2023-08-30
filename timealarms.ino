#include "config.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", TIMEZONE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

PulseOximeter pox;

float BPM, SpO2;

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


void setup()
{
  Serial.begin(9600);
  Wire.begin(D2, D1);
  digitalWrite(D0, LOW);
  pinMode(D0, OUTPUT);
  lcd.backlight();
  lcd.init();

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif

  lcd.setCursor(0, 0);
  // print message
  lcd.print("Welcome To");
  delay(1000);
  // clears the display to print new message
  lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(0, 1);
  lcd.print("Thesis Project");

  lcd.clear();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while ( WiFi.status() != WL_CONNECTED ) {
    lcd.setCursor(0, 0);
    // print message
    lcd.clear();
    lcd.print("Connecting to wifi ...");
    delay(1000);

  }
  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 1);
    // print message
    lcd.print("**connected**");
    delay(1500);
    lcd.clear();
  }
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  if (!pox.begin())
  {
    Serial.println("FAILED");
    lcd.setCursor(0, 1);
    // print message
    lcd.print("sensor fail");
    delay(1500);
    lcd.clear();
  }
  else
  {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
    lcd.setCursor(0, 1);
    // print message
    lcd.print("sensor ok");
    delay(1500);
    lcd.clear();
  }
  timeClient.begin();
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  lcd.setCursor(0, 0);
  lcd.print("Getting local");
  lcd.setCursor(0, 1);
  lcd.print("time");


  setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(), currentMonth, timeClient.getDay(), currentYear);
  //  // create the alarms
  Alarm.alarmRepeat(8, 30, 0, MorningMed); // 8:30am every day
  Alarm.alarmRepeat(14, 30, 0, EveningMed); // 2:35pm every day
  Alarm.alarmRepeat(20, 30, 0, NightMed); // 8:305pm every day
  //
  //  Alarm.alarmRepeat(01,23 , 0, MorningMed); // 8:30am every day
  //  //  Alarm.alarmRepeat(dowSaturday, 8, 30, 30, WeeklyAlarm); // 8:30:30 every Saturday

  Alarm.timerRepeat(180, Repeats);            // timer for every 15 seconds
  //  //  Alarm.timerOnce(10, OnceOnly);             // called once after 10 seconds
  delay(1500);
  lcd.clear();
}

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void  loop() {

  Alarm.delay(1000); // wait one second between clock display

  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  lcd.setCursor(0, 0);
  lcd.print( String(daysOfTheWeek[timeClient.getDay()]) + " - " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print( "Morning Med 8.30 am" );
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print( String(daysOfTheWeek[timeClient.getDay()]) + " - " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));
  lcd.setCursor(0, 1);
  lcd.print( "Evening Med 2.30 pm" );
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print( String(daysOfTheWeek[timeClient.getDay()]) + " - " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()));
  lcd.setCursor(0, 1);
  lcd.print( "Night Med 8.30 pm" );
  delay(2500);
  lcd.clear();
}

// functions to be called when an alarm triggers:
void MorningMed() {
  digitalWrite(D0, HIGH);
  Serial.println("Morning Med");
  bot.sendMessage(CHAT_ID, "Morning Med : 8.30am : Red Box 3 pill ");
  digitalWrite(D0, LOW);
}

void EveningMed() {
  digitalWrite(D0, HIGH);
  Serial.println("Noon Med");
  bot.sendMessage(CHAT_ID, "Noon Med : 8.30am : Yellow Box 4 pill ");
  digitalWrite(D0, LOW);
}

void NightMed() {
  digitalWrite(D0, HIGH);
  Serial.println("Night Med");
  bot.sendMessage(CHAT_ID, "Night Med : 8.30am : Green Box 4 pill ");
  digitalWrite(D0, LOW);
}



void Repeats() {
  digitalWrite(D0, HIGH);
  Serial.println("15 second timer");
  bot.sendMessage(CHAT_ID, "Morning Med : 8.30am , Noon Med : 2.30pm , Evening med : 8.30pm");
  digitalWrite(D0, LOW);
}

void onBeatDetected()
{
  Serial.println("Beat Detected!");
  Serial.print("Heart rate:");
  Serial.print(BPM);
  Serial.print(" bpm / SpO2:");
  Serial.print(SpO2);
  Serial.println(" %");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BPM: ");
  lcd.setCursor(10, 0);
  lcd.print(pox.getHeartRate());
  lcd.setCursor(0, 1);
  lcd.print("SpO2:");
  lcd.setCursor(10, 1);
  lcd.print(pox.getSpO2());
  lcd.setCursor(14, 1);
  lcd.print("%");
  delay(2000);

}
