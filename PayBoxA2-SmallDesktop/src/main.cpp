#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <JpgMap.h>
#include <PayBox.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <ZdyLwFont_20.h>
#include <string>

TFT_eSPI tft = TFT_eSPI();
char buf[32] = {0};
unsigned long check1s = 0;
unsigned long check5m = 0;
TFT_eSprite drawBuf(&tft);
TFT_eSprite scrollBuf(&tft);
uint16_t bgColor = 0xFFFF;
String cityCode = "101280601";
String WEEK_DAYS[7] = {"日", "一", "二", "三", "四", "五", "六"};
String scrollText[7];
int currentIndex = 0;
int prevTime = 0;

void inline setupJpgDecoder() {
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(
      [](int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *data) -> bool {
        if (y >= tft.height()) {
          return false;
        }
        tft.pushImage(x, y, w, h, data);
        return true;
      });
}

void inline drawPageFrame() {
  TJpgDec.drawJpg(20, 0, watchtop, sizeof(watchtop));
  TJpgDec.drawJpg(20, 220, watchbottom, sizeof(watchbottom));
  tft.setViewport(0, 20, 320, 200);
  tft.fillScreen(0x0000);
  tft.fillRoundRect(0, 0, 320, 200, 4, bgColor);
  tft.drawFastHLine(0, 34, 320, TFT_BLACK);
  tft.drawFastVLine(200, 0, 34, TFT_BLACK);
  tft.drawFastHLine(0, 166, 320, TFT_BLACK);
  tft.drawFastVLine(80, 166, 34, TFT_BLACK);
  tft.drawFastVLine(212, 166, 34, TFT_BLACK);
}

