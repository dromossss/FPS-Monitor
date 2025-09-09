// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "CefValues.h"


namespace p2c::client::util
{
	using namespace std::literals;

	CefRefPtr<CefValue> V8ToCefValue(CefV8Value& v8)
	{
		auto v = CefValue::Create();
		if (v8.IsBool())
		{
			v->SetBool(v8.GetBoolValue());
		}
		else if (v8.IsInt())
		{
			v->SetInt(v8.GetIntValue());
		}
		else if (v8.IsUInt())
		{
			v->SetInt(v8.GetUIntValue());
		}
		else if (v8.IsNull())
		{
			v->SetNull();
		}
		else if (v8.IsDouble())
		{
			v->SetDouble(v8.GetDoubleValue());
		}
		else if (v8.IsString())
		{
			v->SetString(v8.GetStringValue());
		}
		else if (v8.IsArray())
		{
			auto list = CefListValue::Create();
			for (int i = 0; i < v8.GetArrayLength(); i++)
			{
				list->SetValue(i, V8ToCefValue(*v8.GetValue(i)));
			}
			v->SetList(std::move(list));
		}
		else if (v8.IsObject())
		{
			auto dict = CefDictionaryValue::Create();
			CefDictionaryValue::KeyList keys;
			v8.GetKeys(keys);
			for (auto& key : keys)
			{
				dict->SetValue(key, V8ToCefValue(*v8.GetValue(key)));
			}
			v->SetDictionary(std::move(dict));
		}
		else
		{
			pmlog_warn("Unknown V8 type cannot convert");
		}
		return v;
	}

	CefRefPtr<CefV8Value> CefToV8Value(CefValue& cef)
	{
		using Type = CefValueType;
		CefRefPtr<CefV8Value> v8;
		switch (cef.GetType())
		{
		case Type::VTYPE_BOOL:
			v8 = CefV8Value::CreateBool(cef.GetBool());
			break;
		case Type::VTYPE_INT:
			v8 = CefV8Value::CreateInt(cef.GetInt());
			break;
		case Type::VTYPE_DOUBLE:
			v8 = CefV8Value::CreateDouble(cef.GetDouble());
			break;
		case Type::VTYPE_NULL:
			v8 = CefV8Value::CreateNull();
			break;
		case Type::VTYPE_STRING:
			v8 = CefV8Value::CreateString(cef.GetString());
			break;
		case Type::VTYPE_LIST:
		{
			auto list = cef.GetList();
			v8 = CefV8Value::CreateArray((int)list->GetSize());
			for (size_t i = 0; i < list->GetSize(); i++)
			{
				v8->SetValue(int(i), CefToV8Value(*list->GetValue(i)));
			}
		}
		break;
		case Type::VTYPE_DICTIONARY:
		{
			auto dict = cef.GetDictionary();
			v8 = CefV8Value::CreateObject(nullptr, nullptr);
			CefDictionaryValue::KeyList keys;
			dict->GetKeys(keys);
			for (auto& k : keys)
			{
				v8->SetValue(k, CefToV8Value(*dict->GetValue(k)), CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_NONE);
			}
		}
		break;
		default:
			pmlog_warn("Unknown CEF type cannot convert");
		}
		return v8;
	}

	std::string CefValueTypeToString(CefValueType type) noexcept
	{
		using namespace std::string_literals;
		switch (type) {
		case CefValueType::VTYPE_BINARY: return "Binary"s;
		case CefValueType::VTYPE_BOOL: return "Bool"s;
		case CefValueType::VTYPE_DICTIONARY: return "Dictionary"s;
		case CefValueType::VTYPE_DOUBLE: return "Double"s;
		case CefValueType::VTYPE_INT: return "Int"s;
		case CefValueType::VTYPE_INVALID: return "Invalid"s;
		case CefValueType::VTYPE_LIST: return "List"s;
		case CefValueType::VTYPE_NULL: return "Null"s;
		case CefValueType::VTYPE_STRING: return "String"s;
		default: pmlog_warn("encountered unknown CefValue type"); return "Unknown"s;
		}
	}

	CefValueTraverser Traverse(CefRefPtr<CefValue> pVal) noexcept
	{
		return { std::move(pVal) };
	}

	std::wstring CefValueTraverser::AsWString() const
	{
		using str::ToWide;
		if (pCefValue->GetType() == CefValueType::VTYPE_STRING)
		{
			return pCefValue->GetString().ToWString();
		}
		pmlog_error(std::format("Cannot extract std::wstring from CEF {}", CefValueTypeToString(pCefValue->GetType())));
		throw Except<BadCefValueTraversal>();
	}

	std::string CefValueTraverser::AsString() const
	{
		using str::ToWide;
		if (pCefValue->GetType() == CefValueType::VTYPE_STRING)
		{
			return pCefValue->GetString();
		}
		pmlog_error(std::format("Cannot extract std::string from CEF {}",
			CefValueTypeToString(pCefValue->GetType())
		));
		throw Except<BadCefValueTraversal>();
	}

