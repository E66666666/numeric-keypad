#pragma once

enum class KeyType {
	KeyCode,
	Switch,
	Dual,
	None,
};

class KeyInfo {
public:
	KeyInfo(): 
		m_type(KeyType::None), 
		m_keyCode(0),
		m_switchId(-1) {
		
	}
	KeyInfo(KeyType type, int keyCode, int switchId):
		m_type(type),
		m_keyCode(keyCode),
		m_switchId(switchId) { }

	KeyType GetType() {
		return m_type;
	}

	uint8_t GetKeyCode() const {
		return m_keyCode;
	}

	int8_t GetSwitch() const {
		return m_switchId;
	}

private:
	KeyType m_type;
	uint8_t m_keyCode;
	int8_t m_switchId;

};
