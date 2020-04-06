/* 参考文章：https://www.jianshu.com/p/540347299a37，有很详细的App设置
 * Description ： 使用 Arduino UNO 和 W5100以太网扩展板 和 DS18b20温度传感器 , 
 *                上传温度到Blynk服务器，通过App端实时查看
 * Device ：ArduinoUNO
 *          W5100以太网扩展板
 *          DS18b20温度传感器
 *          接线参考： 
 *           ArduinoUNO =======> DS18b20
 *               D2     =======> Data  
*/
#define BLYNK_PRINT Serial
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// 数据输出脚接开发板数字引脚2,正极和信号脚之间接一个4.7K欧姆的电阻
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

BlynkTimer timer;

// 复制到的auth code
char auth[] = "Your Auth Taken";

//W5100工作期间占用Arduino UNO的数字引脚 10,11,12,13,注意不要使用这些引脚控制你的传感器
#define W5100_CS 10
#define SDCARD_CS 4

//获取温湿度函数
void getTemp() {

  sensors.requestTemperatures(); // 发送命令获取温度
  Serial.print("Temperature for the device 1 (index 0) is: ");
  float val = sensors.getTempCByIndex(0);
  Serial.println(val); 

 //发送给app
  Blynk.virtualWrite(V5,val); 
//  delay(2000);
}
void setup()
{
  // Debug console
  Serial.begin(9600);
  
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH);
  
//注意，最后一个参数就是服务器地址
  Blynk.begin(auth,ip, 8080);
  sensors.begin();
//定时
  timer.setInterval(5000, getTemp);
}

void loop()
{
  Blynk.run();
  timer.run();
}
