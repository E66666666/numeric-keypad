#pragma once
#include <StandardCplusplus.h>
#include <vector>

enum class MacroType {
	HelloWorld,
	CRTL_ALT_DEL,
	Admin_CMD,
	WinMin,
	WinMax,
	WinLeft,
	WinRight,
};

struct Stroke {
	Stroke(uint8_t key, unsigned long keyDown, unsigned long keyUp): 
		   Key(key), KeyDown(keyDown), KeyUp(keyUp) {}

	uint8_t Key;
	unsigned long KeyDown;
	unsigned long KeyUp;
	bool Active = false;
};

struct Macro {
	Macro(std::vector<Stroke> strokes):
		  Strokes(strokes){}

	std::vector<Stroke> Strokes;
};
