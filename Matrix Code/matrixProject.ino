#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
const byte rs = 9;
const byte en = 8;
const byte d4 = A5;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//variables for driver conections
const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN = 10;
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

//variables regarding the joystick
const int JoyStickZoneZero = 50;
const int movementInterval = 200;
unsigned long lastMoveTime = 0;
const int matrixSize = 8;
const int xPin = A0;
const int yPin = A1;
const int joystickButton = 2;

// variables to define the player position
int playerX = 0;
int playerY = 0;
int playerLifeCount = 3; //i want the player to have 3 lifes to rely on, a life is taken if he is in bomb's range when one is detonated

bool walls[matrixSize][matrixSize]; //matrix true/false to keep track of the leds state
bool wallsGenerated = false; //flag if the map of walls was generated or not
bool displayHiFlag = true; //flag for displaying the greeting image, just this image worked

//variables for time
unsigned long previousMillis = 0;
const long interval = 200;

//variables for the bomb, yes, all of them were needed
const int intervalBomb = 50; //for blinking the bomb
unsigned long previousMillisBomb = 0; //for the time difference
bool bombPlaced = false; //flag to know if the bomb was placed
bool detonateFlag = false; //flag to know if the bomb was detonated
int bombX, bombY; // to remember where the bomb is
bool canPlaceBomb = false; //flag to know if the bomb can be placed
int countPresses = -1; //to count how many times the button was pressed because I use the button for 3 things and arduino was doing all of them at the same time
bool canBlowTheBomb = false; //flag to detonate the bomb

const int ledPinBombPlaced = A2; //led that lights up when the bomb is placed end goes off when bomb is boom
const int buzzerBombBlown = 13; // buzzer to beep when bomb boom
const int bombFrequency = 1500;
const int boomBombSound = 500;

unsigned long gameStartTime = 0; //to keep track of time since the start of the game
int score = 0; // to remember the score

// variables for LCD
int lcdBrightness = 128; // "implicit" value for lcd brigthness that needs to get changed in the settings, didn't really work out
const int contrastLCDpin = 3;

int currentLevel = 1;
unsigned long gameTime = 0; // variable to keep track of time since game started

// flags and variables for menu and displaying
bool gameInProgress = false;
const String greetingMessage = "Welcome!";
int menuState = 0; //to switch between the options of the menu
bool menuIsOn = false;
bool displayGreeting = true; 
unsigned long arduinoIsRunning = 0; //to get when arduino started working
bool gameStarted = false; // to know when the game is running
int currentMessageIndex = 0;
bool submenuSettingsIsOn = false; //flag for the settings submenu
int submenuState = 0;
bool displayAbout = false; // flag for the about section
bool changeLightIsOn = false; //flag for the lcd light changing option
bool displayHighScore = false;
bool enterNameIsOn = false;
const int nameSize = 5;
char currName[nameSize];
int letterPos = 0;
bool soundSettingsIsOn = false;
int soundState = 0;
bool soundIsOn = true;
const int topScores = 3;
int highScore[topScores];
bool changeMatrixLightIsOn = false;
int matrixBrightness = 10; // Initial matrix brightness value

const int xDOWN = 100;
const int yLEFT = 100;
const int yRIGHT = 900;
const int xUP = 900;

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(contrastLCDpin, OUTPUT); // 
  analogWrite(contrastLCDpin, lcdBrightness); // Setează nivelul de contrast (0-255)

  pinMode(joystickButton, INPUT);
  digitalWrite(joystickButton, HIGH);

  randomSeed(analogRead(A3)); //to make sure the random is random everytime the Arduino resets, Arduino reference for more details
  pinMode(ledPinBombPlaced, OUTPUT);
  pinMode(buzzerBombBlown, OUTPUT);
  //generateRandomWalls();
  arduinoIsRunning = millis();
  //initializeName();
}

