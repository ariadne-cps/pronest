/***************************************************************************
 *            configuration_search_parameter.hpp
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

/*! \file configuration/configuration_search_parameter.hpp
 *  \brief Classes for handling configuration search parameters.
 */

#ifndef PRONEST_CONFIGURATION_SEARCH_PARAMETER_HPP
#define PRONEST_CONFIGURATION_SEARCH_PARAMETER_HPP

#include "declarations.hpp"
#include "container.hpp"
#include "macros.hpp"
#include "configuration_property_path.hpp"

namespace ProNest {

class ConfigurationSearchParameter {
  public:
    ConfigurationSearchParameter(ConfigurationPropertyPath const& path, bool is_metric, List<int> const& values);
    ConfigurationPropertyPath const& path() const;
    //! \brief Admissible values
    List<int> const& values() const;
    //! \brief Whether the parameter should shift to adjacent values instead of hopping between values
    bool is_metric() const;
    //! \brief Generate a random value, useful for the initial value
    int random_value() const;
    //! \brief Randomly get the result from shifting the given \a value
    int shifted_value_from(int value) const;

    bool operator==(ConfigurationSearchParameter const& p) const;
    bool operator<(ConfigurationSearchParameter const& p) const;

    friend OutputStream& operator<<(OutputStream& os, ConfigurationSearchParameter const& parameter);

  private:
    const ConfigurationPropertyPath _path;
    const bool _is_metric;
    const List<int> _values;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_SEARCH_PARAMETER_HPP
