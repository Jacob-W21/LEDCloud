#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>

// Declare Strip
#define LED_PIN    5
#define NUM_LEDS  90
#define FLASHES    8
#define FREQUENCY 50

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
unsigned int dimmer = 1;

//Needed Variables
int pattern;
bool power = false;
int rInt = -1;
int gInt = -1;
int bInt = -1;
uint8_t gHue = 0;
int brightness;


// Replace with your network credentials
const char* ssid = "[ENTER SSID]";
const char* password = "[ENTER PASSWORD]";

//Parameters
const char* BRIGHT = "amount";
const char* red = "red";
const char* green = "green";
const char* blue = "blue";
const char* CHOICE = "choice";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  currentBlending = LINEARBLEND;

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print Local IP Address
  Serial.println(WiFi.localIP());

  //Setup LED Strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  turnOff();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to handle favicon
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

  // Route for /about web page
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route for /about web page
  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/about.html", "text/html");
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load app.js
  server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/app.js", "text/javascript");
  });

  // Route to turn strip on
  server.on("/on", HTTP_POST, [](AsyncWebServerRequest * request) {
    turnOn();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to turn strip off
  server.on("/off", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = -1;
    turnOff();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to set brightness
  server.on("/brightness", HTTP_POST, [](AsyncWebServerRequest * request) {
    int amount = request->getParam(BRIGHT)->value().toInt();
    brightness = amount * 2.55;
    FastLED.setBrightness(brightness);
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to set color
  server.on("/color", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    String r = request->getParam(red)->value();
    String g = request->getParam(green)->value();
    String b = request->getParam(blue)->value();
    rInt = r.toInt();
    gInt = g.toInt();
    bInt = b.toInt();
    Serial.println("Color set to: rgb(" + r + "," + g + "," + b + ")");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  //Route to set red
  server.on("/red", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    rInt = 255;
    gInt = 0;
    bInt = 0;
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  //Route to set green
  server.on("/green", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    rInt = 0;
    gInt = 128;
    bInt = 0;
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  //Route to set blue
  server.on("/blue", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    rInt = 0;
    gInt = 255;
    bInt = 255;
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  //Route to set purple
  server.on("/purple", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    rInt = 128;
    gInt = 0;
    bInt = 128;
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  //Route to set cyan
  server.on("/cyan", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    rInt = 0;
    gInt = 255;
    bInt = 255;
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  //Route to set yellow
  server.on("/yellow", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = 0;
    rInt = 255;
    gInt = 255;
    bInt = 0;
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Route to set pattern
  server.on("/pattern", HTTP_POST, [](AsyncWebServerRequest * request) {
    pattern = request->getParam(CHOICE)->value().toInt();
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.begin();
}

void loop() {
  switch (pattern) {
    case 0: break;
    case 1: rainbow();    break;
    case 2: lightning();  break;
    case 3: confetti();   break;
    case 4: twinkle();    break;
    case 5: sweep();      break;
    case 6: runPalette(LavaColors_p);    break;
    case 7: runPalette(CloudColors_p);   break;
    case 8: runPalette(PartyColors_p);   break;
    case 9: runPalette(OceanColors_p);   break;
    case 10: runPalette(ForestColors_p); break;
    case 11: runPalette(HeatColors_p);   break;
  }
}

void turnOn() {
  if (rInt == -1 || gInt == -1 || bInt == -1) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  }
}

void turnOff() {
  FastLED.clear();
  FastLED.show();
}

void rainbow() {
  for (int j = 0; j < 255; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(i - (j * 2), 255, 255);
    }
    FastLED.show();
    delay(25);
    if (pattern != 1) {
      return;
    }
  }
}

void lightning() {
  for (int flashCounter = 0; flashCounter < random8(3, FLASHES); flashCounter++) {
    if (flashCounter == 0) dimmer = 5;
    else dimmer = random8(1, 3);

    fill_solid(leds, NUM_LEDS, CHSV(255, 0, 255 / dimmer));
    FastLED.show();
    delay(random8(4, 10));
    fill_solid(leds, NUM_LEDS, CHSV(0, 0, 0));
    FastLED.show();

    if (flashCounter == 0) delay (150);
    delay(50 + random8(100));
    if (pattern != 2) {
      return;
    }
  }
  delay(random8(FREQUENCY) * 100);
}

void confetti() {
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  FastLED.show();
  gHue++;
}

void twinkle() {
  FastLED.clear();
  FastLED.setBrightness(200);
  addGlitter(50);
  FastLED.show();
  FastLED.delay(10);
}

void addGlitter( fract8 chanceOfGlitter) {
  if ( random8() < chanceOfGlitter) {
    delay(235);
    if (rInt == -1 || gInt == -1 || bInt == -1) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    } else {
      leds[ random16(NUM_LEDS) ] += CRGB(rInt, gInt, bInt);
    }
  }
}

void sweep() {
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  if (rInt == -1 || gInt == -1 || bInt == -1) {
    leds[pos] += CRGB::White;
  } else {
    leds[pos] += CRGB(rInt, gInt, bInt);
  }
  FastLED.show();
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t b = 255;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, b, currentBlending);
    colorIndex += 3;
  }
}

void runPalette(CRGBPalette16 palette) {
  currentPalette = palette;
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
  FillLEDsFromPaletteColors(startIndex);
  FastLED.show();
  FastLED.delay(1000 / 100);
}
