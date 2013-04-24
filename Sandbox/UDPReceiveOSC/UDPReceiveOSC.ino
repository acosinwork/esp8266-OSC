
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    

#include <SoftPWM.h>
#include <OSCBundle.h>

/*
* Set the LED according to incoming OSC control
* This compiles correctly on Teensy 2.0 and fails
* 

*/
#include <stdlib.h>



//UDP communication


EthernetUDP Udp;
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // you can find this written on the board of some Arduino Ethernets or shields

//the Arduino's IP
// pull thise from board? xxx
IPAddress ip(128, 32, 122, 252);

//port numbers
const unsigned int inPort = 8888;

void LEDcontrol(OSCMessage &msg)
{
 // I had to add this to make it work on Leonardo: static const int LEDBUILTIN=13;
      if (msg.isInt(0))
      {
             pinMode(LED_BUILTIN, OUTPUT);
             digitalWrite(LED_BUILTIN, (msg.getInt(0) > 0)? HIGH: LOW);
       }
       else if(msg.isString(0))
       {
         int length=msg.getDataLength(0);
         if(length<5)
         {
           char str[length];
           msg.getString(0,str,length);
           if((strcmp("on", str)==0)|| (strcmp("On",str)==0))
           {
                pinMode(LED_BUILTIN, OUTPUT); 
                digitalWrite(LED_BUILTIN, HIGH);
           }
           else if((strcmp("Of", str)==0)|| (strcmp("off",str)==0))
           {
                pinMode(LED_BUILTIN, OUTPUT); 
                digitalWrite(LED_BUILTIN, LOW);
           }
         }
       }
      else  if (msg.isFloat(0))
      {
        //test if that pin is a PWM
        if (digitalPinHasPWM(LED_BUILTIN)) // missingn on leonardo
        {
          pinMode(LED_BUILTIN, OUTPUT);
          analogWrite(LED_BUILTIN, (int)(msg.getFloat(0)*127));
        }
        else
         SoftPWMSet(LED_BUILTIN, (int)(msg.getFloat(0)*127));
      } 
   
}


void setup() {
  SoftPWMBegin();
  //setup ethernet part
  Ethernet.begin(mac,ip);
  Udp.begin(inPort);

}
//reads and dispatches the incoming message
void loop(){ 
    OSCBundle bundleIN;
   int size;
 
   if( (size = Udp.parsePacket())>0)
   {
     while(size--)
       bundleIN.fill(Udp.read());

      if(!bundleIN.hasError())
       bundleIN.dispatch("/led", LEDcontrol);
   }
}


