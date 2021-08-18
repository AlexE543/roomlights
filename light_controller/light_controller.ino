#include <SPI.h>
#include <FastLED.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ArduinoJson.h>

#define SERVICE_UUID        "f468ff61-b23c-419c-b183-e36e12bcdb11" // Generate a uuid for this
#define CHARACTERISTIC_UUID "dc7d122f-d75e-49a9-ad4f-fc4fda07e848" // Generate a uuid for this

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

const int LED = 25; // Change this as necessary for whatever pin you connect your leds data pin to
const int NUM_LEDS = 70; // Change this for the length of your LED strip
CRGB leds[NUM_LEDS];
uint32_t timer;

char command_buffer[5000];

int state = 0;
int pattern = 0;

const int OFF = 0;
const int PIANO = 1;
const int PONG = 2;
const int TWINKLE = 3;
const int FADE = 4;
const int FILL_LEFT = 5;
const int RAINBOW = 6;
const int SPOTIFY = 7;
const int SOLID = 8;

JsonArray genres;

int r = 0;
int g = 0;
int b = 0;

double wait_time = 2000;
double color_change_time = 40000;
uint32_t color_change_timer = millis();
double bpm = 0;
int time_signature = 4;

void showLeds() {
    FastLED.show();
}

void setAll(int r, int g, int b) {
    for (int i=0; i < NUM_LEDS; i++) {
        setPixel(i, r, g, b);
    }
    showLeds();
}

void setPixel(int pixel, int r, int g, int b) {
    leds[pixel].r = r;
    leds[pixel].g = g;
    leds[pixel].b = b;
}

void fadeInAndOut(int speed_=1){
  float red, green, blue;
     
  for(int k = 0; k < 256; k=k+speed_) {
    red = (k/256.0)*r;
    green = (k/256.0)*g;
    blue = (k/256.0)*b;
    setAll(red, green, blue);
    showLeds();
  }
     
  for(int k = 255; k >= 0; k=k-speed_) {
    red = (k/256.0)*r;
    green = (k/256.0)*g;
    blue = (k/256.0)*b;
    setAll(red, green, blue);
    showLeds();
  }
}

void pong() {
  for (int i = 0;i < NUM_LEDS-10-2; i++) {
    setAll(0, 0, 0);
    setPixel(i, r/10, g/10, b/10);
    for (int j = 1; j< 10;j++) {
      setPixel(i+j, r, g, b);
    }
    setPixel(i+10, r/10, g/10, b/10);
    delay(10);
    showLeds();
  }
  delay(50);
  for (int i = NUM_LEDS-10-2;i > 0 ; i--) {
    setAll(0, 0, 0);
    setPixel(i, r/10, g/10, b/10);
    for (int j = 1; j< 10;j++) {
      setPixel(i+j, r, g, b);
    }
    setPixel(i+10, r/10, g/10, b/10);
    delay(5);
    showLeds();
  }
}

void piano() {
  int one = rand() % (NUM_LEDS - 10);
  int two = rand() % (NUM_LEDS - 10);
  int three = rand() & (NUM_LEDS - 10);
  setAll(0, 0, 0);
  for (int i = one; i < one+10; i++) {
    setPixel(i, r, g, b);
  }
  for (int j = two; j < two+10; j++) {
    setPixel(j, r, g, b);
  }
  for (int k = three; k < three+10; k++) {
    setPixel(k, r, g, b);
  }
  showLeds();
}

void twinkle() {
  setAll(0, 0, 0);
  for (int i = 0; i<40; i++) {
    int l = rand() % NUM_LEDS;
    setPixel(l, r, g, b);
    showLeds();
    delay(25);
  }
}

void fillLeft() {
  double grad = 0.1;
  for (int i = 0;i < NUM_LEDS; i=i+10) {
    for (int j = 0;j < i; j++) {
      setPixel(j, int(r+(grad*i)), int(g+(grad*i)), int(b+(grad*i)));
    }
    for (int k = i;k < NUM_LEDS; k++) {
      setPixel(k, 0, 0, 0);
    }
    showLeds();
    delay(NUM_LEDS);
  }
}

void rainbow() {
  for(int y = 0; y < NUM_LEDS; y++)
    {
      setPixel(y, rand() % 150, rand() % 150, rand() % 150);
    }
    delay(500);
    showLeds();
}

boolean isDone(char *command) {
  for (int i=0;i < strlen(command); i++) {
    if (command[i] == '}') {
      Serial.println("true");
      return true;
    }
  }
  return false;
}

void turnOff() {
  setAll(0, 0, 0);
}

