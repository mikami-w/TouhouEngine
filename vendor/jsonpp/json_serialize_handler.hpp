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

#ifndef JSONPP_JSON_SERIALIZE_HANDLER_HPP
#define JSONPP_JSON_SERIALIZE_HANDLER_HPP

#include <string>
#include <string_view>
#include <ostream>

namespace jsonpp::details
{
    class StringSerializeHandler
    {
        std::string& m_buffer;

    public:
        explicit StringSerializeHandler(std::string& buf): m_buffer(buf) {}

        void append(char ch)
        {
            m_buffer.push_back(ch);
        }

        void append(std::string_view str)
        {
            m_buffer.append(str);
        }

        void append(char const* cstr, std::size_t length)
        {
            m_buffer.append(cstr, length);
        }
    };

    class OStreamSerializeHandler
    {
        std::ostream& m_out;

    public:
        explicit OStreamSerializeHandler(std::ostream& os): m_out(os) {}

        void append(char ch)
        {
            m_out.put(ch);
        }

        void append(std::string_view str)
        {
            m_out.write(str.data(), str.size());
        }

        void append(char const* cstr, std::size_t length)
        {
            m_out.write(cstr, length);
        }

        bool bad() const
        {
            return m_out.bad();
        }
    };

}


#endif //JSONPP_JSON_SERIALIZE_HANDLER_HPP