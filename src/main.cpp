// ***************************************************************************
// TemperaturSteuerung HeizungsPumpe Seminarraum 2023 - v2
// - by Philipp Rauch - VigilanteSystems - opensource - 23.03.2023
// ***************************************************************************

// This code controls a heating pump based on temperature readings from a
// DallasTemperature sensor. The pump is turned on when the temperature is
// below TEMP_MIN_C and turned off when it is above TEMP_MAX_C. A hysteresis
// of TEMP_HYSTERESIS_C is used to avoid frequent toggling of the pump.
// The code also supports a manual mode where the pump is always on.
// The code is optimized for low power consumption by using the LowPower
// library to put the device to sleep when the pump is off and there is no
// user input.

// include libraries
#include <Arduino.h>
#include <LowPower.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// manual mode switch port pin 3 to be able to use interrupt there
static const int MANUAL_MODE_PIN = 3;
// Data wire is plugged into pin 6 on the Arduino
static const int ONE_WIRE_BUS_PIN = 6;
// relay switch port pin
static const int RELAY_TOGGLE_PIN = 9;
// values defined
static const int SHORT_TIME_MS = 500;
static const int LONG_TIME_MS = 8888;
// temp controls
static const float TEMP_MIN_C = 18.0;
static const float TEMP_MAX_C = 18.5;
static const float TEMP_HYSTERESIS_C = 0.2;
// sleep mode i.e. check temp or manual mode every 5 minutes
static const int SLEEP_DURATION_S = 300; // 5 minutes from 5*60sec==300sec

//manual modes from booly to enum, define manual mode states statemachine
enum ManualModeState
{
  MANUAL_MODE_OFF,
  MANUAL_MODE_ON
};
ManualModeState manualMode = MANUAL_MODE_OFF;
// Define relay states statemachine
enum RelayState
{
  RELAY_OFF,
  RELAY_ON
};
RelayState lastRelayState = RELAY_OFF;
RelayState relayState = RELAY_OFF;

// set pin modes
void setPinModes()
{
  pinMode(ONE_WIRE_BUS_PIN, INPUT);
  pinMode(MANUAL_MODE_PIN, INPUT);
  pinMode(RELAY_TOGGLE_PIN, OUTPUT);
  // pinMode(LED_BUILTIN, OUTPUT);    // gone to save more power
  // digitalWriteFast(LED_BUILTIN, LOW); // gone to save more power
  digitalWrite(RELAY_TOGGLE_PIN, LOW);
}

// handle relay state
void handleRelayState(float tempCurrent)
{
  switch (relayState)
  {
  case RELAY_OFF:
    if (tempCurrent < TEMP_MIN_C - TEMP_HYSTERESIS_C)
    {
      relayState = RELAY_ON;
    }
    break;
  case RELAY_ON:
    if (tempCurrent > TEMP_MAX_C + TEMP_HYSTERESIS_C)
    {
      relayState = RELAY_OFF;
    }
    break;
  }
}

// check if manual mode switch is on
void checkManualModeSwitch()
{
  if (digitalRead(MANUAL_MODE_PIN))
  {
    manualMode = MANUAL_MODE_ON;
    // digitalWriteFast(LED_BUILTIN, HIGH);   // gone to save more power
  }
  else
  {
    manualMode = MANUAL_MODE_OFF;
    // digitalWriteFast(LED_BUILTIN, LOW);    // gone to save more power
  }
}

// Read temperature from DallasTemperature sensor
float readTemperature()
{
  static OneWire oneWire(ONE_WIRE_BUS_PIN);
  static DallasTemperature sensors(&oneWire);
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

// do nothing, just wait SHORT_TIME_MS
void waitShortTime()
{
  unsigned long startMillis = millis();
  while (millis() - startMillis < SHORT_TIME_MS)
  {
    // do nothing, just wait SHORT_TIME_MS
  }
}

// do nothing, just wait LONG_TIME_MS
void waitLongTime()
{
  unsigned long startMillis = millis();
  while (millis() - startMillis < LONG_TIME_MS)
  {
    // do nothing, just wait LONG_TIME_MS
  }
}

// Toggle relay
void toggleRelay()
{
  // If the last relay state was different, delay to protect the pump
  if (relayState != lastRelayState)
  {
    waitLongTime();
  }
  // Toggle relay
  if (relayState)
  {
    digitalWrite(RELAY_TOGGLE_PIN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_TOGGLE_PIN, LOW);
  }
  // Save the current relay state
  lastRelayState = relayState;
}

// sleep/powerdown for x sleepDuration seconds + about 1 second from 2*waitShortTime()
void goToSleep(int sleepDuration)
{
  // some time to get to bed
  waitShortTime();
  //  makes the device sleep for 1 second * sleepDuration
  for (int i = 0; i < sleepDuration; i++)
  {
    // makes the device sleep but keeps digital pins high, unlinke LowPower.sleep() but powerStandby() is better
    // LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    LowPower.powerStandby(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  // some time to get up
  waitShortTime();
}

// setup
void setup()
{
  // setup initial pin modes
  setPinModes();
}

// main loop
void loop()
{
  // Check for manual mode switch on
  checkManualModeSwitch();

  // Temperature control stuff or manual mode
  if (manualMode)
  {
    // Manual mode
    relayState = RELAY_ON;
  }
  else
  {
    // Read temperature from DallasTemperature sensor
    float tempCurrent = readTemperature();
    // relay state machine
    handleRelayState(tempCurrent);
  }

  // Relay control stuff
  toggleRelay();

  // Go to sleep/idle
  goToSleep(SLEEP_DURATION_S);
}
// FIN