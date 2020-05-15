/*********************
Project by: Xiangmin Mo
Course: CSCI 267
Professor: Eric Schweitzer

Libraries used:
1) Wire.h                       - This library allows you to communicate with I2C / TWI devices
2) Adafruit_RGBLCDShield.h      - The Adafruit_RGBLCDShield library is a derivative of the LiquidCrystal library that comes with Arduino
                                  so you can call any of the functions that you're used to and they'll work just the same.
3) utility/Adafruit_MCP23017.h  - This is for the I2C port expander chip, which is a 16-pin expander chip, that uses the i2c bus
4) Adafruit_MotorShield.h       - Adafruit library that allows for the usage of stepper motors

**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <Adafruit_MotorShield.h>

/*
 * Motor Shield set up
 * Create the motor shield object with the default I2C address
 */
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

/* Connect a stepper motor with 200 steps per revolution (1.8 degree)
 * to motor port #2 (M3 and M4)
 */
Adafruit_StepperMotor *myMotor = AFMS.getStepper(513,2);

/*
 * LCD shield set up
 */ 
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield(); // The shield uses the I2C SCL and SDA pins.

/*
 * This part is setting up the number of screens on the LCD using a 2D array
 */
const int numOfScreens = 7;
int currentScreen = 0; //initially set the current screen as 0 so the "welcome screen" is displayed when the program starts
String screens[numOfScreens][2] = {{"Choose a mode","and press select"}, {"GAME MODE ONE", "SELECT?"}, {"GAME MODE TWO", "SELECT?"}, 
                                   {"GAME MODE ONE", "SCORE:"}, {"GAME MODE TWO", "SCORE:"}, {"GAME OVER", "FINAL SCORE:"}, {"YOU WIN!", "RESET?"}};

/*
 * Game set up
 */
int steps= 0; //used to control when the stepper motor changes direction
int counter= 0; //flag to determine a button is pressed
int lives= 3; //life counter
int score= 0; //total score
int gameMode = 0; //needed for check button input
bool didPress = false; //flag used for determining whether they pushed a button the entire duration

/*
 * LED set up
 */
int LED1= 11; //red
int LED2= 10; //green
int LED3= 9;  //yellow (blue leg of the tri-color LED anode)

/*
 * Button set up
 */
int yButton= 2; //digital pin 2 is used to control the amount of "blue" in the LED
int gButton= 4; //digital pin 4 is used to control the amount of "green" in the LED
int rButton= 7; //digital pin 7 is used to control the amount of "red" in the LED

/*
 * Button State is set to 1 instead of 0 because there was an issue where button input was not being read correctly
 */
int yBS= 1;
int gBS= 1;
int rBS= 1;

int led_color = 0; //variable that will be used to *randomly* determine what color the LED is

/*
 * Extra variable declarations needed for game mode 2
 */
const unsigned long gameTime = 60000; //number of milliseconds for the entire game, 60 seconds
unsigned long currentTime = 0; //will be the value of millis() when called, time of the game
unsigned long startTime = 0; //will also be the value of millis() when called
int remainingTime = 60; //60 seconds, used to display the time on the LCD

//END OF VARIABLE DECLARATION

//BEGINNING OF CODE

void setup() {
  
  /* 
   * Debugging output 
   */
  Serial.begin(9600);
  
  /* 
   *  PWM, Common Anode Tri-Color LED
   *  LED1 = RED
   *  LED2 = BLUE
   *  LED3 = GREEN
   */
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  pinMode(yButton, INPUT);
  pinMode(gButton, INPUT);
  pinMode(rButton, INPUT);
  
  /* 
   * Set up the LCD: (Prints the welcome page)
   */
  lcd.begin(16, 2);
  lcd.clear(); //necessary after every time the reset button is pressed
  lcd.setCursor(0,0);
  lcd.print(screens[currentScreen][0]); //currentScreen is 0 at the beginning
  lcd.setCursor(0,1);
  lcd.print(screens[currentScreen][1]);
  
  /*
   * Used to generate true random values by using the analogRead function to read a value on an unused analog pin.
   * This noise value can seed the randomSeed() function to produce differing sequences of random numbers every time the sketch is run.
   * Code from: Michael James at https://www.programmingelectronics.com/using-random-numbers-with-arduino/
   */
  randomSeed(analogRead(A0));
  
  /*
   * Motor setup
   */
  AFMS.begin();
  myMotor->setSpeed(5);
  
  setColor(255, 255, 255); //makes sure that the LED is dull and not a color (from the previous game)
}

void loop() {

  /*
   * All the buttons are read at once when you call lcd.readButtons()
   * which returns a variable that has individual bits set for the buttons.
   */
  uint8_t buttons = lcd.readButtons();
  readLCDButton(buttons);
}

