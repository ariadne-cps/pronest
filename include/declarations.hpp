/***************************************************************************
 *            declarations.hpp
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

/*! \file declarations.hpp
 *  \brief
 */

#ifndef PRONEST_DECLARATIONS_HPP
#define PRONEST_DECLARATIONS_HPP

#include <cstdint>
#include <iosfwd>
#include <type_traits>
#include <string>
#include <ostream>

namespace ProNest {

//! \brief Internal name for standard output stream.
using OutputStream = std::ostream;
//! \brief Internal name for unsigned integer.
using Nat = unsigned int;
//! \brief Internal name for standard size type, used for sizes of containers.
using SizeType = std::size_t;

//! \brief Internal name for standard initializer list.
template<class T> using InitializerList = std::initializer_list<T>;
//! \brief Internal name for standard pair.
template<class T1, class T2> using Pair = std::pair<T1,T2>;
//! \brief Internal name for standard tuple.
template<class... TS> using Tuple = std::tuple<TS...>;

} // namespace ProNest

#endif // PRONEST_DECLARATIONS_HPP
