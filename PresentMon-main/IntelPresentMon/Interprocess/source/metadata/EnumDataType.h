#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

// enum annotation (enum_name_fragment, key_name_fragment, name, short_name, description)
#define ENUM_KEY_LIST_DATA_TYPE(X_) \
		X_(DATA_TYPE, DOUBLE, "Double Precision Floating Point Value", "double", "64-bit double precision floating point number in IEEE 754 format") \
		X_(DATA_TYPE, INT32, "32-bit Signed Integer", "int32_t", "32-bit signed integer") \
		X_(DATA_TYPE, UINT32, "32-bit Unsigned Integer", "uint32_t", "32-bit unsigned integer") \
		X_(DATA_TYPE, ENUM, "Enumeration", "int", "Integral value of an enum key, guaranteed to fit within a 32-bit signed integer") \
		X_(DATA_TYPE, STRING, "String", "char[260]", "Textual value, typically for non-numeric data") \
		X_(DATA_TYPE, UINT64, "64-bit Unsigned Integer", "uint64_t", "64-bit unsigned integer") \
		X_(DATA_TYPE, BOOL, "Boolean Value", "bool", "8-bit boolean flag value") \
		X_(DATA_TYPE, VOID, "Void Value", "void", "Value does not exist, is not accessible")