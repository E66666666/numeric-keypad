#include <Keyboard.h>
#include "KeyInfo.h"

const uint8_t COLUMNS = 4;
const uint8_t ROWS = 5;

const uint8_t NL_KEYCODE = 0xDB;
const uint8_t DEBOUNCE_MS = 20;

// These are the column pins. They're configured for input with
// internal pullup

const uint8_t input_pins[COLUMNS] = {10, 16, 14, 15};

// These are the row strobes. They're configured for output
// and initially all set to high. Individual pins are set to
// low to read that row. Only one row is low at any time.

const uint8_t strobe_pins[ROWS] = {9, 8, 7, 6, 5};

unsigned long key_state[ROWS][COLUMNS];
KeyInfo *keycode[ROWS][COLUMNS];

uint8_t strobe_row = 0;
bool num_down = false;
bool num_down_prev = num_down;

/*
 * Define all keys we're gonna be using
 */

KeyInfo NUM(KeyType::KeyCode, 0xDB, -1);
KeyInfo DIV(KeyType::Dual, 0xDC, 0);
KeyInfo MUL(KeyType::Dual, 0xDD, 1);
KeyInfo MIN(KeyType::Dual, 0xDE, 2);
KeyInfo PLS(KeyType::Dual, 0xDF, 3);

KeyInfo PR0(KeyType::Switch, 0, 0);
KeyInfo PR1(KeyType::Switch, 0, 1);
KeyInfo PR2(KeyType::Switch, 0, 2);
KeyInfo PR3(KeyType::Switch, 0, 3);

KeyInfo INS(KeyType::KeyCode, 0xD1, -1);
KeyInfo DEL(KeyType::KeyCode, 0xD4, -1);
KeyInfo HOM(KeyType::KeyCode, 0xD2, -1);
KeyInfo END(KeyType::KeyCode, 0xD5, -1);
KeyInfo PGU(KeyType::KeyCode, 0xD3, -1);
KeyInfo PGD(KeyType::KeyCode, 0xD6, -1);

KeyInfo RET(KeyType::KeyCode, 0xB0, -1);

KeyInfo _U_(KeyType::KeyCode, 0xDA, -1);
KeyInfo _D_(KeyType::KeyCode, 0xD9, -1);
KeyInfo _L_(KeyType::KeyCode, 0xD8, -1);
KeyInfo _R_(KeyType::KeyCode, 0xD7, -1);

KeyInfo NM0(KeyType::KeyCode, 0xEA, -1);
KeyInfo NM1(KeyType::KeyCode, 0xE1, -1);
KeyInfo NM2(KeyType::KeyCode, 0xE2, -1);
KeyInfo NM3(KeyType::KeyCode, 0xE3, -1);
KeyInfo NM4(KeyType::KeyCode, 0xE4, -1);
KeyInfo NM5(KeyType::KeyCode, 0xE5, -1);
KeyInfo NM6(KeyType::KeyCode, 0xE6, -1);
KeyInfo NM7(KeyType::KeyCode, 0xE7, -1);
KeyInfo NM8(KeyType::KeyCode, 0xE8, -1);
KeyInfo NM9(KeyType::KeyCode, 0xE9, -1);
KeyInfo NMD(KeyType::KeyCode, 0xEB, -1);
KeyInfo NMR(KeyType::KeyCode, 0xE0, -1);

KeyInfo XXX;

/*
 * Switching keys: Hold NumLock, press 
 *  / Layout 0
 *  * Layout 1
 *  - Layout 2
 *  + Layout 3
 */

// Layout 0: Numeric pad
// NUM  /   *   -
//  7   8   9   x
//  4   5   6   +
//  1   2   3   x
//  0   0   . ENTER


KeyInfo* keycode_preset0[ROWS][COLUMNS] = {
	{ &NUM, &DIV, &MUL, &MIN },
	{ &NM7, &NM8, &NM9, &XXX },
	{ &NM4, &NM5, &NM6, &PLS },
	{ &NM1, &NM2, &NM3, &XXX },
	{ &NM0, &NM0, &NMD, &NMR }
};

// Layout 1: Navigation cluster
// NUM   P0   P1   P2
// INS  HOM  PGU   x
// DEL  END  PGD   P3
//  x    U    x    x
//  L    D    R  ENTER

KeyInfo* keycode_preset1[ROWS][COLUMNS] = {
	{ &NUM, &PR0, &PR1, &PR2 },
	{ &INS, &HOM, &PGU, &XXX },
	{ &DEL, &END, &PGD, &PR3 },
	{ &XXX, &_U_, &XXX, &XXX },
	{ &_L_, &_D_, &_R_, &RET }
};

// Layout 2: TBD

// Layout 3: TBD


void setup() {
	memcpy(keycode, keycode_preset0, sizeof(keycode_preset0));
	for (uint8_t cnt = 0; cnt < ROWS; cnt++) {
		pinMode(strobe_pins[cnt], OUTPUT);
		digitalWrite(strobe_pins[cnt], HIGH);

		for (uint8_t cnt2 = 0; cnt2 < COLUMNS; cnt2++)
			key_state[cnt][cnt2] = 0;
	}

	for (uint8_t cnt = 0; cnt < COLUMNS; cnt++) {
		pinMode(input_pins[cnt], INPUT_PULLUP);
	}

	Keyboard.begin();
}


bool debounce(unsigned long t_now, unsigned long t_prev) {
	// need to check for underflow?
	if ((t_now - t_prev) <= DEBOUNCE_MS) 
		return true;
	return false;
}

void loop() {
	unsigned long tick_now = millis();
	int8_t nextMap = -1;

	// since we use non zero to indicate pressed state, we need
	// to handle the edge case where millis() returns 0

	if (tick_now == 0) tick_now = 1;

	// every time we enter this loop, we're reading only the switches 
	// on strobe_row
	if (strobe_row >= ROWS) strobe_row = 0;
	digitalWrite(strobe_pins[strobe_row], LOW);

	// We check all the switches in the row
	for (uint8_t cnt = 0; cnt < COLUMNS; cnt++) {
		// ignore state change for pin if in debounce period
		if (key_state[strobe_row][cnt] != 0) {
			if (debounce(tick_now, key_state[strobe_row][cnt]) == true) {
				continue;
			}
		}

		auto keyInfo = *keycode[strobe_row][cnt];
		if (digitalRead(input_pins[cnt]) == HIGH) { 
			// key up
			if (key_state[strobe_row][cnt] != 0) {
				if (num_down && keyInfo.GetType() == KeyType::Dual ||
					num_down && keyInfo.GetType() == KeyType::Switch) {
					break;
				}

				if (keyInfo.GetKeyCode() == NL_KEYCODE) {
					num_down = false;
				}
				else {
					Keyboard.release(keyInfo.GetKeyCode());
				}
				key_state[strobe_row][cnt] = 0;
			}
		}
		else {
			// key down
			if (key_state[strobe_row][cnt] == 0) {
				if (num_down && keyInfo.GetType() == KeyType::Dual ||
					num_down && keyInfo.GetType() == KeyType::Switch) {
					nextMap = keyInfo.GetSwitch();
					break;
				}

				if (keyInfo.GetKeyCode() == NL_KEYCODE) {
					num_down = true;
				}
				else {
					Keyboard.press(keyInfo.GetKeyCode());
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
}
