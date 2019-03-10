#include <Adafruit_NeoPixel.h>
#include <RotaryEncoder.h>

// PORTS
#define ROTARY_A 5
#define ROTARY_B 6
#define BUTTON 7
#define N_LED 8
#define LED_PORT A0

// MAX/MIN VALUES
#define MAX_BRIGHT 255
#define MIN_BRIGHT 10
#define SAMPLING_RATE 2000
#define TMP_BRIGHT 50
#define DISTANCE (MAX_BRIGHT - MIN_BRIGHT)

/*
 *  GLOBAL VARIABLES
 */

// LED Strip variable based on Adafruit_NeoPixel library
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LED, LED_PORT, NEO_GRB + NEO_KHZ800);

// Encoder variable based on RotaryEncoder library
RotaryEncoder encoder(ROTARY_A, ROTARY_B);          

// Variable counts time of button down, used for long-short click recognition
float button_down_counter = 0;
int last_pos = -1;              // variable for left-right recognition
unsigned char K = 2;                    // K is in range 2 to 6
unsigned char led_program = 0;

// Control flags
bool is_changing_K = false;     // variable for K setting sequence recognition
bool U1 = false;                // User programs bools
bool U2 = false;
bool U3 = false;
bool light = false;             // bool for U3 program   
int curr_u3_led = 0;            // current led of U3 program that is turned on
bool is_up = true;              // bool for U3 program to go led up
bool was_change = false;         // variable checks if there was a change from input

signed char bright_offset = 0;


/*
 *  MAIN PROGRAM
 */ 

/*
 * Setup of the board and inputs, setting default values of 
 * LED strip, brightness levels and initalization
 */
void setup() 
{
    // Serial communication for debugging
    Serial.begin(9600);

    // Button from encoder initialization
    pinMode(BUTTON, INPUT);

    // Led initialization
    strip.begin();
    strip.setBrightness(TMP_BRIGHT);
    // set_brightness();
    strip.show();

    // Set first program to LEDs
    set_program();
}

/*
 * Main loop of program, reads all inputs from button and encoder, 
 * decides which LED program should be displayed and notices every
 * change form inputs
 */
void loop() 
{ 
  // Check if there was a change in program/encoder
  was_change = false;

  // Set brightness of LED strip as current brightness level in table
  // based on K value
  
  // For loop is for sampling the input to be granted more than one
  // CPU cycle for input check
  for(int i = 0; i < SAMPLING_RATE; i++)
  {
    // Input check
    check_button();
    check_encoder();
    
    // If there was any change there is no point in checking input
    if(was_change)  {   break;  }
  }

  // Set LED strip program based on U1 U2 and U3 booleans
  if(!is_changing_K)
  {
    if(U1) {  Serial.print("loop: U1, button_down_counter = "); Serial.println(button_down_counter); program_U1(); }
    else if(U2) {  Serial.print("loop: U2, button_down_counter = "); Serial.println(button_down_counter); program_U2(); }
    else if(U3) {  Serial.print("loop: U3, button_down_counter = "); Serial.println(button_down_counter); program_U3(curr_u3_led, is_up); } 
    else { /*Serial.println("set_program()");*/ set_program(); }
  }
}

/*******************************************************************/
void set_brightness()
{
  Serial.print("In set brightness, K = ");
  Serial.println(K);
  
  for(int i = 0; i < N_LED; i++)
  {
    //brightness[i] = ((unsigned int)((MAX_BRIGHT - MIN_BRIGHT) / (int)(pow(2, (int)K) - 1))) * ((int)(abs(i + bright_offset)) % (unsigned int)pow(2, (int)K)) + MIN_BRIGHT;
    Serial.print(i);
    Serial.print(": ");
//    Serial.println(brightness[i]); 
  }
}

