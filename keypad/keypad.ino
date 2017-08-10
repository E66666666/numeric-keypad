#include <Keyboard.h>
#include <StandardCplusplus.h>
#include <vector>

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
std::vector<std::vector<KeyInfo>> keycode;

uint8_t strobe_row = 0;
bool num_down = false;
bool num_down_prev = num_down;
bool switchedLayer = false;

/*
 * Adding a macro:
 * Add a macro here and add a MacroType enum member in Macro.h,
 * then create a KeyInfo with that MacroType.
 */

Macro createMacro(MacroType macro) {
	switch (macro) {
	case MacroType::HelloWorld: {
		return Macro(std::vector<Stroke>{
			{ Stroke('H', 0, 10) },
			{ Stroke('e', 20, 10) },
			{ Stroke('l', 40, 10) },
			{ Stroke('l', 60, 10) },
			{ Stroke('o', 80, 10) },
			{ Stroke(',', 100, 10) },
			{ Stroke(' ', 110, 10) },
			{ Stroke('W', 120, 10) },
			{ Stroke('o', 150, 10) },
			{ Stroke('r', 160, 10) },
			{ Stroke('l', 200, 10) },
			{ Stroke('d', 220, 10) },
			{ Stroke('!', 240, 10) },
		});
	}
	case MacroType::CRTL_ALT_DEL: {
		return Macro(std::vector<Stroke>{
			{ Stroke(KEY_LEFT_CTRL, 0, 100) },
			{ Stroke(KEY_LEFT_ALT, 20, 80) },
			{ Stroke(KEY_DELETE, 40, 60) },
		});
	}
	case MacroType::Admin_CMD: {
		return Macro(std::vector<Stroke>{
			{ Stroke(0x87, 00, 50) },
			{ Stroke('x',  20, 20) },
			{ Stroke('a',  200, 20) },
		});
	}
	case MacroType::WinMin: {
		return Macro(std::vector<Stroke>{
			{ Stroke(0x87, 00, 20) },
			{ Stroke(KEY_DOWN_ARROW,  05, 10) },
		});
	}
	case MacroType::WinMax: {
		return Macro(std::vector<Stroke>{
			{ Stroke(0x87, 00, 20) },
			{ Stroke(KEY_UP_ARROW,  05, 10) },
		});
	}
	case MacroType::WinLeft: {
		return Macro(std::vector<Stroke>{
			{ Stroke(0x87, 00, 20) },
			{ Stroke(KEY_LEFT_ARROW,  05, 10) },
		});
	}
	case MacroType::WinRight: {
		return Macro(std::vector<Stroke>{
			{ Stroke(0x87, 00, 20) },
			{ Stroke(KEY_RIGHT_ARROW,  05, 10) },
		});
	}
	case MacroType::FiveMTest: {
		return Macro(std::vector<Stroke> {
			{ Stroke('r', 00, 10) },
			{ Stroke('e', 10, 10) },
			{ Stroke('s', 20, 10) },
			{ Stroke('t', 30, 10) },
			{ Stroke('a', 40, 10) },
			{ Stroke('r', 50, 10) },
			{ Stroke('t', 60, 10) },
			{ Stroke(' ', 70, 10) },
			{ Stroke('t', 80, 10) },
			{ Stroke('h', 90, 10) },
			{ Stroke('i', 100, 10) },
			{ Stroke('n', 110, 10) },
			{ Stroke('g', 120, 10) },
			{ Stroke(KEY_RETURN, 130, 10) },
		});
	}
	default:
		return Macro(std::vector<Stroke>{});
	}
}

/*
 * Switching keys: Hold NumLock, press 
 *  / for layer 0
 *  * for layer 1
 *  - for layer 2
 *  + for layer 3
 */

