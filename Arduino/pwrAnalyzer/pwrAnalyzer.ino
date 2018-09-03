#include <SoftwareSerial.h>
#include <genieArduino.h>
Genie genie;

SoftwareSerial pwrAnalyzer(2, 3); //Rx,Tx

char hexArray [70];
int x = 0;
int q = 0;
int w = 0;
int e = 0;
int r = 0;
int t = 0;
int y = 0;
String Watts;
String VA;
String VAR;
String PF;
String Volt;
String Amp;
String wValue;
String vValue;
String aValue;
int Threshold = 10; //default Wattage Threshold
boolean state = true;

#define relayPin 7
#define RESETLINE 4

void setup() {
  Serial.begin(9600);
  pwrAnalyzer.begin(9600);
  genie.Begin(Serial);
  genie.AttachEventHandler (myGenieEventHandler);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, state);
  pinMode(RESETLINE, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
  digitalWrite(RESETLINE, 1);  // Reset the Display via D4
  delay(100);
  digitalWrite(RESETLINE, 0);  // unReset the Display via D4
  delay (3500); //let the display start up after the reset (This is important)
}

void loop() {
  genie.WriteStr(0, wValue);
  genie.WriteObject(GENIE_OBJ_SCOPE, 0, wValue.toDouble());
  genie.WriteStr(1, vValue);
  genie.WriteObject(GENIE_OBJ_SCOPE, 0, vValue.toDouble());
  genie.WriteStr(2, aValue);
  genie.WriteObject(GENIE_OBJ_SCOPE, 0, aValue.toDouble());
  genie.DoEvents();
  pwr();
}

void pwr() {
  if (pwrAnalyzer.available()) {
    for (int i = 0; i < 50; i++) {
      byte incomingByte = pwrAnalyzer.read();
      hexArray[i] = incomingByte;
      if (hexArray[i] == 'I') {
        x = 0;
        q = 0;
        w = 0;
        e = 0;
        r = 0;
        t = 0;
        y = 0;
        Watts = "";
        VA = "";
        VAR = "";
        PF = "";
        Volt = "";
        Amp = "";
      }
      else if (hexArray[i] == ',') {
        //Serial.println(hexArray[i]);
        x++;
        if (x == 1) {
          //Serial.print("Watts: ");
        } else if (x == 2) {
          //Serial.print("VA:    ");
        } else if (x == 3) {
          //Serial.print("VAR:   ");
        } else if (x == 4) {
          //Serial.print("PF:    ");
        } else if (x == 5) {
          //Serial.print("VOLT:  ");
        } else if (x == 6) {
          //Serial.print("AMP:   ");
        }
      }
      else if (isGraph(hexArray[i])) {
        if (x == 1) {
          Watts += (hexArray[i]);
          q++;
          if (q == 6) {
            //Serial.print(Watts);
            wValue = Watts;
            if (Watts.toDouble() >= Threshold) {
              state = true;
              digitalWrite(relayPin, state);
              genie.WriteObject(GENIE_OBJ_USERBUTTON, 4, 0);
            }
            //genie.WriteObject(GENIE_OBJ_STATIC_TEXT,0, Watts);
          }
        } else if (x == 2) {
          VA += (hexArray[i]);
          w++;
          if (w == 6) {
            //Serial.print(VA);
          }
        } else if (x == 3) {
          VAR += (hexArray[i]);
          e++;
          if (e == 6) {
            //Serial.print(VAR);
          }
        } else if (x == 4) {
          PF += (hexArray[i]);
          r++;
          if (r == 6) {
            //Serial.print(PF);
          }
        } else if (x == 5) {
          Volt += (hexArray[i]);
          t++;
          if (t == 6) {
            //Serial.print(Volt);
            //genie.WriteStr(1, Volt);
            vValue = Volt;
          }
        } else if (x == 6) {
          Amp += (hexArray[i]);
          y++;
          if (y == 6) {
            //Serial.print(Amp);
            //genie.WriteStr(2, Amp);
            aValue = Amp;
          }
        }
      }
    }
  }
}

void myGenieEventHandler(void) {
  genieFrame Event;
  genie.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below

  if (Event.reportObject.cmd == GENIE_REPORT_EVENT) {
    if (Event.reportObject.object == GENIE_OBJ_USERBUTTON) {

      if (Event.reportObject.index == 0) {
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 1, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 2, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 3, 0);
        Threshold = 10;
      }
      else if (Event.reportObject.index == 1) {
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 0, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 2, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 3, 0);
        Threshold = 50;
      }
      else if (Event.reportObject.index == 2) {
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 1, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 0, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 3, 0);
        Threshold = 100;
      }
      else if (Event.reportObject.index == 3) {
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 1, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 2, 0);
        genie.WriteObject(GENIE_OBJ_USERBUTTON, 0, 0);
        Threshold = 500;
      }
      else if (Event.reportObject.index == 4) {
        state = !state;
        digitalWrite(relayPin, state);
      }
    }
  }
}


