# numeric-keypad
Numeric keypad implemented for Pro Micro using Arduino IDE

[Original repo by theimmc](https://github.com/theimmc/numeric-keypad)

Developed for [this layout](http://www.keyboard-layout-editor.com/#/gists/6e0ff19db4f3ac36ccb57e5ba46f8258)

![thing](http://kle-render.herokuapp.com/api/6e0ff19db4f3ac36ccb57e5ba46f8258)

New features and planned features:

* Multiple layers by holding num lock + div/mult/min/plus
* (N/A) Macro layer
* (N/A) Something to notify the user which layer/num lock we're on

Current usage:

* Starts with normal numpad layout
* Num lock state switches on num lock key _release_
* NumLock + DIV/MUL/MIN/PLS switches modes
  * DIV: Normal numpad
  * MUL: Navigation keys
  * MIN: WASD keys ("Game")
  * PLS: TBD (Macros?)
