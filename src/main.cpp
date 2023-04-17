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
#include <digitalWriteFast.h>

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


void checkManualModeSwitch()
{
  if (digitalReadFast(MANUAL_MODE_PIN))
  {
    manualMode = true;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    manualMode = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
}

float readTemperature()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

void toggleRelay()
{
  if (relayStateON)
  {
    digitalWrite(RELAY_TOGGLE_PIN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_TOGGLE_PIN, LOW);
  }

  // If the last relay state was different, delay to protect the pump
  if (relayStateLAST != relayStateON)
  {
    delay(LONG_TIME);
  }

  // Save the current relay state
  relayStateLAST = relayStateON;
}

void waitShortTime()
{
  delay(SHORT_TIME);
}

void goToSleep()
{
  for (int i = 0; i < SLEEP_FOR_SECONDS; i++)
  {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
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
  // Check for manual mode switch on
  checkManualModeSwitch();

  // Temperature control stuff
  if (!manualMode)
  {
    float tempCurrent = readTemperature();
    handleRelayState(tempCurrent);
    waitShortTime();
  }
  else
  {
    relayStateON = true;
  }

  // Relay control stuff
  toggleRelay();

  // Go to sleep/idle if not in manual mode and the relay is off
  if (!manualMode && !relayStateON)
  {
    goToSleep();
  }
}
