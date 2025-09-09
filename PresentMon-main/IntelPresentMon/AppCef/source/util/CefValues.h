// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <CommonUtilities/win/WinAPI.h>
#include <include/cef_v8.h>
#include <type_traits>
#include <string>
#include <algorithm>
#include <iterator>
#include "Logging.h"
#include <CommonUtilities/str/String.h>
#include <CommonUtilities/Exception.h>
#include <CommonUtilities/Meta.h>
#include <string_view>
#include <concepts>
#include <optional>

namespace p2c::client::util
{
	using namespace ::pmon::util;

	PM_DEFINE_EX(BadCefValueTraversal);

	inline CefRefPtr<CefValue> CefValueNull() noexcept
	{
		auto null_val = CefValue::Create();
		null_val->SetNull();
		return null_val;
	}

	inline CefRefPtr<CefListValue> MakeCefList(size_t size) noexcept
	{
		auto l = CefListValue::Create();
		l->SetSize(size);
		return l;
	}

	template <class V>
	CefRefPtr<CefValue> CefValueDecay(CefRefPtr<V> specific) noexcept
	{
		auto val = CefValue::Create();
		if constexpr (std::is_same_v<V, CefDictionaryValue>)
		{
			val->SetDictionary(std::move(specific));
		}
		else if constexpr (std::is_same_v<V, CefListValue>)
		{
			val->SetList(std::move(specific));
		}
		else
		{
			static_assert("unsupported specific value type to convert from");
		}
		return val;
	}

	CefRefPtr<CefValue> V8ToCefValue(CefV8Value& v8);

	CefRefPtr<CefV8Value> CefToV8Value(CefValue& cef);

	template<typename T1>
	CefRefPtr<CefV8Value> MakeV8Value(T1&& val)
	{
		using T = std::remove_reference_t<std::remove_const_t<T1>>;
		if constexpr (std::is_same_v<bool, T>)
		{
			return CefV8Value::CreateBool(val);
		}
		else if constexpr (std::is_integral_v<T>)
		{
			return CefV8Value::CreateInt(int(val));
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return CefV8Value::CreateDouble(double(val));
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return CefV8Value::CreateInt(int(val));
		}
		else if constexpr (std::is_same_v<std::string, T> || std::is_same_v<std::wstring, T>)
		{
			return CefV8Value::CreateString(val);
		}
		else
		{
			static_assert(!std::is_same_v<T, T>, "Unsupported type for MakeV8Value");
		}
	}

	template<typename T1>
	CefRefPtr<CefValue> MakeCefValue(T1&& val)
	{
		using T = std::remove_reference_t<std::remove_const_t<T1>>;
		CefRefPtr<CefValue> v;
		if constexpr (std::is_same_v<bool, T>)
		{
			v = CefValue::Create();
			v->SetBool(val);
		}
		else if constexpr (std::is_integral_v<T>)
		{
			v = CefValue::Create();
			v->SetInt(val);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			v = CefValue::Create();
			v->SetDouble(val);
		}
		else if constexpr (std::is_enum_v<T>)
		{
			v = CefValue::Create();
			v->SetInt((int)val);
		}
		else if constexpr (std::is_same_v<std::string, T> || std::is_same_v<std::wstring, T>)
		{
			v = CefValue::Create();
			v->SetString(val);
		}
		else if constexpr (std::is_same_v<CefRefPtr<CefValue>, T>)
		{
			v = std::forward<T1>(val);
		}
		else if constexpr (IsContainer<std::optional, T>)
		{
			if (val) {
				v = MakeCefValue(*val);
			}
			else {
				v = CefValue::Create();
				v->SetNull();
			}
		}
		else
		{
			// TODO: make this compile-time
			pmlog_warn(std::format(L"Encountered unknown value type [{}]",
				str::ToWide(typeid(T).name())));
		}
		return v;
	}

	namespace
	{
		template<typename T1, typename... Ts>
		void BuildV8ArgumentList_(CefV8ValueList& list, T1&& arg1, Ts&&... args)
		{
			list.push_back(MakeV8Value(std::forward<T1>(arg1)));
			if constexpr (sizeof...(args) > 0)
			{
				BuildV8ArgumentList_(list, std::forward<Ts>(args)...);
			}
		}
	}