const char messages[][17] = {
  "   WELCOME  ",
  "GET READY ?",
  "TO BLOW UP",
  "BOMBERMAN",
  "",
  "FOR MENU",
  "SWIPE UP OR",
  "DOWN TO NAVIGATE"
};
void displayMessage() {
  displayHi();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(messages[currentMessageIndex]);

  // Activează autoscroll pentru a afișa mesajele pe un singur rând
  lcd.autoscroll();
  
  // Oprește autoscroll după un timp pentru a putea citi mesajul
  delay(1500);
  lcd.noAutoscroll();

  // Oprește afișarea mesajelor după ultimul mesaj
  if (currentMessageIndex < sizeof(messages) / sizeof(messages[0]) - 1) {
    currentMessageIndex++;
  }
  else{
    lcd.clear();
  }
}
const int greetingTime = 12000;
void loop() {
   //analogWrite(contrastLCDpin, 120);
   if (millis() - arduinoIsRunning <= greetingTime && displayGreeting){ //displays the greeting messages for 12 seconds, i know it gets boring after a while
    //displayHi();
    displayMessage();
   }
    else {
      if(displayHiFlag && !gameStarted){ //goes to display the menu, to see all 3 options, scroll with joystick between them
        displayHi();
        //menuOnLCD();
        displayGreeting = false;
        menuIsOn = true;
      }
    }
    if(menuIsOn){ //displays menu
      menuOnLCD();
    }
    if(submenuSettingsIsOn){ //displays settings submenu
      submenuOnLCD();
    }
    // if (changeLightIsOn && !submenuSettingsIsOn){ //displays the lcd light settings
    //   //changeLCDlight();
    // }
    if(displayAbout){ //displays the about section
      displayAboutOption(); 
    }
    if(displayHighScore){
      //display the existent highScores, 3 in number. initially all are 0, but they change when a new high score is achieved
      displayHighScoreOption();
    }
    if(changeMatrixLightIsOn){
      changeMatrixlight();
    }
    // if(enterNameIsOn){
    //   enterNameOption();
    // }
    if(soundSettingsIsOn){
      soundSettingsOption();
    }
    if (gameStarted) {
      readPlayerDetails(); //reads the input from the joystick and updates the player position
      displayWalls(); //puts walls on the matrix
      displayBlinkingPlayer(); //make player blink
      displayBlinkingBomb(); //make bomb blink blink faster
      checkWalls(); //checks if there still exist walls or not
  }
  checkJoyStickButton(); // reads the button state and decides what to do
}

int joystickMoves(){
  //reads the joystick values
  int joystickX = analogRead(xPin);
  int joystickY = analogRead(yPin);

  if (joystickY < yLEFT){
    return 1; //joystick moved down
  }
  if (joystickY > yRIGHT){
    return 2; //joystick moved up
  }
  if (joystickX < xDOWN){
    return 3; //joystick moved right
  }
  if (joystickX > xUP){
    return 4; //joystick moved left
  }
}