std::vector<std::vector<KeyInfo>> getLayer(uint8_t layerId) {
	// Macros
	KeyInfo M_HelloWorld(KeyType::Macro, 0, -1, (int8_t)MacroType::HelloWorld);
	KeyInfo M_CTRLALTDEL(KeyType::Macro, 0, -1, (int8_t)MacroType::CRTL_ALT_DEL);
	KeyInfo M_CMD(KeyType::Macro, 0, -1, (int8_t)MacroType::Admin_CMD);
	KeyInfo M_WINLEFT(KeyType::Macro, 0, -1, (int8_t)MacroType::WinLeft);
	KeyInfo M_WINRIGHT(KeyType::Macro, 0, -1, (int8_t)MacroType::WinRight);
	KeyInfo M_WINMAX(KeyType::Macro, 0, -1, (int8_t)MacroType::WinMax);
	KeyInfo M_WINMIN(KeyType::Macro, 0, -1, (int8_t)MacroType::WinMin);
	KeyInfo M_FIVEMTEST(KeyType::Macro, 0, -1, (int8_t)MacroType::FiveMTest);

	// Keys that are used
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
	KeyInfo ESC(KEY_ESC);
	KeyInfo CAP(KEY_CAPS_LOCK);
	KeyInfo BAK(KEY_BACKSPACE);
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
	KeyInfo ___; // blank/disabled key
	switch(layerId) {
	case 1: {
		return {
			{ NUM, CAP, WIN, PRT },
			{ INS, HOM, PGU, ___ },
			{ DEL, END, PGD, MEN },
			{ ESC, _U_, BAK, ___ },
			{ _L_, _D_, _R_, RET }
		};
	}
	case 2: {
		return {
			{ NUM, LY0, LY1, LY2 },
			{ __1, __2, __3, ___ },
			{ TAB, SPC, __R, __4 },
			{ __Q, __W, __E, ___ },
			{ __A, __S, __D, RET }
		};
	}
	case 3: {
		return {
			{ NUM, LY0, LY1, LY2 },
			{ M_FIVEMTEST, ___, ___, ___ },
			{ M_CMD, M_HelloWorld, M_CTRLALTDEL, LY3 },
			{ ESC, M_WINMAX, ___, ___ },
			{ M_WINLEFT, M_WINMIN, M_WINRIGHT, RET }
		};
	}
	default:
	case 0: {
		return {
			{ NUM, DIV, MUL, MIN },
			{ NM7, NM8, NM9, ___ },
			{ NM4, NM5, NM6, PLS },
			{ NM1, NM2, NM3, ___ },
			{ NM0, NM0, NMD, NMR }
		};
	}
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
	if ((t_now - t_prev) <= DEBOUNCE_MS) 
		return true;
	return false;
}

void playMacro(MacroType nextMacro) {
	Macro macro = createMacro(nextMacro);
	if (macro.Strokes.size() == 0) {
		return;
	}
	unsigned long t_start = millis();
	unsigned long t_total = macro.Strokes.back().Duration + macro.Strokes.back().KeyDown;
	unsigned extraDelay = 10;

	while (true) {
		if (millis() > t_start + t_total + extraDelay)
			break;

		for (auto &stroke : macro.Strokes) {
			if (millis() > t_start + stroke.KeyDown && !stroke.Active) {
				Keyboard.press(stroke.Key);
				stroke.Active = true;
			}
			if (millis() > t_start + stroke.KeyDown + stroke.Duration && stroke.Active) {
				Keyboard.release(stroke.Key);
			}
		}
	}
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

		auto keyInfo = keycode[strobe_row][cnt];
		if (digitalRead(input_pins[cnt]) == HIGH) { 
			// key up
			if (key_state[strobe_row][cnt] != 0) {
				if ((num_down && keyInfo.GetType() == KeyType::Dual) ||
					(num_down && keyInfo.GetType() == KeyType::Layer)) {
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
				if ((num_down && keyInfo.GetType() == KeyType::Dual) ||
					(num_down && keyInfo.GetType() == KeyType::Layer)) {
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
		playMacro((MacroType)nextMacro);
	}
}
