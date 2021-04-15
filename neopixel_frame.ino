/****************************************************************
  Neopixel Frame
  Tim Pozar
  pozar@LNS.com
  A bunch of code lifted from various Adafruit and other examples.
****************************************************************/

/*********
  WAP and Web server code taken from:
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

/*********
  Control a RGB led with Hue, Saturation and Brightness (HSB / HSV )
  https://www.kasperkamperman.com/blog/arduino/arduino-programming-hsb-to-rgb/
  Hue is change by an analog input. 
  Brightness is changed by a fading function. 
  Saturation stays constant at 255

  getRGB() function based on <http://www.codeproject.com/miscctrl/CPicker.asp>  
  dim_curve idea by Jims

  created 05-01-2010 by kasperkamperman.com
*********/

#define REV   "20210414.01"

// Neopixel includes
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

// Rangefinder
#include <Adafruit_VL53L0X.h>
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// For the Neopixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};
// getRGB function stores RGB values in this array
// use these values for the red, blue, green led. 
int rgb_colors[3]; 
int fadeVal   = 0; // value that changes between 0-255
int fadeSpeed = 4; // 'speed' of fading
int hue;
int saturation;
int brightness;
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
// Many times this is pin 6 but the OLED uses that for one of its buttons.
// #define LED_PIN    6
#define LED_PIN    13
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  190
// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 96 // Set BRIGHTNESS to about 1/4 (max = 255)
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// for the OLED...
// #include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
#define BUTTON_A  0
#define BUTTON_B 16
#define BUTTON_C  2
#elif defined(ESP32)
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
#define BUTTON_A PA15
#define BUTTON_B PC7
#define BUTTON_C PC5
#elif defined(TEENSYDUINO)
#define BUTTON_A  4
#define BUTTON_B  3
#define BUTTON_C  8
#elif defined(ARDUINO_FEATHER52832)
#define BUTTON_A 31
#define BUTTON_B 30
#define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840 and 328p
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#endif

#define LOGO_HEIGHT   32
#define LOGO_WIDTH    32
static const unsigned char PROGMEM lns_logo_bmp[] = {
  B11111111, B11111111, B11111111, B11111111,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00011001,
  B10000000, B00000001, B10000000, B00001101,
  B10010000, B00000001, B10000000, B00001111,
  B10000000, B00000001, B10000000, B00001111,
  B10000000, B00000001, B10000000, B00011111,
  B10000000, B00000001, B10000000, B00111101,
  B10000000, B00001001, B10000000, B11111001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10001000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B11111111, B11111111, B11111111, B11111111,
  B11111111, B11111111, B11111111, B11111111,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00010001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10100000, B00000001,
  B10001000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B01000001,
  B10000000, B00000001, B10000000, B00000001,
  B10000000, B00000001, B10000000, B00000001,
  B11111111, B11111111, B11111111, B11111111
};

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1

// Load Wi-Fi library
#include <WiFi.h>
// Set your Static IP address
IPAddress local_IP(192, 168, 4, 1);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

// Replace with your network credentials
const char* ssid     = "KimsFrame";
const char* password = "perkypenguins";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

void showBanner() {
  // Show logo on the display hardware.
  clearScreen();
  display.drawBitmap( 0, 0, lns_logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
  display.setCursor(35, 0);
  display.print("NeoPixel Frame");
  display.setCursor(35, 8);
  display.print("Rev:");
  display.print(REV);
  display.setCursor(35, 16);
  display.print("LNS.com");
  display.setCursor(35, 24);
  display.print("Tim Pozar");
  display.display();
  delay(5000);
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void printMacAddress(byte mac[]) {
  clearScreen();
  updateScreenStats();
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
      display.print("0");
    }
    Serial.print(mac[i], HEX);
    display.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
      display.print(":");
    }
  }
  Serial.println();
  display.display();
}

