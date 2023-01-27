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

#include <cmath>
#include "declarations.hpp"

namespace ProNest {

//! \brief Interface for conversion from/into the integer search space
template<class T> struct ConfigurationSearchSpaceConverterInterface {
    //! \brief Convert the \a value into an integer value
    virtual int to_int(T const& value) const = 0;
    //! \brief Convert from an integer value \a i into the original value
    virtual T from_int(int i) const = 0;

    virtual ConfigurationSearchSpaceConverterInterface* clone() const = 0;
    virtual ~ConfigurationSearchSpaceConverterInterface() = default;
};

template<class T> struct Log10SearchSpaceConverter;
template<class T> struct Log2SearchSpaceConverter;
template<class T> struct LinearSearchSpaceConverter;

template<> struct Log10SearchSpaceConverter<double> : ConfigurationSearchSpaceConverterInterface<double> {
    int to_int(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::min();
        else return static_cast<int>(std::round(log(value)/log(10.0))); }
    double from_int(int i) const override { return exp(log(10.0)*i); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new Log10SearchSpaceConverter(*this); }
};

template<> struct Log2SearchSpaceConverter<double> : ConfigurationSearchSpaceConverterInterface<double> {
    int to_int(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::min();
        return static_cast<int>(std::round(log(value)/log(2.0))); }
    double from_int(int i) const override { return exp(log(2.0)*i); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new Log2SearchSpaceConverter(*this); }
};

template<> struct LinearSearchSpaceConverter<double> : ConfigurationSearchSpaceConverterInterface<double> {
    int to_int(double const& value) const override {
        if (value == std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::max();
        else if (value == -std::numeric_limits<double>::infinity()) return std::numeric_limits<int>::min();
        return static_cast<int>(std::round(value)); }
    double from_int(int i) const override { return i; }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new LinearSearchSpaceConverter(*this); }
};

template<> struct LinearSearchSpaceConverter<int> : ConfigurationSearchSpaceConverterInterface<int> {
    int to_int(int const& value) const override { return value; }
    int from_int(int i) const override { return static_cast<int>(std::round(i)); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new LinearSearchSpaceConverter(*this); }
};

template<> struct LinearSearchSpaceConverter<SizeType> : ConfigurationSearchSpaceConverterInterface<SizeType> {
    int to_int(SizeType const& value) const override { return static_cast<int>(value); }
    SizeType from_int(int i) const override { return static_cast<SizeType>(i); }
    ConfigurationSearchSpaceConverterInterface* clone() const override { return new LinearSearchSpaceConverter(*this); }
};

} // namespace ProNest

#endif // PRONEST_CONFIGURATION_SEARCH_SPACE_CONVERTER_HPP
