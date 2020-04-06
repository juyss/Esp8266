/*
 *Description ：此程序通过web服务器(Gmail),使app与硬件通讯,需要用到blynk_server,并设置相关参数.
 *Device ：ESP8266 （D1 WiFi ）
 *         DHT11
 *         接线参考： 
 *           ESP8266 =======> DHT11
 *            GPIO 4 =======> Data  
*/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "Your Auth Taken";//修改为你的设备秘钥

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Your WiFissid"; //你的WiFi名称
char pass[] = "Your password"; //WiFi 密码

#define DHTPIN 4          // DHT11连接开发板的引脚（GPIO）

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V6, h); //App中接受数据的引脚号
  Blynk.virtualWrite(V5, t); //App中接受数据的引脚号
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,0,108), 8080);

  dht.begin();

  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
