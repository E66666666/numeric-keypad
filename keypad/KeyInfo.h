#pragma once
#include "Macro.h"

enum class KeyType {
	KeyCode,
	Layer,
	Dual,
	Macro,
	None,
};

class KeyInfo {
public:
	KeyInfo() :
		m_type(KeyType::None),
		m_keyCode(0),
		m_switchId(-1) { }

	KeyInfo(int keyCode) :
		m_type(KeyType::Layer),
		m_keyCode(keyCode),
		m_switchId(-1) { }

	KeyInfo(KeyType type, int keyCode, int switchId):
		m_type(type),
		m_keyCode(keyCode),
		m_switchId(switchId) { }

	KeyInfo(Macro *macro) :
		m_type(KeyType::Macro),
		m_keyCode(0),
		m_switchId(-1) {
		m_macro = macro;
	}

	KeyType GetType() const {
		return m_type;
	}

	uint8_t GetKeyCode() const {
		return m_keyCode;
	}

	int8_t GetSwitch() const {
		return m_switchId;
	}

	Macro* GetMacro() const {
		return m_macro;
	}

private:
	KeyType m_type;
	uint8_t m_keyCode;
	int8_t m_switchId;
	Macro *m_macro = nullptr;
};
