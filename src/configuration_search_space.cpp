/***************************************************************************
 *            configuration_search_space.cpp
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

#include "configuration_property_path.hpp"
#include "configuration_search_point.hpp"
#include "configuration_search_space.hpp"

namespace ProNest {

using Helper::Pair;

ConfigurationSearchSpace::ConfigurationSearchSpace(Set<ConfigurationSearchParameter> const& parameters) {
    //HELPER_PRECONDITION(not parameters.empty());
    for (auto const& p : parameters)
        _parameters.push_back(p);
}

ConfigurationSearchPoint ConfigurationSearchSpace::make_point(ParameterBindingsMap const& bindings) const {
    HELPER_PRECONDITION(bindings.size() == this->dimension())
    ParameterBindingsMap pb;
    for (auto const& p : _parameters) {
        int v = bindings.find(p.path())->second;
        pb.insert(Pair<ConfigurationPropertyPath,int>(p.path(), v));
    }
    return {*this, pb};
}

ConfigurationSearchPoint ConfigurationSearchSpace::initial_point() const {
    ParameterBindingsMap pb;
    for (auto const& p : _parameters) {
        pb.insert(Pair<ConfigurationPropertyPath,int>(p.path(), p.random_value()));
    }
    return {*this, pb};
}

size_t ConfigurationSearchSpace::index(ConfigurationSearchParameter const& p) const {
    for (size_t i=0; i<_parameters.size(); ++i) if (_parameters.at(i) == p) return i;
    HELPER_FAIL_MSG("Task parameter '" << p << "' not found in the space.");
}

size_t ConfigurationSearchSpace::index(ConfigurationPropertyPath const& path) const {
    for (size_t i=0; i<_parameters.size(); ++i) if (_parameters.at(i).path() == path) return i;
    HELPER_FAIL_MSG("Task parameter with path '" << path << "' not found in the space.");
}

ConfigurationSearchParameter const& ConfigurationSearchSpace::parameter(ConfigurationPropertyPath const& path) const {
    for (auto const& p : _parameters) if (p.path() == path) return p;
    HELPER_FAIL_MSG("Task parameter with path '" << path << "' not found in the space.");
}

List<ConfigurationSearchParameter> const& ConfigurationSearchSpace::parameters() const {
    return _parameters;
}

size_t ConfigurationSearchSpace::total_points() const {
    size_t result = 1;
    for (auto const& p : _parameters) result *= p.values().size();
    return result;
}

size_t ConfigurationSearchSpace::dimension() const {
    return _parameters.size();
}

ConfigurationSearchSpace* ConfigurationSearchSpace::clone() const {
    return new ConfigurationSearchSpace(*this);
}

ostream& operator<<(ostream& os, ConfigurationSearchSpace const& space) {
    os << "[";
    if (space._parameters.size()>0) {
        for (size_t i = 0; i < space._parameters.size() - 1; ++i) os << space._parameters[i] << ",";
        os << space._parameters[space._parameters.size() - 1];
    }
    os << "]";
    return os;
}

} // namespace ProNest