/*
 * param 1 - "buttons" which is a variable that has individual bits set for the buttons.
 */
void readLCDButton(uint8_t buttons){
    if (buttons) {

      /*
       * When the LEFT button is pressed on the LCD Shield
       */
      if (buttons & BUTTON_LEFT) {
        lcd.clear();
        
        /*
         * Ensures that the user is only on screens 1 or 2
         * Screen 1 = Game Mode 1 Selection Screen
         * Screen 2 = Game Mode 2 Selection Screen
         */
        currentScreen = currentScreen - 1; //"left" is associated with subtraction
        if(currentScreen != 1){
          currentScreen = 2;
        }
        lcd.setCursor(0,0);
        lcd.print(screens[currentScreen][0]); //currentScreen is either 1 or 2, nothing higher or lower
        lcd.setCursor(0,1);
        lcd.print(screens[currentScreen][1]);
      }

      /*
       * When the RIGHT button is pressed on the LCD Shield
       */
      else if (buttons & BUTTON_RIGHT) {
        lcd.clear();

        /*
         * Ensures that the user is only on screens 1 or 2
         * Screen 1 = Game Mode 1 Selection Screen
         * Screen 2 = Game Mode 2 Selection Screen
         */
        currentScreen = currentScreen + 1; //"right" is associated with addition
        if(currentScreen != 2){
          currentScreen = 1;
        }
        lcd.setCursor(0,0);
        lcd.print(screens[currentScreen][0]); //currentScreen is either 1 or 2, nothing higher or lower
        lcd.setCursor(0,1);
        lcd.print(screens[currentScreen][1]);
      }

      /*
       * When the SELECT button is pressed on the LCD Shield
       * Checks the current screen to determine which game mode should be ran
       */
      else if (buttons & BUTTON_SELECT) {
        if(currentScreen == 1){
          lcd.clear();
          currentScreen = 3; //Screen used for Game Mode 1 and will be consistently updated as game goes on
          printOne(); //function that prints the 3rd screen in the screens 2D array, which is the game mode 1 screen with the score and number of lives
          
          gameMode = 1; //only used in the function that checks whether a button has pressed (will subtract a life if it isn't pressed)

          /*
           * Calls the first game mode until the conditions are not met
           * Conditions: Lives has to be GREATER THAN 0, and SCORE has to be less than 60 (because 60 = win)
           * This is blocking so nothing will run until the conditions are not met
           */
          while(lives > 0 && score < 60){
            gameMode1();
          }

          setColor(255,255,250); //Sets the LED to BLUE indicate that the game is over

          /*
           * Game over screen will be displayed when you run out of lives
           */
          if(lives == 0){
            currentScreen = 5;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(screens[currentScreen][0]);
            lcd.setCursor(0,1);
            lcd.print(screens[currentScreen][1]);
            lcd.setCursor(12,1);
            lcd.print(" ");
            lcd.setCursor(13,1);
            lcd.print(score); //Displays final score before you game over
          }

          /*
           * Displays victory screen when you reach 60 points
           */
          if(score == 60){
            currentScreen = 6;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(screens[currentScreen][0]);
            lcd.setCursor(0,1);
            lcd.print(screens[currentScreen][1]);
          }
        }

        else if(currentScreen == 2){
          lcd.clear();
          currentScreen = 4;
          printTwo(); //function that prints the 4th screen in the screens 2D array, which is the game mode 2 screen with the score, and time starting from 60
          
          currentTime = millis(); //get time when the game starts, used for calculating remaining time
          startTime = millis(); //only set once and used to set the game time limit, wont be the same as currentTime but it really doesn't matter

          /*
           * Condition: Current time is less than 60 seconds
           * startTime is added to gameTime to compensate for the time used to select the game mode
           */
          while(currentTime <= startTime + gameTime){

            gameMode2();
            
            currentTime = millis(); //Update the time to determine whether game has reached the limit
          }

          setColor(255,255,250); //Sets the LED to BLUE indicate that the game is over
  
          currentScreen = 5; //Game is over, switch to game over screen; time is up
          
          /*
           * Game over screen is displayed with score
           * There is technically no such thing as a victory in this game mode
           */
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(screens[currentScreen][0]);
          lcd.setCursor(0,1);
          lcd.print(screens[currentScreen][1]);
          lcd.setCursor(12,1);
          lcd.print(" ");
          lcd.setCursor(13,1);
          lcd.print(score);
        }
      }
  }
}

/*
 * Game Mode 1: Press button, try to get 60 points, will get progressively harder
 * Lose lives if you press wrong button OR you do not press a button at all
 */
