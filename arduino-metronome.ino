/*
  Final Project: Metronome Arduino
  Description: A digital metronome made using an Arduino Mega board with
               pots, a buzzer, a LCD Screen, 1 Digit LED Display, and a RGB LED.
               
  Author: Marcus Balogh
  
  Sources Used: SMRAZA code snippets,
                https://github.com/SwitchAndLever/theMIDInator
                - for Pot val calculation,
                https://github.com/primerobotics/Arduino/blob/master/LCD1602/LCD1602.ino
                - for LCD1602
*/

#include <LiquidCrystal.h>// LCD stuff

// PIN ALLOCATION
// Digital Pins
int buzzer = 52; // Buzzer pin

//RGB Light
int redPin = 36;
int greenPin = 35;
int bluePin = 34;

// Analog pins
int bpmSensor = A1, timeSigSensor = A7; // Pots

// initial values
double last_bpm_val = 0;
int last_timeSig_val = 0;

// LED Display array
int numbers[10][10] = {
  {0, 0, 0, 1, 1, 1, 0, 1, 1, 1}, //0
  {0, 0, 0, 1, 0, 0, 0, 0, 0, 1}, //1
  {0, 0, 0, 0, 1, 1, 1, 0, 1, 1}, //2
  {0, 0, 0, 1, 1, 0, 1, 0, 1, 1}, //3
  {0, 0, 0, 1, 0, 0, 1, 1, 0, 1}, //4
  {0, 0, 0, 1, 1, 0, 1, 1, 1, 0}, //5
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 0}, //6
  {0, 0, 0, 1, 0, 0, 0, 0, 1, 1}, //7
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1}, //8
  {0, 0, 0, 1, 1, 0, 1, 1, 1, 1}, //9
};

// Buzzer frequencies
const int highNote = 880;
const int lowNote = 660;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

void setup()
{
  // Buzzers setup
  pinMode(buzzer, OUTPUT);

  // LED Display setup
  for (int i = 23; i <= 29; i++)
  {
    pinMode(i, OUTPUT);
  }

  // RGB Light
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // LCD Stuff
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows
}

void loop()
{
  if (Read_bpm() && Read_timeSig()) {
    Metronome(Read_bpm(), Read_timeSig());
  }
}

void Metronome(double bpm, int timeSig) {

  int beats = timeSig;
  int duration = 40;
  int delayAmmount = (60 / bpm) * 1000 - 2 - 2;
  lcd.clear();
  lcd.print("BPM:" + String(bpm));
  lcd.setCursor(0, 1); //Display position
  lcd.print("Time Sig:" + String(beats));
  for (int i = 1; i <= beats; i++) {

    // Print Beat
    print_1D_LED(i);

    if (i == 1) {
      tone(buzzer, highNote, duration);
      RGB(1, round(delayAmmount / 2));
      delay(delayAmmount / 2); // get milliseconds from bpm
    }
    else {
      tone(buzzer, lowNote, duration);
      RGB(2, round(delayAmmount / 2));
      delay(delayAmmount / 2); // get milliseconds from bpm
    }
  }
}

int Read_timeSig() {
  // 0 - 1023 - pot tange
  int pot_val = 0;
  int timeSig_val = analogRead(timeSigSensor);

  // CALCULATE DIFFERENCE BETWEEN NEW VALUE AND LAST RECORDED VALUE //
  int timeSig_val_diff = timeSig_val - last_timeSig_val;

  if (abs(timeSig_val_diff) > 2) {
    last_timeSig_val = timeSig_val;
    pot_val = timeSig_val;
  }
  else {
    pot_val = last_timeSig_val;
  }

  // 114 point increment
  int return_val = round(pot_val / 114);

  delay(2);

  Serial.println("TS " + int(round(return_val + 1)));
  return round(return_val + 1); // Min time sig is 1
}

double Read_bpm() {
  // 0 - 1023 - pot tange
  int pot_val = 0;
  int  bpm_val = analogRead(bpmSensor);

  // CALCULATE DIFFERENCE BETWEEN NEW VALUE AND LAST RECORDED VALUE //
  int bpm_val_diff = bpm_val - last_bpm_val;

  if (abs(bpm_val_diff) > 2) {
    last_bpm_val = bpm_val;
    pot_val = bpm_val;
  }
  else {
    pot_val = last_bpm_val;
  }

  // 3 point increment
  double return_val = (pot_val / 3);

  delay(2);
  //  Serial.println(int(return_val + 24));
  return return_val + 24; // lowest bpm is 24
}

void print_1D_LED(int v)
{
  for (int i = 23; i <= 29; i++)
  {
    int x = i - 20;
    digitalWrite(i, numbers[v][x]);
  }
}

void setColor(int red, int green, int blue)
{
#ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
#endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void RGB(int color, int duration) {
  switch (color) {
    case 1:
      setColor(255, 0, 0);  // red
      delay(duration / 2);
      setColor(0, 0, 0); // off
      delay(duration / 2);
      break;
    case 2:
      setColor(0, 255, 0);  // green
      delay(duration / 2);
      setColor(0, 0, 0); // off
      delay(duration / 2);
      break;
  }
}
