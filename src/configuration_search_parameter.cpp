/***************************************************************************
 *            configuration_search_parameter.cpp
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

#include "configuration_search_parameter.hpp"

namespace ProNest {

ConfigurationSearchParameter::ConfigurationSearchParameter(ConfigurationPropertyPath const& path, bool is_metric, List<int> const& values) :
    _path(path), _is_metric(is_metric), _values(values) {
    PRONEST_PRECONDITION(values.size()>1);
}

ConfigurationPropertyPath const& ConfigurationSearchParameter::path() const {
    return _path;
}

List<int> const& ConfigurationSearchParameter::values() const {
    return _values;
}

bool ConfigurationSearchParameter::is_metric() const {
    return _is_metric;
}

int ConfigurationSearchParameter::random_value() const {
    return _values[(SizeType)rand() % _values.size()];
}

int ConfigurationSearchParameter::shifted_value_from(int value) const {
    SizeType num_values = _values.size();
    if (_is_metric) {
        if (value == _values[0]) return value+1;
        if (value == _values[num_values-1]) return value-1;
        if ((SizeType)rand() % 2 == 0) return value+1;
        else return value-1;
    } else {
        SizeType rand_value = (SizeType)rand() % (num_values-1);
        if (_values[rand_value] == value) return _values[num_values-1];
        else return _values[rand_value];
    }
}

bool ConfigurationSearchParameter::operator==(ConfigurationSearchParameter const& p) const {
    return path() == p.path();
}

bool ConfigurationSearchParameter::operator<(ConfigurationSearchParameter const& p) const {
    return path() < p.path();
}

OutputStream& operator<<(OutputStream& os, ConfigurationSearchParameter const& p) {
    os << "{'" << p._path << "', is_metric=" << p._is_metric << ", values=";
    if (p._is_metric) os << "[" << p._values[0] << ":" << p._values[p._values.size()-1] << "]";
    else {
        os << "[";
        for (SizeType i=0; i<p._values.size()-1; ++i) os << p._values[i] << ",";
        os << p._values[p._values.size()-1] << "]";
    }
    return os << "}";
}

} // namespace ProNest
