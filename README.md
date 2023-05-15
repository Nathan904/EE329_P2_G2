# NUCLEO-STM32L4A6ZG Waveform Generator

This repository contains a C program designed for the STM32L4A6ZG Nucleo board. The program generates and controls different types of waveforms (square, sine, and ramp) at various frequencies (100Hz, 200Hz, 300Hz, 400Hz, 500Hz) using a Digital-to-Analog Converter (DAC).
The program also allows for user interaction via a keypad and provides feedback through an LCD display.

## Features

- Generation of square, sine, and ramp waveforms
- Control of waveform frequency (100Hz, 200Hz, 300Hz, 400Hz, 500Hz)
- User interaction via a keypad
- Feedback through an LCD display
- Error handling

### Prerequisites

- STM32L4A6ZG Nucleo board
- Keypad (Adafruit 4x4 Matrix Keypad)
- LCD display (Standard 5V 1602 LCD)
- DAC (MCP4821 12-bit resolution DAC)
- STM32CubeIDE

### Installation

1. Clone the repository to your local machine.
2. Open the project in STM32CubeIDE
3. Build the project and flash it to your STM32L4A6ZG Nucleo board.

## Usage

After flashing the program to your STM32L4A6ZG Nucleo board:

1. The program will start by generating a 100Hz square wave with a 50% duty cycle.
2. Use the keypad to control the waveform type, frequency, duty cycle (for square waves), and polarity (for ramp waves).
3. The LCD display will show the current waveform type, frequency, duty cycle (for square waves), and the last key pressed.

## Wiring

If desired, the wiring configuration can be changed using the peripheral's header file.

### 2x16 1602 LCD Pinout:
```
+------------------------+
|    16x2 1602 LCD       |
|                        |
| [1] VSS  ---- GND      |
| [2] VDD  ---- 5V       |
| [3] V0   ---- VR1.Vout |
| [4] RS   ---- PD4      |
| [5] RW   ---- PD5      |
| [6] E    ---- PD6      |
| [7] D0   ---- NC       |
| [8] D1   ---- NC       |
| [9] D2   ---- NC       |
| [10] D3  ---- NC       |
| [11] D4  ---- PD0      |
| [12] D5  ---- PD1      |
| [13] D6  ---- PD2      |
| [14] D7  ---- PD3      |
| [15] A   ---- 5V       |
| [16] K   ---- GND      |
|                        |
| VR1.Vin ---- 5V        |
| VR1.GND ---- GND       |
+------------------------+
```
Note: NC stands for "No Connection".

### MCP-4821 12-bit DAC Pinout:
```
 _________________________________________________
/                                                 \
|1 VDD ----3.3V|                  |5 LDAC ---- PA6|
|                                                 |
|2 CS  ---- PA4|                  |6 SHDN ----  NC|
|                                                 |
|3 SCK ---- PA5|                  |7 VSS ----- GND|
|                                                 |
|4 SDI ---- PA7|                  |8 VOUT---- VOUT|
|                                                 |
\_________________________________________________/
```
### Adafruit 4x4 Matrix Keypad Pinout:
```
+------------------------+
| Adafruit 4x4 Keypad    |
|                        |
| [1] R1 ---- PE09       |
| [2] R2 ---- PE11       |
| [3] R3 ---- PE13       |
| [4] R4 ---- PE08       |
| [5] C1 ---- PE07       |
| [6] C2 ---- PE10       |
| [7] C3 ---- PE12       |
| [8] C4 ---- PE14       |
|                        |
+------------------------+
```
## Bill of Materials
[EE329_P2_BOM.xlsx](https://github.com/Nathan904/EE329_P2_G2/files/11473286/EE329_P2_BOM.xlsx)
