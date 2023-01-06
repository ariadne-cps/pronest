/***************************************************************************
 *            configuration_search_space.hpp
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

/*! \file configuration_search_space.hpp
 *  \brief Class for handling a search space of configuration properties.
 */

#ifndef PRONEST_CONFIGURATION_SEARCH_SPACE_HPP
#define PRONEST_CONFIGURATION_SEARCH_SPACE_HPP

#include "configuration_search_parameter.hpp"

namespace ProNest {

class ConfigurationSearchPoint;
class Real;
template<class R> class Variable;

using ParameterBindingsMap = Map<ConfigurationPropertyPath,int>;

class ConfigurationSearchSpace {
  public:
    ConfigurationSearchSpace(Set<ConfigurationSearchParameter> const& parameters);

    ConfigurationSearchPoint make_point(ParameterBindingsMap const& bindings) const;
    ConfigurationSearchPoint initial_point() const;

    List<ConfigurationSearchParameter> const& parameters() const;

    //! \brief The total number of points identified by the space
    SizeType total_points() const;
    //! \brief The number of parameters in the space
    SizeType dimension() const;
    //! \brief The index of the given parameter in the ordered space
    SizeType index(ConfigurationSearchParameter const& p) const;
    //! \brief The index of the given parameter identifier in the ordered space
    SizeType index(ConfigurationPropertyPath const& name) const;
    //! \brief The parameter corresponding to the path \a path
    ConfigurationSearchParameter const& parameter(ConfigurationPropertyPath const& path) const;

    ConfigurationSearchSpace* clone() const;

    friend OutputStream& operator<<(OutputStream& os, ConfigurationSearchSpace const& space);

  private:
    List<ConfigurationSearchParameter> _parameters;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_SEARCH_SPACE_HPP
