#include <MeccaBrain.h>


//*****************************************
// Must #define RIGHT_GATE for right-side obstacle, otherwise leave
// undefined for left-side obstacle
//*****************************************

//#define RIGHT_GATE 1
#undef RIGHT_GATE

#define MAX_BRIGHTNESS 120

int delayval = 100; // delay for 15th sec
uint8_t  irPin = 2 ; // Adafruit beam break sensor out

// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            7
#define SERVOPIN       5

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24
//#define STEP 10
/*This changes how fast the servo moves by changing how far it moves on each loop.
   It has less to do with speed and more to do with position of the servo.
   If you see weird issues, or your servos don't change color you probably
   have a power issue.
*/

// Abstracts a NeoPixel ring
// https://www.adafruit.com/product/1586
//
class Ring
{
    int lastPos = 0 ;
    int numPixels = 0 ;
    int backR = 0 ;
    int backG = 0 ;
    int backB = 0 ;

  public:

    Adafruit_NeoPixel& pixels ;

    Ring ( Adafruit_NeoPixel& pPixels, int pNumPixels ) : pixels( pPixels )
    {
      numPixels = pNumPixels ;
      pixels.begin(); // This initializes the NeoPixel library.
    }

    // All on
    void allOn ( int r, int g, int b )
    {
      for ( int i = 0 ; i < NUMPIXELS ; i++ )
      {
        pixels.setPixelColor( i, pixels.Color( r, g, b ) ) ;
        pixels.show ( ) ;
      }
    }

    // All off
    void allOff ( )
    {
      allOn( 0, 0, 0 ) ;
    }

    // Move one step
    void oneStep ( int dir, int r, int g, int b )
    {
      int newPos ;

      if ( dir )
      {
        newPos = lastPos + 1 ;
        if ( newPos > ( numPixels - 1 )  )
        {
          newPos = 0 ;
        }
      }
      else
      {
        newPos = lastPos - 1 ;
        if ( newPos < 0 )
        {
          newPos = numPixels - 1  ;
        }
      }

      Serial.println ( newPos ) ; // debugging

      // Set and show the colored pixel
      pixels.setPixelColor(newPos, pixels.Color(r, g, b));
      pixels.show(); // This sends the updated pixel color to the hardware.

      // Erase the previous pixel in the ring by setting it to background (usually off)
      pixels.setPixelColor(lastPos, pixels.Color(backR, backG, backB));
      pixels.show ( ) ;

      lastPos = newPos ;
    } // end oneStep

    // rotate by nTimes, in a given direction, with specified color
    void spin ( int nTimes, int dir, int r, int g, int b  )
    {
      for ( int i = 0 ; i < nTimes * numPixels ; i++ )
      {
        oneStep ( dir, r, g, b ) ;
        delay ( 50 ) ;
      }

    } // end spin

    // rotate both this ring, and the other ring passed as a parameter
    void spin2 ( int nTimes, int dir, int r, int g, int b, Ring& otherRing )
    {
      for ( int i = 0 ; i < nTimes * numPixels ; i++ )
      {
        oneStep ( dir, r, g, b ) ;
        otherRing.oneStep ( dir, r, g, b ) ;
        delay ( 50 ) ;
      }

    } // end spin

    void strobe ( int nTimes, int r, int g, int b )
    {
      for ( int i = 0 ; i < nTimes ; i++ )
      {
        allOn ( r, g, b ) ;
        delay ( 100 ) ;
        allOff ( ) ;
        delay ( 100 ) ;
      }
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos) {
      WheelPos = 255 - WheelPos;
      if (WheelPos < 85) {
        return pixels.Color(MAX_BRIGHTNESS - WheelPos * 3, 0, WheelPos * 3);
      }
      if (WheelPos < 170) {
        WheelPos -= 85;
        return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      WheelPos -= 170;
      return pixels.Color(WheelPos * 3, MAX_BRIGHTNESS - WheelPos * 3, 0);
    }

    //Theatre-style crawling lights with rainbow effect
    void theaterChaseRainbow(uint8_t wait ) {
      for (int j = 0; j < MAX_BRIGHTNESS ; j++) {   // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++) {
          for (int i = 0; i < pixels.numPixels(); i = i + 3) {
            pixels.setPixelColor(i + q, Wheel( (i + j) % MAX_BRIGHTNESS)); //turn every third pixel on
          }
          pixels.show();

          if ( digitalRead ( irPin ) == LOW )
          {
            return ;
          }
          else
          {
            delay(wait);
          }

          for (int i = 0; i < pixels.numPixels(); i = i + 3) {
            pixels.setPixelColor(i + q, 0);      //turn every third pixel off
          }
        }
      }
    }

