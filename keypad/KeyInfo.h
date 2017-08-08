#pragma once

enum class KeyType : uint8_t {
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
		m_switchId(-1),
		m_macroId(-1) { }

	KeyInfo(uint8_t keyCode) :
		m_type(KeyType::Layer),
		m_keyCode(keyCode),
		m_switchId(-1),
		m_macroId(-1) { }

	KeyInfo(KeyType type, uint8_t keyCode, int8_t switchId, int8_t macroId = -1):
		m_type(type),
		m_keyCode(keyCode),
		m_switchId(switchId),
		m_macroId(macroId) { }

	KeyType GetType() const {
		return m_type;
	}

	uint8_t GetKeyCode() const {
		return m_keyCode;
	}

	int8_t GetSwitch() const {
		return m_switchId;
	}

	int8_t GetMacro() const {
		return m_macroId;
	}

private:
	KeyType m_type;
	uint8_t m_keyCode;
	int8_t m_switchId;
	int8_t m_macroId;
};
