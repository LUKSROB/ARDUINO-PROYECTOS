// Adafruit_ImageReader test for Adafruit ILI9341 TFT Shield for Arduino.
// Demonstrates loading images from SD card or flash memory to the screen,
// to RAM, and how to query image file dimensions.
// Requires three BMP files in root directory of SD card:
// purple.bmp, parrot.bmp and wales.bmp.
// As written, this uses the microcontroller's SPI interface for the screen
// (not 'bitbang') and must be wired to specific pins (e.g. for Arduino Uno,
// MOSI = pin 11, MISO = 12, SCK = 13). Other pins are configurable below.

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

// Comment out the next line to load from SPI/QSPI flash instead of SD card:
#define USE_SD_CARD

// TFT display and SD card share the hardware SPI interface, using
// 'select' pins for each to identify the active device on the bus.

#define SD_CS   4 // SD card select pin
#define TFT_CS 10 // TFT select pin
#define TFT_DC  8 // TFT display/command pin
#define TFT_RST 9 // TFT reset pin

#define B1 2 // define buttons for images
#define B2 3
#define B3 5
#define B4 6
#define B5 7

ImageReturnCode stat; // Status from image-reading functions

#if defined(USE_SD_CARD)
  SdFat                SD;         // SD card filesystem
  Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
#else
  // SPI or QSPI flash filesystem (i.e. CIRCUITPY drive)
  #if defined(__SAMD51__) || defined(NRF52840_XXAA)
    Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
      PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
  #else
    #if (SPI_INTERFACES_COUNT == 1)
      Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
    #else
      Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
    #endif
  #endif
  Adafruit_SPIFlash    flash(&flashTransport);
  FatFileSystem        filesys;
  Adafruit_ImageReader reader(filesys); // Image-reader, pass in flash filesys
#endif

Adafruit_ILI9341     tft    = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;

void setup(void) {

  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);
  pinMode(B5, INPUT);

  Serial.begin(9600);
#if !defined(ESP32)
  while(!Serial);       // Wait for Serial Monitor before continuing
#endif

  tft.begin();          // Initialize screen

  // The Adafruit_ImageReader constructor call (above, before setup())
  // accepts an uninitialized SdFat or FatFileSystem object. This MUST
  // BE INITIALIZED before using any of the image reader functions!
  Serial.print(F("Initializing filesystem..."));

#if defined(USE_SD_CARD)
  // SD card is pretty straightforward, a single call...
  if(!SD.begin(SD_CS, SD_SCK_MHZ(25))) { // ESP32 requires 25 MHz limit
    Serial.print(F("SD begin() failed"));
    delay(500);
    }
#else
  // SPI or QSPI flash requires two steps, one to access the bare flash
  // memory itself, then the second to access the filesystem within...
  if(!flash.begin()) {
    Serial.print(F("flash begin() failed"));
    delay(500);
  }
  if(!filesys.begin(&flash)) {
    Serial.print(F("filesys begin() failed"));
    delay(500);
  }
  
#endif

  Serial.println(F("OK!"));

  tft.fillScreen(ILI9341_BLUE);
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  // Fill screen blue. Not a required step, this just shows that we're
  // successfully communicating with the screen.

  }

void loop() {

  if(digitalRead(B1) == HIGH){
    Serial.print(F("  Loading google.bmp to screen..."));
    stat = reader.drawBMP("/google.bmp", tft, 0, 0);
    reader.printStatus(stat);   // How'd we do?
    Serial.println("  ");
    delay(1000);
    tft.fillScreen(ILI9341_BLACK);
  }

  if(digitalRead(B2) == HIGH){
    Serial.print(F("  Loading mario.bmp to screen..."));
    stat = reader.drawBMP("/mario.bmp", tft, 0, 0);
    reader.printStatus(stat);   // How'd we do?
    Serial.println("  ");
    delay(500);
    tft.fillScreen(ILI9341_BLACK);
  }

  if(digitalRead(B3) == HIGH){
    Serial.print(F("  Loading minecraft.bmp to screen..."));
    stat = reader.drawBMP("/minecraft.bmp", tft, 0, 0);
    reader.printStatus(stat);   // How'd we do?
    Serial.println("  ");
    delay(2000);
    tft.fillScreen(ILI9341_BLACK);
  }

  if(digitalRead(B4) == HIGH){
    Serial.print(F("  Loading pacman.bmp to screen..."));
    stat = reader.drawBMP("/pacman.bmp", tft, 0, 0);
    reader.printStatus(stat);   // How'd we do?
    Serial.println("  ");
    tft.fillScreen(ILI9341_BLACK);
  }

  if(digitalRead(B5) == HIGH){
    Serial.print(F("  Loading purple.bmp to screen..."));
    stat = reader.drawBMP("/purple.bmp", tft, 0, 0);
    reader.printStatus(stat);   // How'd we do?
    Serial.println("  ");
  }

}