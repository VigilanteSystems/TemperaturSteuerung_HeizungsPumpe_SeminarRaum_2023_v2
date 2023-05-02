

BING KI VERSION:
This code controls a heating pump based on temperature readings from a DallasTemperature sensor. The pump is turned on when the temperature is below TEMP_MIN_C and turned off when it is above TEMP_MAX_C. A hysteresis of TEMP_HYSTERESIS_C is used to avoid frequent toggling of the pump. The code also supports a manual mode where the pump is always on. The code is optimized for low power consumption by using the LowPower library to put the device to sleep when the pump is off and there is no user input.

Here's a brief explanation of the important parts of the code:

The constants TEMP_MIN_C, TEMP_MAX_C, and TEMP_HYSTERESIS_C define the temperature range at which the pump should turn on and off, and the hysteresis to avoid frequent toggling of the pump.
The function handleRelayState() checks the current temperature reading and sets the relay state based on whether the temperature is below TEMP_MIN_C, above TEMP_MAX_C, or within the hysteresis range. If the relay state is changed, it also sets a delay to protect the pump before toggling the relay.
The function toggleRelay() toggles the relay pin based on the current relay state. If the state was changed, it also sets a delay to protect the pump.
The function goToSleep() puts the device into sleep mode using the LowPower library for a specified duration, which is defined by the constant SLEEP_DURATION_S.
The code also includes functions to read the temperature from the DallasTemperature sensor, check the state of the manual mode switch, and wait for a short time. Overall, the code is well-organized and easy to read, with clear comments to explain each section.

### TemperaturSteuerung_HeizungsPumpe_SeminarRaum_2023_v1

#### PlatformIO project

- Arduino Pro Mini 5V 16MHz 328P
- controls 5V High-State Relay-Module to switch heater pump on&off
- temperature control by OneWire temperature-probe (the DS18B20 with 3m cable and steelcasing)
- manual control via on/off switch, presumably with LED light feedback..
- plugin system to put between powerleads of pumpcircuit, self-powered via 5V USB wallwart plug
- set to 18 degree Celcius temps to switch on below 17.9 and switch off over 18.6
    - #define TEMP_MIN 18
    - #define TEMP_MAX 18.5
    - #define TEMP_HYSTERESE 0.1
- manual mode to override and run heaters on full..
- using LowPower.idle modes as iteration to enable >8s idle times.. its currently set to 20sec idle
    - we repeat 1s sleep idle for SLEEP_FOR_SECONDS times to get sleeptime 20sec
    - #define SLEEP_FOR_SECONDS 20

Have Fun and regards
Vigilante -
VigilanteSystems - April 2023

PS:
schematics or so are upcoming, still live-testing, looks good so far..
