/*
 Car Alarm
*/

/*
  SOS signal

  Created by Vaidotas on 4/16/2012.
  Copyright (c) 2012 http://www.instructables.com/id/Arduino-SOS-signal-with-8ohms-speaker-LED-blinki/
*/
/*
 	C	C#	D	Eb	E	F	F#	G	G#	A	Bb	B
0	16.35	17.32	18.35	19.45	20.60	21.83	23.12	24.50	25.96	27.50	29.14	30.87
1	32.70	34.65	36.71	38.89	41.20	43.65	46.25	49.00	51.91	55.00	58.27	61.74
2	65.41	69.30	73.42	77.78	82.41	87.31	92.50	98.00	103.8	110.0	116.5	123.5
3	130.8	138.6	146.8	155.6	164.8	174.6	185.0	196.0	207.7	220.0	233.1	246.9
4	261.6	277.2	293.7	311.1	329.6	349.2	370.0	392.0	415.3	440.0	466.2	493.9
5	523.3	554.4	587.3	622.3	659.3	698.5	740.0	784.0	830.6	880.0	932.3	987.8
6	1047	1109	1175	1245	1319	1397	1480	1568	1661	1760	1865	1976
7	2093	2217	2349	2489	2637	2794	2960	3136	3322	3520	3729	3951
8	4186	4435	4699	4978	5274	5588	5920	6272	6645	7040	7459	7902
*/

// Arm/disarm variables
boolean armed = false;
boolean lastButton = LOW;
boolean currentButton = LOW;
const int armIPin = 52;
const int armOPin = 42;
// Armed led blink timer
long previousMS = 0;
const long armedBlinkInterval = 1000;
boolean armedLedState = LOW;

// Sensor input
const int warnIPin = 51;
const int alarmIPin = 53;

// feedback
const int warnOPin = 46;
const int alarmOPin = 47;
const int speakerOPin = 43;

// speaker notes
const int warnNote = 830;  //G#5
const int alarmNote = 1316; //E6

void setup() {
  pinMode(armIPin, INPUT);
  pinMode(armOPin, OUTPUT);
  pinMode(warnIPin, INPUT);
  pinMode(alarmIPin, INPUT);
  pinMode(warnOPin, OUTPUT);
  pinMode(alarmOPin, OUTPUT);
  pinMode(speakerOPin, OUTPUT);
}

void loop() {
  updateArmed();
  updateArmedLed();
  if (armed) {
    // Check the warn sensor
    // Note: warn default state is strangely inverted
    if (!digitalRead(warnIPin)) {
      activateWarning();
    } else {
      digitalWrite(warnOPin, LOW);
    }
    // Check the alarm sensor
    if (digitalRead(alarmIPin)) {
      for (int i=0; i < 1; i++) {
        threeDots();
        threeDashes();
        threeDots();
        delay(3000);
      }
    }
  }
}

void activateWarning() {
  for (int i=0; i < 10; i++) {
    // Exit if the alarm gets triggered. We do not want
    // to continue the warning if the alarm is triggered
    if (digitalRead(alarmIPin)) {
      return;
    }
    updateArmedLed();
    digitalWrite(warnOPin, HIGH);
    tone(speakerOPin, warnNote, 100);
    digitalWrite(warnOPin, LOW);
    delay(200);
    noTone(speakerOPin);
  }
}

boolean debounce(boolean last) {
  boolean current = digitalRead(armIPin);
  if (last != current) {
    delay(5);
    current = digitalRead(armIPin);
  }
  return current;
}

// three short signals
void threeDots()
{
  for (int i=0; i<3; i++){
    updateArmedLed();
    digitalWrite(alarmOPin, HIGH);
    tone(speakerOPin, alarmNote, 100);
    digitalWrite(alarmOPin, LOW);
    delay(200);
    noTone(speakerOPin);
  }
  updateArmedLed();
  delay(200);
}

// three long signals
void threeDashes()
{
  for (int i=0; i<3; i++){
    updateArmedLed();
    digitalWrite(alarmOPin, HIGH);
    tone(speakerOPin, alarmNote, 300);
    digitalWrite(alarmOPin, LOW);
    updateArmedLed();
    delay(400);
    noTone(speakerOPin);
  }
  updateArmedLed();
  delay(200);
}

// Check for changes in the alarm state
void updateArmed() {
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    armed = !armed;
    // Simulate 4 second delay of alarm sensors
    if (armed) {
      for (int i=0; i< 8; i++) {
        digitalWrite(armOPin, HIGH);
        delay(250);
        digitalWrite(armOPin, LOW);
        delay(250);
      }
      // reset the blink timer
      previousMS = millis();
      armedLedState = HIGH;
    }
  }
  lastButton = currentButton;
}

// We need to be able to blink the armed led even if the 
// Alarm is going off
void updateArmedLed() {
  if (armed) {
      unsigned long currentMS = millis();
      if (currentMS - previousMS > armedBlinkInterval) {
        // Save the last time the led was blinked
        previousMS = currentMS;
        // toggle the led state
        if (armedLedState == LOW)
          armedLedState = HIGH;
        else
          armedLedState = LOW;
      }
  } else {
    armedLedState = LOW;
  }
  digitalWrite(armOPin, armedLedState);
}
