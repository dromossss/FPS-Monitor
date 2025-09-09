// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Key.h"
#include <Core/source/win/WinAPI.h>

namespace p2c::win
{
	class Key::Registry
	{
	public:
		Registry()
		{
			keyList = {
				{ Code::Backspace, "Bksp", VK_BACK },
				{ Code::Tab, "Tab", VK_TAB },
				{ Code::Return, "Return", VK_RETURN },
				{ Code::Shift, "Shift", VK_SHIFT },
				{ Code::Ctrl, "Ctrl", VK_CONTROL },
				{ Code::Alt, "Alt", VK_MENU },
				{ Code::Pause, "Pause", VK_PAUSE },
				{ Code::Caps, "Caps", VK_CAPITAL },
				{ Code::Escape, "Escape", VK_ESCAPE },
				{ Code::Space, "Space", VK_SPACE },
				{ Code::PageUp, "PgUp", VK_PRIOR },
				{ Code::PageDown, "PgDown", VK_NEXT },
				{ Code::End, "End", VK_END },
				{ Code::Home, "Home", VK_HOME },
				{ Code::Left, "Left", VK_LEFT },
				{ Code::Up, "Up", VK_UP },
				{ Code::Right, "Right", VK_RIGHT },
				{ Code::Down, "Down", VK_DOWN },
				{ Code::Print, "Print", VK_PRINT },
				{ Code::PrintScreen, "PrtScr", VK_SNAPSHOT },
				{ Code::Insert, "Ins", VK_INSERT },
				{ Code::Delete, "Del", VK_DELETE },
				{ Code::N0, "0", uint32_t('0') },
				{ Code::N1, "1", uint32_t('1') },
				{ Code::N2, "2", uint32_t('2') },
				{ Code::N3, "3", uint32_t('3') },
				{ Code::N4, "4", uint32_t('4') },
				{ Code::N5, "5", uint32_t('5') },
				{ Code::N6, "6", uint32_t('6') },
				{ Code::N7, "7", uint32_t('7') },
				{ Code::N8, "8", uint32_t('8') },
				{ Code::N9, "9", uint32_t('9') },
				{ Code::A, "A", uint32_t('A') },
				{ Code::B, "B", uint32_t('B') },
				{ Code::C, "C", uint32_t('C') },
				{ Code::D, "D", uint32_t('D') },
				{ Code::E, "E", uint32_t('E') },
				{ Code::F, "F", uint32_t('F') },
				{ Code::G, "G", uint32_t('G') },
				{ Code::H, "H", uint32_t('H') },
				{ Code::I, "I", uint32_t('I') },
				{ Code::J, "J", uint32_t('J') },
				{ Code::K, "K", uint32_t('K') },
				{ Code::L, "L", uint32_t('L') },
				{ Code::M, "M", uint32_t('M') },
				{ Code::N, "N", uint32_t('N') },
				{ Code::O, "O", uint32_t('O') },
				{ Code::P, "P", uint32_t('P') },
				{ Code::Q, "Q", uint32_t('Q') },
				{ Code::R, "R", uint32_t('R') },
				{ Code::S, "S", uint32_t('S') },
				{ Code::T, "T", uint32_t('T') },
				{ Code::U, "U", uint32_t('U') },
				{ Code::V, "V", uint32_t('V') },
				{ Code::W, "W", uint32_t('W') },
				{ Code::X, "X", uint32_t('X') },
				{ Code::Y, "Y", uint32_t('Y') },
				{ Code::Z, "Z", uint32_t('Z') },
				{ Code::LWin, "LWin", VK_LWIN },
				{ Code::RWin, "RWin", VK_RWIN },
				{ Code::Num0, "Num 0", VK_NUMPAD0 },
				{ Code::Num1, "Num 1", VK_NUMPAD1 },
				{ Code::Num2, "Num 2", VK_NUMPAD2 },
				{ Code::Num3, "Num 3", VK_NUMPAD3 },
				{ Code::Num4, "Num 4", VK_NUMPAD4 },
				{ Code::Num5, "Num 5", VK_NUMPAD5 },
				{ Code::Num6, "Num 6", VK_NUMPAD6 },
				{ Code::Num7, "Num 7", VK_NUMPAD7 },
				{ Code::Num8, "Num 8", VK_NUMPAD8 },
				{ Code::Num9, "Num 9", VK_NUMPAD9 },
				{ Code::Multiply, "Mul", VK_MULTIPLY },
				{ Code::Add, "Add", VK_ADD },
				{ Code::Subtract, "Sub", VK_SUBTRACT },
				{ Code::Decimal, "Dec", VK_DECIMAL },
				{ Code::Divide, "Div", VK_DIVIDE },
				{ Code::F1, "F1", VK_F1 },
				{ Code::F2, "F2", VK_F2 },
				{ Code::F3, "F3", VK_F3 },
				{ Code::F4, "F4", VK_F4 },
				{ Code::F5, "F5", VK_F5 },
				{ Code::F6, "F6", VK_F6 },
				{ Code::F7, "F7", VK_F7 },
				{ Code::F8, "F8", VK_F8 },
				{ Code::F9, "F9", VK_F9 },
				{ Code::F10, "F10", VK_F10 },
				{ Code::F11, "F11", VK_F11 },
				{ Code::F12, "F12", VK_F12 },
				{ Code::F13, "F13", VK_F13 },
				{ Code::F14, "F14", VK_F14 },
				{ Code::F15, "F15", VK_F15 },
				{ Code::F16, "F16", VK_F16 },
				{ Code::F17, "F17", VK_F17 },
				{ Code::F18, "F18", VK_F18 },
				{ Code::F19, "F19", VK_F19 },
				{ Code::F20, "F20", VK_F20 },
				{ Code::F21, "F21", VK_F21 },
				{ Code::F22, "F22", VK_F22 },
				{ Code::F23, "F23", VK_F23 },
				{ Code::F24, "F24", VK_F24 },
				{ Code::Numlock, "Numlk", VK_NUMLOCK },
				{ Code::Scroll, "Scroll", VK_SCROLL },
				{ Code::LShift, "LShift", VK_LSHIFT },
				{ Code::RShift, "RShift", VK_RSHIFT },
				{ Code::LCtrl, "LCtrl", VK_LCONTROL },
				{ Code::RCtrl, "RCtrl", VK_RCONTROL },
				{ Code::LAlt, "LAlt", VK_LMENU },
				{ Code::RAlt, "RAlt", VK_RMENU },
			};
			codeMap.reserve(keyList.size());
			textMap.reserve(keyList.size());
			platformMap.reserve(keyList.size());
			for (const auto& d : keyList)
			{
				codeMap[d.code] = &d;
				textMap[d.text] = &d;
				platformMap[d.platformCode] = &d;
			}
		}
		const Descriptor* LookupCode(Code c) const
		{
			if (auto i = codeMap.find(c); i != codeMap.end()) return i->second;
			return nullptr;
		}
		const Descriptor* LookupPlatformCode(uint32_t p) const
		{
			if (auto i = platformMap.find(p); i != platformMap.end()) return i->second;
			return nullptr;
		}
		const Descriptor* LookupText(const std::string& t) const
		{
			if (auto i = textMap.find(t); i != textMap.end()) return i->second;
			return nullptr;
		}
		const std::vector<Descriptor>& EnumerateKeys() const
		{
			return keyList;
		}
		static const Registry& Get()
		{
			static Registry reg{};
			return reg;
		}
	private:
		std::vector<Descriptor> keyList;
		std::unordered_map<Code, const Descriptor*> codeMap;
		std::unordered_map<uint32_t, const Descriptor*> platformMap;
		std::unordered_map<std::string, const Descriptor*> textMap;
	};




