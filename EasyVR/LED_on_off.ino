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

//Groups and Commands
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

pinMode(11,OUTPUT); //Initialize LED.
digitalWrite(11,LOW); //Turn LED off.
  
  // setup PC serial port
  pcSerial.begin(9600);

  // bridge mode?
  int mode = easyvr.bridgeRequested(pcSerial);
  switch (mode)
  {
  case EasyVR::BRIDGE_NONE:      //This is the case statement used.
    // setup EasyVR serial port
    port.begin(9600);
    // run normally
    pcSerial.println(F("---"));
    pcSerial.println(F("Bridge not started!"));
    Serial.println("*** case EasyVR::BRIDGE_NONE: ***");
    break;
    
  case EasyVR::BRIDGE_NORMAL:
    // setup EasyVR serial port (low speed)
    port.begin(9600);
    // soft-connect the two serial ports (PC and EasyVR)
    easyvr.bridgeLoop(pcSerial);
    // resume normally if aborted
    pcSerial.println(F("---"));
    pcSerial.println(F("Bridge connection aborted!"));
     Serial.println("*** case EasyVR::BRIDGE_NORMAL: ***");
    break;
    
  case EasyVR::BRIDGE_BOOT:
    // setup EasyVR serial port (high speed)
    port.begin(115200);
    // soft-connect the two serial ports (PC and EasyVR)
    easyvr.bridgeLoop(pcSerial);
    // resume normally if aborted
    pcSerial.println(F("---"));
    pcSerial.println(F("Bridge connection aborted!"));
    Serial.println("*** case EasyVR::BRIDGE_BOOT: ***");
    break;
  
  } //end case.



  while (!easyvr.detect())
  {
    Serial.println("EasyVR not detected!");
    delay(1000);
  } //end while

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5); //Set the number of seconds to listen for each command.
  easyvr.setLanguage(0); //Set language to English

  group = EasyVR::TRIGGER; //<-- start group (customize)

 easyvr.playSound(10, EasyVR::VOL_FULL); //Play woman asking asking question.
 Serial.println("Now exit setup...");

} //end setup.




void action();


//******************************************************************************
void loop()
{

 Serial.println("Now in loop...");
  
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)

 

  Serial.print("Say a command in Group ");
  Serial.println(group);
  easyvr.recognizeCommand(group); //Recognition of a built-in word.

  do
  {
    // can do some processing while waiting for a spoken command
  }
  while (!easyvr.hasFinished());//Polls the status of on-going recogntion,training or asynchronous playback tasks.
  
  easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

     easyvr.setMicDistance(3);//Set mic operating distance. 1 nearest - 3 farthest.
     easyvr.setLevel(1);//Set strictness level for recognition of custom commands.
     easyvr.setKnob(0);//Set confidence threshold for recognition of built-in words.



  idx = easyvr.getWord();//Get recognised word index from built-in sets or custom grammars.
  if (idx >= 0)
  {
    // built-in trigger (ROBOT)
    // group = GROUP_X; <-- jump to another group X
 
    return;
  } //end if.

  idx = easyvr.getCommand();//Get the recognised command index if any.

  if (idx >= 0)
  {
    // print debug message
 
    uint8_t train = 0;
    char name[32];
    Serial.print("Command: ");
    Serial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train)) //Retrieves the name and training data of a custom command.
    //Group = 0-16 target group or values in #Groups.
    //idx = 0-31 index of the command within the selected group.
    //name = points to array of at least 32 characters that holds command label when function returns.
    
    {
      Serial.print(" = ");
      Serial.println(name);
    } //end if.
    else
      Serial.println();
    easyvr.playSound(0, EasyVR::VOL_FULL); //Play bell sound.


    
    // perform some action
    
    action(); //Turn LED on or off.

  } //end if.
 
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      Serial.println("Timed out, try again...");
    int16_t err = easyvr.getError(); //Gets last error code. 0-255.
    if (err >= 0)
    {
      Serial.print("Error ");
      Serial.println(err, HEX);
      
    } //end if.

  } //end else.

} //end loop.


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
        // write your action code here
         group = GROUP_1; //<-- or jump to another group X for composite commands
        break;

      } //end case GROUP_0.
      break;
      
    case GROUP_1:
      switch (idx)
      {
      case G1_ON:
        // write your action code here
        digitalWrite(11, HIGH);   // set the LED on
        // group = GROUP_X; <-- or jump to another group X for composite commands
           easyvr.playSound(0, EasyVR::VOL_FULL); //Play bell sound.
        break;

      case G1_OFF:
        // write your action code here
        digitalWrite(11, LOW);    // set the LED off
        // group = GROUP_X; <-- or jump to another group X for composite commands
           easyvr.playSound(11, EasyVR::VOL_FULL); //Play Dave sound.
        break;

      } //end idx case GROUP_1.

      break;
    
    } //end group case stmt.

} //end action.
