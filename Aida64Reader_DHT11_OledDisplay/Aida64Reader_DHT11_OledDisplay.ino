/* 原作者：flyAkari 会飞的阿卡林 bilibili UID:751219
 * 修改者：shmebluk 
 * 本代码适用于ESP8266 NodeMCU + 128*64 OLED 屏幕（驱动芯片SSD1306）+ DHT 11 温湿度传感器模块
7pin SPI引脚，正面看，从左到右依次为GND、VCC、D0、D1、RES、DC、CS
   ESP8266 ---  OLED
     3V    ---  VCC
     G     ---  GND
     D7    ---  D1
     D5    ---  D0
     D2orD8---  CS
     D1    ---  DC
     RST   ---  RES

4pin IIC引脚，正面看，从左到右依次为GND、VCC、SCL、SDA
     OLED  ---  ESP8266
     VCC   ---  3.3V
     GND   ---  G (GND)
     SCL   ---  D1(GPIO5)
     SDA   ---  D2(GPIO4)

DHT 11 模块       ESP8266模块
  date引脚 ---  D5引脚（GPIO14） 
*/
/*------------我修改的1----上------*/
#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 14//定义DHT11模块date引脚（GPIO14）

#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);
/*------------我修改的1----下------*/

#include <U8g2lib.h>

/*--------------我修改的2----------上----------*/
// #ifdef U8X8_HAVE_HW_SPI
// #include <SPI.h>
// #endif
// #ifdef U8X8_HAVE_HW_I2C
// #include <Wire.h>
// #endif

// //定义OLED 屏幕引脚
// //SPI总线：
// U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 4, /* data=*/ 5,
//                                            /* cs=*/ 3, /* dc=*/ 7, /* reset=*/ 6);//软件SPI接线定义
// //IIC总线
// #define SCL 5
// #define SDA 6
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R2, /* clock=*/ SCL, /* data=*/ SDA, 
//                                           /* reset=*/ U8X8_PIN_NONE);   // 软件IIC接线定义

#define SUN_CLOUD 1 //定义了一个天气图标
/*--------------我修改的2----------下----------*/

/*--------------作者自己的引脚定义----------上----------*/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 4, /* dc=*/ 5, /* reset=*/ 3); //使用7个引脚SPI屏幕的取消注释这行并注释掉上一行
/*--------------作者自己的引脚定义----------下----------*/

char tcpu[25], scpuuti[25], scpuclk[25], vcpu[25]; //用来存着4个数值
char id[15];
char value[25];
byte inByte;
int count = 0;//定义一个计数器

//获取电脑信息
void getCpuDate()
{
  if (Serial.available() > 0)
  { //数据帧格式：?TCPU=37!
    inByte = Serial.read();
    if (inByte == '?')
    {
      int i = 0;
      while (inByte != '=')
      {
        while (Serial.available() == 0)
          ;
        inByte = Serial.read();
        if (inByte != '=')
        {
          id[i++] = inByte;
        }
        else
        {
          id[i] = '\0';
        }
      }
      i = 0;
      while (inByte != '!')
      {
        while (Serial.available() == 0)
          ;
        inByte = Serial.read();
        if (inByte != '!')
        {
          value[i++] = inByte;
        }
        else
        {
          value[i] = '\0';
        }
      }
      i = 0;
      Serial.println(id); //如果需要显示其他的信息，去上位机输出里查名称
      Serial.println(value);
      if (strcmp("TCPU", id) == 0)
        strcpy(tcpu, value);
      if (strcmp("SCPUUTI", id) == 0)
        strcpy(scpuuti, value);
      if (strcmp("SCPUCLK", id) == 0)
        strcpy(scpuclk, value);
      if (strcmp("VCPU", id) == 0)
        strcpy(vcpu, value);
      refresh();
    }
  }
}

