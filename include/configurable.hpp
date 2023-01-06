/***************************************************************************
 *            configurable.hpp
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

/*! \file configurable.hpp
 *  \brief Classes for configurable tasks.
 *  \details If a class C needs a configuration, then it:
 *  1) must derive from Configurable<C>
 *  2) a Configuration<C> must be specified, which derives from SearchableConfiguration
 *  3) C constructors must take a Configuration<C> object
 *  For a base class B and one of its derivations D, D derives from B as usual, while the configuration design is a bit more complicated:
 *  1) B follows the design for a configurable class as described above
 *  2) Configuration<D> derives from Configuration<B>, with get/set accessors for Configuration<B> also, since Configuration<B> can't be passed
 *     (you could make Configuration<B> constructors protected, if you prefer)
 *  3) D needs its own configuration() override, which static casts B::configuration() onto Configuration<D>
 */

#ifndef PRONEST_CONFIGURABLE_HPP
#define PRONEST_CONFIGURABLE_HPP

#include "declarations.hpp"

namespace ProNest {

class SearchableConfiguration;

class ConfigurableInterface {
  public:
    virtual SearchableConfiguration const& searchable_configuration() const = 0;
};

//! \brief Base template class to be specialised while deriving from SearchableConfigurationInterface
template<class C> struct Configuration;

//! \brief Is-a component that provides a configuration
//! \details Since the configuration returned is const, a Configurable object should be constructed from
//! a pre-set configuration. If the configuration must specify certain properties or if some properties
//! must be coherent with the Configurable (e.g., the system used by a Configurable evolver), then a Builder
//! approach should be used for creation of the configuration, which should become an immutable object.
template<class C> class Configurable : public ConfigurableInterface {
    friend struct Configuration<C>;
  public:
    Configurable(Configuration<C> const& config);
    Configuration<C> const& configuration() const;
    SearchableConfiguration const& searchable_configuration() const override;
  private:
    SharedPointer<Configuration<C>> _configuration;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_HPP
