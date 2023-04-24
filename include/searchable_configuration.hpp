/***************************************************************************
 *            searchable_configuration.hpp
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

/*! \file configuration/searchable_configuration.hpp
 *  \brief Classes for configuration and related helpers.
 */

#ifndef PRONEST_SEARCHABLE_CONFIGURATION_HPP
#define PRONEST_SEARCHABLE_CONFIGURATION_HPP

#include <ostream>
#include <type_traits>
#include "helper/macros.hpp"
#include "configuration_interface.hpp"
#include "configuration_property_interface.hpp"
#include "configuration_property_path.hpp"

namespace ProNest {

class ConfigurationSearchSpace;

//! \brief Extension of ConfigurationInterface to deal with search in the properties space
class SearchableConfiguration : public ConfigurationInterface {
  public:
    SearchableConfiguration() = default;
    SearchableConfiguration(SearchableConfiguration const& c);
    SearchableConfiguration& operator=(SearchableConfiguration const& c);
    virtual ~SearchableConfiguration() = default;

    //! \brief Construct a search space from the current configuration
    ConfigurationSearchSpace search_space() const;

    //! \brief If the configuration is made of single values
    bool is_singleton() const;

    Map<String,std::shared_ptr<ConfigurationPropertyInterface>>& properties();
    Map<String,std::shared_ptr<ConfigurationPropertyInterface>> const& properties() const;

    //! \brief Accessors for get and set of a property identified by a path \a path with type \a P
    //! \details Used in practice to get/set properties for verification
    template<class P> P& at(ConfigurationPropertyPath const& path) {
        auto prop_ptr = _properties.find(path.first());
        HELPER_ASSERT_MSG(prop_ptr != _properties.end(),"The property '" << path.first() << "' was not found in the configuration.");
        auto p_ptr = dynamic_cast<P*>(prop_ptr->second->at(path.subpath()));
        HELPER_ASSERT_MSG(p_ptr != nullptr, "Invalid property cast, check the property class with respect to the configuration created.")
        return *p_ptr;
    }
    template<class P> P const& at(ConfigurationPropertyPath const& path) const {
        auto prop_ptr = _properties.find(path.first());
        HELPER_ASSERT_MSG(prop_ptr != _properties.end(),"The property '" << path.first() << "' was not found in the configuration.");
        auto p_ptr = dynamic_cast<P*>(prop_ptr->second->at(path.subpath()));
        HELPER_ASSERT_MSG(p_ptr != nullptr, "Invalid property cast, check the property class with respect to the configuration created.")
        return *p_ptr;
    }
    //! \brief Easier accessors for get and set starting from an \a identifier, to be used for Configuration class accessors
    template<class P> P& at(String const& identifier) {
        return at<P>(ConfigurationPropertyPath(identifier));
    }
    template<class P> P const& at(String const& identifier) const {
        return at<P>(ConfigurationPropertyPath(identifier));
    }

    //! \brief Add a property to the configuration
    void add_property(String const& name, ConfigurationPropertyInterface const& property);

    ostream& _write(ostream& os) const override;
  private:
    Map<String,std::shared_ptr<ConfigurationPropertyInterface>> _properties;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_HPP
