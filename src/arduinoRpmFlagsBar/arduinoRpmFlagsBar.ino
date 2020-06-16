// Serial Message Passing based on code from Example 5 here: https://forum.arduino.cc/index.php?topic=396450

// Message Formats
// <0> - INACTIVE                   - Turn off LEDs
// <1> - DISPLAY_PIT_LIMTIER        - Toggle Cyan LEDs off and on at PIT_LIMITER_MS
// <2> - DISPLAY_CHECKERED_FLAG     - Toggle White/Gray LEDs off and on at CHECKERED_FLAG_MS
// <3> - DISPLAY_RED_FLAG           - Toggle RED LEDs off and on at RED_FLAG_MS
// <4> - DISPLAY_YELLOW_FLAG        - Toggle Yellow LEDs off and on at YELLOW_FLAG_MS
// <5> - DISPLAY_GREEN_FLAG         - Toggle Green LEDs off and on at GREEN_FLAG_MS
// <6> - DISPLAY_BLUE_FLAG          - Toggle Blue LEDs off and on at BLUE_FLAG_MS
// <7> - DISPLAY_WHITE_FLAG         - Toggle White LEDs off and on at WHITE_FLAG_MS
// <8,rpmBarPercent> - DISPLAY_RPM  - Display a bar of LEDS from left to right, in GREEN, YELLOW, RED, with rpmBarPercent being the percentage of the width of the bar to be displayed. Range interger from 0 to 100. 0% doesn't correspond to 0 rpm, but rather the minium rpm for a LED to be turned on.

// Neopixel Library
#include <Adafruit_NeoPixel.h>

// Neopixel Settings
#define NEO_PIN 3           // The pwn signal pin the neopixels are connected to
#define LED_COUNT 17        // The number of LEDs in the strip
#define LED_BRIGHTNESS 10  // 0 to 255

// Create instance of Adafruit_NeoPixel class
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

// Set LED Timing Parameters
const unsigned int PIT_LIMITER_MS = 350;      // The time for one half cycle of the pit limiter. Full cycle = On, Off.
const unsigned int CHECKERED_FLAG_MS = 500;   // The time for one half cycle of flashing white/gray LEDs. Full cycle = On, Off.
const unsigned int RED_FLAG_MS = 250;         // The time for one half cycle of flashing red LEDs. Full cycle = On, Off.
const unsigned int YELLOW_FLAG_MS = 250;      // The time for one half cycle of flashing yellow LEDs. Full cycle = On, Off.
const unsigned int GREEN_FLAG_MS = 250;       // The time for one half cycle of flashing green LEDs. Full cycle = On, Off.
const unsigned int BLUE_FLAG_MS = 250;       // The time for one half cycle of flashing blue LEDs. Full cycle = On, Off.
const unsigned int WHITE_FLAG_MS = 250;       // The time for one half cycle of flashing white LEDs. Full cycle = On, Off.

// Set message variables
const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];       // Use while parsing array
bool firstDataReceived = false; // True once any data has been received
int integerFromPC = 0;          // Holds the message format number
int rpmPer = 0;                 // RPM Percentage
unsigned int commaCount = 0;
char delimCharS = ',';         // Must be single quotes, not double, this isn't python.
const char* delimCharD = ",";

// True for a complete message
boolean newData = false;

// Loading bar timers
unsigned long nextUpdate = 0;               // Time of the next update to the loading bar
unsigned long loadingUpdateDuration = 200;  // ms, time between updating the loading bar
unsigned int currLoadingPos = 0;            // Position of the last on pixel for the loading bar

// Flag Timer Variables
unsigned long flagCycleEnd = 0; // End of the current cycle (either On cycle or off cycle)
int timedFlagId = 0;  // ID of the flag action
unsigned int currentFlagMS = 0; // The cycle timer difference for the current flag
boolean currentFlagState = false; // Whether the cycle is on or off

// Flag Colours
uint32_t loadingBarColor = leds.Color(0, 255, 255);
uint32_t currColor = leds.Color(0, 0, 0);
uint32_t magentaColor = leds.Color(255, 0, 255);
uint32_t redColor = leds.Color(255, 0, 0);
uint32_t greenColor = leds.Color(0, 255, 0);
uint32_t yellowColor = leds.Color(255, 255, 0);
uint32_t blueColor = leds.Color(0, 0, 255);
uint32_t whiteColor = leds.Color(255, 255, 255);
uint32_t orangeColor = leds.Color(255, 127, 0);
uint32_t checkeredColor1 = leds.Color(255, 255, 255);
uint32_t checkeredColor2 = leds.Color(40, 40, 40);
uint32_t rpmColors[LED_COUNT];
uint32_t rpmColor1 = greenColor;
uint32_t rpmColor2 = orangeColor;
uint32_t rpmColor3 = redColor;