void randColor() {
    r = rand() % 255;
    g = rand() % 255;
    b = rand() % 255;
}

void setGenre(JsonArray genres) {
  for (JsonVariant genre : genres) {
      const char* text = genre.as<const char*>();
      char * pop;
      pop = strstr (text,"pop");
      char * rock;
      rock = strstr (text,"rock");
      char * low;
      low = strstr (text,"lo-fi");
      char * indie;
      indie = strstr (text,"indie");
      char * folk;
      folk = strstr (text,"folk");
      char * latin;
      latin = strstr (text,"latin");
      char * metal;
      metal = strstr (text,"metal");
      char * jazz;
      jazz = strstr (text,"jazz");
      char * rap;
      rap = strstr (text,"rap");
      if (rock!=NULL) {
        r = 0;
        g = 70;
        b = 0;
        pattern = 1;
        Serial.println(text);
        break;
      } else if (pop!=NULL) {
        r = 70;
        g = 0;
        b = 0;
        pattern = 2;
        Serial.println(text);
        break;
      } else if (low!=NULL) {
        r = 0;
        g = 10;
        b = 60;
        pattern = 3;
        Serial.println(text);
        break;
      } else if (indie!=NULL) {
        r = 0;
        g = 0;
        b = 10;
        pattern = 4;
        Serial.println(text);
        break;
      } else if (folk!=NULL) {
        r = 10;
        g = 50;
        b = 0;
        pattern = 5;
        Serial.println(text);
        break;
      } else if (latin!=NULL) {
        r = 50;
        g = 30;
        b = 0;
        pattern = 6;
        Serial.println(text);
        break;
      } else if (metal!=NULL) {
        r = 50;
        g = 50;
        b = 50;
        pattern = 2;
        Serial.println(text);
        break;
      } else if (jazz!=NULL) {
        r = 50;
        g = 0;
        b = 30;
        pattern = 3;
        Serial.println(text);
        break;
      } else if (rap!=NULL) {
        r = 50;
        g = 50;
        b = 10;
        pattern = 5;
        Serial.println(text);
        break;
      } else {
        int char_text = int(text);
        r = (char_text / 3) % 100;
        g = (char_text / 5) % 100;
        b = (char_text / 7) % 100;
      }
  }
}

class Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if( !value.empty() ) {
        strcat(command_buffer, value.c_str());
        if (isDone(command_buffer)) {
          Serial.println(command_buffer);
          StaticJsonDocument<500> doc;
          DeserializationError error = deserializeJson(doc, command_buffer);
          if (error) {
            memset(command_buffer, 0, sizeof command_buffer);
            Serial.println("command_buffer clear");
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
          }
          int command = doc["command"];
          if (command == SOLID) {
            r = doc["red"].as<int>();
            g = doc["green"].as<int>();
            b = doc["blue"].as<int>();
            setAll(r, g, b);
          } else if (command == SPOTIFY) {
            genres = doc["g"].as<JsonArray>();
            setGenre(genres);
            bpm = doc["b"];
            time_signature = doc["ts"];
          }
          state = command;
          Serial.println(state);
          memset(command_buffer, 0, sizeof command_buffer);
        }
      }
    }
};

void setup() {
  Serial.begin(115200); // Set up serial port
  delay(100);

  Serial.println("Starting BLE work!");

  BLEDevice::init("lights"); // Change this to whatever you want your bluetooth device name to be
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello, World!");
  pCharacteristic->setCallbacks(new Callbacks());
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in the Client!");

  pinMode(25,OUTPUT); digitalWrite(25,0);//in case you're controlling your screen with pin 25
  FastLED.addLeds<WS2812, LED, GRB>(leds, NUM_LEDS);
  timer = millis();
  randColor();
  Serial.println("SETUP DONE");
}

void handleLights(int state_var) {
  if ((millis() - timer) >= wait_time) {
    switch(state_var) {
      case OFF: turnOff(); break;
      case PIANO: piano(); break;
      case PONG: pong(); break;
      case TWINKLE: twinkle(); break;
      case FADE: fadeInAndOut(); break;
      case FILL_LEFT: fillLeft(); break;
      case RAINBOW: rainbow(); break;
    }
    timer = millis();
  }
}

void loop() {
  if (state == SPOTIFY) {
    if (bpm != 0) {
      wait_time = 60.0*1000.0/bpm;
      color_change_time = (wait_time*8.0*time_signature);
    } else {
      wait_time = 1000;
      color_change_time = 32000;
    }
    if ((millis() - color_change_timer) >= color_change_time) {
      randColor();
      color_change_timer = millis();
    }
    handleLights(pattern);
  } else {
    handleLights(state);
  }
}
