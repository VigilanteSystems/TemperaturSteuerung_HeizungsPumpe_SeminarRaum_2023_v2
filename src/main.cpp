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
#define ONE_WIRE_BUS 2
// state toggle pins
#define MANUAL_MODE_PIN 3
#define RELAY_TOGGLE_PIN 6
// value defines, maybe put seperately and include that file
#define TEMP_CHECK_COUNT 5
#define SHORT_TIME 1200
#define LONG_TIME (SHORT_TIME * TEMP_CHECK_COUNT)
// temp controls
#define TEMP_MIN 18        // 18.0
#define TEMP_MAX 18.5      // 18.5
#define TEMP_HYSTERESE 0.1 // 18.6 17.9
// we repeat 1s sleep idle just x times to get sleep like below
#define SLEEP_FOR_SECONDS   20

// control vars maybe use next two in a dynamic tempreading scenario,thendefines are not useful
// volatile float tempMin = 26.5;
// volatile float tempMax = 27.5;
volatile float tempCurrent = 0.0;
// is manual Mode is switched on
volatile bool manualMode = false;
volatile bool relayStateON = false;
volatile bool relayStateLAST = relayStateON;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// get average float from floatArray
float averageFloatArray(float *array, int len) // assuming array is int.
{
  long sum = 0L; // sum will be larger than an item, long for safety.
  for (int i = 0; i < len; i++)
    sum += array[i];
  return ((float)sum) / len; // average will be fractional, so float may be appropriate.
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

  // start serial port
  // Serial.begin(9600);
  // Serial.println("Dallas Temperature IC Control Library Demo");
}

void loop(void)
{
  ///************************************
  // DEBUG STUFF
  // // call sensors.requestTemperatures() to issue a global temperature
  // // request to all devices on the bus
  // // Serial.print("Requesting temperatures... ");
  // sensors.requestTemperatures(); // Send the command to get temperatures
  // // Serial.println("DONE");
  // // Serial.print("Temperature for Device 1 is: ");
  // // Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  ///************************************
  ///************************************

  ///************************************
  // CHECK FOR MANUAL MODE SWITCH ON
  // check again if manual mode switch is switched ON
  if (digitalRead(MANUAL_MODE_PIN))
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
    // always a fresh floatStoreArray for each round
    float tempCheckStore[TEMP_CHECK_COUNT] = {};
    // read sensor X times and take mean for more precise value
    for (int index = 0; index < TEMP_CHECK_COUNT; index++)
    {
      // read sensor1
      sensors.requestTemperatures(); // Send the command to get temperatures
      tempCurrent = sensors.getTempCByIndex(0);
      tempCheckStore[index] = tempCurrent;
      // DEBUG
      // Serial.println(tempCurrent);
      // so we check each SHORT_TIME ms for new temp, so its SHORT_TIME*TEMP_CHECK_COUNT==6x1200==7200
      delay(SHORT_TIME);
    }
    // take means of just read temperatures
    tempCurrent = averageFloatArray(tempCheckStore, TEMP_CHECK_COUNT);

    // DEBUG
    // Serial.print("tempCurrent: "); // ONLY FOR DEBUG DEV
    // Serial.println(tempCurrent);   // ONLY FOR DEBUG DEV

    // check if temps over or under with TEMP_HYSTERESE
    // var to switch relay to pump OFF
    if (tempCurrent > TEMP_MAX + TEMP_HYSTERESE)
    {
      relayStateON = false;
    }
    // var to switch relay to pump relays ON
    else if (tempCurrent < TEMP_MIN - TEMP_HYSTERESE)
    {
      relayStateON = true;
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
    digitalWrite(LED_BUILTIN, HIGH);
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
    delay(SHORT_TIME * .5); // FOR DEBUG ONLY
    // sleepNow();                  // TODO: make sleep stuff
    // this lowpower idles for 8sec, presumably using watchdog, cuz it max 8sec only..:(
      // but we run it SLEEP_FOR_SECONDS times, to get what i want.. 1x20 + delayoverhead == 22
    for (int index = 0; index < SLEEP_FOR_SECONDS; index++)
    {
      LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    }
    delay(SHORT_TIME * .5); // give it some sec to wakeupo

    // Serial.print("WAKEUP AFTER: "); // ONLY FOR DEBUG DEV
    // Serial.print(SLEEP_FOR_SECONDS); // ONLY FOR DEBUG DEV
    // // Serial.println(" seconds sleep"); // ONLY FOR DEBUG DEV
  }
  ///************************************
  ///************************************
  delay(SHORT_TIME); // KEEP OR NOT??
}

// // FIN