void setup() {
  // Begin serial communication
  Serial.begin(250000);
  
  // Start the LED Strip
  leds.begin();
  leds.setBrightness(LED_BRIGHTNESS);
  clearLEDs();    // Set all LEDs off to begin with
  leds.show();

  // Setup pin 13
  pinMode(LED_BUILTIN, OUTPUT);

  // Generate RPM Colors
  generateRpmColors();
}

void loop() {
  // Try and receive serial data and handle input
  recvDataWithMarkers();
  // Parse a fully received message
  if (newData == true) {
    strcpy(tempChars, receivedChars); // Copy to avoid losing parsed message
    // Parse the data
    parseData();
    // Reset message flag
    newData = false;
  }

  // Display loading bar if waiting for first data from serial connection
  if (!firstDataReceived) {
    displayLoadingBar();
  }

  // Check current flag states
  checkFlagState(timedFlagId);
}


void recvDataWithMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;  // Current in char index
  char startMarker = '<';
  char endMarker = '>';
  char rc;              // Received char

  while (Serial.available() > 0 && newData == false) {
    // Try and read data
    rc = Serial.read();

    // Start new message if receiving not already in progress
    if (recvInProgress == true) {
      if (rc != endMarker) {
        // Do not have the end of the message
        receivedChars[ndx] = rc;
        ndx++;
        // Write over last character if we overfill out message buffer
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        // Received end of message
        receivedChars[ndx] = '\0'; // Terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;           // Received a complete message
        firstDataReceived = true;
      }
    } else if (rc == startMarker) {
      // Received start of message
      recvInProgress = true;
    }
  }
}


void parseData() {
  // Find the number of commas in the array
  commaCount = 0;
  for(unsigned int i=0; i < numChars; i++) {
    if (tempChars[i] == delimCharS) {
      commaCount++;
    }
  }
  // Split the received string by commas
  if (commaCount > 0) {
    char * strtokIndx;                           // used by strtok() as an index
    strtokIndx = strtok(tempChars, delimCharD);  // Gets the first part of the string
    integerFromPC = atoi(strtokIndx);            // Save the message format id
    // Save the second argument
    strtokIndx = strtok(NULL, delimCharD);       // Continues where previous call left off
    if (integerFromPC == 8) {
      rpmPer = atoi(strtokIndx);  
      displayRpm(rpmPer);
    }
  } else {
    // Single command integer
    integerFromPC = atoi(tempChars);
    handleFlags(integerFromPC);
  }
  
}

void handleFlags(int flagInt) {
  switch(flagInt) {
    case 0: {
      //Serial.println("INACTIVE");
      // Turn off LEDs
      digitalWrite(LED_BUILTIN, LOW);
      clearLEDs();
      break;
    }
    case 1: {
      startNewFlag(flagInt, PIT_LIMITER_MS);
      //Serial.println("PIT_LIMITER");
      currColor = magentaColor;
      break;
    }
    case 2: {
      //Serial.println("CHECKERED_FLAG");
      startNewFlag(flagInt, CHECKERED_FLAG_MS);
      break;
    }
    case 3: {
      //Serial.println("RED_FLAG");
      startNewFlag(flagInt, RED_FLAG_MS);
      currColor = redColor;
      break;
    }
    case 4: {
      //Serial.println("YELLOW_FLAG");
      startNewFlag(flagInt, YELLOW_FLAG_MS);
      currColor = yellowColor;
      break;
    }
    case 5: {
      //Serial.println("GREEN_FLAG");
      startNewFlag(flagInt, GREEN_FLAG_MS);
      currColor = greenColor;
      break;
    }
    case 6: {
      //Serial.println("BLUE_FLAG");
      startNewFlag(flagInt, BLUE_FLAG_MS);
      currColor = blueColor;
      break;
    }
    case 7: {
      //Serial.println("WHITE_FLAG");
      startNewFlag(flagInt, WHITE_FLAG_MS);
      currColor = whiteColor;
      break;
    }
    default: {}
  }
}

void startNewFlag(unsigned int flagId, unsigned int flagMS) {
  // Set start of current flag
  timedFlagId = flagId;
  currentFlagMS = flagMS;
  flagCycleEnd = millis() + flagMS;
  currentFlagState = false; // Let checkFlagState start the first flag
  // Set LEDs On
  checkFlagState(timedFlagId);
}

void checkFlagState(unsigned int currentFlag) {
  // Check we are still on the current flag
  if (currentFlag !=0 && currentFlag !=8) {
    if (currentFlag == integerFromPC) {
      if (millis() > flagCycleEnd) {
        // Toggle current cycle
        currentFlagState = !currentFlagState;
        // Update timers
        flagCycleEnd = flagCycleEnd + currentFlagMS;
        // Update LED State
        if (currentFlag == 2) {
          // Checkered Flag
          displayCheckered(currentFlagState);
        } else {
          if (currentFlagState) {
            //digitalWrite(LED_BUILTIN, HIGH);
            displayAllOneColor(currColor);
          } else {
            //digitalWrite(LED_BUILTIN, LOW);
            clearLEDs();
          }
        }
      }    
    }
  }
}

