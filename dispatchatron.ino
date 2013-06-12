// Fire Truck Launcher
// Jeff Highsmith
// January 2012

#include <Servo.h>
#include <IRremote.h>

#define PIN_IR 3
#define PIN_DETECT A1
#define PIN_STATUS 13
#define PIN_AUDIO_RELAY 2
#define PIN_AUDIO_LEVEL 5
#define PIN_LEFTLED A3
#define PIN_RIGHTLED A2
#define PIN_CLOSED_SWITCH 8
#define PIN_OPEN_SWITCH 7

#define AUDIO_THRESHOLD 40
#define TIME_BETWEEN_LAUNCHES 2000

IRsend irsend;
Servo doorServo;
long audioLevel;
long lastLaunch;
int lightPosition;
int closingDoor;

void setup() 
{ 
  pinMode(PIN_DETECT, INPUT);
  pinMode(PIN_STATUS, OUTPUT);
  irsend.enableIROut(38);
  irsend.mark(0);
  
  pinMode(PIN_AUDIO_RELAY, OUTPUT);
  pinMode(PIN_AUDIO_LEVEL, INPUT);
  
  pinMode(PIN_LEFTLED, OUTPUT);
  pinMode(PIN_RIGHTLED, OUTPUT);
  digitalWrite(PIN_LEFTLED, LOW);
  digitalWrite(PIN_RIGHTLED, LOW);
  
  Serial.begin(9600);
  doorServo.attach(6); 
  pinMode(PIN_CLOSED_SWITCH, INPUT);
  digitalWrite(PIN_CLOSED_SWITCH, HIGH);
  if (!digitalRead(PIN_CLOSED_SWITCH)) {
    Serial.println("Door completely closed during setup");
  }
  pinMode(PIN_OPEN_SWITCH, INPUT);
  digitalWrite(PIN_OPEN_SWITCH, HIGH);
  if (!digitalRead(PIN_OPEN_SWITCH)) {
    Serial.println("Door completely open during setup");
    closeDoor();
  }
  pinMode(PIN_DETECT, INPUT);
  if (!digitalRead(PIN_DETECT)) {
    Serial.println("IR detect during setup");
  }
  lastLaunch = 0;
  lightPosition = 0;
  closingDoor = 0;
  lightsOff();
  Serial.println("Setup complete.");
  
  // Troubleshooting
  //openDoor();
  //digitalWrite(PIN_AUDIO_RELAY, HIGH);
  //delay(60000);
} 
 
void loop() 
{ 
  //digitalWrite(PIN_STATUS, !digitalRead(PIN_DETECT));
  
  audioLevel = analogRead(PIN_AUDIO_LEVEL);
  //flashLights();
  //delay(500);
  Serial.println(audioLevel);
  //delay(500);

  if(audioLevel > AUDIO_THRESHOLD) {
    digitalWrite(13, HIGH);
    if ((lastLaunch + TIME_BETWEEN_LAUNCHES) < millis()) {
      digitalWrite(PIN_AUDIO_RELAY, HIGH); // Enable external speaker
      for(int i = 0; i < 4; i++) {
        flashLights();
        delay(250);
      }
      lastLaunch = millis();
      Serial.println(" ");
      Serial.println("Dispatching a truck...");
      openDoor();
      /*if(audioLevel > AUDIO_THRESHOLD) { // If the call is still in progress...
        while(audioLevel > AUDIO_THRESHOLD) { // Wait for call to finish
          audioLevel = analogRead(PIN_AUDIO_LEVEL);
          delay(1000);
        }
      }*/
      if(closingDoor == 0) {
        Serial.println("not last");
        for(int i = 0; i < 72; i++) {
          flashLights();
          delay(250);
        }
      } else {
        Serial.println("This is the last door");
        for(int i = 0; i < 20; i++) {
          flashLights();
          delay(250);
        }
        closingDoor = 0;
      }
      //delay(19000);
      audioLevel = analogRead(PIN_AUDIO_LEVEL);
      while(audioLevel > AUDIO_THRESHOLD) {
        delay(500);
        audioLevel = analogRead(PIN_AUDIO_LEVEL);
      }
      digitalWrite(PIN_AUDIO_RELAY, LOW);
      lightsOff();
    }
    Serial.print("audioLevel:");
    Serial.println(audioLevel);
    delay(200);
    digitalWrite(13, LOW);
  } else {
    if(digitalRead(PIN_OPEN_SWITCH)) {
      Serial.println("Open switch HIGH");
    } else {
      Serial.println("Open switch LOW");
    }
    if(digitalRead(PIN_CLOSED_SWITCH)) {
      Serial.println("Close switch HIGH");
    } else {
      Serial.println("Close switch LOW");
    }
    if(digitalRead(PIN_DETECT)) {
      Serial.println("IR Detect HIGH");
    } else {
      Serial.println("IR Detect LOW");
    }
//    Serial.print("N");
    delay(1000);
  }
  
  /*openDoor();
  for(int d=0; d<15; d++) {
    delay(250);
  }*/
  
  
}

void openDoor(void) {
  if(!digitalRead(PIN_OPEN_SWITCH)) {
    Serial.println("Door was already completely open");
    return;
  }
  Serial.println("Opening door...");
  
  doorServo.write(150);
  flashLights();
  delay(250);
  flashLights();
  delay(250);
  flashLights();
  delay(250);
  flashLights();
  delay(250);
  //long microsecs = 54;
  while(digitalRead(PIN_OPEN_SWITCH) && !digitalRead(PIN_DETECT)) {
    flashLights();
    Serial.println("Switch open and IR reading...");
    doorServo.write(150);
    if(digitalRead(PIN_DETECT)) {
      Serial.println("IR blocked, stopping servo.");
      doorServo.write(90);
    } else if(!digitalRead(PIN_OPEN_SWITCH)) {
      Serial.print("Door opened all the way.");
      closeDoor();
      return;
    }
  }
  doorServo.write(90);
  Serial.print("Done opening door.");
}

void closeDoor(void) {
  Serial.println("Closing door...");
  closingDoor = 1;
  int cease = digitalRead(PIN_CLOSED_SWITCH);
  while(cease > 0) {
    cease = digitalRead(PIN_CLOSED_SWITCH);
    doorServo.write(30);
    flashLights();
    delay(15);
  }
  doorServo.write(150);
  delay(100);
  doorServo.write(90);
  Serial.print("Done Closing door.");
}

void lightsOff(void) {
  digitalWrite(PIN_LEFTLED, LOW);
  digitalWrite(PIN_RIGHTLED, LOW);
}

void flashLights(void) {
  //Serial.print("flashing");
  //Serial.print(lightPosition);
  if(lightPosition == 0) {
    digitalWrite(PIN_LEFTLED, HIGH);
    digitalWrite(PIN_RIGHTLED, LOW);
    lightPosition = 1;
  } else {
    digitalWrite(PIN_LEFTLED, LOW);
    digitalWrite(PIN_RIGHTLED, HIGH);
    lightPosition = 0;
  }
}
