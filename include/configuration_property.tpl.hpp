/***************************************************************************
 *            configuration_property.tpl.hpp
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

#ifndef PRONEST_CONFIGURATION_PROPERTY_TPL_HPP
#define PRONEST_CONFIGURATION_PROPERTY_TPL_HPP

#include <ostream>
#include <type_traits>
#include "helper/writable.hpp"
#include "configuration_interface.hpp"
#include "configuration_property.hpp"
#include "configuration_property_path.hpp"
#include "searchable_configuration.hpp"
#include "configurable.hpp"

namespace ProNest {

using std::min;
using std::max;
using Helper::Pair;

inline bool possibly(bool value) { return value; }

template<class T> ConfigurationPropertyBase<T>::ConfigurationPropertyBase(bool const& is_specified) : _is_specified(is_specified) { }

template<class T> void ConfigurationPropertyBase<T>::set_specified() {
    _is_specified = true;
}

template<class T> bool ConfigurationPropertyBase<T>::is_specified() const {
    return _is_specified;
}

template<class T> Map<ConfigurationPropertyPath,List<int>> ConfigurationPropertyBase<T>::integer_values() const {
    Map<ConfigurationPropertyPath,List<int>> result;
    result.insert(Pair<ConfigurationPropertyPath,List<int>>(ConfigurationPropertyPath(), local_integer_values()));
    return result;
}

template<class T> ostream& ConfigurationPropertyBase<T>::_write(ostream& os) const {
    auto vals = values();
    if (vals.empty()) { os << "<unspecified>"; }
    else if (vals.size() == 1) { os << *vals[0]; }
    else {
        os << "{";
        for (size_t i=0; i<vals.size()-1; ++i) os << *vals[i] << ",";
        os << *vals[vals.size()-1] << "}";
    }
    return os;
}

template<class T> RangeConfigurationProperty<T>::RangeConfigurationProperty(ConfigurationSearchSpaceConverterInterface<T> const& converter) :
        ConfigurationPropertyBase<T>(false), _lower(T()), _upper(T()),
        _converter(shared_ptr<ConfigurationSearchSpaceConverterInterface<T>>(converter.clone())) { }

template<class T> RangeConfigurationProperty<T>::RangeConfigurationProperty(T const& lower, T const& upper, ConfigurationSearchSpaceConverterInterface<T> const& converter) :
        ConfigurationPropertyBase<T>(true), _lower(lower), _upper(upper),
        _converter(shared_ptr<ConfigurationSearchSpaceConverterInterface<T>>(converter.clone())) {
    HELPER_PRECONDITION(not possibly(upper < lower));
}

template<class T> RangeConfigurationProperty<T>::RangeConfigurationProperty(T const& value, ConfigurationSearchSpaceConverterInterface<T> const& converter) :
                RangeConfigurationProperty(value,value,converter) { }

template<class T> T const& RangeConfigurationProperty<T>::get() const {
    HELPER_PRECONDITION(this->is_specified());
    HELPER_ASSERT_MSG(this->is_single(),"The property should have a single value when actually used. Are you accessing it outside the related task?");
    return _upper;
}

template<class T> bool RangeConfigurationProperty<T>::is_single() const {
    if (not this->is_specified()) return false;
    else return possibly(_lower == _upper);
}

template<class T> bool RangeConfigurationProperty<T>::is_metric(ConfigurationPropertyPath const& path) const {
    HELPER_PRECONDITION(path.is_root());
    return true;
}

template<class T> bool RangeConfigurationProperty<T>::is_configurable() const {
    return false;
}

template<class T> size_t RangeConfigurationProperty<T>::cardinality() const {
    if (is_single()) return 1;
    else if (not this->is_specified()) return 0;
    else return 1+(size_t)(_converter->to_int(_upper) - _converter->to_int(_lower));
}

template<class T> List<int> RangeConfigurationProperty<T>::local_integer_values() const {
    List<int> result;
    if (this->is_specified()) {
        int min_value = _converter->to_int(_lower);
        int max_value = _converter->to_int(_upper);
        HELPER_ASSERT_MSG(not(max_value == std::numeric_limits<int>::max() and min_value < std::numeric_limits<int>::max()),"An upper bounded range is required.");
        HELPER_ASSERT_MSG(not(min_value == std::numeric_limits<int>::min() and max_value > std::numeric_limits<int>::min()),"A lower bounded range is required.");
        if (min_value == max_value) result.push_back(min_value); // Necessary to address the +inf case
        else for (int i = min_value; i <= max_value; ++i) result.push_back(i);
    }
    return result;
}

template<class T> void RangeConfigurationProperty<T>::set_single(ConfigurationPropertyPath const& path, int integer_value) {
    HELPER_PRECONDITION(path.is_root());
    local_set_single(integer_value);
}

template<class T> void RangeConfigurationProperty<T>::local_set_single(int integer_value) {
    int min_value = _converter->to_int(_lower);
    int max_value = _converter->to_int(_upper);
    HELPER_PRECONDITION(not is_single());
    HELPER_PRECONDITION(integer_value >= min_value and integer_value <= max_value);
    if (integer_value == min_value) _upper = _lower; // Avoids rounding error
    else if (integer_value == max_value) _lower = _upper; // Avoids rounding error
    else { _lower = _upper = _converter->from_int(integer_value); }
}

template<class T> ConfigurationPropertyInterface* RangeConfigurationProperty<T>::clone() const {
    return new RangeConfigurationProperty(*this);
}

template<class T> ConfigurationPropertyInterface* RangeConfigurationProperty<T>::at(ConfigurationPropertyPath const& path) {
    HELPER_ASSERT_MSG(path.is_root(),"The path " << path << " is not a root but a range property can't have configurable objects below.");
    return this;
}

template<class T> void RangeConfigurationProperty<T>::set(T const& lower, T const& upper) {
    HELPER_PRECONDITION(not possibly(upper < lower));
    this->set_specified();
    _lower = lower;
    _upper = upper;
}

template<class T> void RangeConfigurationProperty<T>::set(T const& value) {
    this->set_specified();
    _lower = value;
    _upper = value;
}

template<class T> List<shared_ptr<T>> RangeConfigurationProperty<T>::values() const {
    List<shared_ptr<T>> result;
    if (this->is_specified()) {
        result.push_back(std::make_shared<T>(_lower));
        if (not is_single()) result.push_back(std::make_shared<T>(_upper));
    }
    return result;
}

template<class T> EnumConfigurationProperty<T>::EnumConfigurationProperty()
        : ConfigurationPropertyBase<T>(false) {
    HELPER_PRECONDITION(std::is_enum<T>::value);
}

template<class T> EnumConfigurationProperty<T>::EnumConfigurationProperty(Set<T> const& values)
        : ConfigurationPropertyBase<T>(true), _values(values) {
    HELPER_PRECONDITION(std::is_enum<T>::value);
    HELPER_PRECONDITION(values.size()>0);
}

template<class T> EnumConfigurationProperty<T>::EnumConfigurationProperty(T const& value)
        : ConfigurationPropertyBase<T>(true) {
    HELPER_PRECONDITION(std::is_enum<T>::value);
    _values.insert(value);
}

template<class T> bool EnumConfigurationProperty<T>::is_single() const {
    return (_values.size() == 1);
}

template<class T> bool EnumConfigurationProperty<T>::is_metric(ConfigurationPropertyPath const& path) const {
    HELPER_PRECONDITION(path.is_root());
    return false;
}

template<class T> bool EnumConfigurationProperty<T>::is_configurable() const {
    return false;
}

template<class T> size_t EnumConfigurationProperty<T>::cardinality() const {
    return _values.size();
}

template<class T> List<int> EnumConfigurationProperty<T>::local_integer_values() const {
    List<int> result;
    for (size_t i=0; i<_values.size(); ++i) result.push_back(static_cast<int>(i));
    return result;
}


template<class T> void EnumConfigurationProperty<T>::set_single(ConfigurationPropertyPath const& path, int integer_value) {
    HELPER_PRECONDITION(path.is_root());
    local_set_single(integer_value);
}

template<class T> void EnumConfigurationProperty<T>::local_set_single(int integer_value) {
    HELPER_PRECONDITION(not is_single());
    HELPER_PRECONDITION(integer_value >= 0 and integer_value < (int)cardinality());
    auto iter = _values.begin();
    for (size_t i=0;i<(size_t)integer_value;++i) ++iter;
    T value = *iter;
    _values.clear();
    _values.insert(value);
}

template<class T> ConfigurationPropertyInterface* EnumConfigurationProperty<T>::clone() const {
    return new EnumConfigurationProperty(*this);
}

template<class T> ConfigurationPropertyInterface* EnumConfigurationProperty<T>::at(ConfigurationPropertyPath const& path) {
    HELPER_ASSERT_MSG(path.is_root(),"The path " << path << " is not a root but an enum property can't have configurable objects below.");
    return this;
}

template<class T> T const& EnumConfigurationProperty<T>::get() const {
    HELPER_PRECONDITION(this->is_specified());
    HELPER_ASSERT_MSG(this->is_single(),"The property should have a single value when actually used. Are you accessing it outside the related task?");
    return *_values.begin();
}

template<class T> void EnumConfigurationProperty<T>::set(T const& value) {
    this->set_specified();
    _values.clear();
    _values.insert(value);
}

template<class T> void EnumConfigurationProperty<T>::set(Set<T> const& values) {
    HELPER_PRECONDITION(not values.empty());
    this->set_specified();
    _values = values;
}

template<class T> List<shared_ptr<T>> EnumConfigurationProperty<T>::values() const {
    List<shared_ptr<T>> result;
    for (auto const& v : _values) result.push_back(shared_ptr<T>(new T(v)));
    return result;
}

template<class T> HandleListConfigurationProperty<T>::HandleListConfigurationProperty()
    : ConfigurationPropertyBase<T>(false)
{ }

template<class T> HandleListConfigurationProperty<T>::HandleListConfigurationProperty(List<T> const& values)
    : ConfigurationPropertyBase<T>(true), _values(values) {
        HELPER_PRECONDITION(not values.empty());
}

template<class T> HandleListConfigurationProperty<T>::HandleListConfigurationProperty(T const& value)
    : ConfigurationPropertyBase<T>(true) {
        _values.push_back(value);
}

template<class T> bool HandleListConfigurationProperty<T>::is_single() const {
    return (_values.size() == 1);
}

template<class T> bool HandleListConfigurationProperty<T>::is_metric(ConfigurationPropertyPath const& path) const {
    if (path.is_root()) return false;
    if (is_configurable()) {
        HELPER_PRECONDITION(is_single());
        auto properties = dynamic_cast<const ConfigurableInterface*>(_values.at(0).const_pointer())->searchable_configuration().properties();
        auto p_ptr = properties.find(path.first());
        if (p_ptr != properties.end()) {
            return p_ptr->second->is_metric(path.subpath());
        } else {
            HELPER_FAIL_MSG("A property for " << path << " has not been found.");
        }
    } else {
        HELPER_FAIL_MSG("The object is not configurable, a property for " << path << " could not been found.");
    }
}

template<class T> bool HandleListConfigurationProperty<T>::is_configurable() const {
    HELPER_ASSERT_MSG(this->is_specified(),"Cannot check if configurable if the property is not specified.");
    HELPER_PRECONDITION(is_single());
    auto const configurable_interface_ptr = dynamic_cast<const ConfigurableInterface*>(_values.at(0).const_pointer());
    return (configurable_interface_ptr != nullptr);
}

template<class T> size_t HandleListConfigurationProperty<T>::cardinality() const {
    return _values.size();
}

template<class T> List<int> HandleListConfigurationProperty<T>::local_integer_values() const {
    List<int> result;
    for (size_t i=0; i<_values.size(); ++i) result.push_back(static_cast<int>(i));
    return result;
}

template<class T> void HandleListConfigurationProperty<T>::local_set_single(int integer_value) {
    HELPER_PRECONDITION(not is_single());
    HELPER_PRECONDITION(integer_value >= 0 and integer_value < (int)cardinality());
    T value = _values[(size_t)integer_value];
    _values.clear();
    _values.push_back(value);
}

template<class T> void HandleListConfigurationProperty<T>::set_single(ConfigurationPropertyPath const& path, int integer_value) {
    if (path.is_root()) {
        local_set_single(integer_value);
    } else { // NOTE : we assume that we already checked for being single when getting the integer_values
        bool been_set = false;
        auto configurable_interface_ptr = dynamic_cast<ConfigurableInterface*>(_values.at(0).pointer());
        if (configurable_interface_ptr != nullptr) {
            auto properties = configurable_interface_ptr->searchable_configuration().properties();
            auto p_ptr = properties.find(path.first());
            if (p_ptr != properties.end()) {
                p_ptr->second->set_single(path.subpath(),integer_value);
                been_set = true;
            }
        }
        HELPER_ASSERT_MSG(been_set,"A property for " << path << " has not been found.");
    }
}

template<class T> Map<ConfigurationPropertyPath,List<int>> HandleListConfigurationProperty<T>::integer_values() const {
    Map<ConfigurationPropertyPath,List<int>> result;
    result.insert(Pair<ConfigurationPropertyPath,List<int>>(ConfigurationPropertyPath(),local_integer_values()));
    if (is_single()) { // NOTE: we could extend to multiple values by using indexes
        auto configurable_interface_ptr = dynamic_cast<const ConfigurableInterface*>(_values.at(0).const_pointer());
        if (configurable_interface_ptr != nullptr) {
            for (auto const& p : configurable_interface_ptr->searchable_configuration().properties()) {
                for (auto const& entry : p.second->integer_values()) {
                    auto prefixed_path = entry.first;
                    prefixed_path.prepend(p.first);
                    result.insert(Pair<ConfigurationPropertyPath,List<int>>(prefixed_path,entry.second));
                }
            }
        }
    }
    return result;
}

template<class T> ConfigurationPropertyInterface* HandleListConfigurationProperty<T>::clone() const {
    return new HandleListConfigurationProperty(*this);
}

template<class T> ConfigurationPropertyInterface* HandleListConfigurationProperty<T>::at(ConfigurationPropertyPath const& path) {
    if (path.is_root()) return this;
    else {
        HELPER_ASSERT_MSG(is_configurable(),"The object held is not configurable, path error.");
        HELPER_ASSERT_MSG(is_single(),"Cannot retrieve properties if the list has multiple objects.");
        auto configurable_ptr = dynamic_cast<ConfigurableInterface*>(_values.at(0).pointer());
        auto properties = configurable_ptr->searchable_configuration().properties();
        auto prop_ptr = properties.find(path.first());
        HELPER_ASSERT_MSG(prop_ptr != properties.end(),"The property '" << path.first() << "' was not found in the configuration.");
        return prop_ptr->second->at(path.subpath());
    }
}

template<class T> T const& HandleListConfigurationProperty<T>::get() const {
    HELPER_PRECONDITION(this->is_specified());
    HELPER_ASSERT_MSG(this->is_single(),"The property should have a single value when actually used. Are you accessing it outside the related task?");
    return _values.back();
}

template<class T> void HandleListConfigurationProperty<T>::set(T const& value) {
    this->set_specified();
    _values.clear();
    _values.push_back(value);
}

template<class T> void HandleListConfigurationProperty<T>::set(List<T> const& values) {
    HELPER_PRECONDITION(not values.empty());
    this->set_specified();
    _values = values;
}

template<class T> List<shared_ptr<T>> HandleListConfigurationProperty<T>::values() const {
    List<shared_ptr<T>> result;
    for (auto const& v : _values) result.push_back(shared_ptr<T>(new T(v)));
    return result;
}

template<class T> InterfaceListConfigurationProperty<T>::InterfaceListConfigurationProperty() : ConfigurationPropertyBase<T>(false) { }

template<class T> InterfaceListConfigurationProperty<T>::InterfaceListConfigurationProperty(List<shared_ptr<T>> const& list) : ConfigurationPropertyBase<T>(true), _values(list) {
    HELPER_PRECONDITION(not list.empty());
}

template<class T> InterfaceListConfigurationProperty<T>::InterfaceListConfigurationProperty(T const& value) : ConfigurationPropertyBase<T>(true) {
    _values.push_back(shared_ptr<T>(value.clone()));
}

template<class T> bool InterfaceListConfigurationProperty<T>::is_single() const {
    return (_values.size() == 1);
}

template<class T> bool InterfaceListConfigurationProperty<T>::is_metric(ConfigurationPropertyPath const& path) const {
    if (path.is_root()) return false;
    if (is_configurable()) {
        HELPER_PRECONDITION(is_single());
        auto properties = dynamic_cast<ConfigurableInterface*>(_values.back().get())->searchable_configuration().properties();
        auto p_ptr = properties.find(path.first());
        if (p_ptr != properties.end()) {
            return p_ptr->second->is_metric(path.subpath());
        } else {
            HELPER_FAIL_MSG("A property for " << path << " has not been found.");
        }
    } else {
        HELPER_FAIL_MSG("The object is not configurable, a property for " << path << " could not been found.");
    }
}

template<class T> bool InterfaceListConfigurationProperty<T>::is_configurable() const {
    HELPER_ASSERT_MSG(this->is_specified(),"Cannot check if configurable if the property is not specified.");
    HELPER_PRECONDITION(is_single());
    auto configurable_interface_ptr = dynamic_cast<ConfigurableInterface*>(_values.back().get());
    return (configurable_interface_ptr != nullptr);
}

template<class T> size_t InterfaceListConfigurationProperty<T>::cardinality() const {
    return _values.size();
}

template<class T> List<int> InterfaceListConfigurationProperty<T>::local_integer_values() const {
    List<int> result;
    for (size_t i=0; i<_values.size(); ++i) result.push_back(static_cast<int>(i));
    return result;
}

template<class T> void InterfaceListConfigurationProperty<T>::local_set_single(int integer_value) {
    HELPER_PRECONDITION(not is_single());
    HELPER_PRECONDITION(integer_value >= 0 and integer_value < (int)cardinality());
    shared_ptr<T> value = _values[(size_t)integer_value];
    _values.clear();
    _values.push_back(value);
}

template<class T> void InterfaceListConfigurationProperty<T>::set_single(ConfigurationPropertyPath const& path, int integer_value) {
    if (path.is_root()) {
        local_set_single(integer_value);
    } else { // NOTE : we assume that we already checked for being single when getting the integer_values
        bool been_set = false;
        auto configurable_interface_ptr = dynamic_cast<ConfigurableInterface*>(_values.back().get());
        if (configurable_interface_ptr != nullptr) {
            auto properties = configurable_interface_ptr->searchable_configuration().properties();
            auto p_ptr = properties.find(path.first());
            if (p_ptr != properties.end()) {
                p_ptr->second->set_single(path.subpath(),integer_value);
                been_set = true;
            }
        }
        HELPER_ASSERT_MSG(been_set,"A property for " << path << " has not been found.");
    }
}

template<class T> Map<ConfigurationPropertyPath,List<int>> InterfaceListConfigurationProperty<T>::integer_values() const {
    Map<ConfigurationPropertyPath,List<int>> result;
    result.insert(Pair<ConfigurationPropertyPath,List<int>>(ConfigurationPropertyPath(),local_integer_values()));
    if (is_single()) { // NOTE: we could extend to multiple values by using indexes
        auto configurable_interface_ptr = dynamic_cast<ConfigurableInterface*>(_values.back().get());
        if (configurable_interface_ptr != nullptr) {
            for (auto const& p : configurable_interface_ptr->searchable_configuration().properties()) {
                for (auto const& entry : p.second->integer_values()) {
                    auto prefixed_path = entry.first;
                    prefixed_path.prepend(p.first);
                    result.insert(Pair<ConfigurationPropertyPath,List<int>>(prefixed_path,entry.second));
                }
            }
        }
    }
    return result;
}

template<class T> ConfigurationPropertyInterface* InterfaceListConfigurationProperty<T>::clone() const {
    List<shared_ptr<T>> values;
    for (auto const& ptr : _values) values.push_back(shared_ptr<T>(ptr->clone()));
    return new InterfaceListConfigurationProperty(values);
}

template<class T> ConfigurationPropertyInterface* InterfaceListConfigurationProperty<T>::at(ConfigurationPropertyPath const& path) {
    if (path.is_root()) return this;
    else {
        HELPER_ASSERT_MSG(is_configurable(),"The object held is not configurable, path error.");
        HELPER_ASSERT_MSG(is_single(),"Cannot retrieve properties if the list has multiple objects.");
        auto configurable_ptr = dynamic_cast<ConfigurableInterface*>(_values.back().get());
        auto properties = configurable_ptr->searchable_configuration().properties();
        auto prop_ptr = properties.find(path.first());
        HELPER_ASSERT_MSG(prop_ptr != properties.end(),"The property '" << path.first() << "' was not found in the configuration.");
        return prop_ptr->second->at(path.subpath());
    }
}

template<class T> T const& InterfaceListConfigurationProperty<T>::get() const {
    HELPER_PRECONDITION(this->is_specified());
    HELPER_ASSERT_MSG(this->is_single(),"The property should have a single value when actually used. Are you accessing it outside the related task?");
    return *_values.back();
}

template<class T> void InterfaceListConfigurationProperty<T>::set(T const& value) {
    this->set_specified();
    _values.clear();
    _values.push_back(shared_ptr<T>(value.clone()));
}

template<class T> void InterfaceListConfigurationProperty<T>::set(shared_ptr<T> const& value) {
    HELPER_PRECONDITION(value != nullptr);
    this->set_specified();
    _values.clear();
    _values.push_back(value);
}

template<class T> void InterfaceListConfigurationProperty<T>::set(List<shared_ptr<T>> const& values) {
    HELPER_PRECONDITION(not values.empty());
    this->set_specified();
    _values = values;
}

template<class T> List<shared_ptr<T>> InterfaceListConfigurationProperty<T>::values() const {
    return _values;
}

} // namespace ProNest


#endif // PRONEST_CONFIGURATION_PROPERTY_TPL_HPP