void gameMode1(){
  
  led_color= random(1,4); //choose a random color 1 - 3 (R,G,Y)
  
  while(lives > 0 && steps <= 48){
    myMotor->step(steps, FORWARD, SINGLE);
 
    changeColor(led_color, counter);

    /*
     * The numbers that "steps" is incremented by are factors of "steps"
     * YMMV depending on what you choose for your value of "steps"
     */
    if(score <= 10){
      steps= steps + 12; //easy (level 1: 12/48 = 5 steps (start at 0 steps- 0, 12, 24, 36, 48))
    }

    else if(score > 10 && score <= 40){
      steps= steps + 16; //normal (level 2: 16/48 = 4 steps (start at 0 steps- 0, 16, 32, 48))
    }

    else if(score > 40 && score <= 60){
      steps= steps + 24; //hard (level 3: 24/48 = 3 steps (start at 0 steps- 0, 24, 48))
    }
  }
  
  /*
   * I'm terrible at this game, so every 10 points, you get an extra life 
   */
  if(score == 10 || score == 20 || score == 30 || score == 40 || score == 50){
    lives++;
  }
  
  checkPressed(); //checks to see if button is pressed; if not then you lose a life
  printOne(); //function is called to update score (currentScreen is 3)

  /*
   * Same code has to be run as above since the motor is now going in the opposite direction
   * If the user runs out of lives or wins the game, this portion will not be run
   */
  if(lives > 0 && score < 60){
    //reset variables
    didPress= false;
    steps= 0;
    counter= 0;

    //choose a random color 1 - 3 (R,G,Y)
    led_color= random(1,4);
  
    while(steps <= 48){
      myMotor->step(steps, BACKWARD, SINGLE);
      
      changeColor(led_color, counter);
      
      if(score <= 10){
        steps= steps + 12; //easy (level 1: 12/48 = 5 steps (start at 0 steps- 0, 12, 24, 36, 48))
      }
  
      else if(score > 10 && score <= 40){
        steps= steps + 16; //normal (level 2: 16/48 = 4 steps (start at 0 steps- 0, 16, 32, 48))
      }
  
      else if(score > 40 && score <= 60){
        steps= steps + 24; //hard (level 3: 24/48 = 3 steps (start at 0 steps- 0, 24, 48))
      }
    }

    /*
     * I'm terrible at this game, so every 10 points, you get an extra life 
     */
    if(score == 10 || score == 20 || score == 30 || score == 40 || score == 50){
      lives++;
    }
    
    checkPressed(); //checks to see if button is pressed; if not then you lose a life
    printOne(); //function is called to update score (currentScreen is 3)
    
    //reset variables
    didPress= false;
    steps= 0;
    counter= 0;
  }
}

/*
 * Game Mode 2: Given 60 seconds, get as many points as you can on the fastest setting of game mode 1
 */
void gameMode2(){
  
  led_color= random(1,4); //choose a random color 1 - 3 (R,G,Y)

  /*
   * Condition: Steps are still less than 48, and there is still time left after every time the while loop is called
   */
  while(steps <= 48 && (currentTime <= startTime + gameTime)){
    myMotor->step(steps, FORWARD, SINGLE);

    changeColor(led_color, counter); //calls function to change color
    
    steps= steps + 24; //fastest setting for game mode 1 (3 steps: 0, 24, 48)
      
    currentTime = millis(); //Update the time to determine whether game has reached the limit
  }

  printTwo(); //function is called to update score and time (currentScreen is 4)
  
  //reset variables
  steps= 0;
  counter= 0;
  led_color= random(1,4);
  
  /*
   * SAME AS ABOVE, but in the opposite direction
   * If time exceeds 60 seconds, then this is not run
   */
  while(steps <= 48 && (currentTime <= startTime + gameTime)){
    myMotor->step(steps, BACKWARD, SINGLE);

    changeColor(led_color, counter);
    
    steps= steps + 24; //fastest setting for game mode 1 (3 steps: 0, 24, 48)

    currentTime = millis(); //Update the time to determine whether game has reached the limit    
  }

  printTwo(); //function is called to update score and time (currentScreen is 4)
  
  //reset variables
  steps= 0;
  counter= 0;
}

/*
 * Function that sets the color of the LED, as well as checks the button input
 * Param 1: "led_color" is a random integer from 1 to 3, with 1 being RED, 2 being GREEN, 3 being YELLOW
 * Param 2: "counterNum" is a flag that dulls the LED to indicate that the user has already made an input
 * The reason why I use 255 as the value to "dull" the LED is because I am using a tri-colored ANODE LED rather than a DIODE
 * If it was a DIODE, the values will be all 0 instead. Thus, in my case, the LOWER the value, the BRIGHTER the LED gets
 */
