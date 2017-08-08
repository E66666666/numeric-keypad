# numeric-keypad
Numeric keypad implemented for Pro Micro using Arduino IDE

[Original repo by theimmc](https://github.com/theimmc/numeric-keypad)

Adapted for [this layout (KLE)](http://www.keyboard-layout-editor.com/#/gists/6e0ff19db4f3ac36ccb57e5ba46f8258), which has multiple layer support.

![KLE-render](http://kle-render.herokuapp.com/api/6e0ff19db4f3ac36ccb57e5ba46f8258)

## Features

* Multiple layers by holding num lock + div/mult/min/plus
* (WIP) Macro layer
* (WIP) Something to notify the user which layer/num lock we're on

## Usage

* Starts with normal numpad layout
* Num lock state switches on num lock key _release_
* NumLock + DIV/MUL/MIN/PLS switches modes
  * DIV: Normal numpad
  * MUL: Navigation keys
  * MIN: WASD keys ("Game")
  * PLS: TBD (Macros?)

## Building

This project uses this fork of [StandardCplusplus](https://github.com/SGSSGene/StandardCplusplus). Follow the instructions there to install the library.

Build this project with the Arduino IDE. Last tested version is 1.8.3. Currently has a few issues with building for Visual Micro.

