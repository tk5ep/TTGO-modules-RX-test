/*
Small software to test the RX sensitivity of the receiver in a Lilygo module

written by TK5EP dec 2023
https://egloff.eu
https://github.com/tk5ep
*/

/***************************************
 ____       _   _   _                 
/ ___|  ___| |_| |_(_)_ __   __ _ ___ 
\___ \ / _ \ __| __| | '_ \ / _` / __|
 ___) |  __/ |_| |_| | | | | (_| \__ \
|____/ \___|\__|\__|_|_| |_|\__, |___/
                            |___/ 
****************************************/
#define Lilygo_Tbeam               // LilyGo T-beam board. Comment out for this board
//#define Lilygo_T3                 // LilyGo T3 board. Comment out for this board
//#define ssd1306                   // defines the OLED driver type comment if using SSD1306 driver. Comment if 1.3" inch uses SH1106
double RXfreq = 433.775;            // frequency in MHz

/*****************************************
  ____             __ _       
 / ___|___  _ __  / _(_) __ _ 
| |   / _ \| '_ \| |_| |/ _` |
| |__| (_) | | | |  _| | (_| |
 \____\___/|_| |_|_| |_|\__, |
                        |___/ 
*****************************************/
#include <RadioLib.h>

String SOFTWARE_DATE = "16.12.23";

#ifdef ssd1306
  #include <Adafruit_SSD1306.h>
#else
  #include <Adafruit_SH110X.h>
#endif

// PINs mapping
// Lilygo modules have same pining, given for example if other modules 
// Lilygo T-beam
#ifdef Lilygo_Tbeam
  // T-beam pins
  // 0.96" OLED with SSD1306. 1.3" with SSD110X
  //#define I2C_SDA         21
  //#define I2C_SCL         22
  #define OLED_RST -1       // shared pin. pin 16 crashes
  // LoRa radio
  #define LORA_SCK        5
  #define LORA_MISO       19
  #define LORA_MOSI       27
  #define LORA_SS         18
  #define LORA_DIO0       26
  #define LORA_DIO1       33  // pin LoRa1 on header not connected to IO pin
  //#define LORA_DIO2       32  // pin LoRa2 on header
  #define LORA_RST        23
#endif

// Lilygo_T3
#ifdef Lilygo_T3
  // Lilygo-T3 pins
  // I2C OLED Display works with SSD1306 driver
  //#define OLED_SDA     21
  //#define OLED_SCL     22
  #define OLED_RST      -1
  // SPI LoRa Radio
  #define LORA_SCK      5
  #define LORA_MISO     19
  #define LORA_MOSI     27
  #define LORA_SS       18
  #define LORA_DIO0     26
  #define LORA_DIO1     33  // pin LoRa1 on header not connected to IO pin
  //#define LORA_DIO2     32  // pin LoRa2 on header
  #define LORA_RST      23
#endif

// OLED line variables
String line1 = "";
String line2 = "";
String line3 = "";
String line4 = "";
String line5 = "";
String line6 = "";

// DISPLAY SSD1306
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// instance for OLED
#ifdef ssd1306
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
#else
  Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
#endif

// instance for LoRa module
SX1278 radio = new Module(LORA_SS, LORA_DIO0, LORA_RST, LORA_DIO1);

/*****************************
 _____      _               
/  ___|    | |              
\ `--.  ___| |_ _   _ _ __  
 `--. \/ _ \ __| | | | '_ \ 
/\__/ /  __/ |_| |_| | |_) |
\____/ \___|\__|\__,_| .__/ 
                     | |    
                     |_|    
******************************/
void setup() {
  Serial.begin(115200);
  // init OLED
  #ifdef ssd1306
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  #else
    display.begin(0x3c, true); // Address 0x3C default
  #endif

  // OLED setting
  display.clearDisplay();
    #ifdef ssd1306
    display.setTextColor(WHITE);
  #else
    display.setTextColor(SH110X_WHITE);
  #endif
  // boot info page on OLED
  display.setTextSize(2);
  display.clearDisplay();             // mandatory to remove Adafruit splash
  #ifdef Lilygo_T3
    line1 = "T3 RX";
  #endif
  #ifdef Lilygo_Tbeam
    line1 = "Tbeam RX";
  #endif

  line2 = "Receiver test";
  line3 = "Displays the RSSI";
  line4 = "by TK5EP v" + SOFTWARE_DATE;
  display.setCursor(0, 0);
  display.print(line1);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(line2);
  display.setCursor(0,30);
  display.print(line3);
  display.setCursor(0,56);
  display.print(line4);
  display.display();
  delay(5000);

  display.clearDisplay();
  display.setTextSize(1);
  
  // initialize SX1278 with default settings
  Serial.print(F("[SX1278] Initializing ... "));
  display.setCursor(0, 0);
  line1 = "SX1278 initializing...";
  display.print(line1);
  display.display();

  int state = radio.beginFSK(RXfreq);

  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
    display.setCursor(0, 10);
    line2="success";
    display.print(line2);
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    display.setCursor(0, 10);
    line2="failed code " + state ;
    while(true);
  }
  display.display();

  delay(3000);
  display.clearDisplay();
}

/*****************************
 _                       
| |                      
| |     ___   ___  _ __  
| |    / _ \ / _ \| '_ \ 
| |___| (_) | (_) | |_) |
\_____/\___/ \___/| .__/ 
                  | |    
                  |_|    
*****************************/
void loop() {
    line1="RSSI";
    line2 = String(radio.getRSSI());
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(line1);
    display.setCursor(0, 30);
    display.print(line2);
    display.display();

    // print the RSSI (Received Signal Strength Indicator)
    Serial.print(F("[SX1278] RSSI:\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    delay(500);
    display.clearDisplay();
}
