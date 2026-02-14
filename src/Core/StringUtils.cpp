#include "StringUtils.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::wstring Core::stringToWstring(std::string const& str)
{
  if (str.empty()) {
    return L"";
  }
  int size_needed = MultiByteToWideChar(
    CP_UTF8,
    0,
    &str[0],
    (int)str.size(),
    NULL,
    0
    );
  std::wstring resultWstr(size_needed, 0);
  MultiByteToWideChar(
    CP_UTF8,
    0,
    &str[0],
    (int)str.size(),
    &resultWstr[0],
    size_needed
    );
  return resultWstr;
}

std::string Core::wstringToString(std::wstring const& wstr)
{
  if (wstr.empty()) {
    return "";
  }
  int size_needed = WideCharToMultiByte(
    CP_UTF8,
    0,
    &wstr[0],
    (int)wstr.size(),
    NULL,
    0,
    NULL,
    NULL
    );
  std::string resultStr(size_needed, 0);
  WideCharToMultiByte(
    CP_UTF8,
    0,
    &wstr[0],
    (int)wstr.size(),
    &resultStr[0],
    size_needed,
    NULL,
    NULL
    );
  return resultStr;
}
