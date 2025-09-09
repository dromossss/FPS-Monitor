#include <Windows.h>
//#include <codecvt>
//#include <format>
#include "StringUtils.h"

std::string ConvertFromWideString(std::wstring wide_string) {
  // Determine the required size of the buffer
  int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1,
                                       nullptr, 0, nullptr, nullptr);
  // Convert the wide string to UTF-8
  std::string str(bufferSize, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wide_string.c_str(), -1, &str[0], bufferSize,
                      nullptr, nullptr);
  // Remove the null-terminator from the resulting string
  str.pop_back();

  return str;
}
