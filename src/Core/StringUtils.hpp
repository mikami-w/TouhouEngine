#pragma once

#include <string>

namespace Core {
std::wstring stringToWstring(std::string const& str);
std::string wstringToString(std::wstring const& wstr);
} // namespace Core
