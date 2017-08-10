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
	FiveMTest,
};

struct Stroke {
	Stroke(uint8_t key, unsigned long keyDown, unsigned long duration): 
		   Key(key), KeyDown(keyDown), Duration(duration) {}

	uint8_t Key;
	unsigned long KeyDown;
	unsigned long Duration;
	bool Active = false;
};

struct Macro {
	Macro(std::vector<Stroke> strokes):
		  Strokes(strokes){}

	std::vector<Stroke> Strokes;
};
