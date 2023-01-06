/***************************************************************************
 *            search_space_converter.hpp
 *
 *  Copyright  2011-20  Luca Geretti
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

/*! \file configuration_search_space_converter.hpp
 *  \brief Classes for converting configuration property data from/to the integer search space.
 */

#ifndef PRONEST_CONFIGURATION_SEARCH_SPACE_CONVERTER_HPP
#define PRONEST_CONFIGURATION_SEARCH_SPACE_CONVERTER_HPP

#include "declarations.hpp"

namespace ProNest {

//! \brief Interface for conversion from/into the integer search space
template<class T> struct ConfigurationSearchSpaceConverterInterface {
    //! \brief Convert the \a value into an integer value
    virtual int to_int(T const& value) const = 0;
    //! \brief Convert the \a value into a double value
    virtual double to_double(T const& value) const = 0;
    //! \brief Convert from an integer value \a i into the original value
    virtual T from_int(int i) const = 0;
    //! \brief Convert from a double into the original value
    virtual T from_double(double value) const = 0;

    virtual ConfigurationSearchSpaceConverterInterface* clone() const = 0;
    virtual ~ConfigurationSearchSpaceConverterInterface() = default;
};

template<class T> struct SearchSpaceConverterBase : public ConfigurationSearchSpaceConverterInterface<T> {
  public:
    T from_int(int i) const override final { return this->from_double(i); }
};

template<class T> struct Log10SearchSpaceConverter;
template<class T> struct Log2SearchSpaceConverter;
template<class T> struct LinearSearchSpaceConverter;

template<> struct Log10SearchSpaceConverter<double> : SearchSpaceConverterBase<double> {
    int to_int(double const& value) const override {
        if (value == inf) return std::numeric_limits<int>::max();
        else if (value == -inf) return std::numeric_limits<int>::min();
        else return std::round(log(value)/log(10.0)); }
    double to_double(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<double>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<double>::min();
        else return log(value)/log(10.0); }
    double from_double(double value) const override { return exp(log(10.0)*value); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new Log10SearchSpaceConverter(*this); }
};

template<> struct Log2SearchSpaceConverter<double> : SearchSpaceConverterBase<double> {
    int to_int(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::min();
        return std::round(log(value)/log(2.0)); }
    double to_double(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<double>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<double>::min();
        return log(value)/log(2.0); }
    double from_double(double value) const override { return exp(log(2.0)*value); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new Log2SearchSpaceConverter(*this); }
};

template<> struct LinearSearchSpaceConverter<double> : SearchSpaceConverterBase<double> {
    int to_int(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::min();
        return (int)std::round(value); }
    double to_double(double const& value) const override { return value.get_d(); }
    ExactDouble from_double(double value) const override { return cast_exact(ApproximateDouble(value)); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new LinearSearchSpaceConverter(*this); }
};

template<> struct LinearSearchSpaceConverter<int> : SearchSpaceConverterBase<int> {
    int to_int(int const& value) const override { return value; }
    double to_double(int const& value) const override { return value; }
    int from_double(double i) const override { return std::round(i); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new LinearSearchSpaceConverter(*this); }
};

template<> struct LinearSearchSpaceConverter<SizeType> : SearchSpaceConverterBase<SizeType> {
    int to_int(SizeType const& value) const override { return value; }
    double to_double(SizeType const& value) const override { return value; }
    SizeType from_double(double i) const override { return SizeType(i); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new LinearSearchSpaceConverter(*this); }
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_SEARCH_SPACE_CONVERTER_HPP
