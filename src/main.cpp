// ***************************************************************************
// TemperaturSteuerung HeizungsPumpe Seminarraum 2023 - v1
// - by Philipp Rauch - VigilanteSystems - opensource - 23.03.2023
// ***************************************************************************
// its to check for tempMin and tempMax to toggle relay on pumpswitch
// ***************************************************************************

// tempCtrl_heatpump_2023_v1

// **** INCLUDES *****
#include <Arduino.h>
// Include the libraries we need
#include <LowPower.h>
// OneWire stuff
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
const int ONE_WIRE_BUS = 2;
// state toggle pins
const int MANUAL_MODE_PIN = 3;
const int RELAY_TOGGLE_PIN = 6;
// value defines, maybe put seperately and include that file
const int TEMP_CHECK_COUNT = 3;
const int SHORT_TIME = 1200;
const int LONG_TIME = (SHORT_TIME * TEMP_CHECK_COUNT);
// temp controls
const float TEMP_MIN = 18;        // 18.0
const float TEMP_MAX = 18.5;      // 18.5
const float TEMP_HYSTERESE = 0.1; // 18.6 17.9
// we repeat 1s sleep idle just x times to get sleep like below
const int SLEEP_FOR_SECONDS = 20;
volatile float tempCurrent = 0.0;

// is manual Mode is switched on
volatile bool manualMode = false;
volatile bool relayStateON = false;
volatile bool relayStateLAST = relayStateON;
unsigned long startTime = millis();

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

enum RelayState
{
  RELAY_OFF,
  RELAY_ON
};

RelayState relayState = RELAY_OFF;

void handleRelayState(float tempCurrent)
{
  switch (relayState)
  {
  case RELAY_OFF:
    if (tempCurrent < TEMP_MIN)
    {
      digitalWrite(RELAY_TOGGLE_PIN, HIGH);
      relayState = RELAY_ON;
    }
    break;
  case RELAY_ON:
    if (tempCurrent > TEMP_MAX)
    {
      digitalWrite(RELAY_TOGGLE_PIN, LOW);
      relayState = RELAY_OFF;
    }
    break;
  }
}

void setup(void)
{
  // switchOFF inbuild LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // set control pins
  // pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  pinMode(MANUAL_MODE_PIN, INPUT);
  pinMode(RELAY_TOGGLE_PIN, OUTPUT);

  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
}

void loop(void)
{
  ///************************************
  // CHECK FOR MANUAL MODE SWITCH ON
  // check again if manual mode switch is switched ON
  if (digitalReadFast(MANUAL_MODE_PIN))
  {
    // Serial.println("MANUAL MODE ON"); // ONLY FOR DEBUG DEV
    manualMode = true;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    // // Serial.println("MANUAL MODE OFF"); // ONLY FOR DEBUG DEV
    manualMode = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
  ///************************************
  ///************************************

  ///************************************
  // TEMPERATURE CONTROL STUFF
  // read Temperatur and control relay ONLY when NOT in manual mode
  // (i.e. manual mode switch, switched on - relayStateON==TRUE)
  if (not manualMode)
  {
    // read sensor1
    sensors.requestTemperatures(); // Send the command to get temperatures
    tempCurrent = sensors.getTempCByIndex(0);
    handleRelayState(tempCurrent);
    // DEBUG
    // Serial.println(tempCurrent);
    // so we check each SHORT_TIME ms for new temp, so its SHORT_TIME*TEMP_CHECK_COUNT==6x1200==7200
    while (millis() - startTime < SHORT_TIME)
    {
      // Do nothing
    }
  }
  else
  {
    relayStateON = true;
  }
  ///************************************
  ///************************************

  ///************************************
  // RELAY control stuff
  // switch relay to pump ON/OFF
  if (relayStateON)
  {
    digitalWrite(RELAY_TOGGLE_PIN, HIGH); // ON
    // Serial.println("RELAY_TOGGLE_PIN HIGH"); // ONLY FOR DEBUG DEV
  }
  else
  {
    digitalWrite(RELAY_TOGGLE_PIN, LOW); // OFF
    // Serial.println("RELAY_TOGGLE_PIN LOW"); // ONLY FOR DEBUG DEV
  }
  // if laste state different, delay to protect pump
  // especially at bootup for RELAY HIGH
  if (relayStateLAST != relayStateON)
  {
    // wait for pump to bootup..
    delay(LONG_TIME);
  }
  // save last relayStateON state..
  relayStateLAST = relayStateON;
  ///************************************
  ///************************************

  ///************************************
  // goto sleep/idle if not manual mode
  // if ((not relayStateON) and (not manualMode))
  if (not manualMode)
  {
    // Serial.println("GOTOSLEEP"); // ONLY FOR DEBUG DEV
    // FOR DEBUG ONLY
    while (millis() - startTime < SHORT_TIME * .5)
    {
      // Do nothing
    }
    // sleepNow();                  // TODO: make sleep stuff
    // this lowpower idles for 8sec, presumably using watchdog, cuz it max 8sec only..:(
    // but we run it SLEEP_FOR_SECONDS times, to get what i want.. 1x20 + delayoverhead == 22
    for (int index = 0; index < SLEEP_FOR_SECONDS; index++)
    {
      LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    }
    // give it some sec to wakeupo
    while (millis() - startTime < SHORT_TIME * .5)
    {
      // Do nothing
    }

  }
  ///************************************
  ///************************************
  // KEEP OR NOT??
  while (millis() - startTime < SHORT_TIME)
  {
    // Do nothing
  }
}

// // FIN