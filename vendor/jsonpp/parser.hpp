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

#ifndef JSONPP_PARSER_H
#define JSONPP_PARSER_H

#include "macro_def.hpp"
#include "traits.hpp"
#include "jsonexception.hpp"
#include "basic_json.hpp"

#include <string_view>
#include <charconv>
#include <cstddef>

namespace jsonpp
{
    using namespace traits;

    namespace details
    {
        /*
         * ParserBase
         */
        template <typename StreamT>
        class ParserBase
        {
            static_assert(is_json_stream_v<StreamT>, "StreamT should be a JSON Stream.");

        protected:
            StreamT& m_stream;

        public:
            int peek() const noexcept { return m_stream.peek(); }
            int advance() noexcept { return m_stream.advance(); }
            std::size_t tell_pos() const noexcept { return m_stream.tell_pos(); }
            bool eof() const noexcept { return m_stream.eof(); }

            template <typename ExceptionT>
            void consume(char expected, ExceptionT const& e) { if (advance() == expected); else throw e; }

            std::size_t size() const { if constexpr (is_sized_stream_v<StreamT>) { return m_stream.size(); }
                else { static_assert(false, ".size() was called, but the stream is not a Sized Stream."); } }

            void seek(std::size_t step) { if constexpr (is_seekable_stream_v<StreamT>) { m_stream.seek(step); }
                else { static_assert(false, ".seek() was called, but the stream is not a Seekable Stream."); } }

            std::string_view get_chunk(std::size_t begin, std::size_t length) { if constexpr (is_contiguous_stream_v<StreamT>) { return m_stream.get_chunk(begin, length); }
                else { static_assert(false, ".get_chunk() was called, but the stream is not a Contiguous Stream."); } }

            template <typename FunctorT>
            std::string_view read_chunk_until(FunctorT predicate) { if constexpr (is_contiguous_stream_v<StreamT>) { return m_stream.read_chunk_until(predicate); }
                else { static_assert(false, ".get_chunk_until() was called, but the stream is not a Contiguous Stream."); } }

            explicit ParserBase(StreamT& stream): m_stream(stream) {}
        };
        /*
         * end ParserBase
         */

        /*
         * JSONStringParser
         */
        template <typename StreamT, typename JsonT>
        class JSONStringParser : public ParserBase<StreamT>
        {
        protected:
            JSONPP_IMPORT_PARSERBASE_MEMBERS_

            using string = typename JsonT::string;

        private:
            string m_result;
            std::size_t m_start;

            enum class UCPStatus: std::uint8_t // Unicode Code Point Status
            {
                SINGLE,
                HIGH,
                LOW
            };

            struct hex4_result
            {
                std::uint16_t value = 0;
                UCPStatus type = UCPStatus::SINGLE;
            };

            hex4_result read_hex4(std::size_t upos);
            void append_utf8(std::uint32_t codepoint);
            static std::uint32_t get_codepoint(std::uint16_t high, std::uint16_t low);
            void unescape_character();

        public:
            JSONStringParser(StreamT& stream, std::size_t _start): ParserBase<StreamT>(stream), m_result(), m_start(_start) {}
            string parse();

        };

        template <typename StreamT, typename JsonT>
        typename JSONStringParser<StreamT, JsonT>::hex4_result JSONStringParser<StreamT, JsonT>::read_hex4(std::size_t upos)
        {
            std::uint16_t value;
            char num_buf[4];
            for (int i = 0; i < 4; ++i)
                num_buf[i] = advance();
            JSONPP_CHECK_EOF_("string", m_start);

            auto [ptr, ec] = std::from_chars(num_buf, num_buf + 4, value, 16);
            if (ec == std::errc() && ptr == num_buf + 4)
            {
                UCPStatus type;
                if (value >= 0xD800 && value <= 0xDBFF)
                    type = UCPStatus::HIGH;
                else if (value >= 0xDC00 && value <= 0xDFFF)
                    type = UCPStatus::LOW;
                else
                    type = UCPStatus::SINGLE;
                return {value, type};
            }
            else throw JsonParseError("Invalid hexadecimal digits found in Unicode escape sequence", upos);
        }

