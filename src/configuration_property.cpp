/***************************************************************************
 *            configuration_property.cpp
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

#include <ostream>
#include "utility/macros.hpp"
#include "configuration_property.hpp"
#include "configuration_property.tpl.hpp"

namespace ProNest {

BooleanConfigurationProperty::BooleanConfigurationProperty()
    : ConfigurationPropertyBase(false), _is_single(false)
{ }

BooleanConfigurationProperty::BooleanConfigurationProperty(bool const& value)
    : ConfigurationPropertyBase(true), _is_single(true), _value(value)
{ }

ConfigurationPropertyInterface* BooleanConfigurationProperty::at(ConfigurationPropertyPath const& path) {
    UTILITY_ASSERT_MSG(path.is_root(),"The path " << path << " is not a root but a boolean property can't have configurable objects below.");
    return this;
}

bool const& BooleanConfigurationProperty::get() const {
    UTILITY_PRECONDITION(this->is_specified());
    UTILITY_ASSERT_MSG(this->is_single(),"The property should have a single value when actually used. Are you accessing it outside the related task?");
    return _value;
}

bool BooleanConfigurationProperty::is_single() const {
    return _is_single;
}

bool BooleanConfigurationProperty::is_metric(ConfigurationPropertyPath const& path) const {
    UTILITY_PRECONDITION(path.is_root());
    return false;
}

bool BooleanConfigurationProperty::is_configurable() const {
    return false;
}

size_t BooleanConfigurationProperty::cardinality() const {
    if (_is_single) return 1;
    else if (this->is_specified()) return 2;
    else return 0;
}

List<int> BooleanConfigurationProperty::local_integer_values() const {
    List<int> result;
    if (_is_single) result.push_back(_value);
    else if (is_specified()) { result.push_back(0); result.push_back(1); }
    return result;
}

void BooleanConfigurationProperty::set_single(ConfigurationPropertyPath const& path, int integer_value) {
    UTILITY_PRECONDITION(path.is_root());
    local_set_single(integer_value);
}

void BooleanConfigurationProperty::local_set_single(int integer_value) {
    UTILITY_PRECONDITION(not _is_single);
    UTILITY_PRECONDITION(integer_value == 0 or integer_value == 1);
    _is_single = true;
    if (integer_value == 1) _value = true;
    else _value = false;
}

ConfigurationPropertyInterface* BooleanConfigurationProperty::clone() const {
    return new BooleanConfigurationProperty(*this);
}

void BooleanConfigurationProperty::set_both() {
    set_specified();
    _is_single = false;
}

void BooleanConfigurationProperty::set(bool const& value) {
    set_specified();
    _is_single = true;
    _value=value;
}

List<shared_ptr<bool>> BooleanConfigurationProperty::values() const {
    List<shared_ptr<bool>> result;
    if (is_specified()) {
        if (_is_single) result.push_back(std::make_shared<bool>(_value));
        else {
            result.push_back(std::make_shared<bool>(true));
            result.push_back(std::make_shared<bool>(false));
        }
    }
    return result;
}



} // namespace ProNest
