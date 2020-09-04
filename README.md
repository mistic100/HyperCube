# HyperCube

Homemade clone of the [HyperCube by the Hyperspace Lighting Company](https://www.hyperspacelight.com/the-hypercube).

![](https://galerie.strangeplanet.fr/_data/i/upload/2020/09/04/20200904192354-e907e9ac-me.jpg)

[Photos on my website](https://galerie.strangeplanet.fr/index.php?/category/199)

## Parts list

### 3D printed parts

- 6x beam
- 2x beam-w-hole
- 2x beam-bottom (one symmetry !)
- 2x beam-bottom-w-hole (one symmetry !)
- 4x corner
- 4x corner-bottom
- 5x corner-cache
- 1x corner-cache-w-hole
- 4x panel-holder
- 1x electronics-case
- 1x electronics-case-top
- 1x support

In doubt you can replace all beam variants by :
- 8x beam-w-hole
- 4x beam-bottom-w-hole (two symmetries !)

The no-hole variants use less material and are faster to print.

### Hardware

- 6x 210mm/2mm square one-way mirrors (or standard glass/PMAA with one-way film)
- 24x M2 15mm wood screws

### Electronics

- 6x 80cm WS2812 LED strips (60 LEDS/m)
- 1x Arduino Nano
- 1x Bluetooth module
- 1x capacitive touch button (20x15mm)
- 1x 10A 5V power supply
- 3-wire cable


## Assembly

Special care must be put in the orientation of each LED strips and the placement of each beam.

- The 6 LED strips are wired in parallel and must be in the same direction on the two sides of each beam (the direction changes every 20cm in the code).
- All wires must output on the same corner.
- All strips must be glued and the wires passed through the holes before puting the bottom panel.

The following diagram shows one way to arrange things (flattened cube seen from top).  
![diagram](diagram.png)


## Code

### Configuration

Edit the `constants.h` file according to your wiring :

- `PIN_LED` output pin use to control LEDS
- `PIN_BUTTON` input pin for the control button
- `BLE_RXD` RX pin for the BLE module
- `BLE_TXD` TX pin for the BLE module

Edit `MAX_MILLI_AMPS` according to the rating of your power supply (keep a safety margin, especially with cheap Chinese stuff).

### Controls

The cube is controlled via Bluetooth and can received basic serial commands:

- `BRIGHT X`(X=0-255) to change the brightness
- `BRIGHT+` to increase the brightness by 16
- `BRIGHT-` to decrease the brightness by 16
- `MODE X` to change the current mode, modes are:
	- `RAINBOW`
	- `RAINDROP`
	- `PULSE`
	- `CYLON`
	- `CHASER`
	- `RANDOM`
- `MODE+` to use the next mode
- `MODE-` to use the previous mode
- `HUE X` to change the current hue, huse are:
	- `RAINBOW`
	- `RED`
	- `YELLOW`
	- `GREEN`
	- `BLUE`
	- `PINK`
	- `WHITE`
- `HUE+` to use the next hue
- `HUE-` to use the previous hue


## License

The SolidWorks files and Arduino code are distributed under the Creative Commons 3.0 BY-SA license.
