#include "KwWork.h"
#include "PayBox.h"
#include <ArduinoJson.h>
#include <Audio.h>
#include <HTTPClient.h>

typedef struct {
  long id;
  String name;
  String artist;
  String url;
} SongItem;

long check1s = 0, check10ms = 0, check300ms = 0;
char buf[128] = {0};
LGFX tft;
LGFX_Sprite sp(&tft);
Audio audio;
int curIndex = 0;
int curVolume = 5;
std::map<u32_t, OneButton *> buttons;
vector<SongItem *> songs;

void nextVolume(int offset) {
  int vol = curVolume + offset;
  if (vol >= 0 && vol <= 21) {
    curVolume = vol;
    audio.setVolume(curVolume);
    sprintf(buf, "音量: %d", curVolume);
    sp.createSprite(160, 16);
    sp.drawString(buf, 10, 0);
    sp.pushSprite(0, 220);
    sp.deleteSprite();
  }
}

inline void fetch_music_url(SongItem *song) {
  const char *url = KwWork::getUrl(song->id).c_str();
  HTTPClient http;
  http.begin(url);
  if (http.GET() > 0) {
    String txt = http.getString();
    http.end();
    vector<string> lines;
    istringstream iss(txt.c_str());
    string line;
    while (getline(iss, line)) {
      lines.push_back(line);
    }
    if (lines.size() > 2) {
      auto url = lines[2].substr(4).c_str();
      audio.connecttohost(url);
      digitalWrite(PAY_AUDIO_CTRL, HIGH);
    }
  } else {
    http.end();
  }
}

inline void get_daily_music_list() {
  HTTPClient http;
  http.begin(URL_KW_DAILY);
  if (http.GET() > 0) {
    String txt = http.getString();
    http.end();
    JsonDocument doc;
    JsonDocument _doc;
    JsonObject _f = _doc.add<JsonObject>();
    _f["id"] = true;
    _f["name"] = true;
    _f["artist"] = true;
    auto ft = DeserializationOption::Filter(_doc);
    deserializeJson(doc, txt, ft);
    for (JsonObject item : doc.as<JsonArray>()) {
      auto *song = new SongItem();
      song->id = item["id"];
      song->name = item["name"].as<String>();
      song->artist = item["artist"].as<String>();
      songs.push_back(song);
    }
  } else {
    http.end();
  }
}

void inline showPlayProgress() {
  uint32_t act = audio.getAudioCurrentTime();
  uint32_t afd = audio.getAudioFileDuration();
  sp.createSprite(320, 32);
  sp.drawRoundRect(58, 0, 204, 10, 3, TFT_ORANGE);
  if (act > 0 && afd > 0) {
    int prog = act * 200 / afd;
    sp.fillRoundRect(60, 2, prog, 6, 2, TFT_GREEN);
    sprintf(buf, "%02i:%02d", (act / 60), (act % 60));
    sp.drawString(buf, 60, 16);
    sprintf(buf, "%02i:%02d", (afd / 60), (afd % 60));
    sp.drawRightString(buf, 260, 16);
  }
  sp.pushSprite(0, 50);
  sp.deleteSprite();
}

void playNext(int offset) {
  digitalWrite(PAY_AUDIO_CTRL, LOW);
  audio.stopSong();
  int total = songs.size();
  curIndex += offset;
  if (curIndex >= total) {
    curIndex %= total;
  } else if (curIndex < 0) {
    curIndex += total;
  }
  auto *song = songs[curIndex];
  auto *name = song->name.c_str();
  auto *artist = song->artist.c_str();
  sprintf(buf, "%d.%s - %s", curIndex + 1, name, artist);
  sp.createSprite(320, 16);
  sp.drawCentreString(buf, 160, 0);
  sp.pushSprite(0, 20);
  sp.deleteSprite();
  fetch_music_url(song);
}

void onButtonClick(void *p) {
  u32_t pin = (u32_t)p;
  switch (pin) {
  case PAY_KEY_ON:
    playNext(10);
    break;
  case PAY_KEY_REFRESH:
    audio.pauseResume();
    break;
  case PAY_KEY_ADD:
    playNext(1);
    break;
  case PAY_KEY_MINUS:
    playNext(-1);
    break;
  default:
    break;
  }
}

void onButtonDoubleClick(void *p) {
  u32_t pin = (u32_t)p;
  switch (pin) {
  case PAY_KEY_ON:
    break;
  case PAY_KEY_ADD:
    nextVolume(1);
    break;
  case PAY_KEY_MINUS:
    nextVolume(-1);
    break;
  default:
    break;
  }
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setFont(FONT16);
  tft.setColorDepth(8);
  sp.setFont(FONT16);
  sp.setColorDepth(8);
  tft.setBrightness(30);
  autoConfigWifi();
  startConfigTime();
  setupOTAConfig();
  setupAudoPlayer();
  setupButtons();
  tft.clear();
  showClientIP();
  get_daily_music_list();
  nextVolume(0);
  playNext(0);
}

void loop() {
  audio.loop();
  auto ms = millis();
  if (ms - check1s > 1000) {
    check1s = ms;
    ArduinoOTA.handle();
    showPlayProgress();
  }
  if (ms - check300ms > 300) {
    check300ms = ms;
    showCurrentTime();
  }
  if (ms - check10ms >= 10) {
    check10ms = ms;
    for (auto it : buttons) {
      it.second->tick();
    }
  }
}

void audio_eof_stream(const char *info) { playNext(1); }
