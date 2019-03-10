#include <Adafruit_NeoPixel.h>

// Defines for quick port change
#define OUTPUT_A 5
#define OUTPUT_B 6
#define BUTTON 7
#define N_LED 8
#define LED_PORT A0

/*************************GLOBAL VARIABLES**************************/

Adafruit_NeoPixel led_strip = Adafruit_NeoPixel(N_LED, LED_PORT, NEO_GRB + NEO_KHZ800);
float buttonDownCounter = 0;
int encState;
int lastEncState;
float K = 2;      // K is in range 2 to 6
int ledProgram = 0;
bool isChangingK = false;

/***************************MAIN PROGRAM****************************/

void setup() 
{
  // DEBUG
  Serial.begin(9600);

  // Set pin modes for encoder
  pinMode(OUTPUT_A, INPUT);
  pinMode(OUTPUT_B, INPUT);
  pinMode(BUTTON, INPUT);

  // Set pin modes for led strip
  led_strip.begin();
  led_strip.setBrightness(10);
  led_strip.show();
}

void loop() 
{
  checkButtonAction();
  checkTurnAction();
}

/*******************************************************************/

void checkButtonAction()
{
  // Check if K control is triggered
  if(digitalRead(BUTTON) == 0)
  {
    // Applay changes to K after button was clicked to approve
    if(isChangingK)
    {
      // DEBUG
      Serial.println("K change sequence ended.");
      
      isChangingK = false;
    }
    else
    {
      // Button is pushed without K control
      buttonDownCounter += 0.1;
  
      // Button is down for long push
      if(buttonDownCounter >= 8000)
      {
        // DEBUG
        Serial.println("K change sequence triggered");
        delay(1000);

        isChangingK = true;
        buttonDownCounter = 0;
        // K change sequence
        setK();
      } 
    }
  }
  else
  {
    if(!isChangingK)
    {
      // Short click for LED program
      if(buttonDownCounter > 300)
      {
        // DEBUG
        Serial.println("Change LED mode triggered");
  
        // Change LED program
        ledProgram++;
        setProgram();
        buttonDownCounter = 0;
      }
    }
    else
    {
      isChangingK = true;
    }
  }
}

/*******************************************************************/

void checkTurnAction()
{
  // Read current state of rotor
  encState = digitalRead(OUTPUT_A);

  // Change if state has changed
  if(encState != lastEncState)
  {
    // Check direction of rotating
    if(digitalRead(OUTPUT_B) != encState)
    {
      // Check if K is being changed
      if(isChangingK)
      {
        if(K < 6) 
        {
          // DEBUG
          Serial.println("K++");
          
          K += 0.5;
          setK();  
        }
      }
      else
      {
        // DEBUG
        Serial.println("Brightness up");

        // Brightness up
        // @TODO ...  
      }
    }
    else
    {
      // Check if K is being changed
      if(isChangingK)
      {
        if(K > 2)
        {
          // DEBUG
          Serial.println("K--");
          K -= 0.5;  
          setK();
        }
      }
      else
      {
        // DEBUG
        Serial.println("Brightness down");

        // Brightness down
        // @TODO ... 
      }
    }
  }

  // Store current state of rotor
  lastEncState = encState;
}

/*******************************************************************/

void setK()
{
  isChangingK = true;

  // Debug
  Serial.print("In setK(): K is currently: ");
  Serial.println(K);
  Serial.print("Current isChangingK:");
  Serial.println(isChangingK);

  for(int i = 0; i < N_LED; i++)
  {
    led_strip.setPixelColor(i, 0,0,0);
  }

  for(int i = 0; i < (int)K; i++)
  {
    led_strip.setPixelColor(i, 255,255,255);
    led_strip.show();
  }

  
}

/*******************************************************************/

void setProgram()
{
  // Switch
}

