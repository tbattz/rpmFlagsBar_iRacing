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

// Set LED Timing Parameters
const unsigned int PIT_LIMITER_MS = 500;      // The time for one half cycle of the pit limiter. Full cycle = On, Off.
const unsigned int CHECKERED_FLAG_MS = 500;   // The time for one half cycle of flashing white/gray LEDs. Full cycle = On, Off.
const unsigned int RED_FLAG_MS = 250;         // The time for one half cycle of flashing red LEDs. Full cycle = On, Off.
const unsigned int YELLOW_FLAG_MS = 250;      // The time for one half cycle of flashing yellow LEDs. Full cycle = On, Off.
const unsigned int GREEN_FLAG_MS = 250;       // The time for one half cycle of flashing green LEDs. Full cycle = On, Off.
const unsigned int BLUE_FLAG_MS = 250;       // The time for one half cycle of flashing blue LEDs. Full cycle = On, Off.
const unsigned int WHITE_FLAG_MS = 250;       // The time for one half cycle of flashing white LEDs. Full cycle = On, Off.

// Set message variables
const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];     // Use while parsing array
int integerFromPC = 0;        // Holds the message format number
int rpmPer = 0;               // RPM Percentage
unsigned int commaCount = 0;
const char* delimCharS = ',';         // Must be single quotes, not double, this isn't python.
const char* delimCharD = ",";

// True for a complete message
boolean newData = false;

// Flag Timer Variables
unsigned long flagCycleEnd = 0; // End of the current cycle (either On cycle or off cycle)
int timedFlagId = 0;  // ID of the flag action
unsigned int currentFlagMS = 0; // The cycle timer difference for the current flag
boolean currentFlagState = false; // Whether the cycle is on or off



void setup() {
  Serial.begin(2000000);
  // TODO - Setup flashing lights on initialisation


  // Setup pin 13
  pinMode(LED_BUILTIN, OUTPUT);
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
      Serial.println("INACTIVE");
      // Turn off LEDs
      digitalWrite(LED_BUILTIN, LOW);
      break;
    }
    case 1: {
      startNewFlag(flagInt, PIT_LIMITER_MS);
      Serial.println("PIT_LIMITER");
      break;
    }
    case 2: {
      Serial.println("CHECKERED_FLAG");
      startNewFlag(flagInt, CHECKERED_FLAG_MS);
      break;
    }
    case 3: {
      Serial.println("RED_FLAG");
      startNewFlag(flagInt, RED_FLAG_MS);
      break;
    }
    case 4: {
      Serial.println("YELLOW_FLAG");
      startNewFlag(flagInt, YELLOW_FLAG_MS);
      break;
    }
    case 5: {
      Serial.println("GREEN_FLAG");
      startNewFlag(flagInt, GREEN_FLAG_MS);
      break;
    }
    case 6: {
      Serial.println("BLUE_FLAG");
      startNewFlag(flagInt, BLUE_FLAG_MS);
      break;
    }
    case 7: {
      Serial.println("WHITE_FLAG");
      startNewFlag(flagInt, WHITE_FLAG_MS);
      break;
    }
    default: {}
  }
}

void displayRpm(unsigned int currRpmPer) {
  Serial.print("RPM-Percentage: ");
  Serial.println(currRpmPer); 
  digitalWrite(LED_BUILTIN, HIGH);
}

void startNewFlag(unsigned int flagId, unsigned int flagMS) {
  // Set start of current flag
  timedFlagId = flagId;
  currentFlagMS = flagMS;
  flagCycleEnd = millis() + flagMS;
  currentFlagState = true;
  // Set LEDs On
  digitalWrite(LED_BUILTIN, HIGH);  
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
        if (currentFlagState) {
          digitalWrite(LED_BUILTIN, HIGH);
        } else {
          digitalWrite(LED_BUILTIN, LOW);
        }
      }    
    }
  }
}
