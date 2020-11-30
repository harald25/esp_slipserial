// This program converts UDP OSC messages into SLIP Serial OSC messages on the ESP8266 wifi board
// At the moment there is no OSC bundle support, not sure I need it at the moment


#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>


int status = WL_IDLE_STATUS;
const char* ssid = "";  //  your network name (SSID)
const char* pass = "";       // your network password

int localPort = 8000;
int destPort = 9000;
IPAddress outIp(10, 0, 10, 49); //Messages sent out by the ESP will go to this IP
                                //outIp is overwritten when the ESP receives a message.
                                //outIp is the set to match the sender of the message.
IPAddress espip(10, 0, 10, 50);  //IP of the ESP
IPAddress gateway(10,0,10,1);
IPAddress subnet(255,255,255,0);
WiFiUDP Udp;
SLIPEncodedSerial SLIPSerial(Serial);


void setup()
{
  SLIPSerial.begin(115200);
  Serial.begin(115200);
  delay(1000);
  Serial.println("LOL");

  WiFi.begin(ssid, pass);
  WiFi.config(espip,gateway,subnet);

  int tries=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tries++;
    if (tries > 600){
      break;
    }
  }
  Udp.begin(localPort);
}

void loop(){

  // read=udp->serial, write=serial->udp
  int rd,wr;
  OSCMessage rMsg, wMsg;
  //static unsigned int bp = 0;
  static bool packet = 0;
  static unsigned long  tr = 0, tw = 0;

  if((rd = Udp.parsePacket())>0)
  {
    if(tr - micros() > 10000)
    {
      outIp = Udp.remoteIP();
      while (rd--)
      {
        rMsg.fill(Udp.read());
      }
      if(!rMsg.hasError())
      {
        SLIPSerial.beginPacket();
        rMsg.send(SLIPSerial);
        SLIPSerial.endPacket();
      }
      rMsg.empty();
    }
    else
    {
      while (rd--)
        Udp.read();
    }
    tr = micros();
  }


  while(!SLIPSerial.endofPacket()) {
    if(wr = SLIPSerial.available()>0){
      tw = micros();
      if(!packet)
        packet = 1;
      while(wr--) { //this needs a byte limit
        wMsg.fill(SLIPSerial.read());
        //if(++bp >= 512) break; //packets seem to get truncated about here anyway
      }
    }
    if((micros() - tw) > 10000) break; //Timeout for no eoP()
    //if(bp >= 512) break; //break again
  }

  if(packet) {
    if(!wMsg.hasError()) {
      //Serial.println("Sender melding til:");
      Udp.beginPacket(outIp, destPort);
      //Serial.println(outIp);
      //Serial.print(":");
      //Serial.print(destPort);
      wMsg.send(Udp);
      Udp.endPacket();
      packet = 0;
    }
  }
}