void clearLEDs() {
  for(unsigned int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
  leds.show();
}

void generateRpmColors() {
  // Find middle pixel
  float middlePixel = (float)LED_COUNT / 2.0; // Less than this (for both even and odd LED_COUNT) determines which color scale to use
  // Set rgb references
  // Color 1
  uint8_t r1 = rpmColor1 >> 16;
  uint8_t g1 = rpmColor1 >> 8;
  uint8_t b1 = rpmColor1;
  // Color 2
  uint8_t r2 = rpmColor2 >> 16;
  uint8_t g2 = rpmColor2 >> 8;
  uint8_t b2 = rpmColor2;
  // Color 3
  uint8_t r3 = rpmColor3 >> 16;
  uint8_t g3 = rpmColor3 >> 8;
  uint8_t b3 = rpmColor3;

  // Calculate lengths of each section
  float lenn = middlePixel; // (middlePixel - 0)
  // Calculate gradients and intercept
  // Section 1
  float m1R = (r2 - r1) / lenn;
  float m1G = (g2 - g1) / lenn;
  float m1B = (b2 - b1) / lenn;
  float c1R = r1;
  float c1G = g1;
  float c1B = b1;
  // Section 2
  float m2R = (r3 - r2) / lenn;
  float m2G = (g3 - g2) / lenn;
  float m2B = (b3 - b2) / lenn;
  float c2R = r2 - (m2R*lenn);
  float c2G = g2 - (m2G*lenn);
  float c2B = b2 - (m2B*lenn); 
  
  for(unsigned int i=0; i < LED_COUNT; i++) {
    if (i < middlePixel) {
      // Calculate RGB for this point on the strip
      uint8_t newRed = (m1R*i) + c1R;
      uint8_t newGreen = (m1G*i) + c1G;
      uint8_t newBlue = (m1B*i) + c1B;
      // Store color
      rpmColors[i] = leds.Color(newRed, newGreen, newBlue);
    } else {
      // Calculate RGB for this point on the strip
      uint8_t newRed = (m2R*i) + c2R;
      uint8_t newGreen = (m2G*i) + c2G;
      uint8_t newBlue = (m2B*i) + c2B;
      // Store color
      rpmColors[i] = leds.Color(newRed, newGreen, newBlue);
    }
  }
}

void displayLoadingBar() {
  if (millis() > nextUpdate) {
    // Set the next update
    nextUpdate = millis() + loadingUpdateDuration;
    // Increment position
    currLoadingPos += 1;
    if (currLoadingPos > LED_COUNT - 1) {
      currLoadingPos = 0;
      for(unsigned int i=0; i < LED_COUNT; i++) {
        leds.setPixelColor(i, 0);
      }
    }
    // Set pixels
    leds.setPixelColor(currLoadingPos, loadingBarColor);
    
    leds.show();
  }
}

void displayAllOneColor(uint32_t color2Show) {
  for(unsigned int i=0; i<LED_COUNT; i++) {
    leds.setPixelColor(i, color2Show);
  }
  leds.show();
}

void displayRpm(unsigned int currRpmPer) {
  //Serial.print("RPM-Percentage: ");
  //Serial.println(currRpmPer); 
  //digitalWrite(LED_BUILTIN, HIGH);
  // Determine which LEDs should be shown
  unsigned int lastLedOn = round(currRpmPer * LED_COUNT / 100.0);
  
  // Update led colors
  for(unsigned int i=0; i < lastLedOn + 1; i++) {
    leds.setPixelColor(i, rpmColors[i]);
  }
  for(unsigned int i=lastLedOn; i < LED_COUNT; i++) {
    leds.setPixelColor(i, 0);
  }
  leds.show();
}

void displayCheckered(bool cycleOne) {
  if (cycleOne) {
    // Every even value is white
    for(unsigned int i=0; i < LED_COUNT; i+=2) {
      leds.setPixelColor(i, checkeredColor1);
    }
    for (unsigned int i=1; i < LED_COUNT; i+=2) {
      leds.setPixelColor(i, checkeredColor2);
    }
  } else {
    // Every odd value is white
    for(unsigned int i=0; i < LED_COUNT; i+=2) {
      leds.setPixelColor(i, checkeredColor2);
    }
    for (unsigned int i=1; i < LED_COUNT; i+=2) {
      leds.setPixelColor(i, checkeredColor1);
    }
  }
  leds.show();
}