void clearScreen() {
  // Clear the buffer.
  display.clearDisplay();
  display.display(); // display a blank screen
  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0); // put the cursor at the upper left.
}

void updateScreenStats() {
  display.print("SSID: ");
  display.println(ssid);
  display.print("Pass: ");
  display.println(password);
  display.print("http://");
  // IPAddress IP = WiFi.localIP();
  // display.println(IP);
  display.println(WiFi.localIP());
  // display.print("Listening");
  display.display(); // display all of the above and leave the cursor at the start of the fourth line for a future message
}

void getRGB(int hue, int sat, int val, int colors[3]) { 
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.      
  */

  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];

  int r;
  int g;
  int b;
  int base;

  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 

    base = ((255 - sat) * val)>>8;

    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;

    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;

    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;

    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;

    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;

    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }

    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }   
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void setup() {
  Serial.begin(115200);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");
  showBanner();
  Serial.println("IO test");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi network with SSID and password
  Serial.println("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
  clearScreen();
  updateScreenStats();
  display.print("Listening");
  display.display();
  // wait 5 seconds for connection:
  delay(5000);
}

void loop() {
  int white;
  int range_mm;
  
  VL53L0X_RangingMeasurementData_t measure;
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
    range_mm = measure.RangeMilliMeter;
    white = 0;
  } else {
    Serial.println(" out of range ");
    range_mm = 1;
    white = BRIGHTNESS;
  }

  // fade from 0 - 255 and back with a certain speed   
  // fadeVal = fadeVal + fadeSpeed;         // change fadeVal by speed
  // fadeVal = constrain(fadeVal, 128, 255);  // keep fadeVal between 128 and 255
  // if(fadeVal==255 || fadeVal==128)         // change from up>down or down-up (negative/positive)
  // { fadeSpeed = -fadeSpeed;  
  // }
  // set HSB values
  hue        = map(range_mm,0,750,0,359);         // hue is a number between 0 and 360
  saturation = 255;                               // saturation is a number between 0 - 255
  // brightness = fadeVal;                           // value is a number between 128 - 255
  brightness = BRIGHTNESS;
  // Be nice to have a increase/decrease brightness function here.
  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
  if (white > 0) {
    rgb_colors[0] = 0;
    rgb_colors[1] = 0;
    rgb_colors[2] = 0;
  }
  Serial.print("rgbw_colors: ");
  Serial.print(rgb_colors[0]);
  Serial.print(",");
  Serial.print(rgb_colors[1]);
  Serial.print(",");
  Serial.print(rgb_colors[2]);
  Serial.print(",");
  Serial.println(white);
  Serial.print("Brightness: ");
  Serial.println(brightness);
  colorWipe(strip.Color(rgb_colors[0], rgb_colors[1], rgb_colors[2], white), 1);
  
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /purple/on") >= 0) {
              Serial.println("Purple on");
              colorWipe(strip.Color(BRIGHTNESS, 0, BRIGHTNESS, 0), 1);
              delay(10000);
            } else if (header.indexOf("GET /red/on") >= 0) {
              Serial.println("Red on");
              colorWipe(strip.Color(BRIGHTNESS, 0, 0, 0), 1);
              delay(10000);
            } else if (header.indexOf("GET /blue/on") >= 0) {
              Serial.println("Blue on");
              colorWipe(strip.Color(0, 0, BRIGHTNESS, 0), 1);
              delay(10000);
            } else if (header.indexOf("GET /green/on") >= 0) {
              Serial.println("Green on");
              colorWipe(strip.Color(0, BRIGHTNESS, 0, 0), 1);
              delay(10000);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Kim's Neopixel Frame</h1>");

            client.println("<p>Purple</p>");
            client.println("<p><a href=\"/purple/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Red</p>");
            client.println("<p><a href=\"/red/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Blue</p>");
            client.println("<p><a href=\"/blue/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<p>Green</p>");
            client.println("<p><a href=\"/green/on\"><button class=\"button\">ON</button></a></p>");

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

