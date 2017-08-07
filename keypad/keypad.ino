#include <Keyboard.h>

#include "KeyInfo.h"
#include "Macro.h"

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
//KeyInfo *keycode[ROWS][COLUMNS];
KeyInfo ***keycode;

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
KeyInfo PRT(KeyType::Dual, 0xCE, 0);
KeyInfo WIN(KeyType::Dual, 0x87, 1);
KeyInfo MEN(KeyType::Dual, 0xED, 2);
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

Macro createMacro(int macro) {
	switch (macro) {
	case 0: {
		Stroke* strokes = (Stroke *)malloc(13 * sizeof(Stroke));
		
		
		strokes[0] = Stroke('H', 0, 19)   ;
		strokes[1] = Stroke('e', 20, 40)  ;
		strokes[2] = Stroke('l', 40, 60)  ;
		strokes[3] = Stroke('l', 60, 80)  ;
		strokes[4] = Stroke('o', 80, 100) ;
		strokes[5] = Stroke(',', 100, 110);
		strokes[6] = Stroke(' ', 110, 120);
		strokes[7] = Stroke('W', 120, 139);
		strokes[8] = Stroke('o', 140, 160);
		strokes[9] = Stroke('r', 160, 180);
		strokes[10] = Stroke('l', 200, 220);
		strokes[11] = Stroke('d', 220, 240);
		strokes[12] = Stroke('!', 240, 260);
		
		return Macro(13, strokes);
	}
	case 1: {
		Stroke* strokes = (Stroke *)malloc(3 * sizeof(Stroke));
		strokes[0] = Stroke(KEY_LEFT_CTRL, 0, 100);
		strokes[1] = Stroke(KEY_LEFT_ALT, 20, 100);
		strokes[2] = Stroke(KEY_DELETE, 40, 100);
		
		return Macro(3, strokes);
	}
	default:
		return Macro(0, nullptr);
	}
}

KeyInfo M_HelloWorld(KeyType::Macro, 0, -1, 0);
KeyInfo M_CTRLALTDEL(KeyType::Macro, 0, -1, 1);

/*
 * Switching keys: Hold NumLock, press 
 *  / for layer 0
 *  * for layer 1
 *  - for layer 2
 *  + for layer 3
 */

void makeLayer(KeyInfo *keycode_layer3[5][4], KeyInfo ***&layer) {
	layer = (KeyInfo ***)malloc(ROWS * sizeof(KeyInfo **));
	for (uint8_t row = 0; row < ROWS; row++) {
		layer[row] = (KeyInfo **)malloc(COLUMNS * sizeof(KeyInfo *));
	}

	for (uint8_t row = 0; row < ROWS; row++) {
		for (uint8_t col = 0; col < COLUMNS; col++) {
			layer[row][col] = keycode_layer3[row][col];
		}
	}
}

KeyInfo*** getLayer(uint8_t layerId) {
	switch(layerId) {
	case 0: {
		KeyInfo* keycode_layer0[ROWS][COLUMNS] = {
			{ &NUM, &DIV, &MUL, &MIN },
			{ &NM7, &NM8, &NM9, &___ },
			{ &NM4, &NM5, &NM6, &PLS },
			{ &NM1, &NM2, &NM3, &___ },
			{ &NM0, &NM0, &NMD, &NMR }
		};

		KeyInfo ***layer;
		makeLayer(keycode_layer0, layer);
		return layer;
	}
	case 1: {
		KeyInfo* keycode_layer1[ROWS][COLUMNS] = {
			{ &NUM, &PRT, &WIN, &MEN },
			{ &INS, &HOM, &PGU, &___ },
			{ &DEL, &END, &PGD, &LY3 },
			{ &___, &_U_, &___, &___ },
			{ &_L_, &_D_, &_R_, &RET }
		};

		KeyInfo ***layer;
		makeLayer(keycode_layer1, layer);
		return layer;
	}
	case 2: {
		KeyInfo* keycode_layer2[ROWS][COLUMNS] = {
			{ &NUM, &LY0, &LY1, &LY2 },
			{ &__1, &__2, &__3, &___ },
			{ &TAB, &SPC, &__R, &__4 },
			{ &__Q, &__W, &__E, &___ },
			{ &__A, &__S, &__D, &RET }
		};

		KeyInfo ***layer;
		makeLayer(keycode_layer2, layer);
		return layer;
	}
	case 3: {
		KeyInfo *keycode_layer3[ROWS][COLUMNS] = {
			{ &NUM, &LY0, &LY1, &LY2 },
			{ &___, &___, &___, &___ },
			{ &___, &M_HelloWorld, &M_CTRLALTDEL, &LY3 },
			{ &___, &___, &___, &___ },
			{ &___, &___, &___, &RET }
		};

		KeyInfo ***layer;
		makeLayer(keycode_layer3, layer);
		return layer;
	}
	default:
		return nullptr;
	}
}

void setup() {
	keycode = getLayer(0);

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

void playMacro(int nextMacro) {
	Macro macro_ = createMacro(nextMacro);
	Macro* macro = &macro_;
	if (macro == nullptr) {
		return;
	}
	if (macro->NumStrokes == 0) {
		return;
	}
	unsigned long t_start = millis();
	unsigned long t_total = macro->Strokes[macro->NumStrokes - 1].KeyUp - macro->Strokes[0].KeyDown;
	unsigned extraDelay = 10;

	for (int i = 0; i < macro->NumStrokes; i++) {
		auto key = macro->Strokes[i].Key;
		auto active = macro->Strokes[i].Active;
		auto keyup = macro->Strokes[i].KeyUp;
		auto keydown = macro->Strokes[i].KeyDown;
	}

	while (true) {
		if (millis() > t_start + t_total + extraDelay)
			break;

		for (int i = 0; i < macro->NumStrokes; i++) {
			if (millis() > t_start + macro->Strokes[i].KeyDown && !macro->Strokes[i].Active) {
				Keyboard.press(macro->Strokes[i].Key);
				macro->Strokes[i].Active = true;
			}
			if (millis() > t_start + macro->Strokes[i].KeyUp && macro->Strokes[i].Active) {
				Keyboard.release(macro->Strokes[i].Key);
			}
		}
	}
	free(macro->Strokes);
}

void loop() {
	unsigned long tick_now = millis();
	int8_t nextMap = -1;
	int8_t nextMacro = -1;

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

				if (keyInfo.GetType() == KeyType::Macro) {
					nextMacro = keyInfo.GetMacro();
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
		case 1:
		case 2:
		case 3:
			for (int row = 0; row < ROWS; row++) {
				free(keycode[row]);
			}
			free(keycode);
			keycode = getLayer(nextMap);
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

	if (nextMacro >= 0) {
		playMacro(nextMacro);
	}
}
