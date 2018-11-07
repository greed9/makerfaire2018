
#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11

#define TRIGGER_PIN 8

#define FADESPEED 15     // make this higher to slow down

void flash ( int dly, int nTimes, int r, int g, int b )
{
  for ( int i = 0 ; i < nTimes ; i++)
  {
    analogWrite ( REDPIN, r ) ;
    analogWrite ( GREENPIN, g ) ;
    analogWrite ( BLUEPIN, b ) ;
    delay ( dly ) ;

    analogWrite ( REDPIN, 0 ) ;
    analogWrite ( GREENPIN, 0 ) ;
    analogWrite ( BLUEPIN, 0) ;
    delay ( dly ) ;
  }
}

void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode ( TRIGGER_PIN, INPUT_PULLUP ) ;
}

void loop() {
  int r, g, b;
  //Set red to full brightness
  analogWrite(REDPIN, 255);

  // fade from red to yellow
  for (g = 0; g < 256; g++) {
    analogWrite(GREENPIN, g);
    if ( digitalRead ( TRIGGER_PIN ) == LOW )
    {
      flash( 200, 10, 0, 0, 255 ) ;
    }
    delay(FADESPEED);
  }
  
  // fade from yellow to red
  for (g = 255; g > 0; g--) {
    analogWrite(GREENPIN, g);
    if ( digitalRead ( TRIGGER_PIN ) == LOW )
    {
      flash( 200, 10, 0, 0, 255 ) ;
    }
    delay(FADESPEED);
  }

}
