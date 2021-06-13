#if !( defined(ESP8266) )
#error This code is intended to run on ESP8266 platform! Please check your Tools->Board setting.
#endif

//Include Libraries
#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
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
String nameString = "LEDCloud";
int pattern;
bool power = false;
int rInt = -1;
int gInt = -1;
int bInt = -1;
uint8_t gHue = 0;
int brightness;

//Parameters
const char* BRIGHT = "amount";
const char* red = "red";
const char* green = "green";
const char* blue = "blue";
const char* CHOICE = "choice";

ESP8266WebServer server(80);
ESP_WiFiManager wifiManager("LEDCloud");
ESP8266HTTPUpdateServer httpUpdateServer;

//Setup ESP
void setup() {
  Serial.begin(115200);
  currentBlending = LINEARBLEND;

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //wifiManager.resetSettings(); testing purposes
  //Create wifi manager instance on LEDCloud
  wifiManager.autoConnect("LEDCloud");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("Connected. Local IP: "));
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println(wifiManager.getStatus(WiFi.status()));
  }

  //Setup LED Strip
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  turnOff();

  // Route to turn strip on
  server.on("/on", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("on");
    turnOn();
  });

  // Route to turn strip off
  server.on("/off", HTTP_POST, []() {
    pattern = -1;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("off");
    turnOff();
  });

  // Route to set brightness
  server.on("/brightness", HTTP_POST, []() {
    int amount = server.arg(BRIGHT).toInt();
    brightness = amount * 2.55;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendInt(brightness);
    FastLED.setBrightness(brightness);
    FastLED.show();
  });

  // Route to set color
  server.on("/color", HTTP_POST, []() {
    pattern = 0;
    String r = server.arg(red);
    String g = server.arg(green);
    String b = server.arg(blue);
    rInt = r.toInt();
    gInt = g.toInt();
    bInt = b.toInt();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendInt(rInt);
    Serial.println("Color set to: rgb(" + r + "," + g + "," + b + ")");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  //Route to set red
  server.on("/red", HTTP_POST, []() {
    pattern = 0;
    rInt = 255;
    gInt = 0;
    bInt = 0;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("red");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  //Route to set green
  server.on("/green", HTTP_POST, []() {
    pattern = 0;
    rInt = 0;
    gInt = 128;
    bInt = 0;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("green");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  //Route to set blue
  server.on("/blue", HTTP_POST, []() {
    pattern = 0;
    rInt = 0;
    gInt = 255;
    bInt = 255;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("blue");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  //Route to set purple
  server.on("/purple", HTTP_POST, []() {
    pattern = 0;
    rInt = 128;
    gInt = 0;
    bInt = 128;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("purple");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  //Route to set cyan
  server.on("/cyan", HTTP_POST, []() {
    pattern = 0;
    rInt = 0;
    gInt = 255;
    bInt = 255;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("cyan");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  //Route to set yellow
  server.on("/yellow", HTTP_POST, []() {
    pattern = 0;
    rInt = 255;
    gInt = 255;
    bInt = 0;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendString("yellow");
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  });

  // Route to set pattern
  server.on("/pattern", HTTP_POST, []() {
    pattern = server.arg(CHOICE).toInt();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    sendInt(pattern);
  });

  httpUpdateServer.setup(&server);

  server.serveStatic("/", SPIFFS, "/", "max-age=86400");

  MDNS.begin(nameString);
  server.begin();
}

//Function for sending int to server
void sendInt(uint8_t value)
{
  sendString(String(value));
}

//Function for sending string to server
void sendString(String value)
{
  server.send(200, "text/plain", value);
}

void loop() {
  server.handleClient();
  MDNS.update();
  //Wait to connect
  static bool hasConnected = false;
  if (WiFi.status() != WL_CONNECTED) {
    hasConnected = false;
  }
  else if (!hasConnected) {
    hasConnected = true;
    MDNS.begin(nameString);
    MDNS.setHostname(nameString);
    server.begin();
    Serial.println("HTTP web server started");
    Serial.print("Connected! Open http://");
    Serial.print(WiFi.localIP());
    Serial.print(" or http://");
    Serial.print(nameString);
    Serial.println(".local in your browser");
  }
  //Switch statement for pattern
  switch (pattern) {
    case 0: break;
    case 1: runPalette(RainbowStripeColors_p); break;
    case 2: confetti();   break;
    case 3: twinkle();    break;
    case 4: sweep();      break;
    case 5: runPalette(LavaColors_p);    break;
    case 6: runPalette(CloudColors_p);   break;
    case 7: runPalette(PartyColors_p);   break;
    case 8: runPalette(OceanColors_p);   break;
    case 9: runPalette(ForestColors_p); break;
    case 10: runPalette(HeatColors_p);   break;
  }
}

//Turn on LED Strip
void turnOn() {
  if (rInt == -1 || gInt == -1 || bInt == -1) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(rInt, gInt, bInt));
    FastLED.show();
  }
}

//Turn off LED Strip
void turnOff() {
  FastLED.clear();
  FastLED.show();
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