	template<typename T1, typename... Ts>
	CefV8ValueList BuildV8ArgumentList(T1&& arg1, Ts&&... args)
	{
		CefV8ValueList list;
		BuildV8ArgumentList_(list, std::forward<T1>(arg1), std::forward<Ts>(args)...);
		return list;
	}

	inline CefV8ValueList BuildV8ArgumentList()
	{
		return {};
	}

	std::string CefValueTypeToString(CefValueType type) noexcept;

	class CefValueTraverser
	{
	public:
		CefValueTraverser(CefRefPtr<CefValue> pCefValue) : pCefValue{ std::move(pCefValue) } {}
		std::wstring AsWString() const;
		std::string AsString() const;
		template<typename T>
		std::optional<T> AsOptional() const
		{
			if (IsNull()) {
				return std::nullopt;
			}
			return T(*this);
		}
		template<typename T>
		operator T() const
		{
			using str::ToWide;
			if constexpr (std::is_same_v<T, bool>)
			{
				if (pCefValue->GetType() == CefValueType::VTYPE_BOOL)
				{
					return pCefValue->GetBool();
				}
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				if (pCefValue->GetType() == CefValueType::VTYPE_INT)
				{
					return (T)pCefValue->GetInt();
				}
				if (pCefValue->GetType() == CefValueType::VTYPE_DOUBLE)
				{
					return (T)pCefValue->GetDouble();
				}
			}
			else if constexpr (std::is_enum_v<T>)
			{
				if (pCefValue->GetType() == CefValueType::VTYPE_INT)
				{
					return (T)pCefValue->GetInt();
				}
			}
			else if constexpr (std::is_same_v<T, CefRefPtr<CefValue>>)
			{
				return AsCefValue();
			}
			pmlog_error(std::format("Cannot extract {} from CEF {}",
				typeid(T).name(),
				CefValueTypeToString(pCefValue->GetType())
			));
			throw Except<BadCefValueTraversal>();
		}
		template<typename T>
		std::vector<T> ToVector()
		{
			const auto size = GetLength();
			std::vector<T> container;
			container.reserve(size);
			for (size_t i = 0; i < size; i++) {
				if constexpr (std::is_same_v<T, std::wstring>) {
					container.push_back((*this)[i].AsWString());
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					container.push_back((*this)[i].AsString());
				}
				else {
					container.push_back((*this)[i]);
				}
			}
			return container;
		}
		template<typename T>
		std::vector<T> PluckAs(const char* key) const
		{
			const auto size = GetLength();
			std::vector<T> container;
			container.reserve(size);
			for (size_t i = 0; i < size; i++) {
				container.push_back((*this)[i][key]);
			}
			return container;
		}
		CefValueTraverser operator[](const char* key);
		CefValueTraverser operator[](size_t index);
		CefRefPtr<CefValue> AsCefValue() const &;
		CefRefPtr<CefValue> AsCefValue() &&;
		size_t GetLength() const;
		bool IsNull() const;
		bool IsAggregate() const;
		std::string Dump(int initial = 1, int increment = 3) const;
	private:
		// functions
		std::string Dump_(int depth, int increment, bool labeled) const;
		// data
		CefRefPtr<CefValue> pCefValue;
	};

	CefValueTraverser Traverse(CefRefPtr<CefValue> pVal) noexcept;

	template<typename T>
	struct CefProp
	{
		std::string key;
		T value;
	};

	namespace
	{
		template<typename T, typename...P>
		void PopulateCefObject_(CefDictionaryValue& dict, CefProp<T>&& first, CefProp<P>&&...rest)
		{
			dict.SetValue(first.key, MakeCefValue(std::forward<T>(first.value)));
			if constexpr (sizeof...(rest) > 0)
			{
				PopulateCefObject_(dict, std::forward<CefProp<P>>(rest)...);
			}
		}
	}

	template<typename...P>
	CefRefPtr<CefValue> MakeCefObject(CefProp<P>&&...props)
	{
		auto dict = CefDictionaryValue::Create();
		if constexpr (sizeof...(props) > 0)
		{
			PopulateCefObject_(*dict, std::forward<CefProp<P>>(props)...);
		}
		return CefValueDecay(std::move(dict));
	}
}