    void rainbow(uint8_t wait) {
      uint16_t i, j;

      for (j = 0; j < MAX_BRIGHTNESS ; j++) {
        for (i = 0; i < pixels.numPixels(); i++) {
          pixels.setPixelColor(i, Wheel((i + j) & MAX_BRIGHTNESS ));
        }
        pixels.show();
        delay(wait);
      }
    }

} ; // end Ring class

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

// Two ring instances, one controlled by Pin 6, and one by Pin 7. Each can be controlled
// seperately, or they can cooperate, as in the spin2 ( ) method below.
Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(NUMPIXELS, PIN + 1, NEO_GRB + NEO_KHZ800);

// A ring for each
Ring ring1 ( pixels1, 24 ) ;
//Ring ring2 ( pixels2, 24 ) ;

// Mechanno servo - something is weird in this code...
MeccaBrain gServo (SERVOPIN);

// Abstracts a servo as a ball gate - had to make it static, something about
// Meccano's servo class...
class Gate
{

  public:

    static MeccaBrain gServo ;
    static int gatePin ;
    static int closedPos ;
    static int openPos  ;
    static int servoStep ;

    Gate ( int pin )
    {
      //gServo = MeccaBrain ( pin ) ;

    }

    static void init ( )
    {
      pinMode(gatePin, OUTPUT);

      for (int i = 0; i < 51; i++)
      {
        gServo.communicate();
      }

      gServo.communicate();
      gServo.setServoPosition(0, 0);
    }

    static void closeGate ( )
    {
      gServo.setServoColor(0, 0xF4);
      gServo.communicate();

      for (int i = closedPos; i < openPos; i = i + servoStep ) {
        gServo.communicate();
        gServo.setServoPosition(0, i);
      }

    }

    static void openGate ( )
    {
      gServo.setServoColor(0, 0xF2);
      gServo.communicate();
      for (int i = openPos; i > closedPos ; i = i - servoStep) {
        gServo.communicate();
        gServo.setServoPosition(0, i);
      }
    }

} ; // end Gate class

// Something is up with the Meccano servo class that requires I make
// everything static -- ugh initialize everything here.

int Gate::gatePin = SERVOPIN ;

#ifdef RIGHT_GATE
int Gate::closedPos = 120; /*Left:50, Right: 120*/
int Gate::openPos = 225; /*Left:145, Right: 225*/
#else
int Gate::closedPos = 50; /*Left:50, Right: 120*/
int Gate::openPos = 145; /*Left:145, Right: 225*/
#endif

int Gate::servoStep = 10 ;
MeccaBrain Gate::gServo = MeccaBrain ( SERVOPIN ) ;

// For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
// pixels.Color takes RGB values, from 0,0,0 up to 255,255,255

/*Could probably make it check a variable if it's left or right instead of manually recoding the program.*/

void setup() {
  pinMode ( irPin, INPUT_PULLUP ) ;
  Serial.begin( 9600 ) ;  // May be disabling interrupts?
  Serial.println ( "Starting" ) ;

  // In case a ball is stuck.
  Gate::init ( ) ;
  ring1.allOn ( 150, 0, 0 ) ;
  Serial.println ( "Clearing..." ) ;
#ifdef RIGHT_GATE
  Gate::closeGate ( ) ; /*Fliped the order for the right side*/
  delay ( 10000 ) ;
  Gate::openGate ( ) ; /*Fliped the order for the right side*/
#else
  Gate::openGate ( ) ; /*Fliped the order for the right side*/
  delay ( 10000 ) ;
  Gate::closeGate ( ) ; /*Fliped the order for the right side*/
#endif
  ring1.allOn ( 0, 150, 0 ) ;
  delay ( 2000 ) ;
  Serial.println ( "Done clearing." ) ;
  ring1.allOff ( ) ;
}

void loop()
{
  Serial.println ( "in loop" ) ;

  //ring1.oneStep ( 1, 150, 0, 150 ) ;
  ring1.theaterChaseRainbow( 100 ) ;

  if ( digitalRead ( irPin ) == LOW )
  {
    int nSpins = random ( 10 ) + 1 ;
    ring1.rainbow ( nSpins ) ;
#ifdef RIGHT_GATE
    Gate::closeGate ( ) ; /*Fliped the order for the right side*/
    delay ( 2000 ) ;
    Gate::openGate ( ) ; /*Fliped the order for the right side*/
#else
    Gate::openGate ( ) ; /*Fliped the order for the right side*/
    delay ( 2000 ) ;
    Gate::closeGate ( ) ; /*Fliped the order for the right side*/
#endif
  }

  delay(delayval); // Delay for a period of time (in milliseconds).

}
