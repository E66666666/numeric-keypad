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

	int GetKeyCode() {
		return m_keyCode;
	}

	int GetSwitch() {
		return m_switchId;
	}

private:
	KeyType m_type;
	int m_keyCode;
	int m_switchId;

};
