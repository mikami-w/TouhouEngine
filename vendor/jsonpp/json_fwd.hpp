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

#ifndef JSONPP_JSON_FWD_HPP
#define JSONPP_JSON_FWD_HPP

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <memory> // for std::allocator

namespace jsonpp
{
    using null_t = std::nullptr_t;
    constexpr null_t null = nullptr;

    template <
        template<typename U, typename V, typename... Args> class ObjectType = std::map,
        template<typename U, typename... Args> class ArrayType = std::vector,
        typename StringType = std::string,
        typename BooleanType = bool,
        typename NumberIntegerType = std::int64_t,
        typename NumberFloatType = double,
        template<typename U> class AllocatorType = std::allocator,
        // template<typename T, typename SFINAE = void> class JSONSerializer = adl_serializer, // 待实现
        // typename BinaryType = std::vector<std::uint8_t>, // 待实现
        typename CustomBaseClass = void
    >
    class basic_json;

    using json = basic_json<>;
    using unordered_json = basic_json<std::unordered_map>;

}
#endif //JSONPP_JSON_FWD_HPP