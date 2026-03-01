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

#ifndef JSONPP_JSON_STREAM_ADAPTOR_HPP
#define JSONPP_JSON_STREAM_ADAPTOR_HPP

#include <cstddef>
#include <algorithm>
#include <cassert>
#include <string_view>
#include <istream>

namespace jsonpp
{
    namespace details
    {
        class StringViewStream
        {
            std::string_view m_data;
            std::size_t m_pos;

        public:
            int peek() const noexcept { if (m_pos < m_data.size()) return static_cast<unsigned char>(m_data[m_pos]); else return EOF; } // To ensure that 255 is correctly handled
            int advance() noexcept { if (m_pos < m_data.size()) return static_cast<unsigned char>(m_data[m_pos++]); else return EOF; }
            std::size_t tell_pos() const noexcept { return m_pos; }
            bool eof() const noexcept { return m_pos >= m_data.size(); }

            std::size_t size() const noexcept { return m_data.size(); }

            void seek(std::size_t step) noexcept
            {
                assert(m_pos + step <= m_data.size() && "StringViewStream::seek out of bounds!");
                m_pos += step;
            }

            std::string_view get_chunk(std::size_t begin, std::size_t length) const noexcept
            {
                assert(begin <= m_data.size() && "StringViewStream::get_chunk invalid begin!");
                return m_data.substr(begin, length);
            }

            template <typename FunctorT>
            std::string_view read_chunk_until(FunctorT predicate) &
            {
                std::string_view remaining = m_data.substr(m_pos);
                auto it = std::find_if(remaining.begin(), remaining.end(), predicate);
                auto chunk_size = std::distance(remaining.begin(), it);
                // Get chunk (if chunk_size=0, return empty view)
                std::string_view chunk = m_data.substr(m_pos, chunk_size);

                m_pos += chunk_size;

                return chunk;
            }
            explicit StringViewStream(std::string_view doc): m_data(doc), m_pos(0) {}
        };

        class IStreamStream
        {
            std::istream& m_is;
            std::size_t m_pos;
        public:
            int peek() const { return m_is.peek(); }
            int advance()
            {
                auto c = m_is.get();
                if (c != EOF)
                    ++m_pos;
                return c;
            }
            std::size_t tell_pos() const noexcept { return m_pos; }
            bool eof() const { return m_is.peek() == EOF; }

            explicit IStreamStream(std::istream& _is): m_is(_is), m_pos(0) { m_is.unsetf(std::ios::skipws); }
        };
    }
}
#endif //JSONPP_JSON_STREAM_ADAPTOR_HPP