void changeColor(int led_color, int counterNum) {

  if(counterNum == 1){
    setColor(255,255,255);
  }
  
  //red
  else if(led_color == 1 && counterNum == 0){
    setColor(250, 255, 255);
    checkButtonInput(rBS, gBS, yBS, rButton, gButton, yButton); //calls function that affects the score
  }

  //green
  else if(led_color == 2 && counterNum == 0){
    setColor(255, 250, 255); 
    checkButtonInput(gBS, yBS, gBS, gButton, yButton, rButton); //calls function that affects the score
  }

  //yellow
  else if(led_color == 3 && counterNum == 0){
    setColor(240, 240, 255); //yellow is created by mixing red and green????
    checkButtonInput(yBS, rBS, gBS, yButton, rButton, gButton); //calls function that affects the score
  }
}

/*
 * Function that changes the LED color; writes an analog value (PWM wave) to the LED pins (9-B, 10-G, 11-R)
 * Param 1- "redValue" is a number from 0-255, with 0 being bright red, and 255 being no color/dull/off
 * Param 2- "redValue" is a number from 0-255, with 0 being bright green, and 255 being no color/dull/off
 * Param 3- "yellowValue" is a number from 0-255, with 0 being bright blue, and 255 being no color/dull/off
 */
void setColor(int redValue, int greenValue, int yellowValue) {
  analogWrite(LED1, redValue);
  analogWrite(LED2, greenValue);
  analogWrite(LED3, yellowValue);
}

/*
 * Function that will update the score by checking the button input
 * Params 1-3: either 1 or 0. if either param 2 or 3 is 0, then life decrements since it indicates the wrong button is pressed 
 * Params 4-6: pin number of the buttons
 */
void checkButtonInput(int rightBS, int wrongBS1, int wrongBS2, int rightButton, int wrongButton1, int wrongButton2){
    rightBS= digitalRead(rightButton);
    wrongBS1= digitalRead(wrongButton1);
    wrongBS2= digitalRead(wrongButton2);

    if(rightBS == 0 && counter == 0){
      setColor(255, 255, 255); //set color to white to indicate they have made a choice/pressed a button
      
      counter= 1; //set flag to 1 to indicate that they cannot push anymore buttons 
      didPress= true; //used only in game mode 1
      
      //if and only if the right button is pressed, the score goes up by one
      if(rightBS == 0 && wrongBS1 == 1 && wrongBS2 == 1){
        
        score++;

        /*
         * Used in testing/debugging
         */
        Serial.println("Yay!");
        Serial.print("Score: ");
        Serial.println(score);
      }
    }
      
    if((wrongBS1 == 0 || wrongBS2 == 0) && counter == 0){
      counter= 1; //set flag to 1 to indicate that they cannot push anymore buttons 
      
      Serial.println("Wrong Button"); //Used in testing/debugging

      /*
       * Game Mode 1 Specific: if the wrong button is pressed, take a life off
       */
      if(gameMode == 1){
        didPress= true; //set flag to true to indicate that button is pushed to prevent them from pressing another button until the next cycle
        lives--;
        
        /*
         * Used in testing/debugging
         */
        Serial.print("Lives: ");
        Serial.println(lives);
      }
    }
}


/*
 * Game Mode 1 Function, if button is NOT pressed, take a life away and update screen
 */
void checkPressed(){
    if(!didPress){
      lives--;
      
      didPress = false; //reset the flag

      /*
       * Used in testing/debugging
       */
      Serial.println("Did not press");
      Serial.print("Lives: ");
      Serial.println(lives);
    }
}

/*
 * Function that prints out the 3rd screen in the screens 2D arra with the number of lives and the current score
 */
void printOne(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(screens[currentScreen][1]);
  lcd.setCursor(6,1);
  lcd.print(score);
  lcd.setCursor(10,1);
  lcd.print("Life: ");
  lcd.setCursor(15,1);
  lcd.print(lives);
}

/*
 * Function that prints out the 4th screen in the screens 2D arra with the remaining time and the current score
 * Time shown on the LCD is calculated here
 */
void printTwo(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(screens[currentScreen][1]);
  lcd.setCursor(6,1);
  lcd.print(score);
  lcd.setCursor(9,1);
  lcd.print("Time:");

  /*
   * Initial value is 60
   * Remaining time will be calculated only if 60 seconds have not passed
   * Divided by 1000 to convert milliseconds to seconds
   */
  if(remainingTime > 0){
    remainingTime = ((startTime + gameTime)-currentTime)/1000; 
  }

  /*
   * If there are less than 10 seconds left, display the single digits on the far right of the screen
   */
  if(remainingTime > 9){
    lcd.setCursor(14,1);
  }
  else{
    lcd.setCursor(15,1);
  }
  
  lcd.print(remainingTime);
}
