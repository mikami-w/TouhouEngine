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

#ifndef JSONPP_BASIC_JSON_HPP
#define JSONPP_BASIC_JSON_HPP

#include "json_fwd.hpp"
#include "json_serializer.hpp"
#include "jsonexception.hpp"
#include "json_stream_adaptor.hpp"
#include "macro_def.hpp"
#include "traits.hpp"

#include <string>
#include <variant>
#include <type_traits>
#include <cstdint>

namespace jsonpp
{
    namespace details
    {
        struct EmptyBaseClass {};
    }

    enum class Type: std::uint8_t
    {
        empty,
        null,
        boolean,
        number_int,
        number_float,
        string,
        array,
        object
    };

    BASIC_JSON_TEMPLATE
    class basic_json : public std::conditional_t<std::is_same_v<CustomBaseClass, void>, details::EmptyBaseClass, CustomBaseClass>
    {
        // =============================================================
        // * Internal Helper Types
        // =============================================================
    private:
        static constexpr bool _is_map_like =
            traits::details_t::is_std_map_like_v<ObjectType>;
        static constexpr bool _is_unordered_map_like =
            traits::details_t::is_std_unordered_map_like_v<ObjectType>;
        using _object_allocator_t = AllocatorType<std::pair<StringType const, basic_json>>;

        template <bool IsMapLike, bool IsUnorderedMapLike, typename Dummy = void>
        struct _object_type_selector
        {
            // assume it only needs K, V (non-standard container)
            using type = ObjectType<StringType,
                                    basic_json>;
        };

        template <typename Dummy>
        struct _object_type_selector<false, false, Dummy>
        {
            // assume it's a non-standard map container with 3 template parameters (Key, Value, Allocator)
            using type = ObjectType<StringType,
                                    basic_json,
                                    _object_allocator_t>;
        };

        template <typename Dummy>
        struct _object_type_selector<true, false, Dummy>
        {
            // assume it's a std::map-like container
            using type = ObjectType<StringType,
                                    basic_json,
                                    std::less<>,
                                    _object_allocator_t>;
        };

        template <typename Dummy>
        struct _object_type_selector<false, true, Dummy>
        {
            // assume it's a std::unordered_map-like container
            using type = ObjectType<StringType,
                                    basic_json,
                                    std::hash<StringType>,
                                    std::equal_to<StringType>,
                                    _object_allocator_t>;
        };

        template <bool IsMapLike, bool IsUnorderedMapLike>
        using _object_type_selector_t = typename _object_type_selector<IsMapLike, IsUnorderedMapLike>::type;

        // =============================================================
        //  * Public Types & Aliases
        // =============================================================
    public:
        using boolean = BooleanType;
        using number_int = NumberIntegerType;
        using number_float = NumberFloatType;
        using string = StringType;
        using array = ArrayType<basic_json, AllocatorType<basic_json>>;
        using object = _object_type_selector_t<_is_map_like, _is_unordered_map_like>;
        using json_t = BASIC_JSON_TYPE;
        using value_t = std::variant <
            std::monostate,
            null_t,
            boolean,
            number_int,
            number_float,
            string,
            array,
            object
        >;

        // Iterator Support
        using iterator = null_t;
        using const_iterator = null_t;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // [STL Compatibility] Standard container aliases
        using value_type = basic_json;
        using allocator_type = AllocatorType<value_type>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = value_type const&;
        using pointer = value_type*;
        using const_pointer = value_type const*;

        template <typename T>
        constexpr static bool is_json_value_type =
            std::is_same_v<T, std::monostate> ||
            std::is_same_v<T, null_t> ||
            std::is_same_v<T, bool> ||
            std::is_same_v<T, boolean> ||
            std::is_integral_v<T> ||
            std::is_same_v<T, number_int> ||
            std::is_floating_point_v<T> ||
            std::is_same_v<T, number_float> ||
            std::is_convertible_v<T, string> ||
            std::is_same_v<T, array> ||
            std::is_same_v<T, object>;

        // =============================================================
        //  * Lifecycle (Constructors, Destructor, Assignment)
        // =============================================================
    public:
        basic_json() = default;
        ~basic_json() = default;

        // Type constructors
        basic_json(null_t): m_value(null_t()) {}
        explicit basic_json(boolean val): m_value(val) {}
        // Constructor for integral types (including char, which will be treated as an integer)
        template <typename T_Integer,
            std::enable_if_t<std::is_integral_v<T_Integer>, int> = 0>
        basic_json(T_Integer val): m_value(val) {}
        basic_json(number_int val): m_value(val) {}
        // Constructor for floating-point types
        template <typename T_Float,
            std::enable_if_t<std::is_floating_point_v<T_Float>, int> = 0>
        basic_json(T_Float val): m_value(val) {}
        basic_json(number_float val): m_value(val) {}
        basic_json(string val): m_value(std::move(val)) {}
        basic_json(char const* val): m_value(val) {}
        explicit basic_json(std::string_view val): m_value(string(val)) {} // Explicit to prevent expensive, implicit copies from a non-owning string_view.
        basic_json(array val): m_value(std::move(val)) {}
        basic_json(object val): m_value(std::move(val)) {}

