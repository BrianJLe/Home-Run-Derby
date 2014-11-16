// Author: Brian Le
// Capstone Project Spring 2012
// Team 2 - Home Run Derby for Visually Impaired/Blind

#include <EEPROM.h>

#define FACT 2
#define OUTS 10

int Led1Pin = 2;         // The pin of the first LED.
int Led2Pin = 3;         // The pin of the second LED.
int Led3Pin = 4;         // The pin if the third LED
int Led4Pin = 5;         // The pin of the button LED

int ButtonPin = 9;       // The pin of the button.
int PlayPin = 6;         // Play button for MP3
int PrevPin = 7;         // Previous button for MP3
int NextPin = 10;        // Next button for MP3
int Power = 13;          // Power reset pin for the MP3

boolean old_val = LOW;   // The state of the button the last time we checked.
int LightPosition = 0;   // Stores which LED is on.
int pause = 1000;        // How much time in between LED light changes.
int pause2 = 7500;       // How much time for a MP3 play - 7 seconds.
int pause3 = 5000;       // How much time for a MP3 play - 5 seconds.
int strikes = 0;         // Initialize number of strikes to 0.
int score = 0;           // Initialize score to 0.

unsigned long 
earlyTime,   // Time measured for an early foul
lateTime,    // Time measured for a late foul
buttonTime,  // Time measured when the button was pressed
Time1,       // Calculation of an early foul
Time2,       // Calculation of a late foul

int x;

long lastMove = millis();        // What time it was when we last moved the light.
long lastMove2 = millis();       // Timer for alternative delay
int highscore = EEPROM.read(0);  // Saves the highest score in EEPROM

int latchPin = 8;                // Pin connected to ST_CP of 74HC595
int clockPin = 12;               // Pin connected to SH_CP of 74HC595
int dataPin = 11;                // Pin connected to DS of 74HC595

