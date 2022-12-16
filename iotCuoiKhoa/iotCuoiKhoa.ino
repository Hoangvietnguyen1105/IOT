#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#define BLYNK_PRINT Serial    
#define BLYNK_TEMPLATE_ID "TMPLltUAQXA9"
#define BLYNK_DEVICE_NAME "nong nghiep thong minh"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

LiquidCrystal_I2C lcd(0x27, 16,2);
 
WidgetLED bom(V0);  // Đèn trạng thái bơm
WidgetLED den(V1);  // Đèn trạng thái đèn sưởi
#define DHTPIN D3 
#define doAmDatPin A0

#define bomPin D6   //Bom
#define denPin D7   //Den
#define DHTTYPE DHT11   // DHT 11

int nguongDoKho= 66;
#define nguongDoAm      85
int nguongDoLanh = 20;
#define nguongDoNong      22

//set thời gian hoạt động

#define docDoAm  1L //Đọc cảm biến ẩm đất
#define docDht  1L  //Đọc DHT
#define hienThi  1L //Gửi dữ liệu lên terminal
#define guiBlynk   1L //Gửi dữ liệu lên blynk
#define cheDoAuto   1L //Chế độ tư động

char auth[] = "62aFfb6t-c9A9ths_Byw6j-NJTH0s3v-";
char ssid[] = "Room_201";
char pass[] = "tu1den8_@";
int auto1 = 1;
float doAm = 0;
float nhietDo = 0;
float doAmDat =0;
boolean trangThaiBom = 0;
boolean trangThaiDen = 0;
BlynkTimer timer;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Wire.begin(D1,D2);
  lcd.clear();
  lcd.init();
  lcd.backlight();
  lcd.home();
  lcd.print("hoangviet");
  pinMode(bomPin, OUTPUT);
  pinMode(denPin, OUTPUT);
  thucHien();
  Serial.begin(115200);
  Serial.println("Nguyen Hoang Viet B19DCCN714");
  dht.begin();
  Blynk.begin(auth, ssid, pass);
  bom.off();
  den.off();
  startTimers();
}

void loop() {
  timer.run(); 
  Blynk.run();
}
BLYNK_WRITE(V3) // Điều khiển bơm
{
  int i = param.asInt();
    trangThaiBom = i;
    thucHien(); 
}

BLYNK_WRITE(V4) // Điều khiển đèn
{
   int i = param.asInt(); 
   trangThaiDen = i;
    thucHien(); 
   
}
BLYNK_WRITE(V2) // Điều khiển bơm
{
  int i = param.asInt(); 
    if(i==0)
    {
      auto1 = 0;
      trangThaiBom = 0;
      trangThaiDen = 0;
      thucHien();
      
      
    }
    else
    {
      auto1 = 1;
    }
     
}

void getDoAmDat(void)
{
  int i = 0;
  doAmDat = 0;
  for (i = 0; i < 10; i++)  //
  {
    doAmDat += analogRead(doAmDatPin); //Đọc giá trị cảm biến độ ẩm đất
    delay(50);   // Đợi đọc giá trị ADC
  }
  doAmDat = doAmDat / (i);
  doAmDat = map(doAmDat, 1023, 0, 0, 100); //Ít nước:0%  ==> Nhiều nước 100%

}
void getDht(void)
{

  nhietDo = dht.readTemperature();
  doAm = dht.readHumidity();
  if (isnan(nhietDo) || isnan(doAm))   // Kiểm tra kết nối lỗi thì thông báo.
  {
    Serial.println("Dữ liệu DHT bị lỗi");
    return;
  }
}
void hienThi1(void)
{
 
  lcd.clear();
  // IN thông tin ra màn hình
  Serial.print("Do am: ");
  lcd.print(nhietDo);
  lcd.print("/");
  lcd.print(doAm);
  lcd.print("/");
  lcd.print(doAmDat);
  
  lcd.setCursor(0, 1);
  
  if(trangThaiDen == 1)
  {
    lcd.print("den:on");
  }
  else
  {
    lcd.print("den:off");
  }
  if(trangThaiBom == 1)
  {
    lcd.print("bom:on");
  }
  else
  {
    lcd.print("bom:off");
  }
  
  Serial.print(doAm);
  Serial.print(" %\t");
  Serial.print("Nhiet do: ");
  Serial.print(nhietDo);
  Serial.print(" *C\t");

  Serial.print(" %\t");
  Serial.print("Do am dat: ");
  Serial.print(doAmDat);
  Serial.println(" %");
}
void thucHien()
{
  if (trangThaiBom == 1)
  {
    Blynk.notify("NDTRBOT: Canh bao ==>> BOM ON");
    digitalWrite(bomPin, LOW);
    bom.on();
  }

  else {
    digitalWrite(bomPin, HIGH);
    bom.off();
  }

  if (trangThaiDen == 1)
  {
    Blynk.notify("NDTRBOT: Canh bao ==>> DEN ON");
    digitalWrite(denPin, LOW);
    den.on();
  }
  else
  {
    digitalWrite(denPin, HIGH);
    den.off();
  }
}
void cheDoAuto1(void)
{
  if(auto1 == 0)return;
  if (doAmDat < nguongDoKho)
  {
    batBom();
  }
  if (doAmDat > 70)
  {
    tatBom();
  }


  if (nhietDo < nguongDoLanh)
  {
    batDen();
  }
   if (nhietDo > nguongDoLanh)
  {
    tatDen();
  }
}
void batBom()
{
  trangThaiBom = 1;
  thucHien();
 
}
void batDen()
{
  trangThaiDen = 1;
  thucHien();
}
void tatBom()
{
  trangThaiBom = 0;
  thucHien();
}
void tatDen()
{
  trangThaiDen = 0;
  thucHien();
}
void startTimers(void)
{
 
  timer.setInterval(docDht * 1000, getDht);
  timer.setInterval(docDoAm * 1000, getDoAmDat);
  timer.setInterval(guiBlynk * 1000, sendUptime);
  timer.setInterval(cheDoAuto * 1000, cheDoAuto1);
  timer.setInterval(hienThi * 1000, hienThi1);
}
void sendUptime()
{
  Blynk.virtualWrite(V10, nhietDo); //Nhiệt độ với pin V10
  Blynk.virtualWrite(V11, doAm); // Độ ẩm với pin V11
  Blynk.virtualWrite(V12, doAmDat); // Độ ẩm đất với V12
}
