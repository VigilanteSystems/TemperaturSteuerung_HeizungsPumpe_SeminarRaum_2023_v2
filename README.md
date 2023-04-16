# TemperaturSteuerung_HeizungsPumpe_SeminarRaum_2023_v1

TemperaturSteuerung_HeizungsPumpe_SeminarRaum_2023_v1

PlatformIO project

- Arduino Pro Mini 5V 16MHz 328P
- controls 5V High-State Relay-Module to switch heater pump on&off
- temperature control by OneWire temperature-probe (the one with 3m cable and steelcasing)
- manual control via on/off switch, presumably with LED light feedback..
- plugin system to put between powerleads of pumpcircuit, self-powered via 5V USB wallwart plug
- set to 18 degree Celcius temps to switch on below 17.9 and switch off over 18.6
- -- #define TEMP_MIN 18
- -- #define TEMP_MAX 18.5
+ -- #define TEMP_HYSTERESE 0.1
- manual mode to override and run heaters on full..
- using LowPower.idle modes as iteration to enable >8s idle times.. its currently set to 20sec idle
- -- we repeat 1s sleep idle for SLEEP_FOR_SECONDS times to get sleeptime 20sec
- -- #define SLEEP_FOR_SECONDS 20

Have Fun and regards
Vigilante -
VigilanteSystems - April 2023

PS:
schematics or so are upcoming, still live-testing, looks good so far..