        // Copy and move
        basic_json(basic_json const& other) = default;
        basic_json(basic_json&& other) noexcept = default;
        reference operator=(basic_json const& other) = default;
        reference operator=(basic_json&& other) noexcept = default;

        // Templated assignment
        template <typename T,
            std::enable_if_t<is_json_value_type<std::decay_t<T>> &&
                            !std::is_same_v<std::decay_t<T>, basic_json>, int> = 0>
        reference operator=(T&& val) { m_value = std::forward<T>(val); return *this; }

        // =============================================================
        //  * Capacity & Property (查询状态)
        //  (新增：size, max_size, capacity, reserve, shrink_to_fit)
        // =============================================================
    public:
        Type type() const noexcept { return static_cast<Type>(m_value.index()); }

        template <Type T>
        void set_type(bool clear_content = false);
        void set_type(Type const& t, bool clear_content = false);

        // State Checkers
        bool empty() const noexcept { return std::holds_alternative<std::monostate>(m_value); }
        size_type size() const noexcept;       // [New]
        size_type max_size() const noexcept;   // [New]
        size_type capacity() const;            // [New] (Array only)

        // Capacity Modifiers (虽是修改，但归类于容量更直观)
        void reserve(size_type new_cap);       // [New] (Array/Object)
        void shrink_to_fit();                  // [New] (Array/String)

        // Type Predicates
        bool is_null() const noexcept { return std::holds_alternative<null_t>(m_value); }
        bool is_bool() const noexcept { return std::holds_alternative<boolean>(m_value); }
        bool is_number() const noexcept { return std::holds_alternative<number_int>(m_value) || std::holds_alternative<number_float>(m_value); }
        bool is_int() const noexcept { return std::holds_alternative<number_int>(m_value); }
        bool is_float() const noexcept { return std::holds_alternative<number_float>(m_value); }
        bool is_string() const noexcept { return std::holds_alternative<string>(m_value); }
        bool is_array() const noexcept { return std::holds_alternative<array>(m_value); }
        bool is_object() const noexcept { return std::holds_alternative<object>(m_value); }

        // =============================================================
        //  * Element Access (获取数据)
        //  (新增：front, back, value)
        // =============================================================
    public:
        // Array access
        reference operator[](size_type index);
        value_type const& operator[](size_type index) const;
        reference at(size_type index);
        value_type const& at(size_type index) const;
        // Object access
        reference operator[](std::string const& key);
        value_type const& operator[](std::string const& key) const;
        reference at(std::string const& key);
        value_type const& at(std::string const& key) const;

        // [New] Front & Back (Array only)
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;

        // [New] Value with Default (Object only)
        template <typename ValueType>
        ValueType value(string const& key, ValueType const& default_value) const;

        // =============================================================
        //  * Lookup (Object 查找)
        //  (新增：find, count, contains)
        // =============================================================
    public:
        // [New] Find
        iterator find(string const& key);
        const_iterator find(string const& key) const;

        // [New] Count & Contains
        size_type count(string const& key) const;
        bool contains(string const& key) const;

        // =============================================================
        //  * Iterators
        //  (待实现，先占位)
        // =============================================================
    public:
        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;
        reverse_iterator rbegin();
        const_reverse_iterator rbegin() const;
        const_reverse_iterator crbegin() const;
        reverse_iterator rend();
        const_reverse_iterator rend() const;
        const_reverse_iterator crend() const;

        // =============================================================
        //  * Modifiers (修改内容)
        //  (新增：clear, pop_back, insert, erase, update, resize, swap)
        // =============================================================
    public:
        // General
        void clear() noexcept;
        void swap(reference other) noexcept { m_value.swap(other.m_value); }
        friend void swap(reference lhs, reference rhs) noexcept { lhs.m_value.swap(rhs.m_value); } // for ADL (Argument-Dependent Lookup)

        // Array Modifiers
        void push_back(basic_json&& val);
        void push_back(const_reference val);
        template <typename... Args>
        reference emplace_back(Args&&... args);
        void pop_back();

        // Resize (Array)
        void resize(size_type new_size);
        void resize(size_type new_size, const_reference value);

        // Object/Array Insertion
        // [New] Insert (Range)
        template <typename InputIt>
        void insert(InputIt first, InputIt last);

        // [New] Insert (Init List)
        void insert(std::initializer_list<basic_json> il);

