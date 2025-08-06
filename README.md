# DK Bongos (and Other Nintendo Controllers)

&copy; 2018-2025 Stephen Barrack

I am not offering any license to this repository at this time. Please be patient while I figure out the ideal license for this repo.

## Supplies

* Teensy 3.2
* USB cable
* Soldering/breadboarding accoutrement
* DK Bongos
* 2 Wii nunchucks

### Optional

I strongly recommend these so that you don't have to permanently affix your controllers to the Arduino.

* 1 GCC extension cord
* 2 Wii remote extension cords

### Non-supplies

* Pull-up/down resistors
    * All of the controllers already have these :)
* Digital logic-level converter(s)
    * Teensy 3.2 is 5V tolerant :)

## Setup

* Cut the trace between Vusb & Vin.

```sh
sudo apt install teensy-loader-cli
sudo curl -o /etc/udev/rules.d/00-teensy.rules https://www.pjrc.com/teensy/00-teensy.rules
```

* [ ] Wiring guide comming soon™

## Usage

```sh
# Plug in your Teensy.
cd bongos
make
# Press the Teensy's restart button when prompted.
```
