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

#ifndef JSONPP_BASIC_JSON_IMPL_HPP
#define JSONPP_BASIC_JSON_IMPL_HPP

#include "macro_def.hpp"
#include "basic_json.hpp"
#include "json_serializer.hpp"

namespace jsonpp
{
    using namespace traits;

    BASIC_JSON_TEMPLATE
    template <Type T>
    void BASIC_JSON_TYPE::set_type_impl()
    {
        if constexpr (T == Type::empty)
            m_value.template emplace<std::monostate>();
        else if constexpr (T == Type::null)
            m_value.template emplace<null_t>();
        else if constexpr (T == Type::object)
            m_value.template emplace<object>();
        else if constexpr (T == Type::array)
            m_value.template emplace<array>();
        else if constexpr (T == Type::string)
            m_value.template emplace<string>();
        else if constexpr (T == Type::boolean)
            m_value.template emplace<boolean>(false);
        else if constexpr (T == Type::number_int)
            m_value.template emplace<number_int>(0);
        else if constexpr (T == Type::number_float)
            m_value.template emplace<number_float>(0.0);
    }

    BASIC_JSON_TEMPLATE
    template <typename T>
    T& BASIC_JSON_TYPE::as_impl(value_t& v, char const* typeName)
    {
        try
        {
            return std::get<T>(v);
        }
        catch (const std::bad_variant_access&)
        {
            throw JsonTypeError(std::string("Value is not a ") + typeName);
        }
    }

    BASIC_JSON_TEMPLATE
    template <typename T>
    T const& BASIC_JSON_TYPE::as_impl(value_t const& v, char const* typeName)
    {
        try
        {
            return std::get<T>(v);
        }
        catch (const std::bad_variant_access&)
        {
            throw JsonTypeError(std::string("Value is not a ") + typeName);
        }
    }

