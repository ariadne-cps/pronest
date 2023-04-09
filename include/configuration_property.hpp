/***************************************************************************
 *            configuration_property.hpp
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

/*! \file configuration_property.hpp
 *  \brief Classes for properties of a configuration.
 */

#ifndef PRONEST_CONFIGURATION_PROPERTY_HPP
#define PRONEST_CONFIGURATION_PROPERTY_HPP

#include <ostream>
#include <type_traits>
#include "utility/macros.hpp"
#include "utility/container.hpp"
#include "utility/handle.hpp"
#include "configuration_interface.hpp"
#include "configuration_property_interface.hpp"
#include "configuration_search_space_converter.hpp"

using namespace Utility;

namespace ProNest {

template<class T> class ConfigurationPropertyBase : public ConfigurationPropertyInterface {
  protected:
    ConfigurationPropertyBase(bool const& is_specified);
    void set_specified();
    virtual List<shared_ptr<T>> values() const = 0;
    virtual void local_set_single(int integer_value) = 0;
    virtual List<int> local_integer_values() const = 0;
  public:
    virtual T const& get() const = 0;
    virtual void set(T const& value) = 0;

    bool is_specified() const override;
    Map<ConfigurationPropertyPath,List<int>> integer_values() const override;

    //! \brief Supplies the values from the property, empty if not specified, the lower/upper bounds if a range
    OutputStream& _write(OutputStream& os) const override;
  private:
    bool _is_specified;
};

//! \brief A property for a boolean value.
class BooleanConfigurationProperty final : public ConfigurationPropertyBase<bool> {
  public:
    BooleanConfigurationProperty();
    BooleanConfigurationProperty(bool const& value);

    bool is_single() const override;
    bool is_metric(ConfigurationPropertyPath const& path) const override;
    bool is_configurable() const override;
    size_t cardinality() const override;

    ConfigurationPropertyInterface* clone() const override;

    ConfigurationPropertyInterface* at(ConfigurationPropertyPath const& path) override;

    bool const& get() const override;
    void set(bool const& value) override;
    void set_both(); //! \brief Set to both true and false
    void set_single(ConfigurationPropertyPath const& path, int integer_value) override;
  protected:
    void local_set_single(int integer_value) override;
    List<int> local_integer_values() const override;
    List<shared_ptr<bool>> values() const override;
  private:
    bool _is_single;
    bool _value;
};

//! \brief A range configuration property offers a range of values with a distance metric
//! \details This property needs a converter to decide how to distribute the integer values in the search space.
template<class T> class RangeConfigurationProperty final : public ConfigurationPropertyBase<T> {
  public:
    RangeConfigurationProperty(ConfigurationSearchSpaceConverterInterface<T> const& converter = LinearSearchSpaceConverter<T>());
    RangeConfigurationProperty(T const& lower, T const& upper, ConfigurationSearchSpaceConverterInterface<T> const& converter = LinearSearchSpaceConverter<T>());
    RangeConfigurationProperty(T const& value, ConfigurationSearchSpaceConverterInterface<T> const& converter = LinearSearchSpaceConverter<T>());

    bool is_single() const override;
    bool is_metric(ConfigurationPropertyPath const& path) const override;
    bool is_configurable() const override;
    size_t cardinality() const override;

    ConfigurationPropertyInterface* clone() const override;

    ConfigurationPropertyInterface* at(ConfigurationPropertyPath const& path) override;

    T const& get() const override;
    void set(T const& lower, T const& upper);
    //! \brief Set a single value
    //! \details An unbounded single value is accepted
    void set(T const& value) override;
    void set_single(ConfigurationPropertyPath const& path, int integer_value) override;
  protected:
    void local_set_single(int integer_value) override;
    List<int> local_integer_values() const override;
    List<shared_ptr<T>> values() const override;
  private:
    T _lower;
    T _upper;
    shared_ptr<ConfigurationSearchSpaceConverterInterface<T>> const _converter;
};

//! \brief A property that specifies distinct values from an enum
template<class T> class EnumConfigurationProperty final : public ConfigurationPropertyBase<T> {
public:
    EnumConfigurationProperty();
    EnumConfigurationProperty(Set<T> const& values);
    EnumConfigurationProperty(T const& value);

    bool is_single() const override;
    bool is_metric(ConfigurationPropertyPath const& path) const override;
    bool is_configurable() const override;
    size_t cardinality() const override;

    ConfigurationPropertyInterface* clone() const override;

    ConfigurationPropertyInterface* at(ConfigurationPropertyPath const& path) override;

    T const& get() const override;
    void set(T const& value) override;
    void set(Set<T> const& values);
    void set_single(ConfigurationPropertyPath const& path, int integer_value) override;
protected:
    void local_set_single(int integer_value) override;
    List<int> local_integer_values() const override;
    List<shared_ptr<T>> values() const override;
private:
    Set<T> _values;
};

//! \brief A property that specifies a set of distinct values from handle class \a T
//! \details This can be used either for an enum or for distinct objects of a class or handle class
template<class T> class HandleListConfigurationProperty final : public ConfigurationPropertyBase<T> {
public:
    HandleListConfigurationProperty();
    HandleListConfigurationProperty(List<T> const& values);
    HandleListConfigurationProperty(T const& value);

    bool is_single() const override;
    bool is_metric(ConfigurationPropertyPath const& path) const override;
    bool is_configurable() const override;
    size_t cardinality() const override;

    ConfigurationPropertyInterface* clone() const override;

    ConfigurationPropertyInterface* at(ConfigurationPropertyPath const& path) override;

    T const& get() const override;
    void set(T const& value) override;
    void set(List<T> const& values);
    void set_single(ConfigurationPropertyPath const& path, int integer_value) override;
    Map<ConfigurationPropertyPath,List<int>> integer_values() const override;
  protected:
    void local_set_single(int integer_value) override;
    List<int> local_integer_values() const override;
    List<shared_ptr<T>> values() const override;
  private:
    List<T> _values;
};

//! \brief A property that specifies a list of objects deriving from an interface \a T
//! \details T must define the clone() method to support interfaces.
template<class T> class InterfaceListConfigurationProperty final : public ConfigurationPropertyBase<T> {
  public:
    InterfaceListConfigurationProperty();
    InterfaceListConfigurationProperty(List<shared_ptr<T>> const& list);
    InterfaceListConfigurationProperty(T const& value);

    bool is_single() const override;
    bool is_metric(ConfigurationPropertyPath const& path) const override;
    bool is_configurable() const override;
    size_t cardinality() const override;

    ConfigurationPropertyInterface* clone() const override;

    ConfigurationPropertyInterface* at(ConfigurationPropertyPath const& path) override;

    T const& get() const override;
    Map<ConfigurationPropertyPath,List<int>> integer_values() const override;
    void set(T const& value) override;
    void set(shared_ptr<T> const& value);
    void set(List<shared_ptr<T>> const& values);
    void set_single(ConfigurationPropertyPath const& path, int integer_value) override;
  protected:
    void local_set_single(int integer_value) override;
    List<int> local_integer_values() const override;
    List<shared_ptr<T>> values() const override;
  private:
    List<shared_ptr<T>> _values;
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_PROPERTY_HPP
