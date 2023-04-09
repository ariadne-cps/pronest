/***************************************************************************
 *            randomiser.hpp
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

/*! \file randomiser.hpp
 *  \brief Generators of random numbers for a type.
 *  \details The values are generated uniformly in the provided interval.
 */

#ifndef PRONEST_RANDOMISER_HPP
#define PRONEST_RANDOMISER_HPP

#include <cstdlib>
#include <time.h>

using namespace std;

namespace ProNest {

template<class T> struct Randomiser;

template<> struct Randomiser<double> {
    //! \get Return a value between 0 and \a value
    static double get(double min, double max) {
        return ((max-min)*static_cast<size_t>(rand())/RAND_MAX + min);
    }
};

template<> struct Randomiser<size_t> {
    //! \get Return a value between 0 and \a value
    static size_t get(size_t const& min, size_t const& max) {
        return size_t(min + static_cast<size_t>(rand()) % (max-min+1));
    }
};

} // namespace ProNest

inline bool _init_randomiser() {
    srand(static_cast<unsigned int>(time(nullptr)));
    return true;
}

static const bool init_randomiser = _init_randomiser();

#endif /* PRONEST_RANDOMISER_HPP */