        // [New] Insert (Pair - Existing but improved signatures?)
        auto insert(std::pair<string, basic_json> const& pair);
        auto insert(std::pair<string, basic_json>&& pair);

        // [New] Insert at pos (Array) - Blocked until iterator ready
        iterator insert(const_iterator pos, const_reference val);

        // Emplace (Object)
        template <typename... Args>
        auto emplace(Args&&... args);
        template <typename... Args>
        std::pair<iterator, bool> try_emplace(string const& key, Args&&... args);

        // Object/Array Erasure
        // [New] Erase by Key
        size_type erase(string const& key);
        // [New] Erase by Index
        void erase(size_type index);
        // [New] Erase by Iterator - Blocked
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        // Object Merge
        // [New] Update
        void update(const_reference other, bool merge_objects = false);
        void update(const_iterator first, const_iterator last, bool merge_objects = false);

        // =============================================================
        //  * Conversions (类型转换)
        // =============================================================
    public:
        // Pointers (Safe)
        boolean const* get_if_bool() const noexcept { return std::get_if<boolean>(&m_value); }
        boolean* get_if_bool() noexcept { return std::get_if<boolean>(&m_value); }

        number_int const* get_if_int() const noexcept { return std::get_if<number_int>(&m_value); }
        number_int* get_if_int() noexcept { return std::get_if<number_int>(&m_value); }

        number_float const* get_if_float() const noexcept { return std::get_if<number_float>(&m_value); }
        number_float* get_if_float() noexcept { return std::get_if<number_float>(&m_value); }

        std::string const* get_if_string() const noexcept { return std::get_if<string>(&m_value); }
        string* get_if_string() noexcept { return std::get_if<string>(&m_value); }

        array const* get_if_array() const noexcept { return std::get_if<array>(&m_value); }
        array* get_if_array() noexcept { return std::get_if<array>(&m_value); }

        object const* get_if_object() const noexcept { return std::get_if<object>(&m_value); }
        object* get_if_object() noexcept { return std::get_if<object>(&m_value); }

        // References (Asserted)
        boolean as_bool() const { return as_impl<boolean>(m_value, "bool"); }
        boolean& as_bool() { return as_impl<boolean>(m_value, "bool"); }

        number_int as_int() const { return as_impl<number_int>(m_value, "int64"); }
        number_int& as_int() { return as_impl<number_int>(m_value, "int64"); }

        number_float as_float() const { return as_impl<number_float>(m_value, "double"); }
        number_float& as_float() { return as_impl<number_float>(m_value, "double"); }

        string const& as_string() const { return as_impl<string>(m_value, "string"); }
        std::string& as_string() { return as_impl<string>(m_value, "string"); }

        array const& as_array() const { return as_impl<array>(m_value, "array"); }
        array& as_array() { return as_impl<array>(m_value, "array"); }

        object const& as_object() const { return as_impl<object>(m_value, "object"); }
        object& as_object() { return as_impl<object>(m_value, "object"); }

        // =============================================================
        //  * Operators & Serialization
        // =============================================================
    public:
        bool operator==(const_reference other) const;
        bool operator!=(const_reference other) const { return !(*this == other); }

        static basic_json parse(std::string_view json_doc);
        static basic_json parse(std::istream& json_istream);
        template <typename StreamT,
            std::enable_if_t<traits::is_json_stream_v<StreamT>, int> = 0>
        static basic_json parse(StreamT& stream);

        void dump(std::string& buffer) const;
        void dump(std::ostream& os) const;
        template <typename SerializeHandlerT,
            std::enable_if_t<traits::is_json_serialize_handler_v<SerializeHandlerT>, int> = 0>
        void dump(SerializeHandlerT& handler);

        std::string stringify() const;

        // =============================================================
        //  * Private Implementation
        // =============================================================
    private:
        template <typename JsonT, typename SerializeHandlerT>
        friend class details::JsonSerializer;

    private:
        value_t m_value;

    private:
        // [To Implement] 必须存储 Allocator 实例，否则无法支持 Stateful Allocator
        // [[no_unique_address]] AllocatorType m_allocator = AllocatorType();

        // [新增] 统一的内部容器创建函数 (建议实现)
        template <typename ContainerType>
        void create_container();

        template <Type T>
        void set_type_impl();

        template <typename T>
        static T& as_impl(value_t& v, char const* typeName);

        template <typename T>
        static T const& as_impl(value_t const& v, char const* typeName);

    }; // class basic_json

    // The stream itself has the function of adding \ (escaping) to characters that need to be escaped. This process occurs from memory to the stream.
    BASIC_JSON_TEMPLATE
    std::ostream& operator<<(std::ostream& os, BASIC_JSON_TYPE const& val)
    {
        val.dump(os);
        return os;
    }


} // namespace jsonpp

#endif //JSONPP_BASIC_JSON_HPP
