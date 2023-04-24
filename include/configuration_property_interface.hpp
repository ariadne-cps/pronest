/***************************************************************************
 *            configuration_property_interface.hpp
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

/*! \file concurrency/configuration_property_interface.hpp
 *  \brief Interface for properties of a configuration.
 */

#ifndef PRONEST_CONFIGURATION_PROPERTY_INTERFACE_HPP
#define PRONEST_CONFIGURATION_PROPERTY_INTERFACE_HPP

#include "helper/writable.hpp"
#include "helper/container.hpp"

namespace ProNest {

using Helper::List;
using Helper::Map;
using Helper::WritableInterface;

class ConfigurationPropertyPath;

class ConfigurationPropertyInterface : public WritableInterface {
  public:
    //! \brief If only one value is specified
    virtual bool is_single() const = 0;
    //! \brief If values are specified at all
    virtual bool is_specified() const = 0;
    //! \brief If the property class at the \a path is metric
    virtual bool is_metric(ConfigurationPropertyPath const& path) const = 0;
    //! \brief If the property object is a configurable itself
    virtual bool is_configurable() const = 0;
    //! \brief The number of values stored for the property
    //! \details Returns 1 if single, 0 if not specified.
    virtual size_t cardinality() const = 0;
    //! \brief Set to a single value a given path, starting from this property
    //! \details Supports the storage of objects that are Configurable themselves
    virtual void set_single(ConfigurationPropertyPath const& path, int integer_value) = 0;
    //! \brief The integer values for each property including the current one
    //! \details Supports the storage of objects that are Configurable themselves
    virtual Map<ConfigurationPropertyPath,List<int>> integer_values() const = 0;
    //! \brief Retrieve a pointer to the property at the given \a path
    virtual ConfigurationPropertyInterface* at(ConfigurationPropertyPath const& path) = 0;

    virtual ConfigurationPropertyInterface* clone() const = 0;
    virtual ~ConfigurationPropertyInterface() = default;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_PROPERTY_INTERFACE_HPP