	Key::Key(Code c)
		:
		code{ c }
	{}

	uint32_t Key::GetPlatformCode() const
	{
		return Registry::Get().LookupCode(code)->platformCode;
	}

	const std::string& Key::GetText() const
	{
		return Registry::Get().LookupCode(code)->text;
	}

	Key::Code Key::GetCode() const
	{
		return code;
	}

	bool Key::IsModifierKey() const
	{
		switch (code) {
		case Code::Shift:
		case Code::Ctrl:
		case Code::Alt:
		case Code::LShift:
		case Code::RShift:
		case Code::LCtrl:
		case Code::RCtrl:
		case Code::LAlt:
		case Code::RAlt:
			return true;
		default:
			return false;
		}
	}

	bool Key::operator==(const Key& rhs) const
	{
		return code == rhs.code;
	}

	bool Key::operator<(const Key& rhs) const
	{
		return code < rhs.code;
	}

	const std::vector<Key::Descriptor>& Key::EnumerateKeys()
	{
		return Registry::Get().EnumerateKeys();
	}

	std::optional<Key> Key::FromString(const std::string& s)
	{
		if (auto pDesc = Registry::Get().LookupText(s))
		{
			return Key{ pDesc->code };
		}
		return {};
	}

	std::optional<Key> Key::FromPlatformCode(uint32_t p)
	{
		if (auto pDesc = Registry::Get().LookupPlatformCode(p))
		{
			return Key{ pDesc->code };
		}
		return {};
	}
}