void checkJoyStickButton(){
  if (menuIsOn){
    int optionRead = joystickMoves();
  // verify how joystick is going and what state it is in
  if (optionRead == 1) {
    lcd.clear();
    // Joystick-ul goes down
    menuState = (menuState + 1) % 4;  // scroll options
    menuOnLCD();
    delay(300);  
  }
  }

  if(submenuSettingsIsOn){
  int optionRead2 = joystickMoves();
  if (optionRead2 == 1) {
    lcd.clear();
    // Joystick goes down
    submenuState = (submenuState +1) % 5;
    submenuOnLCD();
    delay(300);  
  }
  if (optionRead2 == 3) { //you have to swipe right on them
      // select option from "Settings"
      if (submenuState == 0) { //option for lcd intensity
        lcd.clear();
        menuIsOn = false;
        submenuSettingsIsOn = false;
        changeLightIsOn = true;
        //changeLCDlight();
      } else if (submenuState == 1) { //option for matrix intensity
          lcd.clear();
          submenuSettingsIsOn = false;
          changeMatrixLightIsOn = true;
        // to be written
      } else if (submenuState == 2){ //returns to menu
        lcd.clear();
        submenuSettingsIsOn = false;
        menuIsOn = true;
        countPresses = 0;
      } else if (submenuState == 3){ //enter name
        lcd.clear();
        submenuSettingsIsOn = false;
        enterNameIsOn = true;
      } else if (submenuState == 4){
        lcd.clear();
        submenuSettingsIsOn = false;
        soundSettingsIsOn = true;
      }
    }
  }

  if (displayAbout){ //to display about
    int joystickX = analogRead(xPin);
    if (joystickX < xDOWN){
      lcd.clear();
      displayAbout = false;
      menuIsOn = true;
      countPresses = 0;
    }
  }

  if (displayHighScore){
    int joystickX = analogRead(xPin);
    int joystickY = analogRead(yPin);
    if (joystickX < xDOWN){ //to return to menu
      lcd.clear();
      displayHighScore = false;
      menuIsOn = true;
      countPresses = 0;
    }
  }

  if (enterNameIsOn){
    // int joystickX = analogRead(xPin);
    // int joystickY = analogRead(yPin);
    // if (joystickY > yRIGHT){ //goes down
    //   //if (currName[letterPos] != 'z') {
    //     currName[letterPos]--;
    //   //}
    // }
    // // if (joystickY < yLEFT) { //goes up
    // //   if (currName[letterPos] != 'a') {
    // //     currName[letterPos]--;
    // //   }
    // // }
    // if (joystickX < xDOWN){ //goes right
    //   letterPos = min(letterPos + 1, nameSize - 1);
    // }
    // if (joystickX > yRIGHT){
    //   //saveName();
    //   // int adrress = 200;
    //   // for (int i = 0 ; i < nameSize; i++) {
    //   //   EEPROM.put(adrress, currName[i]);
    //   //   adrress += sizeof(char);
    //   //}
    //   letterPos = 0;
    //   lcd.clear();
    //   enterNameIsOn = false;
    //   submenuSettingsIsOn = true;
    // }
  }

  if(soundSettingsIsOn){
    int joystickX = analogRead(xPin);
    int joystickY = analogRead(yPin);
    if (joystickY > yRIGHT) {
      lcd.clear();
      // Joystick goes down
      soundState = (soundState + 1) % 2;
      delay(300);  
    }
    if (joystickX < xDOWN){ //goes right
        if (soundState == 0){
          //buzer is on
          soundIsOn = true;
        }
        if (soundState == 1){
          //noTone(buzzerBombBlown);
          soundIsOn = false;
        }
        lcd.clear();
        soundSettingsIsOn = false;
        submenuSettingsIsOn = true;
        //countPresses = 0;
    }
  }
  static bool previousButtonState = HIGH; //variable for the button to make sure it was actually pressed
  bool currentButtonState = digitalRead(joystickButton);

  if (currentButtonState == LOW && previousButtonState == HIGH) { //if pressed
    if(menuIsOn){
      if(menuState == 0){ //to display menu, turns on flags
        menuIsOn = false;
        lcd.clear();
        gameStarted = true;
        displayHiFlag = false;
        //countPresses = -1;
      }
      if (menuState == 1){
        //go to submenu of settings
        displayHiFlag = false;
        lcd.clear();
        menuIsOn = false;
        submenuSettingsIsOn = true;
        //displayHiFlag = true;
      }

      if (menuState == 2){ //about option
        displayHiFlag = false;
        lcd.clear();
        menuIsOn = false;
        displayAbout = true;
        //display about shit
      }

      if (menuState == 3){ //highScore option
        displayHiFlag = false;
        lcd.clear();
        menuIsOn = false;
        displayHighScore = true;
      }
    }
    if (gameStarted){
      countPresses++;
      decideToGenerateWalls();
      decideToPlaceBomb();
      decideToBlowTheBomb();
      countPresses %= 2;
    }
    
  }
  previousButtonState = currentButtonState;
}

void soundSettingsOption(){
  switch (soundState){
    case 0:
      lcd.setCursor(0, 0);
      lcd.print(F("Sound Settings"));
      lcd.setCursor(0, 1);
      lcd.print(F("ON"));
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print(F("Sound Settings"));
      lcd.setCursor(0, 1);
      lcd.print(F("OFF"));
      break;
  }
}


void initializeName() {
  for (int i = 0; i < nameSize; i++) {
        currName[i] = 'a';
  }
  // int adrress = 200;
  // for (int i = 0 ; i < nameSize; i++) {
  //   EEPROM.put(adrress, currName[i]);
  //   adrress += sizeof(char);
  // }

  lcd.clear();
  lcd.setCursor(0,0);
  letterPos = 0;
}