void setup()
{
  pinMode(Led1Pin, OUTPUT);
  pinMode(Led2Pin, OUTPUT);
  pinMode(Led3Pin, OUTPUT);
  pinMode(Led4Pin, OUTPUT);
  pinMode(ButtonPin, INPUT);
  pinMode(PlayPin, OUTPUT);
  pinMode(PrevPin, OUTPUT);
  digitalWrite (PrevPin, LOW); 
  pinMode(NextPin, OUTPUT);
  digitalWrite (NextPin, LOW);
  pinMode(Power, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(Led1Pin, LOW);
  digitalWrite(Led2Pin, LOW);
  digitalWrite(Led3Pin, LOW);
  digitalWrite(Led4Pin, LOW);

  digitalWrite(latchPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
  //EEPROM.write(highscore, 0);    // Resets the score to 0
  back1();
  delay(2000);
  Blinker();          // Blinks starting LEDs

  newGame();          // Start a new game.

  playback(4,4);      // Outputs "Are you ready?"
  delay(2000);
  nextMP3(14);        // Outputs "Let's do this"
  delay(1500);
  back1();            // Restarts MP3
}


void loop()
{
  if(millis() - lastMove >= pause)    // Move the light.
  {
    lastMove = millis();   // Remember when we switched LED

    LightPosition++;       // Increment the Light position.

    // If sequence passed the 4th LED and button was not pressed
    if(LightPosition >= 5 && digitalRead(ButtonPin) == LOW) 
    {
      Strike();
      Again();
      LightPosition = 0;
    }
    move(LightPosition);  // Update the light position.
  }

  //When the player presses the button...
  if(digitalRead(ButtonPin) == HIGH && old_val == LOW)
  {
    buttonTime = millis(); // Time since the button had been pressed

    Time1 = buttonTime - earlyTime; // The amount of time between the button pressed and the 3rd LED
    Time2 = buttonTime - lateTime;  // The amount of time between the button pressed and the 4rd LED

    int factor = pause/FACT;
    digitalWrite(Led4Pin, LOW);
    //If the pressed it when the light was in the middle, speed up and continue.
    if(Time2  <= (factor*.500))
    {
      int p = random(14,17);

      nextMP3(p); // Outputs baseball hitting noise
      delay(3000);

      // Blink 4th LED
      for(x = 0; x<=2;x++)
      {
        digitalWrite(Led4Pin, HIGH);
        delay(500);
        digitalWrite(Led4Pin, LOW);
        delay(1000);
      }

      digitalWrite(Led4Pin, LOW);
      if(p == 14)
      {
        prevMP3(2);
        delay(2000);
      }
      else if(p == 15)
      {
        prevMP3(3); 
        delay(2000);
      }
      else if(p == 16)
      {
        prevMP3(4); 
        delay(2000);
      }
      int y = random(9,16);
      nextMP3(y);
      delay(4000);

      for (int x = 0; x <= 2; x++)     // Blinks the Homerun Indicator
      {
        HomeRun();
      }

      /********* Speed up the game ***************/

      if(pause > 800)
      {
        pause -= 50;
      } 
      else if (pause > 600)
      {
        pause -= 25;
      } 
      else if (pause > 300)
      {
        pause -= 15;
      } 
      else if (pause > 10)
      {
        pause -= 10;
      } 
      else if (pause > 1)
      {
        pause -= 1;
      }

      score++;     // Adds 1 to score
      playback(9,4);
      delay(2000);
      Score(score);       // Outputs the numerical score


        delay(1000);

      LightPosition = 0;   // Restarts the LED position
      delay(500);

      if(pause == 600)
      {
        playback(17,5);
      }
      else if(pause == 400)
      {
        playback(17,5);
      }
      playback(4,3);
      nextMP3(14);
      delay(3000);
      back1();
    }

    // Ball was hit late, indicating a late foul, results in strike.
    else if(Time2 > (factor*.501) && Time2 <= (factor*.999))
    {
      nextMP3(14); // Outputs baseball hitting noise
      delay(3000);
      digitalWrite(Led1Pin, LOW);
      digitalWrite(Led2Pin, LOW);
      digitalWrite(Led3Pin, LOW);
      digitalWrite(Led4Pin, LOW);
      playback(29,5);
      delay(4000);

      prevMP3(10);

      if(Time2 > (factor*.501) && Time2 <= (factor*.750))
      { 
        for(x=0;x<=2;x++)
        {
          // Blinks the 4th outer LED
          digitalWrite(latchPin, LOW);
          // shift out the bits:
          shiftOut(dataPin, clockPin, MSBFIRST, 16);  
          shiftOut(dataPin, clockPin, MSBFIRST, 16);  

          //take the latch pin high so the LEDs will light up:
          digitalWrite(latchPin, HIGH);
          // pause before next value:
          delay(4000);
          digitalWrite(latchPin, LOW);
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          digitalWrite(latchPin, HIGH);  
          delay(500);
        }
      }   

      else if(Time2 > (factor*.751) && Time2 <= (factor*.999))
      {  
        for(x=0;x<=2;x++)
        {
          // Blinks the 5th outer LED
          digitalWrite(latchPin, LOW);
          // shift out the bits:
          shiftOut(dataPin, clockPin, MSBFIRST, 32);  
          shiftOut(dataPin, clockPin, MSBFIRST, 32);  

          //take the latch pin high so the LEDs will light up:
          digitalWrite(latchPin, HIGH);
          // pause before next value:
          delay(4000);
          digitalWrite(latchPin, LOW);
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          digitalWrite(latchPin, HIGH);  
          delay(500);
        }
      }
      Strike();
      Again();
      LightPosition = 0;
    }

    // Ball was hit early, indicating a early foul, results in foul.
    else if(Time1 > (factor*1.000) && Time1 <= (factor*2.000))
    {
      nextMP3(14); // Outputs baseball hitting noise
      delay(3000);
      digitalWrite(Led1Pin, LOW);
      digitalWrite(Led2Pin, LOW);
      digitalWrite(Led3Pin, LOW);
      digitalWrite(Led4Pin, LOW);
      playback(28,4);
      delay(4000);

      prevMP3(9);

      if(Time1 > (factor*1.000) && Time1 <= (factor*1.333))
      {  
        for(x=0;x<=2;x++)
        {
          // Blinks the 1st outer LED
          digitalWrite(latchPin, LOW);
          // shift out the bits:
          shiftOut(dataPin, clockPin, MSBFIRST, 2);  
          shiftOut(dataPin, clockPin, MSBFIRST, 2);  

          //take the latch pin high so the LEDs will light up:
          digitalWrite(latchPin, HIGH);
          // pause before next value:
          delay(4000);
          digitalWrite(latchPin, LOW);
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          digitalWrite(latchPin, HIGH);  
          delay(500);
        }
      }   

      else if(Time1 > (factor*1.334) && Time1 <= (factor*1.666))
      { 
        for(x=0;x<=2;x++)
        {
          // Blinks the 2nd outer LED 
          digitalWrite(latchPin, LOW);
          // shift out the bits:
          shiftOut(dataPin, clockPin, MSBFIRST, 4);  
          shiftOut(dataPin, clockPin, MSBFIRST, 4);  

          //take the latch pin high so the LEDs will light up:
          digitalWrite(latchPin, HIGH);
          // pause before next value:
          delay(4000);
          digitalWrite(latchPin, LOW);
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          digitalWrite(latchPin, HIGH);  
          delay(500);
        }
      }

      else if(Time1 > (factor*1.667) && Time1 <= (factor*2.000))
      {  
        for(x=0;x<=2;x++)
        {
          // Blinks the 3rd outer LED
          digitalWrite(latchPin, LOW);
          // shift out the bits:
          shiftOut(dataPin, clockPin, MSBFIRST, 8);  
          shiftOut(dataPin, clockPin, MSBFIRST, 8);  

          //take the latch pin high so the LEDs will light up:
          digitalWrite(latchPin, HIGH);
          // pause before next value:
          delay(4000);
          digitalWrite(latchPin, LOW);
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          shiftOut(dataPin, clockPin, MSBFIRST, 0); 
          digitalWrite(latchPin, HIGH);  
          delay(500);
        }
      }

      digitalWrite(Led1Pin, LOW);
      digitalWrite(Led2Pin, LOW);
      digitalWrite(Led3Pin, LOW);
      digitalWrite(Led4Pin, LOW);
      
      Strike();
      Again();
      LightPosition = 0;
    }

    // LED not hit in time
    else
    {
      Strike();
      digitalWrite(Led1Pin, LOW);
      digitalWrite(Led2Pin, LOW);
      digitalWrite(Led3Pin, LOW);
      digitalWrite(Led4Pin, LOW);
      Again();
      LightPosition = 0;

      // Speed down the game.
      if(pause > 700)
      {
        pause += 100;
      } 
      else if (pause > 500)
      {
        pause += 50;
      } 
      else if (pause > 300)
      {
        pause += 25;
      } 
      else if (pause > 10)
      {
        pause += 10;
      } 
      else if (pause > 1)
      {
        pause += 1;
      }
    }
  }
  old_val = digitalRead(ButtonPin);
}

//Updates the light's position.
void move(int LightPosition)
{
  //Turn off all LEDs
  for (x = Led1Pin; x <= Led4Pin;x++)
  {
    digitalWrite(x, LOW);
  }

  //Turn on the LED
  digitalWrite(LightPosition+1, HIGH);

  if(LightPosition == 3)
  {
    earlyTime = millis();
  }

  if(LightPosition == 4)
  {
    lateTime = millis();
  }
}
/************************************************************
 * 
 * NEW GAME 
 *
 * INPUT: USER PRESSES BUTTON
 * OUTPUT: INTRO, CURRENT HIGH SCORE
 * 
 ***************************************************************/
void newGame()
{
  digitalWrite(Led1Pin, LOW);
  digitalWrite(Led2Pin, LOW);
  digitalWrite(Led3Pin, LOW);
  digitalWrite(Led4Pin, LOW);

  back1();
  delay(2000);

  nextMP3(1); // Introduction to Homerun Derby

  delay(17000); // Waits 17 seconds

  nextMP3(3); // Are you ready?
  delay(3000);

  while(digitalRead(ButtonPin) == HIGH)
  {
  }
  delay(5);


  while(digitalRead(ButtonPin) != HIGH)
  {
    if(millis() - lastMove2 >= pause2)    //Move the light.
    {
      lastMove2 = millis(); //Remember when we switched LED
      back1();
      nextMP3(2);

    }
  }

  while(digitalRead(ButtonPin) == LOW)
  {
  }

  strikes = 0;
  pause = FACT * 1000;
  score = 0;

  nextMP3(5); // Let's do this!

  delay(5000);

  nextMP3(1); 
  delay(3000);

  Score(highscore); // Outputs the current high score
  delay(1300);

  playback(13,5);
  delay(2500);
  LightPosition = 0;
  back1();
}


void Score(int g)
{

  if(g >=21 && g <30)
  {
    playback(41,4);
    delay(700);
    nextMP3(g-12);
    delay(3000);
  }

  else if(g >=31 && g <40)
  {
    playback(42,4);
    delay(700);
    nextMP3(g-23);
    delay(3000);

  }

  else if(g >=41 && g <50)
  {
    playback(43,4);
    delay(700);
    nextMP3(g-34);
    delay(3000);
  }

  else if(g >=51 && g <60)
  {
    playback(44,3);
    delay(700);
    nextMP3(g-45);
    delay(3000);
  }

  else if(g >=61 && g <70)
  {
    playback(45,3);
    delay(700);
    nextMP3(g-56);
    delay(3000);
  }

  else if(g >=71 && g <80)
  {
    playback(46,3);
    delay(700);
    nextMP3(g-67);
    delay(3000);
  }

  else if(g >=81 && g <90)
  {
    playback(47,3);
    delay(700);
    nextMP3(g-78);
    delay(3000);
  }

  else if(g >=91 && g <100)
  {
    playback(48,3);
    delay(700);
    nextMP3(g-89);
    delay(3000);
  }      

  else if(g >=1 && g<=19)
  {
    playback1(20-g,4);
    delay(1000);
  }

  else if(g == 20 || 30 || 40 || 50 || 60 || 70 || 80 || 90 || 100)
  {
    playback((g/10)+39,3);
  }
}

void Again()
{
  playback(20,4);

  delay(2000);
  playback(4,4);
  delay(2000);
  nextMP3(14);
  delay(1500);
  back1();
}

void prevMP3(int x)
{
  int y = x;
  while(y>0)
  {
    digitalWrite (PrevPin, HIGH);
    delay(40);
    digitalWrite (PrevPin, LOW);
    delay(40);
    y--;
  }
}

void nextMP3(int a)
{
  int b = a;
  while(b>0)
  {
    digitalWrite (NextPin, HIGH);
    delay(50);
    digitalWrite (NextPin, LOW);
    delay(50);
    b--;
  }
}

/*  PlayMP3() - Plays the current audio file */
void PlayMP3(void)
{
  digitalWrite(PlayPin, LOW);
  digitalWrite(PlayPin, HIGH);
  delay(1000);
  digitalWrite(PlayPin, LOW);
}

/* Pause() - Stops the current audio file */
void Pause(void)
{
  digitalWrite(PlayPin, LOW);
  digitalWrite(PlayPin, HIGH);
  delay(1000);
  digitalWrite(PlayPin, LOW);
}

/* playback() - First resets the MP3 module, then continuing 
 presses next to the c-th audio file. It will delay for d seconds. */
void playback(int c, int d)
{
  back1();
  nextMP3(c);

  if(millis() - lastMove2 >= d*1000)    //Move the light.
  {
    lastMove2 = millis();               //Remember when we switched LED
  }
}

void playback1(int e, int f)
{
  back1();
  prevMP3(e);

  if(millis() - lastMove2 >= f*1000)    //Move the light.
  {
    lastMove2 = millis();               //Remember when we switched LED
  }
}

/* back() - Turns on the MP3 module */
void back()
{
  digitalWrite(Power, HIGH);
  digitalWrite(Power, LOW);
  digitalWrite(Power, HIGH);
  delay(1000);
  PlayMP3();
  delay(1000);
}

/* back1() - Resets the MP3 module to the first audio file */
void back1()
{
  digitalWrite(Power, LOW);
  delay(1000);
  digitalWrite(Power, HIGH);
  delay(1000);
  PlayMP3();     // Plays Silence
  delay(1200);
}


void Strike()
{
  strikes +=1;

  back1();
  nextMP3(strikes+29);
  delay(4000);

  if (LightPosition > 4)
  {
    for (int x = 0; x <= 3; x++)
    {
      if(digitalRead(LightPosition) == LOW)
      {
        digitalWrite(LightPosition, HIGH);
      }
      else
      {
        digitalWrite(LightPosition, LOW);
      }
      delay(200);
    }
  }
  else
  {
    // Blink LED after strike
    for (int x = 0; x <= 3; x++)
    {
      if(digitalRead(LightPosition + 1) == LOW)
      {
        digitalWrite(LightPosition + 1, HIGH);
      }
      else
      {
        digitalWrite(LightPosition + 1, LOW);
      }
      delay(200);
    }
  }
  digitalWrite(LightPosition, LOW);
  if(strikes == 3)
  {
    playback(11,15);
    //delay(15000);
  }

  if(strikes >= OUTS)               // If the pressed it at the wrong time, show their final score and start a new game.
  {

    //Game over
    int highscore=EEPROM.read(0);   // Reads current high score from memory

    playback(8,4);
    delay(3000);
    Score(highscore);
    delay(2000);

    if(EEPROM.read(0)<(score-1))    // Decides if the current score is bigger the the stored high score
    {

      EEPROM.write(0,score);       // Sets the users score as the new high score is bigger

      playback(40,4);
      delay(2000);
      Score(score);
    }
    delay(7000);

    //Blink the last LED pressed on
    for (int x = 0; x <= 5; x++)
    {
      if(digitalRead(LightPosition + 1) == LOW)
      {
        digitalWrite(LightPosition + 1, HIGH);
      }
      else

      {
        digitalWrite(LightPosition + 1, LOW);
      }

      delay(200);
    }

    delay(2000);

    newGame();
  }
}

/* Intro blinking function */
void Blinker() 
{

  delay(1000);
  nextMP3(11);
  delay(3000);

  // light each pin one by one using a function A
  for (int j = 0; j < 7; j++) 
  {
    digitalWrite(latchPin, 0);
    lightShiftPinA(j);
    lightShiftPinA(j);
    digitalWrite(latchPin, 1);
    delay(700);
  }
  for (int x = Led1Pin; x <= Led4Pin;x++)
  {
    digitalWrite(x, HIGH);
    delay(1000);
    digitalWrite(x, LOW);
  }
  HomeRun();

}

//This function uses bitwise math to move the pins up
void lightShiftPinA(int p) 
{
  int pin;

  pin = 1<< p;
  shiftOut(dataPin, clockPin, pin);   
}


void lightShiftPinB(int p) 
{
  int pin;
  pin = 1;

  for (int x = 0; x < p; x++) 
  {
    pin = pin * 2; 
  }
  shiftOut(dataPin, clockPin, pin);     // Shifts
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) 
{
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  for (i=7; i>=0; i--)  
  {
    digitalWrite(myClockPin, 0);

    if ( myDataOut & (1<<i) ) 
    {
      pinState= 1;
    }
    else 
    {	
      pinState= 0;
    }
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);    // Register shifts bits on upstroke of clock pin  
    digitalWrite(myDataPin, 0);     // Zero the data pin after shift to prevent bleed through
  }
  digitalWrite(myClockPin, 0);      // Stop shifting
}

void blinkAll_2Bytes(int n, int d) 
{
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, 1);
  delay(200);
  for (int x = 0; x < n; x++) 
  {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 255);
    shiftOut(dataPin, clockPin, 255);
    digitalWrite(latchPin, 1);
    digitalWrite(Led1Pin, HIGH);
    digitalWrite(Led2Pin, HIGH);
    digitalWrite(Led3Pin, HIGH);
    digitalWrite(Led4Pin, HIGH);
    delay(d);
    digitalWrite(latchPin, 0);

    digitalWrite(Led1Pin,LOW);
    digitalWrite(Led2Pin, LOW);
    digitalWrite(Led3Pin, LOW);
    digitalWrite(Led4Pin, LOW);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 0);
    digitalWrite(latchPin, 1);
    delay(d);
  }

}


void HomeRun()

{ 
  // Blinks the 4th outer LED
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, 128);  
  shiftOut(dataPin, clockPin, MSBFIRST, 128);  

  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
  // pause before next value:
  delay(1000);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0); 
  shiftOut(dataPin, clockPin, MSBFIRST, 0); 
  digitalWrite(latchPin, HIGH);  
  delay(500);
}   

void HomeRun1()

{ 
  // Blinks the 4th outer LED
  digitalWrite(latchPin, LOW);

  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
  // pause before next value:
  delay(1000);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0); 
  shiftOut(dataPin, clockPin, MSBFIRST, 0); 
  digitalWrite(latchPin, HIGH);  
  delay(500);
}   







