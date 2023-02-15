#include "WiFi.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ADS1115.h>

#define WRITE_APIKEY "ZQ1Z0P2BE10M5LO5"
#define ONE_WIRE_BUS 5
#define BUILTIN_LED 2

#define brs_suhu 3
#define brs_ph 5

#define min_suhu 0
#define max_suhu 45

#define min_ph 0
#define max_ph 14

#define dingin 0
#define normal_suhu 1
#define panas 2

#define sangat_asam 0
#define asam 1
#define normal_ph 2
#define basa 3
#define sangat_basa 4

#define kosong 0
#define sedikit 1
#define normal_out 2
#define banyak 3
#define sangat_banyak 4


#define pin_up 33
#define pin_down 32

#define up_on digitalWrite(pin_up,HIGH)
#define up_off digitalWrite(pin_up,LOW)

#define down_on digitalWrite(pin_down,HIGH)
#define down_off digitalWrite(pin_down,LOW)

#define led_on digitalWrite(BUILTIN_LED,HIGH)
#define led_off digitalWrite(BUILTIN_LED,LOW)

#define durasi 0.1



//----------Variabel rule dan bobot
double rule[15];
double bobot_suhu[brs_suhu]; 
double bobot_ph[brs_ph];
double data_set_ph[200];
double data_set_suhu[50];

//----------Membership function
double mf_sen_suhu[3][3] = { 
                {0, 10, 25 },           //dingin
                { 20, 28, 35 },         //normal
                { 25, 35, 45 },         //panas
              };

double mf_sen_ph[5][3]={
  { 0, 1, 4.5 },                        //SangatAsam
  { 4, 5.5, 7 },                        //Asam
  { 6.5, 7.25, 8 },                     //Normal
  { 7.5, 8.25, 9 },                     //Basa
  { 8.5, 10, 14 }                       //SangatBasa
};

double mf_ph_up[]={0,50,100,150,200};
double mf_ph_down[]={0,50,100,150,200};

long old_millis;
//long kekentalan;
int pulse;
int total_rule;
bool mode;

int jml_loop_suhu,jml_loop_ph;
float ph=0;
float suhu=0;


#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate
#define samplingInterval 10
#define printInterval 100
#define ArrayLenth  20    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ADS1115 adc;
ADS1115ScaleFloat scale0;
LiquidCrystal_I2C lcd(0x27, 16, 4);
WiFiClient client;

const char* host = "api.thingspeak.com";
const int httpPort = 80;
const char* ssid     = "Dicky Alvian";
const char* password = "dindagundul";
long last_millis_wifi;



