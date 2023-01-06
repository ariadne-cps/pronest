/***************************************************************************
 *            configuration_property_path.hpp
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

/*! \file concurrency/configuration_property_path.hpp
 *  \brief A path across a hierarchic configuration object.
 */

#ifndef PRONEST_CONFIGURATION_PROPERTY_PATH_HPP
#define PRONEST_CONFIGURATION_PROPERTY_PATH_HPP

#include <utility>
#include <deque>
#include "declarations.hpp"

namespace ProNest {

class String;

class ConfigurationPropertyPath {
  public:
    ConfigurationPropertyPath() = default;
    ConfigurationPropertyPath(String const& first);
    ConfigurationPropertyPath(ConfigurationPropertyPath const& path);
    ConfigurationPropertyPath& operator=(ConfigurationPropertyPath const& path);
    Bool operator==(ConfigurationPropertyPath const& path) const;
    Bool operator<(ConfigurationPropertyPath const& path) const;

    String repr() const;

    Bool is_root() const;
    ConfigurationPropertyPath& append(String const& node);
    ConfigurationPropertyPath& prepend(String const& node);
    //! \brief Return the first level of the path
    String first() const;
    //! \brief Return the last level of the path
    String last() const;
    //! \brief Return everything but the first level of the path
    ConfigurationPropertyPath subpath() const;

    friend OutputStream& operator<<(OutputStream& os, ConfigurationPropertyPath const& path);
  private:
    std::deque<String> _path;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_PROPERTY_PATH_HPP
