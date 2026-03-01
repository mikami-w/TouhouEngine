/*
jsonpp - A modern, header-only C++ JSON library
Copyright 2025-2026 Mikami (jsonpp project)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#ifndef JSONPP_JSONEXCEPTION_HPP
#define JSONPP_JSONEXCEPTION_HPP

#include "macro_def.hpp"
#include <stdexcept>
#include <string>

namespace jsonpp
{
    /*
     * JSON exceptions
     */
    class JsonException : public std::runtime_error
    {
    public:
        JsonException(std::string const& msg):
            std::runtime_error(msg) {}
    };

    class JsonParseError : public JsonException
    {
    public:
        static constexpr char const* UNPARSABLE_MESSAGE = "Unparsable character(s)";
        static constexpr char const* UNEXPECTED_EOF_MESSAGE = "Unexpected end of file while parsing JSON document";

        JsonParseError(std::string const& msg):
            JsonException(msg) {}

        JsonParseError(std::string const& msg, std::size_t pos):
            JsonException(msg + " at position " + std::to_string(pos)) {}
    };

    class JsonTypeError : public JsonException
    {
    public:
        JsonTypeError(std::string const& msg) :
            JsonException(msg) {}
    };

    class JsonDepthLimitExceeded : public JsonException
    {
    public:
        static constexpr char const* DEPTH_LIMIT_EXCEEDED_MESSAGE
            = "Maximum nesting depth of " TO_STRING(MAX_NESTING_DEPTH) " exceeded";

        JsonDepthLimitExceeded(std::size_t pos):
            JsonException(DEPTH_LIMIT_EXCEEDED_MESSAGE + std::string(" at position ") + std::to_string(pos)) {}
    };

    class JsonOutOfRange : public JsonException
    {
    public:
        static constexpr char const* ARRAY_OUT_OF_RANGE_MESSAGE = "JSON array index out of range";
        static constexpr char const* KEY_NOT_FOUND_MESSAGE = "Key not found in JSON object";

        JsonOutOfRange(std::string const& msg):
            JsonException(msg) {}
    };
    /*
     * end JSON exceptions
     */
}

#endif //JSONPP_JSONEXCEPTION_HPP