/***************************************************************************
 *            configuration_property_path.cpp
 *
 *  Copyright  2023  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of ProNest, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "utility/macros.hpp"
#include "configuration_property_path.hpp"

namespace ProNest {

ConfigurationPropertyPath::ConfigurationPropertyPath(String const& first) {
    _path.push_back(first);
}

ConfigurationPropertyPath::ConfigurationPropertyPath(ConfigurationPropertyPath const& path) {
    _path = path._path;
}

ConfigurationPropertyPath& ConfigurationPropertyPath::operator=(ConfigurationPropertyPath const& path) {
    _path = path._path;
    return *this;
}

bool ConfigurationPropertyPath::operator<(ConfigurationPropertyPath const& path) const {
    return this->repr() < path.repr();
}

bool ConfigurationPropertyPath::operator==(ConfigurationPropertyPath const& path) const {
    return this->repr() == path.repr();
}

String ConfigurationPropertyPath::repr() const {
    std::ostringstream sstream;
    sstream << *this;
    return sstream.str();
}

bool ConfigurationPropertyPath::is_root() const {
    return _path.empty();
}

ConfigurationPropertyPath& ConfigurationPropertyPath::append(String const& node) {
    UTILITY_PRECONDITION(not node.empty());
    _path.push_back(node);
    return *this;
}

ConfigurationPropertyPath& ConfigurationPropertyPath::prepend(String const& node) {
    UTILITY_PRECONDITION(not node.empty());
    _path.push_front(node);
    return *this;
}

String ConfigurationPropertyPath::first() const {
    UTILITY_PRECONDITION(not is_root());
    return _path.front();
}

String ConfigurationPropertyPath::last() const {
    UTILITY_PRECONDITION(not is_root());
    return _path.back();
}

ConfigurationPropertyPath ConfigurationPropertyPath::subpath() const {
    auto result = *this;
    result._path.pop_front();
    return result;
}

std::ostream& operator<<(std::ostream& os, ConfigurationPropertyPath const& p) {
    auto size = p._path.size();
    auto iter = p._path.begin();
    os << "./";
    for (std::size_t i=0; i<size; ++i) {
        os << *iter << "/";
        ++iter;
    }
    return os;
}

} // namespace ProNest