//发送到OLED 屏幕
void refresh()
{
  u8g2.clearBuffer();                         // 清空显存
  u8g2.setFont(u8g2_font_unifont_t_chinese2); // 选一个合适的字体
  u8g2.drawStr(0, 15, "CPU Temp:");
  u8g2.drawStr(73, 15, tcpu);
  u8g2.drawStr(97, 15, "'C");
  u8g2.drawStr(0, 30, "CPU Util:");
  u8g2.drawStr(73, 30, scpuuti);
  u8g2.drawStr(105, 30, "%");
  u8g2.drawStr(0, 45, "CPU Freq:");
  u8g2.drawStr(73, 45, scpuclk);
  //u8g2.drawStr(105, 45, "MHz");
  u8g2.drawStr(105, 45, "GHz");
  u8g2.drawStr(0, 60, "CPU Volt:");
  u8g2.drawStr(73, 60, vcpu);
  u8g2.drawStr(105, 60, "V");
  u8g2.sendBuffer(); // 打到屏幕上
}

/*------------我修改的3-----------------*/
//绘制天气图标
void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  switch (symbol)
  {
  case SUN_CLOUD:
    u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
    u8g2.drawGlyph(x, y, 69);
    break;
  }
}

//绘制天气图标和温度
void drawWeather(uint8_t symbol, int t, int h)
{
  //绘制天气符号
  drawWeatherSymbol(0, 48, symbol);
  //绘制温度
  u8g2.setFont(/*u8g2_font_logisoso16_tf*/ u8g2_font_lubI12_te);

  u8g2.setCursor(58, 23);
  u8g2.print("T : ");
  u8g2.print(t);
  u8g2.drawGlyph(109, 21, 176);
  u8g2.setCursor(116, 23);
  u8g2.print("C");

  u8g2.setCursor(58, 44);
  u8g2.print("H : ");
  u8g2.print(h);
  u8g2.print(" %"); // requires enableUTF8Print()
}

/*
  Draw a string with specified pixel offset. 
  The offset can be negative.
  Limitation: The monochrome font with 8 pixel per glyph
*/

//绘制滚动字幕
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36]; // should for screen with up to 256 pixel width
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;

  u8g2.setDrawColor(0); // clear the scrolling area
  u8g2.drawBox(0, 49, u8g2.getDisplayWidth() - 1, u8g2.getDisplayHeight() - 1);
  u8g2.setDrawColor(1); // set the color for the text

  len = strlen(s);
  if (offset < 0)
  {
    char_offset = (-offset) / 8;
    dx = offset + char_offset * 8;
    if (char_offset >= u8g2.getDisplayWidth() / 8)
      return;
    visible = u8g2.getDisplayWidth() / 8 - char_offset + 1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset * 8 - dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if (char_offset >= len)
      return; // nothing visible
    dx = offset - char_offset * 8;
    visible = len - char_offset;
    if (visible > u8g2.getDisplayWidth() / 8 + 1)
      visible = u8g2.getDisplayWidth() / 8 + 1;
    strncpy(buf, s + char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
}

//绘制显示界面
void draw(const char *s, uint8_t symbol, int t, int h)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);

  u8g2.clearBuffer();        // clear the internal memory
  drawWeather(symbol, t, h); // draw the icon and degree only once
  for (;;)                   // then do the scrolling
  {

    drawScrollString(offset, s); // no clearBuffer required, screen will be partially cleared here
    u8g2.sendBuffer();           // transfer internal memory to the display

    delay(100); //修改延时毫秒数可以改变字符串滚动速度
    offset += 6;
    if (offset > len * 8 + 1)
      break;
  }
}
/*------------我修改的3-----------------*/

void setup()
{
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.enableUTF8Print(); //开启后能显示一些中文字
  Serial.begin(115200);
  dht.begin();
}

void loop()
{

  /*--------------我修改的4----------------*/
  count = 0;
  while (count < 4)
  {
    getCpuDate();
    count++;
    delay(1000);
  }
  
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  draw("What a beautiful day!", SUN_CLOUD, t, h); //调用函数绘制显示界面
  /*--------------我修改的4----------------*/
}
