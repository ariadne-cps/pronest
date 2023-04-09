/***************************************************************************
 *            configuration_search_point.hpp
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

/*! \file configuration/configuration_search_point.hpp
 *  \brief Class for handling search points of configuration properties.
 */

#ifndef PRONEST_CONFIGURATION_SEARCH_POINT_HPP
#define PRONEST_CONFIGURATION_SEARCH_POINT_HPP

#include "configuration_search_parameter.hpp"
#include "configuration_search_space.hpp"
#include "configurable.hpp"

using namespace Utility;

namespace ProNest {

using ParameterBindingsMap = Map<ConfigurationPropertyPath,int>;

class ConfigurationSearchPoint {
    friend class ConfigurationSearchSpace;
  protected:
    ConfigurationSearchPoint(ConfigurationSearchSpace const& space, ParameterBindingsMap const& bindings);
  public:
    ConfigurationSearchPoint(ConfigurationSearchPoint const& p);
    ~ConfigurationSearchPoint() = default;

    //! \brief The parameter space
    ConfigurationSearchSpace const& space() const;

    //! \brief The coordinates in the natural space, according to the space ordering
    List<int> coordinates() const;

    //! \brief Generate a point adjacent to this one by shifting one parameter
    ConfigurationSearchPoint make_adjacent_shifted() const;
    //! \brief Generate an \a amount of points by shifting one parameter each from the current point,
    //! then the next point to shift from is a random one from those already generated
    //! \details Guarantees that all points are different. Includes the original point.
    //! If \a amount is 1, no new point is generated.
    Set<ConfigurationSearchPoint> make_random_shifted(Nat amount) const;

    ParameterBindingsMap const& bindings() const;

    //! \brief The value of the point for the given parameter path \a path
    int value(ConfigurationPropertyPath const& path) const;
    //! \brief The index in the space for the parameter identifier \a path
    SizeType index(ConfigurationPropertyPath const& path) const;
    //! \brief The parameter corresponding to the identifier \a path
    ConfigurationSearchParameter const& parameter(ConfigurationPropertyPath const& path) const;

    ConfigurationSearchPoint& operator=(ConfigurationSearchPoint const& p);
    //! \brief Equality check is performed under the assumption that we always work with the same parameters,
    //! hence no space check is performed.
    bool operator==(ConfigurationSearchPoint const& p) const;
    //! \brief Ordering is based on point value
    bool operator<(ConfigurationSearchPoint const& p) const;
    //! \brief The distance with respect to another point
    //! \details Distance between values for non-metric parameters is either 1 or 0
    Nat distance(ConfigurationSearchPoint const& p) const;

    //! \brief Compute the breadth of possible shifts of the point for each parameter
    List<Nat> shift_breadths() const;

    friend OutputStream& operator<<(OutputStream& os, ConfigurationSearchPoint const& point);
  private:

    std::shared_ptr<ConfigurationSearchSpace> _space;
    ParameterBindingsMap _bindings;

    mutable List<Nat> _CACHED_SHIFT_BREADTHS;
};

//! \brief Generate an \a amount of new points from \a sources, by shifting one parameter each (ideally, see details)
//! \return The original points plus the shifted ones
//! \details \a size must be greater or equal than \a sources size but still lower than the maximum number of points
//! for the space. Shifting points are chosen by rotation, skipping to the next if the generated point is not new-
//! An effort is made to shift only by 1 with respect to the sources, but if not possible then the generated
//! points are added to the points used for shifting.
Set<ConfigurationSearchPoint> make_extended_set_by_shifting(Set<ConfigurationSearchPoint> const& sources, SizeType size);

//! \brief Make a configuration from another configuration \a cfg and a point \a p in the search space
template<class C> Configuration<C> make_singleton(Configuration<C> const& cfg, ConfigurationSearchPoint const& p) {
    UTILITY_PRECONDITION(not cfg.is_singleton());
    Configuration<C> result = cfg;
    for (auto param : p.space().parameters()) {
        auto prop_ptr = result.properties().find(param.path().first());
        UTILITY_ASSERT_MSG(prop_ptr != cfg.properties().end(), "The ConfigurationSearchPoint parameter '" << param.path() << "' is not in the configuration.");
        prop_ptr->second->set_single(param.path().subpath(),p.value(param.path()));
    }
    UTILITY_ASSERT_MSG(result.is_singleton(),"There are missing parameters in the search point, since the configuration could not be made singleton.");
    return result;
}

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_SEARCH_POINT_HPP