void setup() {
Serial.begin(9600);
sensors.begin();
pinMode(pin_up,OUTPUT);
pinMode(pin_down,OUTPUT);

up_off;
down_off;

pinMode(BUILTIN_LED, OUTPUT);
digitalWrite(BUILTIN_LED, HIGH);
  
lcd.begin();
int index = 0;


adc.setSpeed(ADS1115_SPEED_16SPS);
scale0.setRef(0, 0, 28500, 5000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  led_off;
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("Menghubungkan...");
  lcd.setCursor (0,1);
  lcd.print(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  led_on;
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("Menghubungkan...");
  lcd.setCursor (0,1);
  lcd.print("SUKSES!!!");
  Serial.println("");
  Serial.println("Terhubung Wifi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(3000);
  lcd.clear();

}

long last_on;
int minute,second;

void xloop(){
  get_ph();
  delay(1000);
}


void loop() {
    get_ph();
    cek_wifi();
    sensors.requestTemperatures();
    suhu = sensors.getTempCByIndex(0);
    Serial.print("Suhu: ");
    Serial.print((int)suhu);
    Serial.print(" oC   ");
    Serial.print("PH: ");
    Serial.println(ph); 
    
    //Fuzzyfikasi
    get_bobot_ph (ph, 5);
    get_bobot_suhu(suhu, 3);
    
  
    //Rulebase
    rule[1] = min_val(bobot_suhu[dingin], bobot_ph[sangat_asam]);
    rule[2] = min_val(bobot_suhu[dingin], bobot_ph[asam]);
    rule[3] = min_val(bobot_suhu[dingin], bobot_ph[normal_ph]);
    rule[4] = min_val(bobot_suhu[dingin], bobot_ph[basa]);
    rule[5] = min_val(bobot_suhu[dingin], bobot_ph[sangat_basa]);

    rule[6] = min_val(bobot_suhu[normal_suhu], bobot_ph[sangat_asam]);
    rule[7] = min_val(bobot_suhu[normal_suhu], bobot_ph[asam]);
    rule[8] = min_val(bobot_suhu[normal_suhu], bobot_ph[normal_ph]);
    rule[9] = min_val(bobot_suhu[normal_suhu], bobot_ph[basa]);
    rule[10] = min_val(bobot_suhu[normal_suhu], bobot_ph[sangat_basa]);

    rule[11] = min_val(bobot_suhu[panas], bobot_ph[sangat_asam]);
    rule[12] = min_val(bobot_suhu[panas], bobot_ph[asam]);
    rule[13] = min_val(bobot_suhu[panas], bobot_ph[normal_ph]);
    rule[14] = min_val(bobot_suhu[panas], bobot_ph[basa]);
    rule[15] = min_val(bobot_suhu[panas], bobot_ph[sangat_basa]);

    //defuzzifikasi
    double total_rule=0;
    for (int xx = 1; xx <= 15; xx++){
      total_rule = rule[xx] + total_rule;
      //Serial.println(rule[xx]);
    }

    double defuzzy_up=0;
    defuzzy_up = rule[1] * mf_ph_up[sangat_banyak] +
      rule[2] * mf_ph_up[banyak] +
      rule[3] * mf_ph_up[kosong] +
      rule[4] * mf_ph_up[kosong] +
      rule[5] * mf_ph_up[kosong] +
      rule[6] * mf_ph_up[banyak] +
      rule[7] * mf_ph_up[sedikit] +
      rule[8] * mf_ph_up[kosong] +
      rule[9] * mf_ph_up[kosong] +
      rule[10] * mf_ph_up[kosong] +
      rule[11] * mf_ph_up[banyak] +
      rule[12] * mf_ph_up[sedikit] +
      rule[13] * mf_ph_up[kosong] +
      rule[14] * mf_ph_up[kosong] +
      rule[15] * mf_ph_up[kosong];

    double defuzzy_down=0;
    defuzzy_down = rule[1] * mf_ph_down[kosong] +
      rule[2] * mf_ph_down[kosong] +
      rule[3] * mf_ph_down[kosong] +
      rule[4] * mf_ph_down[sedikit] +
      rule[5] * mf_ph_down[banyak] +
      rule[6] * mf_ph_down[kosong] +
      rule[7] * mf_ph_down[kosong] +
      rule[8] * mf_ph_down[kosong] +
      rule[9] * mf_ph_down[sedikit] +
      rule[10] * mf_ph_down[banyak] +
      rule[11] * mf_ph_down[kosong] +
      rule[12] * mf_ph_down[kosong] +
      rule[13] * mf_ph_down[kosong] +
      rule[14] * mf_ph_down[sedikit] +
      rule[15] * mf_ph_down[sangat_banyak];

    Serial.print("UP:");
    Serial.print(defuzzy_up);
    Serial.print("DOWN:");
    Serial.println(defuzzy_down);
   

    defuzzy_up = defuzzy_up / total_rule;
    defuzzy_down = defuzzy_down / total_rule;
    if (isnan(defuzzy_up )){defuzzy_up =0;}
    if (isnan(defuzzy_down )){defuzzy_down =0;}
    
    if ((millis()-last_on)>1000){
      second=second+((millis()-last_on)/1000);
      if (second>59){second=0;minute++;}
      last_on=millis();
    }
    
    if (minute>=durasi){
      lcd.clear();
      minute=0;
      if (defuzzy_up<=0 && defuzzy_down<=0){
        up_off;down_off;
        lcd.setCursor(0,0);
        lcd.print("BUFFER OFF:");
        lcd.setCursor(0,1);
        lcd.print("ADD: ");
        lcd.print(0);
        lcd.print(" CC   ");
        }
      else if (defuzzy_up>0 && defuzzy_down<=0){
        lcd.setCursor(0,0);
        lcd.print("PH UP AKTIF:");
        lcd.setCursor(0,1);
        lcd.print("ADD: ");
        lcd.print(defuzzy_up);
        lcd.print(" CC   ");
        up_on;delay(defuzzy_up*50);down_off;
        }
      else if (defuzzy_up<=0 && defuzzy_down>0){
                lcd.setCursor(0,0);
        lcd.print("PH DOWN AKTIF:");
        lcd.setCursor(0,1);
        lcd.print("ADD: ");
        lcd.print(defuzzy_down);
        lcd.print(" CC   ");
        
        up_off;down_on;delay(defuzzy_down*50);
        }  
        up_off;down_off;
        delay(2000);
    }
  if ((millis()-last_millis_wifi)>3000){
    Serial.print("Response Server:");
       String data_thingspeak = "/update?api_key=";
       data_thingspeak+= WRITE_APIKEY;
       data_thingspeak+= "&field1=";
       data_thingspeak+= String(ph);
       data_thingspeak+= "&field2=";
       data_thingspeak+= String(suhu);
       
    Serial.println(send(data_thingspeak));
    last_millis_wifi=millis();
  }
    

    lcd.setCursor(0,0);
    lcd.print("SUHU: ");
    lcd.print((int)suhu);
    lcd.print(" oC   ");

    lcd.setCursor(0,1);

    lcd.print("PH:");
    lcd.print(ph);
    lcd.print("   ");
   
    
    lcd.print(minute);
    lcd.print(":");
    lcd.print(second);
    
    lcd.print("    ");
    
    lcd.setCursor(-4,2);
    lcd.print("UP:");
    lcd.print(defuzzy_up);
    lcd.print(" DW:");
    lcd.print(defuzzy_down);
    lcd.print("   ");

    lcd.setCursor(-4,3);
     if (defuzzy_up>0 && defuzzy_down<=0){
      lcd.print("KONDISI PH ASAM ");
    }else if (defuzzy_up<=0 && defuzzy_down>0){
      lcd.print("KONDISI PH BASA ");
    } else if (defuzzy_up<=0 && defuzzy_down<=0){
      lcd.print("KONDISI PH NTRAL");
    }

    delay(1);
}