        template <typename StreamT, typename JsonT>
        void JSONStringParser<StreamT, JsonT>::append_utf8(std::uint32_t codepoint)
        {
            if (codepoint <= 0x7F)
            {
                // ASCII, 0b0xxxxxxx
                m_result += static_cast<char>(codepoint);
            }
            else if (codepoint <= 0x7FF)
            {
                // 2-byte UTF-8, 0b110xxxxx 0b10xxxxxx
                m_result += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
                m_result += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
            else if (codepoint <= 0xFFFF)
            {
                // 3-byte UTF-8, 0b1110xxxx 0b10xxxxxx 0b10xxxxxx
                m_result += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
                m_result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                m_result += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
            else if (codepoint <= 0x10FFFF)
            {
                // 4-byte UTF-8, 0b11110xxx 0b10xxxxxx 0b10xxxxxx 0b10xxxxxx
                m_result += static_cast<char>(0xF0 | (codepoint >> 18));
                m_result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                m_result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                m_result += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
        }

        template <typename StreamT, typename JsonT>
        std::uint32_t JSONStringParser<StreamT, JsonT>::get_codepoint(std::uint16_t high, std::uint16_t low)
        {
            return
                0x10000 | (((std::uint32_t)high & 0x03FF) << 10) | ((std::uint32_t)low & 0x03FF);
        }

        template <typename StreamT, typename JsonT>
        void JSONStringParser<StreamT, JsonT>::unescape_character()
        {
            switch (peek())
            {
            case '\"': m_result += '\"'; advance(); break;
            case '\\': m_result += '\\'; advance(); break;
            case '/': m_result += '/'; advance(); break;
            case 'b': m_result += '\b'; advance(); break;
            case 'f': m_result += '\f'; advance(); break;
            case 'n': m_result += '\n'; advance(); break;
            case 'r': m_result += '\r'; advance(); break;
            case 't': m_result += '\t'; advance(); break;
            case 'u':
                {
                    auto upos = tell_pos();
                    advance();

                    auto [cp, type] = read_hex4(upos);

                    switch (type)
                    {
                    case UCPStatus::SINGLE:
                        append_utf8(cp);
                        break;
                    case UCPStatus::HIGH:
                        {
                            consume('\\', JsonParseError("Expected low surrogate after high surrogate in Unicode escape sequence", tell_pos()));
                            consume('u', JsonParseError("Expected low surrogate after high surrogate in Unicode escape sequence", tell_pos()));
                            auto [cp_low, type_low] = read_hex4(upos);
                            if (type_low != UCPStatus::LOW)
                                throw JsonParseError("Expected low surrogate after high surrogate in Unicode escape sequence", tell_pos());

                            append_utf8(get_codepoint(cp, cp_low));
                            break;
                        }
                    case UCPStatus::LOW:
                        throw JsonParseError("Unexpected low surrogate without preceding high surrogate", upos);
                    }
                    break;
                }
            default:
                throw JsonParseError("Invalid escape character", tell_pos());
            }
        }

        template <typename StreamT, typename JsonT>
        typename JSONStringParser<StreamT, JsonT>::string JSONStringParser<StreamT, JsonT>::parse()
        {
            advance(); // 字符串起点, 跳过左引号

            if constexpr (is_sized_stream_v<StreamT>)
            { // 如果能直接得到整个流的大小则直接预留空间
                m_result.reserve(size());
            }

            while (!eof())
            {
                if constexpr (is_contiguous_stream_v<StreamT>)
                {
                    std::string_view chunk = read_chunk_until([](char c)
                    { // 需要终止搜索的字符, 分别代表转义, 结束, 控制字符(需转义)
                        return c == '\\' || c == '\"' || static_cast<unsigned char>(c) < 0x20;
                    });

                    if (!chunk.empty())
                        m_result.append(chunk);
                    JSONPP_CHECK_EOF_("string", m_start);
                }
                // 下面检查为什么停下

                int ch = peek();
                if (ch == '\"')
                    break;

                if (ch == '\\')
                {
                    advance();
                    unescape_character();
                }
                // JSON 规范 (RFC 8259) 禁止未转义的控制字符 (U+0000 到 U+001F)
                else if (ch < 0x20)
                    throw JsonParseError("Unescaped control character in string", tell_pos());
                else
                {
                    // 只有 IStreamStream 会在这里命中好字符, StringViewStream 已经在 chunk 中处理了它们
                    m_result += ch;
                    advance();
                }
            }
            JSONPP_CHECK_EOF_("string", m_start);

            advance(); // 跳过右引号
            return std::move(m_result);
        }
        /*
         * end JSONStringParser
         */

        /*
         * JSON Parser
         */
        template <typename StreamT, typename JsonT>
        class Parser : public ParserBase<StreamT>
        {
        protected:
            JSONPP_IMPORT_PARSERBASE_MEMBERS_

            using boolean = typename JsonT::boolean;
            using number_int = typename JsonT::number_int;
            using number_float = typename JsonT::number_float;
            using string = typename JsonT::string;
            using array = typename JsonT::array;
            using object = typename JsonT::object;

        private:
            int m_nesting_depth;

            static bool is_whitespace(char ch) noexcept;

            void skip_whitespace() noexcept; // 跳过从 pos 开始的空白字符, 使 pos 指向调用函数后的第一个非空白字符

            void parse_literal(char const* lit, std::size_t len);

            static JsonT parse_number_from_chunk(std::string_view chunk, std::size_t start);

            JsonT parse_value(); // 解析, 返回并跳过从当前 pos 开始的一个 basic_json, 使 pos 指向被解析的 basic_json 后的第一个字节

            JsonT parse_null();
            JsonT parse_true();
            JsonT parse_false();
            JsonT parse_number();
            JsonT parse_string();
            JsonT parse_array();
            JsonT parse_object();

        public:
            Parser() = delete;

            explicit Parser(StreamT& stream) : ParserBase<StreamT>(stream), m_nesting_depth(0) {}

            JsonT parse();
        };

        template <typename StreamT, typename JsonT>
        void Parser<StreamT, JsonT>::parse_literal(char const* lit, std::size_t len)
        {
            for (std::size_t i = 0; i < len; ++i)
            {
                consume(lit[i], JsonParseError(JsonParseError::UNPARSABLE_MESSAGE, tell_pos()));
            }
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_number_from_chunk(std::string_view chunk, std::size_t start)
        {
            std::int64_t val_i{};
            auto res_i = std::from_chars(chunk.data(), chunk.data() + chunk.size(), val_i);
            if (res_i.ptr == chunk.data() + chunk.size() && res_i.ec == std::errc()) // 成功
            {
                return {val_i};
            }

            double val_f{};
            auto res_f = std::from_chars(chunk.data(), chunk.data() + chunk.size(), val_f);
            if (res_f.ptr == chunk.data() + chunk.size() && res_f.ec == std::errc()) // 成功
            {
                return {val_f};
            }

            if (res_f.ec == std::errc::result_out_of_range)
                throw JsonParseError("Number is out of range", start);
            else
                throw JsonParseError(JsonParseError::UNPARSABLE_MESSAGE, res_f.ptr - chunk.data());

        }

        template <typename StreamT, typename JsonT>
        bool Parser<StreamT, JsonT>::is_whitespace(char ch) noexcept
        {
            return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
        }

        template <typename StreamT, typename JsonT>
        void Parser<StreamT, JsonT>::skip_whitespace() noexcept
        {
            while (is_whitespace(peek()))
                advance();
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_value()
        {
            // 调用该函数之前与之后均调用了 skip_whitespace()
            if (eof())
                throw JsonParseError("Unexpected end of file");
            char ch = peek();
            switch (ch)
            {
            case 'n':
                return parse_null();
            case 't':
                return parse_true();
            case 'f':
                return parse_false();
            case '\"':
                return parse_string();
            case '[':
                return parse_array();
            case '{':
                return parse_object();
            default:
                if ((ch >= '0' && ch <= '9') || ch == '-')
                    return parse_number();
                throw JsonParseError(JsonParseError::UNPARSABLE_MESSAGE, tell_pos());
            }
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_null()
        {
            parse_literal("null", 4);
            return {null};
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_true()
        {
            parse_literal("true", 4);
            return JsonT(true);
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_false()
        {
            parse_literal("false", 5);
            return JsonT(false);
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_number()
        {
            auto is_num_char = [](char c) -> bool {
                return isdigit(static_cast<unsigned char>(c))
                    || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E';
            };

            std::size_t start = tell_pos();

            if constexpr (is_contiguous_stream_v<StreamT>)
            {
                while (is_num_char(peek()))
                    advance();
                return parse_number_from_chunk(get_chunk(start, tell_pos() - start), start);
            }
            else
            {
                std::string chunk;
                while (is_num_char(peek()))
                {
                    chunk += advance(); // 停在第 1 个不可能是数字字符的位置
                }
                return parse_number_from_chunk(chunk, start);
            }

        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_string()
        {
            return JSONStringParser<StreamT, JsonT>(m_stream, tell_pos()).parse();
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_array()
        {
            // Increase nesting depth counter and check limit
            if (++m_nesting_depth > MAX_NESTING_DEPTH)
                throw JsonDepthLimitExceeded(tell_pos());

            array arr;
            auto start = tell_pos(); // 跳过左 [
            advance();
            skip_whitespace();
            while (!eof() && peek() != ']')
            {
                arr.push_back(parse_value());
                skip_whitespace();

                // json数组中对象以外的字符只能是空白字符或'['或']'或','
                if (peek() == ']')
                    break;
                else if (peek() != ',')
                {
                    JSONPP_CHECK_EOF_("array", start);
                    throw JsonParseError(JsonParseError::UNPARSABLE_MESSAGE, tell_pos());
                }
                advance(); // 跳过 ','

                skip_whitespace();

                if (peek() == ']')
                    throw JsonParseError("Expected value after comma, but found ']' instead", tell_pos());
            }
            JSONPP_CHECK_EOF_("array", start);

            advance(); // 跳过右 ]
            --m_nesting_depth; // Decrease nesting depth counter before returning
            return {arr};
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse_object()
        {
            // Increase nesting depth counter and check limit
            if (++m_nesting_depth > MAX_NESTING_DEPTH)
                throw JsonDepthLimitExceeded(tell_pos());

            object obj;
            auto start = tell_pos(); // 跳过左 {
            advance();
            skip_whitespace();
            while (!eof() && peek() != '}')
            {
                auto key = parse_value();

                if (!key.is_string()) [[unlikely]]
                    throw JsonParseError("Key of an object must be string");

                skip_whitespace();

                if (peek() != ':')
                {
                    JSONPP_CHECK_EOF_("object", start);
                    throw JsonParseError(JsonParseError::UNPARSABLE_MESSAGE, tell_pos());
                }
                advance();

                skip_whitespace();
                auto val = parse_value();
                obj[key.as_string()] = val;

                skip_whitespace();

                if (peek() == '}')
                    break;
                else if (peek() != ',')
                {
                    JSONPP_CHECK_EOF_("object", start);
                    throw JsonParseError(JsonParseError::UNPARSABLE_MESSAGE, tell_pos());
                }
                advance(); // skip comma

                skip_whitespace();

                if (peek() == '}')
                    throw JsonParseError("Expected value after comma, but found '}' instead", tell_pos());
            }
            JSONPP_CHECK_EOF_("object", start);

            advance(); // 跳过右 }
            --m_nesting_depth; // Decrease nesting depth counter before returning
            return {obj};
        }

        template <typename StreamT, typename JsonT>
        JsonT Parser<StreamT, JsonT>::parse()
        {
            skip_whitespace();
            if (eof()) // doc 为空
                return {};

            auto val = parse_value();
            skip_whitespace();

            if (eof()) // 表示恰好解析整个文档
                return val;
            else
                throw JsonParseError("Unexpected character(s) after JSON value");

            return {};
        }
        /*
         * end JSON Parser
         */
    }

}

#endif //JSONPP_PARSER_H