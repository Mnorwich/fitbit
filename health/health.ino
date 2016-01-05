#include <ST7036.h>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>


Adafruit_MMA8451 mma = Adafruit_MMA8451();
ST7036 lcd = ST7036 ( 2, 16, 0x7c );

//vars for steps vvvvv
long total = 0;
int counterS = 0;
boolean isAbove = false;
int numSteps = 0;
double last = 0;
float buff = .2;
//^^^^

//vars for temp vvvvv
const int analogInPin = A0;  // Analog input pin
int rawValue = 0;
float temp = 0.0;
const int filterSize = 5;
float lastFiveReads[filterSize];
int counter = 0;
int currentTemp;
float sum = 0;
//^^^
//vars for button vvv
int buttonPin = 13; 
int buttonState;
int lastButtonState; 
long lastDebounceTime=0; 
long debounceDelay = 50; 
//^^
//vars for loop vv
int steps_Final; 
int tempFinal; 
//^^
int tempRead();
int filter(float x);
int getSteps();
int checkStep(int val);

void setup() {
 // analogReference(INTERNAL);
  Serial.begin(9600);
  lcd.init();
  lcd.setContrast(0);
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  mma.setRange(MMA8451_RANGE_8_G);

  pinMode(buttonPin,INPUT); //button to reset the steps.
}

void loop() {
  
  steps_Final = getSteps();//calls steps file.
  //temp_Final = tempRead();//call temperature file
  
  int reading=digitalRead(buttonPin);  
  
  if(reading != lastButtonState){
    Serial.println("setIF");
    lastDebounceTime = millis(); 
  }
  
  if((millis()-lastDebounceTime)>debounceDelay){
    Serial.println("in the first if statment");
   
   if(reading !=buttonState){
     buttonState=reading; 
      if(buttonState== HIGH){
        Serial.println("in the second if statment");
        steps_Final = 0; 
        numSteps = 0; 
        counterS = 0; 
        Serial.print("buttonPressed");
      }
   }
  }
  lastButtonState=reading;   
  lcd.setCursor(0, 0);

  // "You Have Taken X Steps.  The Temperature is X Degrees"
  lcd.print("Steps:");
  lcd.print(steps_Final);
  lcd.print("  Temp: "); 
  //lcd.print(temp_Final); 
 

 
}




int getSteps() {
  // Put your code here
  mma.read();

  sensors_event_t event;
  mma.getEvent(&event);

  //lcd.setCursor(1,0);

  int out = checkStep(event.acceleration.z - 1);
  //Serial.print("o: " );Serial.println(out);
  //Serial.print(event.acceleration.z); Serial.println(" z");
  //lcd.print(out);

  if (total > 5000) {
    counterS = 0;
    total = 0;
  }
  return out;

}

//returns numSteps
int checkStep(int val) {
  counterS++;
  total += val;

  double average = (double)total / (double)counterS;
  //Serial.println(average);

  if ((val - buff) > average && isAbove == false && val > last) {
    //if(val - buff > average && isAbove == false){
    numSteps++;
    isAbove = true;
  }
  else if (val < average) {
    isAbove = false;
  }
  last = val;
  return numSteps;
}


int tempRead() {
  //Serial.println("Starting A/D read ");
  rawValue = analogRead(analogInPin);
  //Serial.println(rawValue);
  temp = (float)(rawValue) * 100 * 1.1 / 1023 - 50;
  //Serial.println("Temperature ");
  //Serial.println(temp);
  filter(temp);
  counter++;
  delay(100);
  return currentTemp;
}
//filters last 5 temperature readings
int filter(float x) {
  lastFiveReads[counter % filterSize] = x;
  sum = 0;
  for (int i = 0; i < filterSize; i++) {
    sum += lastFiveReads[i];
  }
  currentTemp = sum / filterSize;
  //Serial.println(currentTemp);
}