void inline refreshTimeInfo() {
  drawBuf.setColorDepth(8);

  struct tm info;
  getLocalTime(&info);

  strftime(buf, 32, "%R", &info);
  drawBuf.createSprite(140, 48);
  drawBuf.fillSprite(bgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(buf, 70, 24, 6);
  drawBuf.pushSprite(77, 40);
  drawBuf.deleteSprite();

  sprintf(buf, "%02d", info.tm_sec);
  drawBuf.createSprite(40, 32);
  drawBuf.fillSprite(bgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(buf, 20, 16, 4);
  drawBuf.pushSprite(226, 60);
  drawBuf.deleteSprite();

  drawBuf.loadFont(ZdyLwFont_20);
  drawBuf.createSprite(58, 32);
  drawBuf.fillSprite(bgColor);
  sprintf(buf, "周%s", WEEK_DAYS[info.tm_wday]);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(buf, 29, 16);
  drawBuf.pushSprite(1, 168);
  drawBuf.deleteSprite();

  sprintf(buf, "%d-%d-%d", info.tm_year + 1900, info.tm_mon + 1, info.tm_mday);
  drawBuf.createSprite(120, 32);
  drawBuf.fillSprite(bgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(buf, 49, 16);
  drawBuf.pushSprite(82, 168);
  drawBuf.deleteSprite();
  drawBuf.unloadFont();
}

void inline showWeatherData(String *cityDZ, String *dataSK, String *dataFC) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, *dataSK);
  JsonObject sk = doc.as<JsonObject>();
  drawBuf.setColorDepth(8);
  drawBuf.loadFont(ZdyLwFont_20);

  drawBuf.createSprite(60, 32);
  drawBuf.fillSprite(bgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(sk["temp"].as<String>() + "℃", 29, 16);
  drawBuf.pushSprite(230, 168);
  drawBuf.deleteSprite();

  // 城市名称
  drawBuf.createSprite(88, 32);
  drawBuf.fillSprite(bgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(sk["cityname"].as<String>(), 44, 16);
  drawBuf.pushSprite(201, 1);
  drawBuf.deleteSprite();

  // PM2.5空气指数
  uint16_t pm25BgColor = tft.color565(156, 202, 127); // 优
  String aqiTxt = "优";
  int pm25V = sk["aqi"];
  if (pm25V > 200) {
    pm25BgColor = tft.color565(136, 11, 32); // 重度
    aqiTxt = "重度";
  } else if (pm25V > 150) {
    pm25BgColor = tft.color565(186, 55, 121); // 中度
    aqiTxt = "中度";
  } else if (pm25V > 100) {
    pm25BgColor = tft.color565(242, 159, 57); // 轻
    aqiTxt = "轻度";
  } else if (pm25V > 50) {
    pm25BgColor = tft.color565(247, 219, 100); // 良
    aqiTxt = "良";
  }
  drawBuf.createSprite(50, 24);
  drawBuf.fillSprite(bgColor);
  drawBuf.fillRoundRect(0, 0, 50, 24, 4, pm25BgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(0xFFFF);
  drawBuf.drawString(aqiTxt, 25, 13);
  drawBuf.pushSprite(5, 130);
  drawBuf.deleteSprite();

  // 湿度
  drawBuf.createSprite(56, 24);
  drawBuf.fillSprite(bgColor);
  drawBuf.setTextDatum(CC_DATUM);
  drawBuf.setTextColor(TFT_BLACK, bgColor);
  drawBuf.drawString(sk["SD"].as<String>(), 28, 13);
  drawBuf.pushSprite(240, 130);
  drawBuf.deleteSprite();

  scrollText[0] = "实时天气 " + sk["weather"].as<String>();
  scrollText[1] = "空气质量 " + aqiTxt;
  scrollText[2] = "风向 " + sk["WD"].as<String>() + sk["WS"].as<String>();

  deserializeJson(doc, *cityDZ);
  JsonObject dz = doc.as<JsonObject>();
  scrollText[3] = dz["weather"].as<String>();

  deserializeJson(doc, *dataFC);
  JsonObject fc = doc.as<JsonObject>();
  scrollText[4] = "最低温度" + fc["fd"].as<String>() + "℃";
  scrollText[5] = "最高温度" + fc["fc"].as<String>() + "℃";
  scrollText[6] = WiFi.localIP().toString();
  drawBuf.unloadFont();
}

void inline scrollTxt(int pos) {
  scrollBuf.createSprite(160, 24);
  scrollBuf.fillSprite(bgColor);
  scrollBuf.setTextWrap(false);
  scrollBuf.setTextDatum(CC_DATUM);
  scrollBuf.setTextColor(TFT_BLACK, bgColor);
  scrollBuf.drawString(scrollText[currentIndex], 74, pos + 12);
  scrollBuf.pushSprite(2, 4);
}

void inline scrollBanner() {
  if (millis() - prevTime > 2500) {
    if (scrollText[currentIndex]) {
      scrollBuf.setColorDepth(8);
      scrollBuf.loadFont(ZdyLwFont_20);
      for (int pos = 24; pos > 0; pos--) {
        scrollTxt(pos);
      }
      scrollBuf.deleteSprite();
      scrollBuf.unloadFont();
      if (currentIndex >= 6) {
        currentIndex = 0;
      } else {
        currentIndex += 1;
      }
    }
    prevTime = millis();
  }
}

void inline refreshWeatherInfo() {
  String URL = "http://d1.weather.com.cn/weather_index/" + cityCode + ".html";
  HTTPClient httpClient;
  httpClient.begin(URL);
  httpClient.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS "
                          "X) AppleWebKit/604.1.38 (KHTML, like Gecko) "
                          "Version/11.0 Mobile/15A372 Safari/604.1");
  httpClient.addHeader("Referer", "http://www.weather.com.cn/");
  int httpCode = httpClient.GET();
  Serial.println(URL);
  if (httpCode == HTTP_CODE_OK) {
    String str = httpClient.getString();
    int indexStart = str.indexOf("weatherinfo\":");
    int indexEnd = str.indexOf("};var alarmDZ");
    String jsonCityDZ = str.substring(indexStart + 13, indexEnd);
    Serial.println(jsonCityDZ);
    indexStart = str.indexOf("dataSK =");
    indexEnd = str.indexOf(";var dataZS");
    String jsonDataSK = str.substring(indexStart + 8, indexEnd);
    Serial.println(jsonDataSK);
    indexStart = str.indexOf("\"f\":[");
    indexEnd = str.indexOf(",{\"fa");
    String jsonFC = str.substring(indexStart + 5, indexEnd);
    Serial.println(jsonFC);
    showWeatherData(&jsonCityDZ, &jsonDataSK, &jsonFC);
  }
  httpClient.end();
}

void inline startConfigTime() {
  const int timeZone = 8 * 3600;
  configTime(timeZone, 0, "ntp6.aliyun.com", "cn.ntp.org.cn", "ntp.ntsc.ac.cn");
  while (time(nullptr) < 8 * 3600 * 2) {
    delay(500);
  }
}

void setup() {
  disableCore0WDT();
  Serial.begin(115200);
  payTFTInit(128);
  setupJpgDecoder();
  autoConfigWifi();
  setupOTAConfig();
  delay(2000);
  startConfigTime();
  tft.fillScreen(TFT_BLACK);
  drawPageFrame();
  refreshWeatherInfo();
};

void loop() {
  unsigned long ms = millis();
  if (ms - check1s >= 500) {
    check1s = ms;
    ArduinoOTA.handle();
    refreshTimeInfo();
  }
  if (ms - check5m >= 5 * 60 * 1000) {
    check5m = ms;
    refreshWeatherInfo();
  }
  scrollBanner();
  imgAnim();
};