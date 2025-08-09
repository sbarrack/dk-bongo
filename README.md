# DK Bongos

&copy; 2018-2025 Stephen Barrack

I am not offering any license to this repository at this time. Please be patient while I figure out the ideal license for this repo.

## Supplies

* Teensy 3.2
* USB cable
* Soldering/bread-boarding accoutrement
* DK Bongos
* 0-2 Wii Remote attachments
    * Solo or dual Nunchucks
    * Guitar

### Optional

I strongly recommend these so that you don't have to permanently affix your controllers to the Arduino. Cut them in half and solder/connect the female ends to the Teensy.

* 1 GameCube controller extension cord
* 0-2 Wii remote extension cords

### Non-supplies

* Pull-up/down resistors
    * All of the controllers already have these :D
* Digital logic-level converter(s)
    * Teensy 3.2 is 5V tolerant :D

## Setup

* Cut the trace between Vusb & Vin.

```sh
sudo apt install teensy-loader-cli
sudo curl -o /etc/udev/rules.d/00-teensy.rules https://www.pjrc.com/teensy/00-teensy.rules
sudo cat 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04[7-9]?", GOTO="nm_modem_probe_end"' >> /lib/udev/rules.d/77-nm-probe-modem-capabilities.rules
```

* [ ] Wiring guide comming soon™

## Usage

```sh
# Plug in your Teensy.
cd bongos
make
# Press the Teensy's restart button when prompted.
```
