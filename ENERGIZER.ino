#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME "FENCE ENERGIZER "
#define BLYNK_AUTH_TOKEN ""

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#define TINY_GSM_MODEM_SIM800

#define SMS_TARGET  ""


// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30

#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "web.gprs.mtnnigeria.net";
char user[] = "";
char pass[] = "";

// Hardware Serial on Mega, Leonardo, Micro
//#define SerialAT Serial1

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3); // RX, TX

TinyGsm modem(SerialAT);
WidgetLCD lcd(V9);

// Setting Activate/De-activate Relay pin and Alarm input pins here
const int Act_Pin = 7;
const int Alarm_Pin = 8;
const int Status_Pin = 9;

BlynkTimer timer;
int ledState = HIGH;
int ledState1 = HIGH;
int Alarm_state = LOW;
int status_state = HIGH;

// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V2);

  // Alternatively, you could override server state using:
  //Blynk.virtualWrite(V2, ledState);
}

// When App button is pushed - switch the state
BLYNK_WRITE(V2) {
  ledState = param.asInt();
  digitalWrite(Act_Pin, ledState);
  delay(500);
}

void Check_Alarm()
{
  if (digitalRead(Alarm_Pin) == HIGH) {
    // Alarm_state is used to avoid sequential toggles
    Blynk.virtualWrite(V6, LOW);
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(0, 0, "ALARM DETECTED!!"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(0, 1, "KINDLY CHECK");
    if (Alarm_state != HIGH) {
      // To send an SMS, call modem.sendSMS(SMS_TARGET, smsMessage)
      String smsMessage = "Alert : Perimeter Intrusion Detected!!!";
      modem.sendSMS(SMS_TARGET, smsMessage);
      Alarm_state = HIGH;
    }
 
  } else {
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(2, 0, "NO ALARM"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(2, 1, "SYSTEM GOOD");
    Alarm_state = LOW;
    Blynk.virtualWrite(V6, HIGH);
  }
}
void Check_Status()
{
  if (digitalRead(Status_Pin) == LOW) {
    // Alarm_state is used to avoid sequential toggles
    Blynk.virtualWrite(V0, HIGH);
    delay(200);
    Blynk.virtualWrite(V0, LOW);
    lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(2, 0, "ENERGIZER"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(2, 1, "OFF !!!");
    
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(9600);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  Blynk.begin(auth, modem, apn, user, pass);

  pinMode(Act_Pin, OUTPUT);
  pinMode(Alarm_Pin, INPUT);
   pinMode(Status_Pin, INPUT);
  digitalWrite(Act_Pin, ledState);

  // Setup a function to be called every 100 ms
  timer.setInterval(5000L, Check_Alarm);
  timer.setInterval(5000L, Check_Status);
}

void loop()
{
  Blynk.run();
  timer.run();
}