/*******************************************************************/
void check_button()
{
  // Check if K control is triggered
  if(digitalRead(BUTTON) == 0)
  {
    // Applay changes to K after button was clicked to approve
    if(is_changing_K)
    {
      // DEBUG
      Serial.println("K change sequence ended.");

      button_down_counter = 0;
      delay(200);
      is_changing_K = false;
      set_program();
    }
    else
    {
      // Button is pushed without K control
//      if(U1 || U2 || U3)
//      {
//        button_down_counter += 1.5;
//      }
//      else { button_down_counter += 0.1; }
      if(U3)
      {
        button_down_counter += 0.5;
      }
      else
      {
        button_down_counter += 0.1; 
      }
      Serial.println(button_down_counter);
  
      // Button is down for long push (K control)
      if(button_down_counter >= 30)
      {
        if(U3)                // Change program by long click in program U3
        {
          Serial.println("Changing program from U3");
          delay(1000);
          led_program++;
          button_down_counter = 0;
          set_program();
        }
        else
        {
          set_color(255, 255, 255);
          // DEBUG
          Serial.println("K change sequence triggered");
          delay(500);
  
          is_changing_K = true;
          button_down_counter = 0;
          // K change sequence
          set_K();
        }
      } 
    }
  }
  else    
  {
    if(!is_changing_K)
    {
      // Short click for LED program
      if(button_down_counter > 1)
      {
        if(U3)              // Program U3 light toggle
        {
          button_down_counter = 0;
          light = !light;
          Serial.print("Now light is: ");
          Serial.println(light);
        }
        else
        {
          // DEBUG
          Serial.println("Change LED mode triggered");
    
          // Change LED program
          led_program++;
          button_down_counter = 0;
          set_program();
          was_change = true;
          //change_program = true;
          //thr_controller.remove(&led_thr);
        }
      }
    }
    else
    {
      is_changing_K = true;
    }
  }
}

/*******************************************************************/
void check_encoder()
{
  encoder.tick();

  // Read position from encoder
  int new_pos = encoder.getPosition();

  // Position has changed
  if(new_pos != last_pos)
  {
    if(new_pos < last_pos)    // Clockwise turn
    {
      Serial.println(new_pos);
      Serial.println(is_changing_K);
      if(is_changing_K)       // K is being set -> K++
      {
        if(K < 6) { Serial.println("K++"); K += 1; set_K(); }
      }
      else                    // Brightnes is being set -> Bright++
      {
        if(bright_offset < (2 * pow(2, K) - N_LED))
        {
          Serial.print("Bright++: ");
          bright_offset++;
        }
        // set_brightness();
      }
    }
    else                      // Counterclockwise turn
    {
      Serial.println(new_pos);
      if(is_changing_K)       // K is being set -> K--
      {
        if(K > 2) { Serial.println("K--"); K -= 1; set_K(); }
      }
      else                    // Brightnes is being set -> Bright--
      {
        if(bright_offset > 0 /** 2*/) 
        {
          Serial.print("Bright--: ");
          bright_offset--; 
        }
        // set_brightness();
      }
    }
    Serial.println(bright_offset);
    last_pos = new_pos;       // Set current position as old one bor next turn
    was_change = true;
  }
  else
  {
    was_change = false;
  }
}

/*******************************************************************/
void set_K()
{
  is_changing_K = true;

  // Debug
  Serial.print("In setK(): K is currently: ");
  Serial.println(K);
  Serial.print("Current isChangingK:");
  Serial.println(is_changing_K);

  for(int i = 0; i < N_LED; i++)
  {
    strip.setPixelColor(i, 0,0,0);
  }

  for(int i = 0; i < (int)K; i++)
  {
    strip.setPixelColor(i, 255,255,255);
    strip.show();
  }

  // set_brightness();
}

/*******************************************************************/
void set_program()
{
  switch(led_program % 9)
    {
      case 0:
        strip.setBrightness(TMP_BRIGHT);
        //Serial.println("program_R");
        U1 = false;
        U2 = false;
        U3 = false;
        program_R();
        break;
      case 1:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_G");
        U1 = false;
        U2 = false;
        U3 = false;
        program_G();
        break;
      case 2:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_B");
        U1 = false;
        U2 = false;
        U3 = false;
        program_B();
        break;
      case 3:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_RG");
        U1 = false;
        U2 = false;
        U3 = false;
        program_RG();
        break;
      case 4:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_RB");
        U1 = false;
        U2 = false;
        U3 = false;
        program_RB();
        break;
      case 5:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_GB");
        U1 = false;
        U2 = false;
        U3 = false;
        program_GB(); 
        break;
      case 6:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_U1");
        U1 = true;
        U2 = false;
        U3 = false;
        //program_U1();
        break;
      case 7:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_U2");
        U1 = false;
        U2 = true;
        U3 = false;
        //program_U2();
        break;
      case 8:
        strip.setBrightness(TMP_BRIGHT);
        Serial.println("program_U3");
        U1 = false;
        U2 = false;
        U3 = true;
        light = true;
        //program_U3();
        break;
      default:
        Serial.println("Nie mam programu o tym numerze");
        break;
    }  
}

/*******************************************************************/
unsigned char calc_brightness(unsigned char i)
{
    if((i + bright_offset) < pow(2, K))
    {
      return MIN_BRIGHT + (((i + bright_offset) * DISTANCE)/(pow(2,K) - 1));
    }
    else
    {
      return MAX_BRIGHT - (((i + bright_offset - pow(2, K)) * DISTANCE)/(pow(2,K) - 1));
    }
}

