// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace p2c::win
{
	class Key
	{
	public:
		// types
		enum class Code : uint32_t
		{
			Backspace,
			Tab,
			Return,
			Shift,
			Ctrl,
			Alt,
			Pause,
			Caps,
			Escape,
			Space,
			PageUp,
			PageDown,
			End,
			Home,
			Left,
			Up,
			Right,
			Down,
			Print,
			PrintScreen,
			Insert,
			Delete,
			N0,
			N1,
			N2,
			N3,
			N4,
			N5,
			N6,
			N7,
			N8,
			N9,
			A,
			B,
			C,
			D,
			E,
			F,
			G,
			H,
			I,
			J,
			K,
			L,
			M,
			N,
			O,
			P,
			Q,
			R,
			S,
			T,
			U,
			V,
			W,
			X,
			Y,
			Z,
			LWin,
			RWin,
			Num0,
			Num1,
			Num2,
			Num3,
			Num4,
			Num5,
			Num6,
			Num7,
			Num8,
			Num9,
			Multiply,
			Add,
			Subtract,
			Decimal,
			Divide,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			F13,
			F14,
			F15,
			F16,
			F17,
			F18,
			F19,
			F20,
			F21,
			F22,
			F23,
			F24,
			Numlock,
			Scroll,
			LShift,
			RShift,
			LCtrl,
			RCtrl,
			LAlt,
			RAlt,
			Count_
		};
		struct Descriptor
		{
			Code code;
			std::string text;
			uint32_t platformCode;
		};
		// functions
		Key(Code c);
		uint32_t GetPlatformCode() const;
		const std::string& GetText() const;
		Code GetCode() const;
		bool IsModifierKey() const;
		bool operator==(const Key& rhs) const;
		bool operator<(const Key& rhs) const;
		static const std::vector<Descriptor>& EnumerateKeys();
		static std::optional<Key> FromString(const std::string& s);
		static std::optional<Key> FromPlatformCode(uint32_t p);
		// size required for an array to hold a direct lookup table for all virtual keys
		static constexpr size_t virtualKeyTableSize = 0xA6;
	private:
		// types
		class Registry;
		// data
		Code code;
	};
}