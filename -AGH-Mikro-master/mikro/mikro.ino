#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, A0, NEO_GRB + NEO_KHZ800); // initialization of LED strip
bool light = true;

//////////////////////////////////

void setup() 
{
  strip.begin();
  strip.show();
}

//////////////////////////////////

void set_color(int R, int G, int B) // main function, which sets colors on LED strip
{
  int i = 0;
  for(i = 0; i < 8; i++)
  {
    strip.setBrightness(10);
    strip.setPixelColor(i, strip.Color(R, G, B));
    strip.show();
  }
}

void program_R() // setting RED color
{
  set_color(255, 0, 0);
}

void program_G() // setting GREEN color
{
  set_color(0, 255, 0);
}

void program_B() // setting BLUE color
{
  set_color(0, 0, 255);
}

void program_RG() // setting RED and GREEN colors
{
  set_color(255, 255, 0);
}

void program_RB() // setting RED and BLUE colors
{
  set_color(255, 0, 255);
}

void program_GB() // setting GREEN and BLUE colors
{
  set_color(0, 255, 255);
}

void program_U1() // user program No. 1
                  // red and blue led lights alternately
{
  int i = 0;
  for(i = 0; i < 8; i++)
  {
    if(i % 2 == 0)
    {
      strip.setBrightness(5);
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
      delay(250);
      strip.setBrightness(0);
    }
    else
    {
      strip.setBrightness(5);
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
      delay(250);
      strip.setBrightness(0);
    }
  }
}

void program_U2() // user program No. 2
                  // rainbow led lights 
{
    unsigned int rainbow_array[3];
    rainbow_array[0] = 255;
    rainbow_array[1] = 0;
    rainbow_array[2] = 0;
    
    for(int i = 0; i < 3; i++)
    {
      int j = i == 2 ? 0 : i + 1;

      for(int k = 0; k < 255; k += 1) 
      {
      rainbow_array[i] -= 1;
      rainbow_array[j] += 1;
      
      set_color(rainbow_array[0], rainbow_array[1], rainbow_array[2]);
      }
    }
}

void program_U3() // user program No. 3
                  // dot moving around from left to right side of led strip

{
  for(int i = 0; i < 8; i++)
  {
    if(light == true)
    {
      strip.setBrightness(5);
      strip.setPixelColor(i, strip.Color(0, 0, 255));
      strip.show();
      delay(150);
      strip.setBrightness(0);
    }
    else 
    {
      break;
    }
  }
  for(int j = 7; j > 0; j--)
  {
    if(light == true)
    {
      if(j != 7)
      {
        strip.setBrightness(5);
        strip.setPixelColor(j, strip.Color(0, 0, 255));
        strip.show();
        delay(150);
        strip.setBrightness(0);
      }
      else
      {
        strip.setBrightness(5);
        strip.setPixelColor(j, strip.Color(255, 0, 0));
        strip.show();
        strip.setBrightness(0);
      }
    }
  }
}

//////////////////////////////////

void loop() 
{
  program_U2();
}
