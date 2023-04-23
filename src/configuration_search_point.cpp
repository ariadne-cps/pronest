/***************************************************************************
 *            configuration_search_point.cpp
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

#include "configuration_search_point.hpp"
#include "configuration_search_space.hpp"
#include "utility/randomiser.hpp"

namespace ProNest {

using Utility::UniformIntRandomiser;

ConfigurationSearchPoint::ConfigurationSearchPoint(ConfigurationSearchSpace const& space, ParameterBindingsMap const& bindings)
    : _space(space.clone()), _bindings(bindings) { }

ConfigurationSearchPoint::ConfigurationSearchPoint(ConfigurationSearchPoint const& p) {
    this->_bindings.clear();
    this->_bindings.adjoin(p._bindings);
    this->_CACHED_SHIFT_BREADTHS = p._CACHED_SHIFT_BREADTHS;
    this->_space.reset(p.space().clone());
}

Set<ConfigurationSearchPoint> ConfigurationSearchPoint::make_random_shifted(unsigned int amount) const {
    Set<ConfigurationSearchPoint> result;
    ConfigurationSearchPoint current_point = *this;
    result.insert(current_point);
    while (result.size() < amount) {
        List<unsigned int> breadths = current_point.shift_breadths();
        unsigned int total_breadth = 0; for (auto const& b : breadths) total_breadth += b;
        auto space = this->space();

        unsigned int offset = UniformIntRandomiser<unsigned int>(0,total_breadth-1).get();
        unsigned int current_breadth = 0;
        ParameterBindingsMap shifted_bindings;
        bool shifted = false;
        for (auto const& binding : current_point.bindings()) {
            auto const& param = current_point.parameter(binding.first);
            int value = binding.second;
            current_breadth += breadths.at(space.index(param));
            if (not shifted and current_breadth > offset) {
                value = param.shifted_value_from(value);
                shifted = true;
            }
            shifted_bindings.insert(std::pair<ConfigurationPropertyPath,int>(param.path(), value));
        }
        result.insert(space.make_point(shifted_bindings));

        unsigned int new_choice = UniformIntRandomiser<unsigned int>(0,result.size()-1).get();
        auto iter = result.begin();
        for (unsigned int i=0; i<new_choice; ++i) ++iter;
        current_point = *iter;
    }
    return result;
}

ConfigurationSearchPoint ConfigurationSearchPoint::make_adjacent_shifted() const {
    List<unsigned int> breadths = this->shift_breadths();
    unsigned int total_breadth = 0;
    for (auto const& b : breadths) total_breadth += b;
    UTILITY_PRECONDITION(total_breadth != 0);
    Set<ConfigurationSearchPoint> result;
    auto space = this->space();

    unsigned int offset = UniformIntRandomiser<unsigned int>(0,total_breadth-1).get();

    unsigned int current_breadth = 0;
    ParameterBindingsMap shifted_bindings;
    bool shifted = false;
    for (auto const& binding : _bindings) {
        auto const &param = parameter(binding.first);
        int value = binding.second;
        current_breadth += breadths.at(space.index(param));
        if (not shifted and current_breadth > offset) {
            value = param.shifted_value_from(value);
            shifted = true;
        }
        shifted_bindings.insert(std::pair<ConfigurationPropertyPath, int>(param.path(), value));
    }
    return space.make_point(shifted_bindings);
}

ConfigurationSearchSpace const& ConfigurationSearchPoint::space() const {
    return *_space;
}

List<int> ConfigurationSearchPoint::coordinates() const {
    return _bindings.values();
}

ParameterBindingsMap const& ConfigurationSearchPoint::bindings() const {
    return _bindings;
}

int ConfigurationSearchPoint::value(ConfigurationPropertyPath const& path) const {
    return _bindings.at(path);
}

size_t ConfigurationSearchPoint::index(ConfigurationPropertyPath const& path) const {
    return _space->index(path);
}

ConfigurationSearchParameter const& ConfigurationSearchPoint::parameter(ConfigurationPropertyPath const& path) const {
    return _space->parameter(path);
}

ConfigurationSearchPoint& ConfigurationSearchPoint::operator=(ConfigurationSearchPoint const& p) {
    this->_bindings.clear();
    this->_bindings.adjoin(p._bindings);
    this->_CACHED_SHIFT_BREADTHS = p._CACHED_SHIFT_BREADTHS;
    this->_space.reset(p.space().clone());
    return *this;
}

bool ConfigurationSearchPoint::operator==(ConfigurationSearchPoint const& p) const {
    for (auto const& b : _bindings)
        if (p._bindings.at(b.first) != b.second)
            return false;

    return true;
}

bool ConfigurationSearchPoint::operator<(ConfigurationSearchPoint const& p) const {
    for (auto const& b : _bindings) {
        auto const this_value = b.second;
        auto const other_value = p._bindings.at(b.first);
        if (this_value < other_value) return true;
        else if (this_value > other_value) return false;
    }
    return false; // They are equal
}

unsigned int ConfigurationSearchPoint::distance(ConfigurationSearchPoint const& p) const {
    unsigned int result = 0;
    for (auto const& b : _bindings) {
        auto const& param = parameter(b.first);
        auto const v1 = b.second;
        auto const v2 = p._bindings.at(param.path());
        if (param.is_metric()) result += (v1 > v2 ? (unsigned int)(v1 - v2) : (unsigned int)(v2 - v1));
        else result += (v1 == v2 ? 0 : 1);
    }
    return result;
}

ostream& operator<<(ostream& os, ConfigurationSearchPoint const& point) {
    return os << point._bindings.values();
}

List<unsigned int> ConfigurationSearchPoint::shift_breadths() const {
    if (_CACHED_SHIFT_BREADTHS.empty()) {
        for (auto const& b : _bindings) {
            auto const& param = parameter(b.first);
            auto const& values = param.values();
            auto size = values.size();
            if (not param.is_metric()) _CACHED_SHIFT_BREADTHS.push_back(static_cast<unsigned int>(size-1)); // all except the current
            else if (b.second == values[size-1] or b.second == values[size-1]) _CACHED_SHIFT_BREADTHS.push_back(1); // can only move down or up
            else _CACHED_SHIFT_BREADTHS.push_back(2); // can move either up or down
        }
    }
    return _CACHED_SHIFT_BREADTHS;
}

Set<ConfigurationSearchPoint> make_extended_set_by_shifting(Set<ConfigurationSearchPoint> const& sources, size_t size) {
    UTILITY_PRECONDITION(size>=sources.size());
    UTILITY_PRECONDITION(sources.begin()->space().total_points() >= size);
    auto expanded_sources = sources; // To be expanded if the previous sources are incapable of getting the required size
    auto result = sources;

    while (result.size() < size) {
        auto source_it = expanded_sources.begin();
        size_t previous_size = result.size();
        while (result.size() < size) {
            result.insert(source_it->make_adjacent_shifted());
            ++source_it; // Will move to next source even if no shift has been found
            if (source_it == expanded_sources.end()) break;
        }
        // At the end of a cycle, if no increment of points has been obtained, add the current results to the sources
        if (result.size() == previous_size) expanded_sources.adjoin(result);
    }
    return result;
}

} // namespace ProNest
