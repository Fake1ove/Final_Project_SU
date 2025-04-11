#include "Arduino.h"
#if !defined(SERIAL_PORT_MONITOR)
#endif

#if defined(SERIAL_PORT_USBVIRTUAL)
  // Shield Jumper on HW (for Leonardo and Due)
  #define port SERIAL_PORT_HARDWARE
  #define pcSerial SERIAL_PORT_USBVIRTUAL
#else
  // Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
  #include "SoftwareSerial.h"
  SoftwareSerial port(12, 13);
  #define pcSerial SERIAL_PORT_MONITOR
#endif

#include "EasyVR.h"

#pragma once   //Stops multiple opening of same .h files.

EasyVR easyvr(port);

// Groups and Commands
enum Groups
{
  GROUP_0  = 0,
  GROUP_1  = 1,
};

enum Group0 
{
  G0_LED = 0,
};

enum Group1 
{
  G1_ON = 0,
  G1_OFF = 1,
};

int8_t group, idx;

//******************************************************************************

void setup()
{
  pinMode(11,OUTPUT); // Initialize LED.
  digitalWrite(11,LOW); // Turn LED off.

  // setup PC serial port with 115200 baud rate
  pcSerial.begin(115200);

  // bridge mode?
  int mode = easyvr.bridgeRequested(pcSerial);
  switch (mode)
  {
    case EasyVR::BRIDGE_NONE:
      port.begin(9600);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge not started!"));
      Serial.println("*** case EasyVR::BRIDGE_NONE: ***");
      break;

    case EasyVR::BRIDGE_NORMAL:
      port.begin(9600);
      easyvr.bridgeLoop(pcSerial);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge connection aborted!"));
      Serial.println("*** case EasyVR::BRIDGE_NORMAL: ***");
      break;

    case EasyVR::BRIDGE_BOOT:
      port.begin(115200);
      easyvr.bridgeLoop(pcSerial);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge connection aborted!"));
      Serial.println("*** case EasyVR::BRIDGE_BOOT: ***");
      break;
  }

  while (!easyvr.detect()) {
    Serial.println("EasyVR not detected!");
    delay(1000);
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER;

  easyvr.playSound(10, EasyVR::VOL_FULL);
  Serial.println("Now exit setup...");
} // end setup.

//******************************************************************************

void action();

void loop()
{
  Serial.println("Now in loop...");
  
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)

  Serial.print("Say a command in Group ");
  Serial.println(group);
  easyvr.recognizeCommand(group); // Recognition of a built-in word.

  do
  {
    // can do some processing while waiting for a spoken command
  }
  while (!easyvr.hasFinished()); // Polls the status of ongoing recognition

  easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  easyvr.setMicDistance(3); // Set mic operating distance. 1 nearest - 3 farthest.
  easyvr.setLevel(1); // Set strictness level for recognition of custom commands.
  easyvr.setKnob(0); // Set confidence threshold for recognition of built-in words.

  idx = easyvr.getWord(); // Get recognized word index from built-in sets or custom grammars.
  if (idx >= 0)
  {
    return;
  } // end if.

  idx = easyvr.getCommand(); // Get the recognized command index if any.

  if (idx >= 0)
  {
    // print debug message
    uint8_t train = 0;
    char name[32];
    Serial.print("Command: ");
    Serial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train)) // Retrieves the name and training data of a custom command.
    {
      Serial.print(" = ");
      Serial.println(name);
    } // end if.
    else
      Serial.println();
    easyvr.playSound(0, EasyVR::VOL_FULL); // Play bell sound.

    // perform some action
    action(); // Turn LED on or off.
  } // end if.
 
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      Serial.println("Timed out, try again...");
    int16_t err = easyvr.getError(); // Gets last error code. 0-255.
    if (err >= 0)
    {
      Serial.print("Error ");
      Serial.println(err, HEX);
    } // end if.
  } // end else.
} // end loop.

//******************************************************************************

void action()
{
  Serial.println("Now in action function...");

  switch (group)
  {
    case GROUP_0:
      switch (idx)
      {
        case G0_LED:
          group = GROUP_1;
          break;
      }
      break;

    case GROUP_1:
      switch (idx)
      {
        case G1_ON:
          digitalWrite(11, HIGH);   // set the LED on
          Serial.println("G1_ON");  // Send "1" at 115200 baud
          easyvr.playSound(0, EasyVR::VOL_FULL); // Play bell sound.
          break;

        case G1_OFF:
          digitalWrite(11, LOW);    // set the LED off
          Serial.println("G1_OFF");  // Send "0" at 115200 baud
          easyvr.playSound(11, EasyVR::VOL_FULL); // Play Dave sound.
          break;
      }
      break;
  }
} // end action.
