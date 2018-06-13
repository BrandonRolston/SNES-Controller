/*
 * Bluetooth Control Prototype
 * Author: Brandon Rolston
 * 
 * Description: This lightweight sketch allows a user to control a Super Nintendo Entertainment System
 * using a bluetooth controller.
 *
 */
 
#include <Wire.h>

// Bitwise definitions
#define CLR(x,y) (x&=(~(1<<y))) // x is the port, y is the place within that port to change
#define SET(x,y) (x|=(1<<y))    // ex: CLR(PORTC,0) would set Digital Pin 53 LOW

/* PIN DEFINITIONS */
const int CLOCK_PIN = 2; // Pin for SNES Control Clock
const int LATCH_PIN = 3; // Pin for SNES Latch Control
const int DATA_PIN = 27; // Located on Port A, position 5

/* VARIABLES */
volatile int state;  // Tracks the state of the bluetooth control inputs (Bits 0-12)
volatile int packet; // The value that is "latched" when the SNES sends a latch command
char command[256]; // Maximum Bluetooth packet size
int index = 0; // Used in error detection over Bluetooth
byte Si = 0;   // SNES Clock tracking variable

/* SETUP */
void setup()
{
  // Bluetooth Setup
  Serial2.begin(115200);

  // Controller Setup
  pinMode(CLOCK_PIN, INPUT);
  pinMode(LATCH_PIN, INPUT);
  pinMode(DATA_PIN, OUTPUT); digitalWrite(DATA_PIN, LOW);

  // All registers, by default, start high
  state = 0xFFFF;
  packet = 0xFFFF;

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(LATCH_PIN) , latch, FALLING); // Latch Detection
  attachInterrupt(digitalPinToInterrupt(CLOCK_PIN) , data, RISING);   // Data Clock Detection
}

void loop()
{
  /* BLUETOOTH SECTION */
  if(Serial2.available())  // If the bluetooth sent any characters
    {
      command[index] = (char)Serial2.read();  // Add the character to the message buffer
      if (command[index] == 0x3F) {             // Check for the "?" character
        char button_press = command[index-1];   // go back from the character and read command
        /* BUTTON REGISTERS */
        switch(button_press)
        {
          /* B BUTTON */
          case 0x62: // 'b'
            bitWrite(state, 0, 0);
            break;
          case 0x42: // 'B'
            bitWrite(state, 0, 1);
            break;
          /* Y BUTTON */
          case 0x79: // 'y'
            bitWrite(state, 1, 0);
            break;
          case 0x59: // 'Y'
            bitWrite(state, 1, 1);
            break;
          /* SELECT */
          case 0x2d: // '-'
            bitWrite(state, 2, 0);
            break;
          case 0x5f: // '_'
            bitWrite(state, 2, 1);
            break;
          /* START */
          case 0x73: // 's'
            bitWrite(state, 3, 0);
            break;
          case 0x53: // 'S'
            bitWrite(state, 3, 1);
            break;
          /* UP BUTTON */
          case 0x75: // 'u'
            bitWrite(state, 4, 0);
            break;
          case 0x55: // 'U'
            bitWrite(state, 4, 1);
            break;
          /* DOWN BUTTON */
          case 0x64: // 'd'
            bitWrite(state, 5, 0);
            break;
          case 0x44: // 'D'
            bitWrite(state, 5, 1);
            break;
          /* LEFT BUTTON */
          case 0x6C: // 'l'
            bitWrite(state, 6, 0);
            break;
          case 0x4C: // 'L'
            bitWrite(state, 6, 1);
            break;
          /* RIGHT BUTTON */
          case 0x72: // 'r'
            bitWrite(state, 7, 0);
            break;
          case 0x52: // 'R'
            bitWrite(state, 7, 1);
            break;
          /* A BUTTON */
          case 0x61: // 'a'
            bitWrite(state, 8, 0);
            break;
          case 0x41: // 'A'
            bitWrite(state, 8, 1);
            break;
          /* X BUTTON */
          case 0x78: // 'x'
            bitWrite(state, 9, 0);
            break;
          case 0x58: // 'X'
            bitWrite(state, 9, 1);
            break;
          /* L BUTTON */
          case 0x31: // '1'
            bitWrite(state, 10, 0);
            break;
          case 0x32: // '2'
            bitWrite(state, 10, 1);
            break;
          /* R BUTTON */
          case 0x33: // '3'
            bitWrite(state, 11, 0);
            break;
          case 0x34: // '4'
            bitWrite(state, 11, 1);
            break;
          default:  // Something unexpected happened
          index = 0; // Let's clear whatever that was. 
          break;
        }
        //index = 0; // Reset the message buffer index
      }
      else 
      {
        index++; // Keep ticking the index forward until you see that '?'
      }
      
    }
}

//Latch interrupt routine

void latch()
{
  // If the button is unpressed
  packet = state;
  if ((((state) >> (0)) & 0x01) == 1)
  {
    // Set our pin high
    SET(PORTA,5);
  }
  else
  {
    // Otherwise set it low
    CLR(PORTA,5);
  }
}

//Data interrupt routine

void data()
{
  //increment clock signal counter
  Si++;

  if ((((packet) >> (Si)) & 0x01) == 1)
  {
    // Set our pin high
    SET(PORTA,5);
  }
  else
  {
    // Otherwise set it low
    CLR(PORTA,5);
  }
  if (Si >= 16) 
  {
    Si = 0;
  }
}
