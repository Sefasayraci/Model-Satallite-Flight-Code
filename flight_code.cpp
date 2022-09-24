#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include<SD.h>
#include<SPI.h>
TinyGPS gps;
SoftwareSerial ss(0, 1);
#include <DFRobot_BMX160.h>
DFRobot_BMX160 bmx160;
#include <Wire.h>
#include <DHT.h> 
#define DHTPIN 12 // pin 0 ve pin 1'e baglamayiniz
#define DHTTYPE DHT11 // DHT11 modulunu tanimliyoruz
#include <Servo.h>
Servo mot;
Servo servo;
DHT dht_sensor(DHTPIN, DHTTYPE);



///// DEĞİŞKENLER /////////

bool buzzerStarted = false;
const int chipSelect = BUILTIN_SDCARD;
uint8_t sat;
float enlem, boylam, gX, gY, gZ, nem, sicaklik;
int yukseklik, spd, year;
unsigned long age;
byte month, day, hour, minute, second;
File data;
String motorDevir;
int buzzerPin = 20;


void setup()
{
    Serial2.begin(9600);
    Serial.begin(9600);
    ss.begin(9600);
    SD.begin(chipSelect);
    Wire.begin();
    bmx160.begin();
    dht_sensor.begin();

    mot.attach(9);
    servo.attach(10);
    mot.writeMicroseconds(1000);
    Serial.begin(9600);
    pinMode(buzzerPin, OUTPUT);
    bmx160.setGyroRange(eGyroRange_500DPS);
}

void loop()
{
    //// GPS VERİLERİNİN ALINMASI ////
    smartdelay(100);
    sat = gps.satellites();
    gps.f_get_position(&enlem, &boylam, &age);
    yukseklik = gps.f_altitude();
    spd = gps.f_speed_kmph();
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second);

    //////////// MOTOR KODU //////////////
    char c = Serial2.read();
    if (c == '1')
    {
        mot.writeMicroseconds(1500);
    }
    if (c == '0')
    {
        mot.writeMicroseconds(1000);
    }
    if (c == '7')
    {
        servo.write(90);
    }
    if (c == '8')
    {
        servo.write(0);
    }

    //// BUZZER ////////////////

    char buz = Serial2.read();

    if (buz == "b")
    {
        digitalWrite(buzzerPin, HIGH);

    }
    if (buzzerStarted == true && buz == "b")
    {
        digitalWrite(buzzerPin, LOW);

    }

    ////// MPU6050 GYROSCOPE VERİLERİ ///////////////
    bmx160SensorData Omagn, Ogyro, Oaccel;
    bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);

    gX = Ogyro.x;
    gY = Ogyro.y;
    gZ = Ogyro.z;

    ////////////// DHT11 VERİLERİ ////////////
    nem = dht_sensor.readHumidity();
    sicaklik = dht_sensor.readTemperature();


    ///// TELEMETRİ VERİLERİNİ SERİ EKRANA YAZMA //////
     //                   0                 1                  2                     3                     4             5              6                7                8                   9                  10                    11             12          13                14            15               16                  17                18                                                                                         
    //Serial2.println(String(sat)+'/'+String(enlem,6)+'/'+String(boylam,6)+'/'+String(yukseklik)+'/'+String(spd)+'/'+String(age)+'/'+String(day)+'/'+String(month)+'/'+String(year)+'/'+String(hour+3)+'/'+String(minute)+'/'+String(second));
    // Serial.println(String(sat)+'/'+String(enlem,6)+'/'+String(boylam,6)+'/'+String(yukseklik)+'/'+String(spd)+'/'+String(age)+'/'+String(day)+'/'+String(month)+'/'+String(year)+'/'+String(hour+3)+'/'+String(minute)+'/'+String(second));
    String Telemetry = String(sat) + '/' + String(enlem, 6) + '/' + String(boylam, 6) + '/' + String(yukseklik) + '/' + String(spd) + '/' + String(age) + '/' + String(day) + '/' + String(month) + '/' + String(year) + '/' + String(hour + 3) + '/' + String(minute) + '/' + String(second) + '/' + String(gX) + '/' + String(gY) + '/' + String(gZ) + '/' + String(nem) + '/' + String(sicaklik) + '/' + String(random(12, 15)) + '/' + String(random(100, 102));
    Serial.println(Telemetry);
    Serial2.println(Telemetry);


    //////TELEMETRİ VERİLERİNİN SD KARTA AKTARILMASI////////////

    data = SD.open("data.txt", FILE_WRITE); // Storage of telemetry sentence to SD Card
    data.println(Telemetry);
    data.close();

}
static void smartdelay(unsigned long ms)
{
    unsigned long start = millis();

    do {
        while (ss.available())

            gps.encode(ss.read());
    } while (millis() - start < ms);

}
