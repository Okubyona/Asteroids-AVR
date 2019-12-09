# Asteroids Puzzle!
<!-- Fix release shield once a release is pushed -->
![Github repo size](https://img.shields.io/github/repo-size/Okubyona/Asteroids-AVR)
![Github contributors](https://img.shields.io/github/contributors/Okubyona/Asteroids-AVR)
![Github activity](https://img.shields.io/github/commit-activity/m/Okubyona/Asteroids-AVR)
![Github commits since release](https://img.shields.io/github/commits-since/Okubyona/Asteroids-AVR/v0.3.1)
![Github commit status](https://img.shields.io/github/commit-status/Okubyona/Asteroids-AVR/master/2858a2505874f809aec7634e675d93f00b610534)
![Github release](https://img.shields.io/github/v/release/Okubyona/Asteroids-AVR?include_prereleases)

Asteroids Puzzle! is a basic puzzle game that has you race against the clock to
try and destroy as many asteroids as you can.

The asteroids vary in 3 different sizes, small medium and large, where the largest
split into 2 of the next size. (i.e. Large breaks into 2 medium and so on).

However, this version of the game is played on an extremely limited 16x2 LCD screen.

**Programmed using Atmel Studio 7**

## User Guide

The game is controlled by the joystick and button seen in the image below.

![Alt text](/images/controls.jpg =400x300)

The player's ship is moved up or down with the joystick (up is towards the wires)
and the ship fires  its laser using the single button connected to the brown wire.

To reset the game and high score, simply press the button built into the joystick.

### Menus
(sorry no images yet)

On startup, the player will be presented with the title screen **Asteroids Puzzle!**.

Using the shoot button, the player will be able to advance through the next few
screens.

Pressing shoot once will bring the player to the difficulty selection screen, allowing
them to select their desired difficulty.

Afterwards, the current high score for that difficulty will be displayed and pressing
shoot for a final time will start the game.

### Gameplay
(same deal here)

The user will have 45 seconds to destroy as many asteroids as they can while making sure
that the asteroids do not hit the player.

However the asteroids are static, and will only move closer to the player when a
larger asteroid is shot and has no free space in its column to split. This will
cause all asteroids in front of it to move towards the player by one column.

There are 3 types of asteroids each one bigger than the last. A large asteroids
will split into 2 medium asteroids and so on.

This means that asteroids can only free up space once they are broken up into the
smallest size and shot a final time.

Large asteroids are worth 100 points. Medium asteroids are worth 50 points.
Small asteroids are worth 25 points.


## Hardware

Extra hardware used aside from the ATmega1284 seen in the following photos
(there is another microcontroller that remains unused for now) are:

* Joystick

* 16x2 LCD Screen

![Alt text](/images/joystick.jpg =400x300)

## Wiring

Joystick and buttons are connected to PORTA with x y joystick ADC channels connected
to A0 and A1, joystick switch on A2 and the single button on A3

*(Blue and purple wires are the ground and 5v wires for the joystick.)*

![Alt text](/images/wiring_input.jpg =400x300)

Wiring for the LCD screen

![Alt text](/images/wiring_output.jpg =400x300)

## Technologies Learned

* Concurrent Synchronous State Machine Design

* AVR functionality

* Atmel Studio 7
