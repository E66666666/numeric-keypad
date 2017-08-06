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
bool switchedLayer = false;

/*
 * Define all keys we're gonna be using
 */

KeyInfo NUM(KeyType::KeyCode, 0xDB, -1);
KeyInfo DIV(KeyType::Dual, 0xDC, 0);
KeyInfo MUL(KeyType::Dual, 0xDD, 1);
KeyInfo MIN(KeyType::Dual, 0xDE, 2);
KeyInfo PLS(KeyType::Dual, 0xDF, 3);

KeyInfo LY0(KeyType::Layer, 0, 0);
KeyInfo LY1(KeyType::Layer, 0, 1);
KeyInfo LY2(KeyType::Layer, 0, 2);
KeyInfo LY3(KeyType::Layer, 0, 3);

KeyInfo INS(0xD1);
KeyInfo DEL(0xD4);
KeyInfo HOM(0xD2);
KeyInfo END(0xD5);
KeyInfo PGU(0xD3);
KeyInfo PGD(0xD6);

KeyInfo RET(0xB0);

KeyInfo _U_(0xDA);
KeyInfo _D_(0xD9);
KeyInfo _L_(0xD8);
KeyInfo _R_(0xD7);

KeyInfo NM0(0xEA);
KeyInfo NM1(0xE1);
KeyInfo NM2(0xE2);
KeyInfo NM3(0xE3);
KeyInfo NM4(0xE4);
KeyInfo NM5(0xE5);
KeyInfo NM6(0xE6);
KeyInfo NM7(0xE7);
KeyInfo NM8(0xE8);
KeyInfo NM9(0xE9);
KeyInfo NMD(0xEB);
KeyInfo NMR(0xE0);

KeyInfo __W('w');
KeyInfo __A('a');
KeyInfo __S('s');
KeyInfo __D('d');
KeyInfo __Q('q');
KeyInfo __E('e');
KeyInfo __F('f');
KeyInfo __R('r');
KeyInfo __1('1');
KeyInfo __2('2');
KeyInfo __3('3');
KeyInfo __4(KeyType::Dual, '4', 3);
KeyInfo TAB(0xB3);
KeyInfo SPC(0x20);

KeyInfo ___;

/*
 * Switching keys: Hold NumLock, press 
 *  / for layer 0
 *  * for layer 1
 *  - for layer 2
 *  + for layer 3
 */

// Layer 0: Numeric pad
// NUM  /   *   -
//  7   8   9   x
//  4   5   6   +
//  1   2   3   x
//  0   0   . ENTER


KeyInfo* keycode_layer0[ROWS][COLUMNS] = {
	{ &NUM, &DIV, &MUL, &MIN },
	{ &NM7, &NM8, &NM9, &___ },
	{ &NM4, &NM5, &NM6, &PLS },
	{ &NM1, &NM2, &NM3, &___ },
	{ &NM0, &NM0, &NMD, &NMR }
};

// Layer 1: Navigation cluster
// NUM   P0   P1   P2
// INS  HOM  PGU   x
// DEL  END  PGD   P3
// TAB   U    x    x
//  L    D    R  ENTER

KeyInfo* keycode_layer1[ROWS][COLUMNS] = {
	{ &NUM, &LY0, &LY1, &LY2 },
	{ &INS, &HOM, &PGU, &___ },
	{ &DEL, &END, &PGD, &LY3 },
	{ &TAB, &_U_, &___, &___ },
	{ &_L_, &_D_, &_R_, &RET }
};

// Layer 2: WASD
// NUM   P0   P1   P2
//  1    2    3    x
// TAB  SPC   R    4
//  Q    W    E    x
//  A    S    D    F

KeyInfo* keycode_layer2[ROWS][COLUMNS] = {
	{ &NUM, &LY0, &LY1, &LY2 },
	{ &__1, &__2, &__3, &___ },
	{ &TAB, &SPC, &__R, &__4 },
	{ &__Q, &__W, &__E, &___ },
	{ &__A, &__S, &__D, &RET }
};

// Layer 3: TBD

KeyInfo* keycode_layer3[ROWS][COLUMNS] = {
	{ &NUM, &LY0, &LY1, &LY2 },
	{ &___, &___, &___, &___ },
	{ &___, &___, &___, &LY3 },
	{ &___, &___, &___, &___ },
	{ &___, &___, &___, &RET }
};

void setup() {
	memcpy(keycode, keycode_layer0, sizeof(keycode_layer0));
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
					num_down && keyInfo.GetType() == KeyType::Layer) {
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
					num_down && keyInfo.GetType() == KeyType::Layer) {
					nextMap = keyInfo.GetSwitch();
					switchedLayer = true;
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
			memcpy(keycode, keycode_layer0, sizeof(keycode_layer0));
			break;
		case 1:
			memcpy(keycode, keycode_layer1, sizeof(keycode_layer1));
			break;
		case 2:
			memcpy(keycode, keycode_layer2, sizeof(keycode_layer2));
			break;
		default:
			break;
	}
	

	if (num_down != num_down_prev && !num_down) {
		if (switchedLayer) {
			switchedLayer = false;
		}
		else {
			Keyboard.press(NL_KEYCODE);
			Keyboard.release(NL_KEYCODE);
		}
	}
	num_down_prev = num_down;

	digitalWrite(strobe_pins[strobe_row], HIGH);
	strobe_row++;
}
