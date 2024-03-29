/***************************************************************************
 *            configurable.tpl.hpp
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

#ifndef PRONEST_CONFIGURABLE_TPL_HPP
#define PRONEST_CONFIGURABLE_TPL_HPP

#include "helper/macros.hpp"
#include "configurable.hpp"

namespace ProNest {

template<class C> Configurable<C>::Configurable(Configuration<C> const& config) : _configuration(new Configuration<C>(config)) { }

template<class C> Configuration<C> const& Configurable<C>::configuration() const {
    return *_configuration;
}

template<class C> SearchableConfiguration const& Configurable<C>::searchable_configuration() const {
    return dynamic_cast<SearchableConfiguration const &>(*_configuration);
}

} // namespace ProNest

#endif // PRONEST_CONFIGURABLE_TPL_HPP