void displayHighScoreOption(){
  
  for (int i = 0; i < topScores; i++){
    lcd.setCursor(0, 0);
    lcd.print("HighScore");
    lcd.setCursor(0, 1);
    lcd.print(i+1);
    lcd.setCursor(1, 1);
    lcd.print(".");
    lcd.setCursor(3, 1);
    lcd.print(highScore[i]);
    delay(2000);
    lcd.clear();
  }

}

void enterNameOption(){
  lcd.setCursor(0, 0);
  lcd.print(F("Enter Name"));
  lcd.setCursor(0, 1);
  for (int i = 0; i< nameSize; i++){
    lcd.print(currName[i]);
  }
}

void changeLCDlight(){ //function to change brigthness
  lcd.setCursor(0, 0);
  lcd.print("LCD light");
  lcd.setCursor(0, 1);
  lcd.print("How you see it");

  int joystickX1 = analogRead(yPin);
  int joystickY1 = analogRead(xPin);

  int brightnessChange = map(joystickY1, 0, 1023, -5, 5);
  lcdBrightness = constrain(lcdBrightness + brightnessChange, 0, 255);
  adjustLcdBrightness(lcdBrightness);

  // Afișează noua valoare a luminozității pe LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Brightness:");
  lcd.setCursor(0, 1);
  lcd.print(lcdBrightness);
  delay(100);  // Pentru a evita schimbări prea rapide

  if (joystickX1 < yLEFT){
   //joystick moved down
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Intensity set");
   delay(1000);
   lcd.clear();
   changeLightIsOn = false;
   submenuSettingsIsOn = true;

  }
  
}

void changeMatrixlight(){ //function to change brigthness
  lcd.setCursor(0, 0);
  lcd.print("Matrix light");
  lcd.setCursor(0, 1);
  lcd.print("How you see it");

  int joystickX1 = analogRead(yPin);
  int joystickY1 = analogRead(xPin);

  int brightnessChange = map(joystickY1, 0, 1023, -1, 1);
  matrixBrightness = constrain(matrixBrightness + brightnessChange, 0, 15);
  adjustMatrixBrightness(matrixBrightness);

  // Afișați noua valoare a luminozității pe 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Matrix Bright:");
  lcd.setCursor(0, 1);
  lcd.print(matrixBrightness);
  delay(100); 

  if (joystickX1 < yLEFT){
   //joystick moved down
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Intensity set");
  }
}

void adjustLcdBrightness(int value) {
  lcdBrightness = constrain(value, 5, 255);
  analogWrite(contrastLCDpin, lcdBrightness);
}

void adjustMatrixBrightness(int value) {
  matrixBrightness = constrain(value, 0, 15);
  lc.setIntensity(0, matrixBrightness);
}

void displayAboutOption(){
  lcd.setCursor(0, 0);
  lcd.print("GITHUB:");
  lcd.setCursor(0, 1);
  lcd.print("@nicoletaiova25");
}

void decideToGenerateWalls() {
  if (!wallsGenerated) {
      generateRandomWalls(); //generates random walls
      wallsGenerated = true;
      displayHiFlag = false;
      countPresses = 0;
      gameStartTime = millis(); //starts the game time counter
    }
}
void decideToPlaceBomb(){
  if (canPlaceBomb && countPresses == 1 && !bombPlaced) {
      placeBomb();
    }
}

void decideToBlowTheBomb(){
  if (canBlowTheBomb && countPresses == 2) {
      if (bombPlaced && detonateFlag) {
        detonateBomb();
        canBlowTheBomb = false;
      }
    }
}

void readPlayerDetails(){
  int xValue = analogRead(xPin); //gets input
  int yValue = analogRead(yPin);
  //analogWrite(3, 128); // Setează nivelul de contrast (0-255)
  if (abs(xValue - 512) > JoyStickZoneZero || abs(yValue - 512) > JoyStickZoneZero) { //if the player trully moved
    if (millis() - lastMoveTime >= movementInterval) { 
      updatePlayerPosition(xValue, yValue); //updates position
      lastMoveTime = millis();
    }
  }
}

void checkWalls() {
  bool wallsExist = false;
  //lcd.clear();
  //checks if there are walls still standing on the map, if not, it restarts the game, if yes, does nothing
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (walls[i][j]) {
        wallsExist = true;
        break;
      }
    }
    if (wallsExist) {
      //lcd.clear();
      displayGameDetails();
      break;
    }
  }

  if (!wallsExist) {
    if(playerLifeCount > 0){
      currentLevel++;
      //new level
      moveLevels();
    }else{
      restartGame();
    }
  }
}

