/***************************************************************************
 *            configuration_interface.hpp
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

/*! \file configuration_interface.hpp
 *  \brief Interface for basic configurations.
 */

#ifndef PRONEST_CONFIGURATION_HPP
#define PRONEST_CONFIGURATION_HPP

#include <ostream>
#include <type_traits>
#include "helper/writable.hpp"

namespace ProNest {

using std::ostream;
using Helper::WritableInterface;

/*! \brief Configuration altering the model of a class.
 *
 *  \details A configuration allows controlled change in the behavior of a class.
 *  Under a model-based viewpoint, the API of a "main class" reflects the model; such model can internally be made
 *  of various components offering specific functionality. Its configuration provides a
 *  degree of freedom in the actual behavior of the API. The configuration must not be confused with components,
 *  that provide functions, or state variables that preserve some information during the lifetime of one or more API calls.
 *
 *  The only exposed method of the interface relates to writing to an output stream. In addition, the assignment operator
 *  is explicitly private to stress the fact that assignment of a specific configuration is not allowed. This is as far as
 *  the language can enforce. In the following, the guidelines for a consistent implementation of configuration classes
 *  are provided.
 *
 *  Each class (here called "main class") with some traits should have its own configuration class as a private shared pointer
 *  field with a getter and setter (where no assignment of the configuration is allowed). In order to have safe access to the main
 *  class at construction time, the configuration field should be the last to be constructed (more on that below).
 *
 *  A configuration class manages some "properties", i.e. fields with a getter and possibly a setter.
 *  Properties should not be modified by the main class itself. If that were the case, then the properties would become
 *  state variables of the class. A main class must have its configuration as a non-mutable object to suggest this constraint.
 *  If a main class has components with their own configuration, but such components are not publicly exposed, then the
 *  configuration of the main class is responsible for updating the configurations of the components, if necessary.
 *
 *  Each property field is private and annotated with its documentation:
 *
 *  //! \brief Documentation here.
 *  PropertyType _property_name;
 *
 *  Public methods are available to read (get) and write (set) the property "property_name", with the following signatures:
 *
 *  const PropertyType& property_name() const;
 *  Void set_property_name(const PropertyType value);
 *
 *  Please note that the argument to the write method must be passed by value for clear responsibility and simplicity of
 *  manually setting the property. If efficiency demands not to perform any copy, then the PropertyType must be a std::shared_ptr
 *  object, in order to properly increment the reference count if necessary; to make this situation clearer, the field name must
 *  be appended with _ptr, and the argument to the write method must be called value_ptr. If the property is also optional,
 *  it is read as a std::shared_ptr type; as a consequence, the read/write method names must be appended with _ptr too.
 *
 *  In the simplest case of the read and write methods strictly getting the field and setting the field to the argument value,
 *  the code may reside in the header file, for simplicity. If on the other hand the implementation is more complex,
 *  it should reside in the source file of the main class. Please note that a read-only property should still
 *  have a write method, the latter being private: this approach allows to hide the actual manipulation required to
 *  update the field. This is a good practice that protects from later modifications to the write method.
 *
 *  In order to have the main class control the construction of the configuration class, the main class constructor
 *  must not accept a configuration object as argument. The only significant way to construct a configuration class is
 *  inside the constructor of the main class itself. The configuration constructor must call the write methods in its body,
 *  instead of initialising the fields directly. This actual double initialisation has a cost, but it helps enforce integrity
 *  between properties. For complex objects or objects with no default constructor, an underlying pointer class field
 *  is to be used instead.
 *
 *  Advanced use cases:
 *  a) On-the-fly property construction: a property has no underlying field, thus being a
 *  derived property. Such property must not have a write method.
 *  b) Simple dependencies (no cyclic dependency is allowed) between properties: one or more properties are updated
 *  as soon as a property changes; consequently one write method triggers the modification of several properties.
 *  Ordering of write calls is necessarily the responsibility of the designer of the class. Properties must then
 *  be constructed respecting dependencies, so to keep consistency.
 *  c) Component dependencies: if the main class has a component whose configuration (being it a Configuration class or some
 *  fields) depends on the value of one property, then the configuration class must have a plain back-reference to the main
 *  class, which is necessarily set at construction. If necessary due to access restrictions, the configuration class must
 *  be added as friend to the main class.
 *  d) Configuration inheritance: the configuration field is introduced at the root abstract class, but the actual
 *  (concrete) configuration is constructed at the concrete main class. Each configuration introduces its own properties and
 *  interacts with the depending components of the corresponding main class. Non-root main classes have to dynamic_cast the
 *  root Configuration object to retrieve their specific Configuration.
 *
 */
class ConfigurationInterface : public WritableInterface {
  private:
    const ConfigurationInterface& operator=(const ConfigurationInterface& other) = delete;
  public:
    virtual ~ConfigurationInterface() = default;
    virtual ostream& _write(ostream& os) const = 0;
    friend ostream& operator<<(ostream& os, const ConfigurationInterface& config) { return config._write(os); }
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_HPP