	CefValueTraverser CefValueTraverser::operator[](const char* key)
	{
		using str::ToWide;
		if (pCefValue->GetType() != CefValueType::VTYPE_DICTIONARY)
		{
			pmlog_error(std::format("Cannot access property [{}] of CefValue type {}",
				key, CefValueTypeToString(pCefValue->GetType())
			));
			throw Except<BadCefValueTraversal>();
		}
		CefString cefKey{ key };
		if (auto dict = pCefValue->GetDictionary(); dict->HasKey(cefKey))
		{
			return { dict->GetValue(cefKey) };
		}
		pmlog_error(std::format("CefDictionary does not contain key [{}]", cefKey.ToString()));
		throw Except<BadCefValueTraversal>();
	}

	CefValueTraverser CefValueTraverser::operator[](size_t index)
	{
		if (pCefValue->GetType() != CefValueType::VTYPE_LIST)
		{
			pmlog_error(std::format("Cannot access index [{}] of CefValue type {}",
				index, CefValueTypeToString(pCefValue->GetType())
			));
			throw Except<BadCefValueTraversal>();
		}
		if (auto list = pCefValue->GetList(); list->GetSize() > index)
		{
			return { list->GetValue(index) };
		}
		else
		{
			pmlog_error(std::format("Index [{}] is out of bounds of CefList size [{}]", index, list->GetSize()));
			throw Except<BadCefValueTraversal>();
		}
	}

	CefRefPtr<CefValue> CefValueTraverser::AsCefValue() const &
	{
		return pCefValue;
	}

	CefRefPtr<CefValue> CefValueTraverser::AsCefValue() &&
	{
		return std::move(pCefValue);
	}

	bool CefValueTraverser::IsAggregate() const
	{
		const auto type = pCefValue->GetType();
		return type == CefValueType::VTYPE_LIST || type == CefValueType::VTYPE_DICTIONARY;
	}

	std::string CefValueTraverser::Dump(int initial, int increment) const
	{
		return Dump_(initial, increment, false);
	}

	std::string CefValueTraverser::Dump_(int depth, int increment, bool labeled) const
	{
		std::ostringstream oss;
		if (!labeled) {
			oss << std::string(depth * increment, ' ');
		}
		switch (pCefValue->GetType()) {
		case CefValueType::VTYPE_BOOL:
			oss << (bool(*this) ? "true"s : "false"s) + " <bool>";
			break;
		case CefValueType::VTYPE_INT:
			oss << std::to_string(int(*this)) + " <int>";
			break;
		case CefValueType::VTYPE_DOUBLE:
			oss << std::to_string(double(*this)) + " <double>";
			break;
		case CefValueType::VTYPE_STRING:
			oss << AsString() + " <string>";
			break;
		case CefValueType::VTYPE_BINARY:
			oss << "# "s + std::to_string(pCefValue->GetBinary()->GetSize()) + " bytes # <binary>";
			break;
		case CefValueType::VTYPE_DICTIONARY:
		{
			depth++;
			const std::string istr(increment * depth, ' ');
			const auto pDict = pCefValue->GetDictionary();
			CefDictionaryValue::KeyList keys;
			pDict->GetKeys(keys);
			if (keys.size() == 0) {
				oss << "{}";
			}
			else {
				oss << "{\n";
				for (auto& key : keys) {
					oss << istr << "\"" << key << "\": " << Traverse(pDict->GetValue(key))
						.Dump_(depth, increment, true) << "\n";
				}
				oss << std::string(increment * (depth - 1), ' ') << "}";
			}
			break;
		}
		case CefValueType::VTYPE_LIST:
		{
			depth++;
			const std::string istr(increment * depth, ' ');
			const auto pList = pCefValue->GetList();
			const auto size = pList->GetSize();
			if (size == 0) {
				oss << "[]";
			}
			else {
				oss << "[\n";
				for (int i = 0; i < pList->GetSize(); i++) {
					oss << istr << i << ": " << Traverse(pList->GetValue(i))
						.Dump_(depth, increment, true) << "\n";
				}
				oss << std::string(increment * (depth - 1), ' ') << "]";
			}
			break;
		}
		case CefValueType::VTYPE_NULL:
			oss << "<NULL>"s;
			break;
		default:
			oss << "<@UNKNOWN@>";
			break;
		}
		return oss.str();
	}


	size_t CefValueTraverser::GetLength() const
	{
		if (pCefValue->GetType() == CefValueType::VTYPE_LIST) {
			return pCefValue->GetList()->GetSize();
		}
		else if (pCefValue->GetType() == CefValueType::VTYPE_DICTIONARY) {
			return pCefValue->GetDictionary()->GetSize();
		}
		pmlog_error(std::format("Failed getting length of non-Array non-Dict CefValue type {}", CefValueTypeToString(pCefValue->GetType())));
		throw Except<BadCefValueTraversal>();
	}

	bool CefValueTraverser::IsNull() const
	{
		return pCefValue->GetType() == CefValueType::VTYPE_NULL;
	}
}