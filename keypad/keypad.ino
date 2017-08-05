#include <Keyboard.h>
#include "KeyInfo.h"

const int COLUMNS = 4;
const int ROWS = 5;
const int HB_LED = 17;
const int NL_LED = 17;

const int NL_KEYCODE = 0xDB;

bool num_down = false;
bool num_down_prev = num_down;

// These are the column pins. They're configured for input with
// internal pullup

int input_pins[COLUMNS] = {10, 16, 14, 15};

// These are the row strobes. They're configured for output
// and initially all set to high. Individual pins are set to
// low to read that row. Only one row is low at any time.

int strobe_pins[ROWS] = {9, 8, 7, 6, 5};
unsigned long key_state[ROWS][COLUMNS];
KeyInfo keycode[ROWS][COLUMNS];

/*
 * NumLock + 
 *  / Preset 0
 *  * Preset 1
 *  - Preset 2
 *  + Preset 3
 */

/*
 * Preset 0: Default numpad
 * Preset 1: Arrows + functions
 * Preset 2: WASD + numbers etc
 * Preset 3: osu? macros? Ehhh.
 */

// Codes for numeric keypad
// NUM  /   *   -
//  7   8   9   x
//  4   5   6   +
//  1   2   3   x
//  0   0   . ENTER

KeyInfo NUM(KeyType::KeyCode, 0xDB, -1);
KeyInfo DIV(KeyType::Dual, 0xDC, 0);
KeyInfo MUL(KeyType::Dual, 0xDD, 1);
KeyInfo MIN(KeyType::Dual, 0xDE, 2);
KeyInfo PLS(KeyType::Dual, 0xDF, 3);

KeyInfo keycode_preset0[ROWS][COLUMNS] = {
	{ NUM, DIV, MUL, MIN },
	{ KeyInfo(KeyType::KeyCode, 0xE7, -1), KeyInfo(KeyType::KeyCode, 0xE8, -1), KeyInfo(KeyType::KeyCode, 0xE9, -1), KeyInfo()},
	{ KeyInfo(KeyType::KeyCode, 0xE4, -1), KeyInfo(KeyType::KeyCode, 0xE5, -1), KeyInfo(KeyType::KeyCode, 0xE6, -1), PLS},
	{ KeyInfo(KeyType::KeyCode, 0xE1, -1), KeyInfo(KeyType::KeyCode, 0xE2, -1), KeyInfo(KeyType::KeyCode, 0xE3, -1), KeyInfo()},
	{ KeyInfo(KeyType::KeyCode, 0xEA, -1), KeyInfo(KeyType::KeyCode, 0xEA, -1), KeyInfo(KeyType::KeyCode, 0xEB, -1), KeyInfo(KeyType::KeyCode, 0xE0, -1) }
};

// NUM   P0   P1   P2
// INS  HOM  PGU   x
// DEL  END  PGD   P3
// BCK   U   FWD   x
//  L    D    R  ENTER

KeyInfo keycode_preset1[ROWS][COLUMNS] = {
	{ NUM, DIV, MUL, MIN },
	{ KeyInfo(), KeyInfo(), KeyInfo(), KeyInfo() },
	{ KeyInfo(), KeyInfo(), KeyInfo(), KeyInfo() },
	{ KeyInfo(KeyType::KeyCode, 0xB3, -1), KeyInfo(KeyType::KeyCode, 0xDA, -1), KeyInfo(KeyType::KeyCode, 0xB2, -1), KeyInfo() },
	{ KeyInfo(KeyType::KeyCode, 0xD8, -1), KeyInfo(KeyType::KeyCode, 0xD9, -1), KeyInfo(KeyType::KeyCode, 0xD7, -1), KeyInfo(KeyType::KeyCode, 0xB0, -1) }
};

int strobe_row = 0;
int q = 0;

void setup() {
	int cnt;
	int cnt2;

	pinMode(NL_LED, OUTPUT);

	memcpy(keycode, keycode_preset0, sizeof(keycode_preset0));
	for (cnt = 0; cnt < ROWS; cnt++) {
		pinMode(strobe_pins[cnt], OUTPUT);
		digitalWrite(strobe_pins[cnt], HIGH);

		for (cnt2 = 0; cnt2 < COLUMNS; cnt2++) key_state[cnt][cnt2] = 0;
	}

	for (cnt = 0; cnt < COLUMNS; cnt++) {
		pinMode(input_pins[cnt], INPUT_PULLUP);
	}

	Keyboard.begin();
}

const int DEBOUNCE_MS = 20;

bool debounce(unsigned long t_now, unsigned long t_prev) {
	unsigned long diff;

	diff = t_now - t_prev; // need to check for underflow?

	if (diff <= DEBOUNCE_MS) return true;
	else return false;
}

void loop() {
	unsigned long tick_now = millis();
	int cnt;

	// since we use non zero to indicate pressed state, we need
	// to handle the edge case where millis() returns 0

	if (tick_now == 0) tick_now = 1;

	// every time we enter this loop, we're reading only the switches
	// on strobe_row

	if (strobe_row >= ROWS) strobe_row = 0;

	digitalWrite(strobe_pins[strobe_row], LOW);
	delay(2); // give it some time to stabilize just in case

	// We check all the switches in the row

	int nextMap = -1;

	for (cnt = 0; cnt < COLUMNS; cnt++) {
		// ignore state change for pin if in debounce period
		if (key_state[strobe_row][cnt] != 0) {
			if (debounce(tick_now, key_state[strobe_row][cnt]) == true) {
				continue;
			}
		}

		if (digitalRead(input_pins[cnt]) == HIGH) {
			if (key_state[strobe_row][cnt] != 0) {
				if (keycode[strobe_row][cnt].GetKeyCode() == NL_KEYCODE) {
					num_down = false;
				}
				else {
					Keyboard.release(keycode[strobe_row][cnt].GetKeyCode());
				}
				key_state[strobe_row][cnt] = 0;
			}
		}
		else {
			if (key_state[strobe_row][cnt] == 0) {
				if (num_down) {
					if (keycode[strobe_row][cnt].GetKeyCode() == DIV.GetKeyCode()) {
						nextMap = 0;
						break;
					}
					if (keycode[strobe_row][cnt].GetKeyCode() == MUL.GetKeyCode()) {
						nextMap = 1;
						break;
					}
				}


				if (keycode[strobe_row][cnt].GetKeyCode() == NL_KEYCODE) {
					num_down = true;
				}
				else {
					Keyboard.press(keycode[strobe_row][cnt].GetKeyCode());
				}
				key_state[strobe_row][cnt] = tick_now;
			}
		}
	}

	switch(nextMap) {
		case 0:
			memcpy(keycode, keycode_preset0, sizeof(keycode_preset0));
			break;
		case 1:
			memcpy(keycode, keycode_preset1, sizeof(keycode_preset1));
			break;
		default:
			break;
	}
	

	if (num_down != num_down_prev && !num_down) {
		Keyboard.press(NL_KEYCODE);
		Keyboard.release(NL_KEYCODE);
	}
	num_down_prev = num_down;

	digitalWrite(strobe_pins[strobe_row], HIGH);
	strobe_row++;
	delay(5);
}