void menuOnLCD(){
  switch (menuState) {
    case 0:
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("MAIN MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("a) Start game"));
      break;
    case 1:
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("MAIN MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("b) Settings"));
      break;
    case 2:
      //lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("MAIN MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("c) About"));
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print(F("MAIN MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("d)Highscore"));
  }
}

void submenuOnLCD(){
  switch (submenuState){
    case 0:
      lcd.setCursor(0, 0);
      lcd.print(F("SETTINGS MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("a)Set LCD light"));
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print(F("SETTINGS MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("b)Set game light"));
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(F("SETTINGS MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("c)Return to menu"));
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print(F("SETTINGS MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("d)Enter name"));
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print(F("SETTINGS MENU"));
      lcd.setCursor(0, 1);
      lcd.print(F("e)Sound on/off"));
      break;
  }
}

void restartGame() {
  lcd.clear();
  analogWrite(3, 120);
  wallsGenerated = false;
  displayHiFlag = true; // to display the gretting message again
  countPresses = 0;
  unsigned long gameTime = millis() - gameStartTime;
  score += gameTime / 1000; // calculates the score
  for (int i = 0; i < topScores; i++){
    if(score > highScore[i]){
      highScore[i] = score;
      break;
    }
  }
  Serial.print(F("For this game you scored:"));
  Serial.println(abs(score));
  lcd.setCursor(0, 0);
  lcd.print(F("Game Over!"));
  lcd.setCursor(0, 1);
  lcd.print(F("Score: "));
  lcd.print(abs(score));
  delay(3000); // afișează scorul pentru 3 secunde
  lcd.clear();
  score = 0;
  currentLevel = 1;
  gameStarted = false;
  menuIsOn = true;
}

void moveLevels(){
  lcd.clear();
  analogWrite(3, 120);
  //wallsGenerated = false;
  //displayHiFlag = true; // to display the gretting message again
  countPresses = 0;
  unsigned long gameTime = millis() - gameStartTime;
  score += gameTime / 1000; // calculates the score
  Serial.print(F("For this game you scored:"));
  Serial.println(abs(score));
  lcd.setCursor(0, 0);
  lcd.print(F("Level Won!"));
  lcd.setCursor(0, 1);
  lcd.print(F("Score: "));
  lcd.print(abs(score));
  delay(3000); // afișează scorul pentru 3 secunde
  lcd.clear();
  score = 0;
  //gameStarted = true;
  generateRandomWalls();
  gameStartTime = millis(); 
  playerLifeCount = 3; 
}

void generateRandomWalls() {
  lcd.clear();
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      walls[i][j] = random(2) == 0; //there is 50% chance there is or not a wall
    }
  }

  do {
    playerX = random(matrixSize); //places the player random depending on the walls, while giving it space to move, to not be trapped between 4 walls
    playerY = random(matrixSize);
  } while (walls[playerY][playerX] || isPlayerSurrounded(playerX, playerY));
}

bool isPlayerSurrounded(int x, int y) { //checks if the player is surrounded
  if ((x > 0 && walls[y][x - 1]) || (x < matrixSize - 1 && walls[y][x + 1]) ||
      (y > 0 && walls[y - 1][x]) || (y < matrixSize - 1 && walls[y + 1][x])) {
    return true;
  }
  return false;
}

void displayWalls() { //shows the walls on the matrix
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (walls[i][j]) {
        lc.setLed(0, i, j, true);
      }
    }
  }
}

void displayBlinkingPlayer() { //makes player go blink on the matrix
  unsigned long currentMillis = millis();
  static bool playerLEDState = false;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    playerLEDState = !playerLEDState; 

    lc.setLed(0, playerY, playerX, false);
    lc.setLed(0, playerY, playerX, playerLEDState);
  }
}

