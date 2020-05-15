# *Eye Squared See*

#### Table of Contents
1. [Project Description](#project)<br/>
2. [Game Modes](#mode)<br/>
3. [Complete Parts List](#parts)


# <a name="project">Project Description</a>
*Eye Squared See* is an **Arduino** project that aims to test and improve hand-eye coordination by having players simultaneously focus their eyes and hands on a moving LED and colored buttons. In other words, it is a reaction game with two game modes that requires the player to use both their eyes and fingers to play.

This is also my first time working with microprocessors and embedded systems.

**Power drawn:** 215mA
**I/O Pins used:** 6 + SCL and SDA pins + RESET,GND and 5V pins.

# <a name="mode">Game Modes</a>
The two game modes are similar, but serve different purposes.

After receiving power, the LCD will display the welcome screen, which asks the player to choose a game mode using the buttons on the LCD shield. In both of the game modes, the RGB LED will randomly shine three different colors: red, green, or yellow. The RBG LED will also be moving back and forth with the assistance of the stepper motor and timing belt. The player would have to press the same colored button as the current color of the RBG LED. The rate that the color changes on the RBG LED is dependent on the steps of the stepper motor. When the stepper motor changes direction, the LCD will update the time, lives, and scores and display them to the player.

In the first game mode, also known as Game Mode 1, the player is given 3 lives to reach a score of 60. If the player does not press a button, or pushes the wrong button, a life is taken away. However, there might be instances where a button is pressed but the input is not read. Thus, I decided to add a life whenever the player reaches a score that is a multiple of 10, up to 50. The speed that the LED changes color will get faster when the player reaches a score of 10 and 40.

In the second game mode, the player is given 60 seconds to get as many points as possible. The rate the color changes is set to the “highest” setting, which is equivalent to the last level of the first game mode (which is when the player reaches a score of 40).

# <a name="parts">Complete Parts List</a>
* [Arduino Uno Rev3](https://store.arduino.cc/usa/arduino-uno-rev3)
* [Adafruit Motor/Stepper/Servo Shield for Arduino v2 Kit - v2.3](https://www.adafruit.com/product/1438)
* [Small Reduction Stepper Motor - 5VDC 32-Step 1/16 Gearing](https://www.adafruit.com/product/858)
* [LCD Shield Kit w/ 16x2 Character Display - Only 2 pins used! - BLUE AND WHITE](https://www.adafruit.com/product/772)
* [Colorful Round Tactile Button Switch Assortment - 15 pack](https://www.adafruit.com/product/1009)
* [Shield stacking headers for Arduino (R3 Compatible)](https://www.adafruit.com/product/85)
* [Jumbo RGB LED - 10mm Common Anode](https://tinkersphere.com/standard-leds/1337-jumbo-rgb-led-10mm-common-anode.html)
* [9V DC Power Supply (Arduino Compatible)](https://tinkersphere.com/arduino-compatible-components/142-9v-wall-power-adapter-arduino-compatible.html)
* [GT2 Timing Belt Pulley, 8pcs 5mm 20 Teeth Timing Pulley Wheel and GT2 5 Meters Rubber 2mm Pitch 6mm Wide Timing Belt with Allen Wrench for 3D Printer CNC by Beauty Star](http://amzn.com/B0776KXY8G)
* Box with a lid