    BASIC_JSON_TEMPLATE
    template <Type T>
    void BASIC_JSON_TYPE::set_type(bool clear_content)
    {
        if (!clear_content && type() == T) return; // do nothing if already of type T
        set_type_impl<T>();
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::set_type(Type const& t, bool clear_content)
    {
        if (!clear_content && type() == t) return;

        switch (t)
        {
        case Type::empty:       set_type_impl<Type::empty>(); break;
        case Type::null:        set_type_impl<Type::null>(); break;
        case Type::boolean:     set_type_impl<Type::boolean>(); break;
        case Type::number_int:  set_type_impl<Type::number_int>(); break;
        case Type::number_float:set_type_impl<Type::number_float>(); break;
        case Type::string:      set_type_impl<Type::string>(); break;
        case Type::array:       set_type_impl<Type::array>(); break;
        case Type::object:      set_type_impl<Type::object>(); break;
        default:
#if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
#elif defined(_MSC_VER)
            __assume(0);
#endif
            break;
        }
    }

    BASIC_JSON_TEMPLATE
    typename BASIC_JSON_TYPE::size_type BASIC_JSON_TYPE::size() const noexcept
    {
        switch (type())
        {
        case Type::empty:
        case Type::null:
            return 0;
        case Type::boolean:
        case Type::number_int:
        case Type::number_float:
        case Type::string:
            return 1;
        case Type::array:
            return as_array().size();
        case Type::object:
            return as_object().size();
        default:
#if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
#elif defined(_MSC_VER)
            __assume(0);
#endif
        }
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::operator[](size_type index)
    {
        return const_cast<basic_json&>(
            static_cast<basic_json const&>(*this).operator[](index)
        );
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::operator[](size_type index) const
    {
        auto const& arr = as_array();
        assert(index < arr.size() && "JSON array index out of range");
        return arr[index];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::at(size_type index)
    {
        return const_cast<basic_json&>(
            static_cast<basic_json const&>(*this).at(index)
        );
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::at(size_type index) const
    {
        auto const& arr = as_array();
        if (index >= arr.size())
            throw JsonOutOfRange(JsonOutOfRange::ARRAY_OUT_OF_RANGE_MESSAGE);
        return arr[index];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::operator[](std::string const& key)
    {
        if (empty() || is_null())
            set_type(Type::object);

        return as_object()[key];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::operator[](std::string const& key) const
    {
        return at(key);
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::at(std::string const& key)
    {
        return const_cast<basic_json&>(
            static_cast<basic_json const&>(*this).at(key)
        );
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::at(std::string const& key) const
    {
        auto const& obj = as_object();
        auto it = obj.find(key);
        if (it == obj.end())
            throw JsonOutOfRange(JsonOutOfRange::KEY_NOT_FOUND_MESSAGE);
        return it->second;
    }

    BASIC_JSON_TEMPLATE
    bool BASIC_JSON_TYPE::contains(string const& key) const
    {
        if (!is_object())
            return false;
        auto& obj = as_object();
        return obj.find(key) != obj.end();
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::push_back(BASIC_JSON_TYPE&& val)
    {
        if (empty() || is_null())
            set_type<Type::array>();
        as_array().emplace_back(std::move(val));
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::push_back(BASIC_JSON_TYPE const& val)
    {
        if (empty() || is_null())
            set_type<Type::array>();
        as_array().emplace_back(val);
    }

    BASIC_JSON_TEMPLATE
    template <typename... Args>
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::emplace_back(Args&&... args)
    {
        if (empty() || is_null())
            set_type<Type::array>();
        return as_array().emplace_back(std::forward<Args>(args)...);
    }

    BASIC_JSON_TEMPLATE
    auto BASIC_JSON_TYPE::insert(std::pair<string, basic_json> const& pair)
    {
        if (empty() || is_null())
            set_type<Type::object>();
        return as_object().insert(pair);
    }

    BASIC_JSON_TEMPLATE
    auto BASIC_JSON_TYPE::insert(std::pair<string, basic_json>&& pair)
    {
        if (empty() || is_null())
            set_type(Type::object);
        return as_object().emplace(std::move(pair));
    }

    BASIC_JSON_TEMPLATE
    template <typename... Args>
    auto BASIC_JSON_TYPE::emplace(Args&&... args)
    {
        if (empty() || is_null())
            set_type<Type::object>();
        return as_object().emplace(std::forward<Args>(args)...);
    }

    BASIC_JSON_TEMPLATE
    bool BASIC_JSON_TYPE::operator==(BASIC_JSON_TYPE const& other) const
    {
        if (m_value.index() != other.m_value.index()) return false;
        return m_value == other.m_value;
    }

    /*
     * Parse a document to JsonType, accessing data with std::string_view.
     */
    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE BASIC_JSON_TYPE::parse(std::string_view json_doc)
    {
        details::StringViewStream svs(json_doc);
        return details::Parser<details::StringViewStream, basic_json>(svs).parse();
    }

    /*
     * Parse a document to JsonType, accessing data with std::istream.
     */
    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE BASIC_JSON_TYPE::parse(std::istream& json_istream)
    {
        details::IStreamStream iss(json_istream);
        return details::Parser<details::IStreamStream, basic_json>(iss).parse();
    }

    /*
     * Parse a document to JsonType, accessing data with a JSON Stream.
     * Data should have been provided to the stream before calling this function.
     */
    BASIC_JSON_TEMPLATE
    template <typename StreamT,
        std::enable_if_t<traits::is_json_stream_v<StreamT>, int>>
    BASIC_JSON_TYPE BASIC_JSON_TYPE::parse(StreamT& stream)
    {
        return details::Parser<StreamT, basic_json>(stream).parse();
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::dump(std::string& buffer) const
    {
        details::StringSerializeHandler ssh(buffer);
        details::JsonSerializer<basic_json, details::StringSerializeHandler>
            serializer(ssh);
        serializer.dump(*this);
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::dump(std::ostream& os) const
    {
        details::OStreamSerializeHandler ossh(os);
        details::JsonSerializer<basic_json, details::OStreamSerializeHandler>
            serializer(ossh);
        serializer.dump(*this);
    }

    BASIC_JSON_TEMPLATE
    template <typename SerializeHandlerT,
        std::enable_if_t<traits::is_json_serialize_handler_v<SerializeHandlerT>, int>>
    void BASIC_JSON_TYPE::dump(SerializeHandlerT& handler)
    {
        details::JsonSerializer<basic_json, SerializeHandlerT> serializer(handler);
        serializer.dump(*this);
    }

    BASIC_JSON_TEMPLATE
    std::string BASIC_JSON_TYPE::stringify() const
    {
        std::string buffer;
        dump(buffer);
        return buffer;
    }

    /*
     * end asserted accessor
     */
}


#endif //JSONPP_BASIC_JSON_IMPL_HPP