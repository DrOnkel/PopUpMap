/*********************************************************
  This is a library for the MPR121 12-channel Capacitive touch sensor

  Designed specifically to work with the MPR121 Breakout in the Adafruit shop
  ----> https://www.adafruit.com/products/

  These sensors use I2C communicate, at least 2 pins are required
  to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>


#include "Adafruit_MPR121.h"

// include libs for work with MPR121 and i2c lib



#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif


int ModeSwCount;   // Counter for button "Mode" anti bounce
byte ModeFlag;    //Delay Flag for Mode text play

#define MODETIMERLIMIT 50                // Threshold for  anti bounce 


#define  MUTELIMIT  7   // Mute timer threshold for delay of amplifier
#define THRESHOLD_UP 30     //Distance in sm for ultrasound sensor
#define THRESHOLD_DN 30     //Distance in sm for ultrasound sensor

#define VOLUME 24   // Initial value of Volume

#define USPAUSETIME 10000  //Time im ms of ultrasound sensor block

#define USFORPAUSETIME 10000   //Time im ms of ultrasound sensor block


//  MP3 Play Mussi Begin
#define SONGBEGIN 102   // The Namber of the first non-text file (for play)


byte Ordnung[4] = {0, 1, 0, 1}, Schlange[4];    // Service Values vor text to play buffer
byte NofSong = SONGBEGIN, SongMode = 0, LastSong, OldSongMode; // Ititial parameters of player



// You can have up to 4 on one i2c bus. We have 2
Adafruit_MPR121 capA = Adafruit_MPR121();
Adafruit_MPR121 capB = Adafruit_MPR121();



// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouchedA = 0;
uint16_t currtouchedA = 0;


byte MuteCount;  // Counter of Mute delay
byte Mode;      // Mode - Geography, Culture, Music, in other Languge

uint16_t lasttouchedB = 0;  //pad last touched
uint16_t currtouchedB = 0;  //pad now touched
byte NofFl[102] = {       // Matrix of mapping of the pad touched and number of file to play

  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, 15, 5, 17, 18, 19, 20, 1, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 29, 41, 42, 43, 44, 13, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
  84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  24, 25, 98, 99, 100, 101

};

// here we define number of pins used for ultrasound sensor
int pinTrig = 4;
int pinLed = 13;


byte LineOfUntouch;  // How many touched pad we have in line
byte DoneR;  // Flag "Sensor pulse is done"
byte HandNear, OldHandNear;   // State of sensor data
unsigned long t0, t1, t2, oldt, Distance, OldDistance; // Service values for ultrasound sensor measuring
long CountMagnetTouch;   // TimeStamp last touch
void PlayText(byte NofSound); // Function "Play file NofSoneun"

void PlayMusic(byte NofSound);  // Function "Play file NofSoneun"  Music Mode


byte LastUnTouch;  // Last pad untouch
byte LastTouch;     // Last pad touch
int LastTouchTime;      // Time from last touch
int TimeFromUS;       // time from Ultrasound Sensor Pulse
long CountForUS, CountFromUS;   // Timers for events sensor data < or > Limit

byte Oldi;    //Service value for data from MPR121




// Set up code - it runs only once at start

void setup() {
  // Pins definition
  pinMode(5, INPUT_PULLUP);

  pinMode(pinLed, OUTPUT);
  pinMode(pinTrig, OUTPUT);

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);


  // interraption program for unltrasound sensor connected to pin 3
  attachInterrupt(digitalPinToInterrupt(3), Countt, FALLING);


  Serial.begin(9600);  // Start Serial Port

  Serial.println("Adafruit MPR121 2 Capacitive Touch sensor test  MPR121   -2 MapWithComments ");   // we print the title of prog

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!capA.begin(0x5A))  // ADDRESS
  {
    Serial.println("MPR121 A not found, check wiring?");
    while (0); // (!Serial.available());
    Serial.println("MPR121  A NOT found! but go anyway");
  }
  else
  {
    Serial.println("MPR121  A found!");
    capA.setThreshholds(8, 4);

  }


  if (!capB.begin(0x5B))  // ADDRESS
  {
    Serial.println("MPR121 B not found, check wiring?");
    while (0); // (!Serial.available());
    Serial.println("MPR121  B NOT found! but go anyway");
  }
  else
  {
    Serial.println("MPR121 B found!");
    capB.setThreshholds(8, 4);
  }


  delay(1000); // delay 1s

  // Set the volume of MP3 player
  delayMicroseconds(10);
  Serial.write(0x7E);
  Serial.write(0xFF);
  Serial.write(0x06);
  Serial.write(0x06);
  Serial.write(0x01);
  Serial.write(0x00);
  Serial.write(VOLUME);
  Serial.write(0xFE);
  Serial.write(0xF4 - VOLUME);

  Serial.write(0xEF);


  // Reset randoom generator for arbitrary music play

  randomSeed(analogRead(0));



}

// the main loop
void loop() {

  if (SongMode == 1) // if we listening music
  {

    if (  digitalRead(9) == HIGH    || OldSongMode == 0) // if MP3 Player is not busy
    {

      delay(10);
      PlayMusic(random(SONGBEGIN, 240));

      //if( ++NofSong>240)
      //NofSong=SONGBEGIN;

      OldSongMode = 1;
      delay(100);
    }





  }









  if (digitalRead(5) == 0) // if touch button Mode
  {
    if (( ModeSwCount++ > MODETIMERLIMIT) && ( ModeFlag == 0)) // anti bounce
    {
      ModeFlag = 1;
      Mode = (++Mode) % 4;

      PlayText(  NofFl[ 98 + Mode]);  // play the file with mode name

    }
  }

  else
  {

    ModeFlag = 0;
    ModeSwCount = 0;
  }














  if ( DoneR == 2) //Start Ultrasound measuring
  {
    digitalWrite(pinTrig, HIGH);  //Turn the Trigger pulse on
    delay(10);                    //  wate
    digitalWrite(pinTrig, LOW);   //Turn the Trigger Off
    t0 = micros();
    DoneR = 1;
    delay(10);
  }
  else
    delay(10);


  if (DoneR == 0)
    DoneR = 2;
  if (       (HandNear == 1)   &&  (   OldHandNear == 2         ))   // Logic of presence near the sensor
  {
    HandNear = 0;
    //Serial.println ("HandNear 1");

    if (millis() - CountForUS >  USFORPAUSETIME  ) // if block time of invitation is over
    {


      PlayText( NofFl[  96]); // play an invitaiton
      CountForUS = millis();

    }


  }
  if (           (HandNear == 2 )            &&  (   OldHandNear == 1         )       )
  {
    HandNear = 0;
    //Serial.println ("HandNear 2");
    if (millis() - CountFromUS >  USPAUSETIME  )
    {
      PlayText(  NofFl[ 98 + Mode]);
      CountFromUS = millis();
    }

  }

  OldHandNear =  HandNear;

  if ((   (PINB & 0x04) == 0x04    )   &&   LineOfUntouch > 0  )
  {
    ;// PlayText(24+LineOfUntouch-1);
    LineOfUntouch = 0;
  }









  // Get the currently touched pads
  currtouchedA = capA.touched();

  for (uint8_t i = 0; i < 12; i++)
  {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouchedA & _BV(i)) && !(lasttouchedA & _BV(i)) )
    {

      if ((   (PINB & 0x04) == 0x04    )  || ( Oldi != i ))
      {
        PlayText(i  + Mode * 24);
        CountMagnetTouch = millis();

        Schlange[3] = Schlange[2];
        Schlange[2] = Schlange[1];

        Schlange[1] = Schlange[0];
        Schlange [0] = i  + Mode * 24;

        SongMode = 1;
        OldSongMode = 0;
        for (byte jSong = 0; jSong < 4; jSong++)
        {
          if (Schlange[jSong] != Ordnung[jSong])
            SongMode *= 0;
          Serial.begin(9600);
          delay(5);
          Serial.print("  ");
          Serial.print(Schlange[jSong]);
          // byte Ordnung[4]={0,1,0,1},Schlange[4];
        }



        Serial.print("SongMode");
        Serial.println(SongMode);

        Serial.end();

        Oldi = i;
      }
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouchedA & _BV(i)) && (lasttouchedA & _BV(i)) )
    {

      {

        if ((    (PINB & 0x04) == 0x04    )  )
        {
          ;// PlayText(24+i);

        }
        else
        {
          LineOfUntouch = i + 1;
        }
      }
    }
  }

  // reset our state
  lasttouchedA = currtouchedA;



  // Get the currently touched pads B
  currtouchedB = capB.touched();

  for (uint8_t i = 0; i < 12; i++)
  {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouchedB & _BV(i)) && !(lasttouchedB & _BV(i)) )
    {

      if ((  (PINB & 0x04) == 0x04    )  || ( Oldi != i + 12 ))
      {
        PlayText(i + 12 + Mode * 24 );
        CountMagnetTouch = millis();


        Schlange[3] = Schlange[2];
        Schlange[2] = Schlange[1];

        Schlange[1] = Schlange[0];
        Schlange [0] = i  + Mode * 24;

        SongMode = 1;
        OldSongMode = 0;
        for (byte jSong = 0; jSong < 4; jSong++)
        {
          if (Schlange[jSong] != Ordnung[jSong])
            SongMode *= 0;
          Serial.begin(9600);
          delay(5);
          Serial.print("  ");
          Serial.print(Schlange[jSong]);
          // byte Ordnung[4]={0,1,0,1},Schlange[4];
        }



        Serial.print("SongMode");
        Serial.println(SongMode);

        Serial.end();













        Oldi = i + 12;
      }
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouchedB & _BV(i)) && (lasttouchedB & _BV(i)) )
    {

      {

        if ((  (PINB & 0x04) == 0x04    )  )
        {
          ;// PlayText(12+i+24);

        }
        else
        {
          LineOfUntouch = i + 1 + 12;
        }
      }
    }
  }

  // reset our state
  lasttouchedB = currtouchedB;


  if (  digitalRead(9) == HIGH   )
  {
    MuteCount++;
  }
  else
  {
    CountForUS = millis();
    CountFromUS = CountForUS;
  }

  if (MuteCount > MUTELIMIT)
  {
    digitalWrite(10, HIGH);

  }



  // comment out this line for detailed data from the sensor!
  return;

  // debugging info, what
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(capA.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(capA.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(capA.baselineData(i)); Serial.print("\t");
  }
  Serial.println();

  // debugging info, what
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(capB.touched(), HEX);
  Serial.print("FilB: ");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(capB.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("BasB: ");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(capB.baselineData(i)); Serial.print("\t");
  }
  Serial.println();






  // put a delay so it isn't overwhelming
  //delay(10);
}
// This interraption function works when the trigger pulse fall down
void Countt(void)
{



  OldDistance = Distance;
  Distance = ( micros() - t0) * 330 / 20000        ;
  DoneR = 0;
  //Serial.println(Distance    );
  if ( (Distance > THRESHOLD_UP)    )

  {
    HandNear = 1;
    digitalWrite(pinLed, HIGH);
  }

  if ( (Distance < THRESHOLD_DN)    )
  {
    HandNear = 2;
    digitalWrite(pinLed, LOW);

  }


}

// This function play sound, see mp3 player datasheet

void PlayText(byte NofSound)
{
  digitalWrite(10, LOW);
  delay(1);

  Serial.begin(9600);
  Serial.write( 0x7E);
  Serial.write( 0xFF);
  Serial.write( 0x06);
  Serial.write( 0x03);
  Serial.write( 0x00);
  Serial.write( 0x00);
  Serial.write( NofFl[NofSound] + 1);
  Serial.write( 0xFE );
  Serial.write( 0xF8 - NofFl[NofSound] - 1);
  Serial.write( 0xEF);
  Serial.println("");
  Serial.print("NofS= ");
  Serial.println(NofSound);

  delay(12);
  //return;
  Serial.end();
  DDRD &= ~0x02;
  //if(NofSound !=96)
  CountForUS = millis();
  CountFromUS = CountForUS;
  MuteCount = 0;


}


// This function play sound, see mp3 player datasheet
void PlayMusic(byte NofSound)
{
  digitalWrite(10, LOW);
  delay(1);

  Serial.begin(9600);
  Serial.write( 0x7E);
  Serial.write( 0xFF);
  Serial.write( 0x06);
  Serial.write( 0x03);
  Serial.write( 0x00);
  Serial.write( 0x00);
  Serial.write( NofSound + 1);
  Serial.write( 0xFE );
  Serial.write( 0xF8 - NofSound - 1);
  Serial.write( 0xEF);
  Serial.println("");
  Serial.print("NofS= ");

  Serial.print(NofSound);

  Serial.print("SongMode= ");

  Serial.println(SongMode);





  delay(12);
  //return;
  Serial.end();
  DDRD &= ~0x02;
  //if(NofSound !=96)
  CountForUS = millis();
  CountFromUS = CountForUS;
  MuteCount = 0;
  delay(100);

}











