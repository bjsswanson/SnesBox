/*
  SNES Controller interface
  Based on NES Controller by Sebastian Tomczak
 
  Pinouts, clock times, communication protocol, etc...:
  http://www.gamefaqs.com/snes/916396-snes/faqs/5395
*/
 
// Controller buttons
// (based on button-to-clock pulse assignment)
#define SNES_B        32768  // 1000000000000000
#define SNES_Y        16384  // 0100000000000000
#define SNES_SELECT   8192   // 0010000000000000
#define SNES_START    4096   // 0001000000000000
#define SNES_UP       2048   // 0000100000000000
#define SNES_DOWN     1024   // 0000010000000000
#define SNES_LEFT      512   // 0000001000000000
#define SNES_RIGHT     256   // 0000000100000000
#define SNES_A         128   // 0000000010000000
#define SNES_X          64   // 0000000001000000
#define SNES_L          32   // 0000000000100000
#define SNES_R          16   // 0000000000010000
 
// Arduino pins vs. SNES controller pins
// (default is latch 2, clock 3, data 4)
int pOneLatchPin  = 2; // Latch
int pOneClockPin  = 3; // Clock
int pOneDataPin   = 4; // Serial Data

int pTwoLatchPin = 10;
int pTwoClockPin = 11;
int pTwoDataPin  = 12;
 
// Current controller data
unsigned int playerOneData = 0;
unsigned int playerTwoData = 0;

unsigned long startExitButtons = 0;
boolean exitButtonsDown = false;

// Setup the controller abwxyand serial output
void setup() {
  Serial.begin(57600);
  
  setupController(pOneLatchPin, pOneClockPin, pOneDataPin);
  setupController(pTwoLatchPin, pTwoClockPin, pTwoDataPin);
  
  Keyboard.begin();
}

void setupController(int latchPin, int clockPin, int dataPin){
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);
  
  digitalWrite(latchPin, HIGH);
  digitalWrite(clockPin, HIGH);
} 
 
// Read controller
void controllerRead() {
  // Reset controller states and data
  playerOneData = 0;
  playerTwoData = 0;
  
  digitalWrite(pOneLatchPin, LOW);
  digitalWrite(pOneClockPin, HIGH);
  
  digitalWrite(pTwoLatchPin, LOW);
  digitalWrite(pTwoClockPin, HIGH);
 
  // Controller needs to latch the state of all buttons
  digitalWrite(pOneLatchPin, HIGH);
  digitalWrite(pTwoLatchPin, HIGH);
  
  delayMicroseconds(12);
  
  digitalWrite(pOneLatchPin, LOW);
  digitalWrite(pTwoLatchPin, LOW);
  
  delayMicroseconds(6);
 
  // Read controller data (initial reading)
  playerOneData = digitalRead(pOneDataPin);
  playerTwoData = digitalRead(pTwoDataPin);
 
  // Send 16 clock pulses, one for each button. 
  for (int i = 0; i < 16; i ++) {
    digitalWrite(pOneClockPin, LOW);
    digitalWrite(pTwoClockPin, LOW);
    
    delayMicroseconds(6);
    
    playerOneData = playerOneData << 1;
    playerTwoData = playerTwoData << 1;
    
    playerOneData = playerOneData + digitalRead(pOneDataPin) ;
    playerTwoData = playerTwoData + digitalRead(pTwoDataPin) ;
    
    delayMicroseconds(6);
    
    digitalWrite(pOneClockPin, HIGH);
    digitalWrite(pTwoClockPin, HIGH);
  }
 
  // Do a NOT, so '1' will be pressed buttons and '0' to the rest
  playerOneData = ~playerOneData;
  playerTwoData = ~playerTwoData;
}
 
// Program code
void loop() {
  // Read controller data
  controllerRead();
  // Print binary information
  //Serial.println(~playerOneData, BIN);
 
  //Print user-friendly information
  
   handleKey(playerOneData, SNES_A, 'x');
   handleKey(playerOneData, SNES_B, 'z');
   handleKey(playerOneData, SNES_Y, 'a');  
   handleKey(playerOneData, SNES_X, 's');
   handleKey(playerOneData, SNES_START, KEY_RETURN);
   handleKey(playerOneData, SNES_SELECT, KEY_RIGHT_SHIFT);
   handleKey(playerOneData, SNES_L, 'q');
   handleKey(playerOneData, SNES_R, 'w');
   handleKey(playerOneData, SNES_LEFT, KEY_LEFT_ARROW);
   handleKey(playerOneData, SNES_RIGHT, KEY_RIGHT_ARROW);
   handleKey(playerOneData, SNES_UP, KEY_UP_ARROW);
   handleKey(playerOneData, SNES_DOWN, KEY_DOWN_ARROW);
 
   handleKey(playerTwoData, SNES_A, 'v');
   handleKey(playerTwoData, SNES_B, 'c');
   handleKey(playerTwoData, SNES_Y, 'd');
   handleKey(playerTwoData, SNES_X, 'f');
   handleKey(playerTwoData, SNES_START, 'o');
   handleKey(playerTwoData, SNES_SELECT, 'p');
   handleKey(playerTwoData, SNES_L, 'e');
   handleKey(playerTwoData, SNES_R, 'r');
   handleKey(playerTwoData, SNES_LEFT, 'j');
   handleKey(playerTwoData, SNES_RIGHT, 'l');
   handleKey(playerTwoData, SNES_UP, 'i');
   handleKey(playerTwoData, SNES_DOWN, 'k');
   
   handleEscape(playerOneData);
   handleEscape(playerTwoData);
   
   //delay(10);
}

void handleEscape(unsigned int playerData) {
  if((playerData & SNES_SELECT) && (playerData & SNES_START)){
      Keyboard.press(KEY_ESC);
   } else {
      Keyboard.release(KEY_ESC);
   }
}

void handleKey(unsigned int playerData, unsigned int snesKey, int keyCode) {
  if(playerData & snesKey) {
    Keyboard.press(keyCode);
  } else {
    Keyboard.release(keyCode);
  }
}
