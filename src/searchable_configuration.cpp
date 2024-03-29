/***************************************************************************
 *            searchable_configuration.cpp
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

#include "helper/container.hpp"
#include "searchable_configuration.hpp"
#include "configuration_search_space.hpp"

namespace ProNest {

using Helper::Pair;
using std::shared_ptr;

SearchableConfiguration::SearchableConfiguration(SearchableConfiguration const& c) {
    for (auto const& p : c.properties()) _properties.insert(Pair<String,shared_ptr<ConfigurationPropertyInterface>>(p.first,shared_ptr<ConfigurationPropertyInterface>(p.second->clone())));
}

SearchableConfiguration& SearchableConfiguration::operator=(SearchableConfiguration const& c) {
    _properties.clear();
    for (auto const& p : c.properties()) _properties.insert(Pair<String,shared_ptr<ConfigurationPropertyInterface>>(p.first,shared_ptr<ConfigurationPropertyInterface>(p.second->clone())));
    return *this;
}

Map<String,shared_ptr<ConfigurationPropertyInterface>>& SearchableConfiguration::properties() {
    return _properties;
}

Map<String,shared_ptr<ConfigurationPropertyInterface>> const& SearchableConfiguration::properties() const {
    return _properties;
}

void SearchableConfiguration::add_property(String const& name, ConfigurationPropertyInterface const& property) {
    _properties.insert(Pair<String,shared_ptr<ConfigurationPropertyInterface>>({name,shared_ptr<ConfigurationPropertyInterface>(property.clone())}));
}

ostream& SearchableConfiguration::_write(ostream& os) const {
    os << "(\n";
    auto iter = _properties.begin(); size_t i=0;
    while (i<_properties.size()-1) {
        os << iter->first << " = " << *iter->second << ",\n";
        ++iter; ++i;
    }
    os << iter->first << " = " << *iter->second << ")"; return os;
}

bool SearchableConfiguration::is_singleton() const {
    for (auto const& p : _properties) {
        auto int_values = p.second->integer_values();
        for (auto const& p_int : int_values) if (p_int.second.size() > 1) return false;
    }
    return true;
}

ConfigurationSearchSpace SearchableConfiguration::search_space() const {
    Set<ConfigurationSearchParameter> result;
    for (auto const& p : _properties) {
        auto integer_values = p.second->integer_values();
        for (auto const& p_int : integer_values) {
            if (p_int.second.size() > 1) {
                ConfigurationPropertyPath path(p_int.first);
                path.prepend(p.first);
                result.insert(ConfigurationSearchParameter(path, p.second->is_metric(p_int.first), p_int.second));
            }
        }
    }
    //HELPER_ASSERT_MSG(not result.empty(),"The search space is empty.");
    return result;
}

} // namespace ProNest