/*******************************************************************/
/************************ Led Programs *****************************/
/*******************************************************************/
void set_color(int R, int G, int B) // main function, which sets colors on LED strip
{
  int i = 0;
  unsigned char tmpR = 0;
  unsigned char tmpG = 0;
  unsigned char tmpB = 0;
  
  for(i = 0; i < N_LED; i++)
  {
//    Serial.print(i);
//    Serial.print(": calc_bright: ");
//    Serial.println(calc_brightness(i));
    if(!U2)
    {
      tmpR = (R * calc_brightness(i)) / MAX_BRIGHT;
      tmpG = (G * calc_brightness(i)) / MAX_BRIGHT;
      tmpB = (B * calc_brightness(i)) / MAX_BRIGHT;
    }
    else
    {
      tmpR = R;
      tmpG = G;
      tmpB = B;
    }
    
    /*Serial.print(i);
    Serial.print(": ");
    Serial.print(tmpR);
    Serial.print(", ");
    Serial.print(tmpG);
    Serial.print(", ");
    Serial.println(tmpB);*/

    strip.setPixelColor(i, strip.Color(tmpR, tmpG, tmpB));
    strip.show();
  }

  //set_brightness();
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
  if(!is_changing_K)
  {
    int i = 0;
    for(i = 0; i < 8; i++)
    {
      if(i % 2 == 0)
      {
        strip.setBrightness(MAX_BRIGHT);
        strip.setPixelColor(i, strip.Color(255, 0, 0));
        strip.show();
        
        for(int i = 0; i < SAMPLING_RATE; i++)
        {
          // Input check
          check_button();
          check_encoder();
          
          if(was_change)  {   break;  }
        }
        if(is_changing_K) {   return;  }
        
        delay(80);
        strip.setBrightness(0); 
      }
      else
      {
        strip.setBrightness(MAX_BRIGHT);
        strip.setPixelColor(i, strip.Color(0, 0, 255));
        strip.show();
  
        for(int i = 0; i < SAMPLING_RATE; i++)
        {
          // Input check
          check_button();
          check_encoder();
          
          if(was_change)  {   break;  }
        }
        if(is_changing_K) {   return;  }
        
        delay(80);
        
        strip.setBrightness(0); 
      }
    }
  }
}

void program_U2() // user program No. 2
                  // rainbow led lights 
{
  
  if(!is_changing_K)
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
        for(int i = 0; i < SAMPLING_RATE / 100; i++)
        {
          // Input check
          check_button();
          check_encoder();
          
          if(was_change)  {   break;  }
        }
        if(is_changing_K) {   return;  }
          
        rainbow_array[i] -= 1;
        rainbow_array[j] += 1;

        strip.setBrightness(TMP_BRIGHT);
        set_color(rainbow_array[0], rainbow_array[1], rainbow_array[2]);
      }
    }  
  }
      
}

void program_U3(int starting_led, bool up) // user program No. 3
{
  if(up)       // up the led
  {
    for(int i = starting_led; i < 8; i++)
    {
      if(light == true)
      {
        curr_u3_led = i;
        strip.setBrightness(MAX_BRIGHT);
        strip.setPixelColor(i, strip.Color(0, 0, 255));
        strip.show();
        for(int i = 0; i < SAMPLING_RATE / 1000; i++)
        {
          // Input check
          check_button();
          check_encoder();
          
          if(was_change)  {   break;  }
        }
        delay(100);
        strip.setBrightness(0);
      }
      else 
      {
        continue;
      }
    }  
    if(curr_u3_led == N_LED - 1)
    {
      is_up = false;
    }
  }
  else          // down the led
  {
    for(int j = starting_led; j >= 0; j--)
    {
      if(light == true)
      {
        curr_u3_led = j;
        if(j != 7)
        {
          strip.setBrightness(MAX_BRIGHT);
          strip.setPixelColor(j, strip.Color(0, 0, 255));
          strip.show();
          for(int i = 0; i < SAMPLING_RATE / 1000; i++)
          {
            // Input check
            check_button();
            check_encoder();
            
            if(was_change)  {   break;  }
          }
          delay(100);
          strip.setBrightness(0);
        }
        else
        {
          strip.setBrightness(MAX_BRIGHT);
          strip.setPixelColor(j, strip.Color(0, 0, 255));
          strip.show();
          strip.setBrightness(0);
        }
      }
      else
      {
        continue;
      }
    }
    if(curr_u3_led == 0)
    {
      is_up = true;  
    }
  }
}




