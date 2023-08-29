#include "config.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <LiquidCrystal_I2C.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", TIMEZONE);
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup()
{
  Serial.begin(9600);
  lcd.backlight();
  lcd.init();

  lcd.setCursor(0, 0);
  // print message
  lcd.print("Welcome To");
  delay(1000);
  // clears the display to print new message
  lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(0, 1);
  lcd.print("Thesis Project");
  delay(1000);
  lcd.clear();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while ( WiFi.status() != WL_CONNECTED ) {
    lcd.setCursor(0, 0);
    // print message
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
  // create the alarms
  //  Alarm.alarmRepeat(08, 30, 0, MorningMed); // 8:30am every day
  //  Alarm.alarmRepeat(14, 30, 0, EveningMed); // 2:35pm every day
  //  Alarm.alarmRepeat(20, 30, 0, NightMed); // 8:305pm every day

  Alarm.alarmRepeat(02, 30, 0, MorningMed); // 8:30am every day
  //  Alarm.alarmRepeat(dowSaturday, 8, 30, 30, WeeklyAlarm); // 8:30:30 every Saturday
  //
  //
  //  Alarm.timerRepeat(15, Repeats);            // timer for every 15 seconds
  //  Alarm.timerOnce(10, OnceOnly);             // called once after 10 seconds
  delay(1500);
  lcd.clear();
}

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void  loop() {
  lcd.setCursor(0, 0);
  lcd.print( String(daysOfTheWeek[timeClient.getDay()]) + " - " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()) );
  lcd.setCursor(0, 1);
  lcd.print( "Morning Med 8.30 am" );
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print( String(daysOfTheWeek[timeClient.getDay()]) + " - " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()) );
  lcd.setCursor(0, 1);
  lcd.print( "Evening Med 2.30 pm" );
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print( String(daysOfTheWeek[timeClient.getDay()]) + " - " + String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds()) );
  lcd.setCursor(0, 1);
  lcd.print( "Night Med 8.30 pm" );
  delay(2500);
  lcd.clear();
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
}

// functions to be called when an alarm triggers:
void MorningMed() {
  Serial.println("Alarm: - turn lights off");
}

void EveningMed() {
  Serial.println("Alarm: - turn lights on");
}

void NightMed() {
  Serial.println("Alarm: - turn lights on");
}

void WeeklyAlarm() {
  Serial.println("Alarm: - its Monday Morning");
}

void ExplicitAlarm() {
  Serial.println("Alarm: - this triggers only at the given date and time");
}

void Repeats() {
  Serial.println("15 second timer");
}

void OnceOnly() {
  Serial.println("This timer only triggers once");
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits)
{

  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);

}