void updatePlayerPosition(int xValue, int yValue) { //changes the position of the player based on input
  int deltaX = xValue - 512;
  int deltaY = yValue - 512;

  int previousX = playerX;
  int previousY = playerY;

  if (abs(deltaX) > JoyStickZoneZero) {
    lc.setLed(0, playerY, playerX, false);

    if (deltaX > 0 && playerX < matrixSize - 1 && !walls[playerY][playerX + 1]) {
      playerX++;
    } else if (deltaX < 0 && playerX > 0 && !walls[playerY][playerX - 1]) {
      playerX--;
    }
  }

  if (abs(deltaY) > JoyStickZoneZero) {
    lc.setLed(0, playerY, playerX, false);

    if (deltaY > 0 && playerY < matrixSize - 1 && !walls[playerY + 1][playerX]) {
      playerY++;
    } else if (deltaY < 0 && playerY > 0 && !walls[playerY - 1][playerX]) {
      playerY--;
    }
  }
  canBlowTheBomb = true;

  if (playerX != previousX || playerY != previousY) {
    lc.setLed(0, playerY, playerX, true);
    canPlaceBomb = true;
  }
}

void placeBomb() {
  if (!bombPlaced) {
    bombX = playerX; //bomb is placed on the position of the player, player should move after placing to actually see the bomb and if no desire to get blown away, player is imune anyway
    bombY = playerY;
    bombPlaced = true;
    detonateFlag = true;
    canPlaceBomb = false;
    digitalWrite(ledPinBombPlaced, HIGH); //output led goes ON
  }
}

void displayBlinkingBomb() { //makes bomb blink blink faster than the player
  unsigned long currentMillisBomb = millis();
  static bool bombLedState = false;

  if (bombPlaced) {
    if (currentMillisBomb - previousMillisBomb >= intervalBomb) {
      previousMillisBomb = currentMillisBomb;
      bombLedState = !bombLedState;
      lc.setLed(0, bombY, bombX, bombLedState);
    }
  }
}

void detonateBomb() { //blows the bomb 
  if (bombPlaced) {
    eliminateFirstWallInDirection(bombX, bombY, 1, 0); //blows the bomb in all directions, right, left, down, up
    eliminateFirstWallInDirection(bombX, bombY, -1, 0);
    eliminateFirstWallInDirection(bombX, bombY, 0, 1);
    eliminateFirstWallInDirection(bombX, bombY, 0, -1);
    lc.setLed(0, bombY, bombX, false);
    lcd.clear();
    digitalWrite(ledPinBombPlaced, LOW); //output led goes off
    bombPlaced = false;
    detonateFlag = false;
    playCrashSound(); //buzzer goes boom
    if (isPlayerInBombRange(playerX, playerY)) {
      playerLifeCount--;
      if (playerLifeCount == 0){
        playerLifeCount = 3;
        restartGame();
      }
      
    }
  }
}

void eliminateFirstWallInDirection(int startX, int startY, int dirX, int dirY) {
  for (int x = startX + dirX, y = startY + dirY;
       x >= 0 && x < matrixSize && y >= 0 && y < matrixSize;
       x += dirX, y += dirY) {
    if (walls[y][x]) {
      // we find the first wall in the wanted direction, we blow it and break out of function
      walls[y][x] = false; 
      score += 10; //we add 10 to the score for every wall blown away
      lc.setLed(0, y, x, false);
      break;
    }
  }
}

bool isPlayerInBombRange(int playerX, int playerY) {
  // Verifică dacă distanța dintre jucător și bombă este mai mică sau egală cu raza bombei
  int distanceX = abs(playerX - bombX);
  int distanceY = abs(playerY - bombY);
  return distanceX <= 1 && distanceY <= 1;
}

void displayGameDetails() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = (currentMillis - gameStartTime) / 1000; // Timpul scurs în secunde
  //analogWrite(3, 128);
  lcd.setCursor(0, 0);
  lcd.print("Lives: ");
  lcd.print(playerLifeCount);

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(elapsedTime);
  lcd.print("s");
}

void displayHi() {
  const uint64_t HI_IMAGE = 0x00b292beb232b200;
  unsigned long currentMillis = millis();
  if (millis() - currentMillis < interval) {
    displayImage(HI_IMAGE);
    lc.clearDisplay(0); // cleans the screen
  }
}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte rowData = (image >> (i * 8)) & 0xFF;
    lc.setRow(0, i, rowData);
 }
}

void playCrashSound() {
  if(soundIsOn){
    tone(buzzerBombBlown, bombFrequency, boomBombSound);
  }
  else{
    noTone(buzzerBombBlown);
  }
}