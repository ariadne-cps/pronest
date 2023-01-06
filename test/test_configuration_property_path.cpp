/***************************************************************************
 *            test_configuration_property_path.cpp
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

#include "configuration_property_path.hpp"
#include "test.hpp"

using namespace ProNest;

class TestConfigurationPropertyPath {
  public:

    void test_construction() {
        ConfigurationPropertyPath p;
        PRONEST_TEST_EQUALS(p.repr(),"./");
        ConfigurationPropertyPath p2("child");
        PRONEST_TEST_EQUALS(p2.repr(),"./child/");
        ConfigurationPropertyPath p3(p);
        PRONEST_TEST_EQUALS(p3.repr(),"./");
    }

    void test_append() {
        ConfigurationPropertyPath p;
        p.append("child1");
        PRONEST_TEST_EQUALS(p.repr(),"./child1/");
        p.append("child2");
        PRONEST_TEST_EQUALS(p.repr(),"./child1/child2/");
    }

    void test_prepend() {
        ConfigurationPropertyPath p;
        p.prepend("child2");
        PRONEST_TEST_EQUALS(p.repr(),"./child2/");
        p.prepend("child1");
        PRONEST_TEST_EQUALS(p.repr(),"./child1/child2/");
    }

    void test_first_last_subpath() {
        ConfigurationPropertyPath p;
        PRONEST_TEST_FAIL(p.first());
        PRONEST_TEST_FAIL(p.last())
        p.append("child1");
        p.append("child2");
        PRONEST_TEST_EQUALS(p.repr(),"./child1/child2/");
        auto sp = p.subpath();
        PRONEST_TEST_EQUALS(sp.repr(),"./child2/");
        auto f = p.first();
        PRONEST_TEST_EQUALS(f,"child1");
        auto l = p.last();
        PRONEST_TEST_EQUALS(l,"child2");
    }

    void test_copy() {
        ConfigurationPropertyPath p1;
        p1.append("child1");
        auto p2 = p1;
        p2.append("child2");
        PRONEST_TEST_EQUALS(p1.repr(),"./child1/");
        PRONEST_TEST_EQUALS(p2.repr(),"./child1/child2/");
    }

    void test_less_equal() {
        ConfigurationPropertyPath p1;
        p1.append("child1");
        ConfigurationPropertyPath p2;
        p2.append("child1");
        PRONEST_TEST_EQUAL(p1,p2);
        p2.append("child2");
        PRONEST_TEST_ASSERT(p1<p2);
        p2.prepend("child0");
        PRONEST_TEST_ASSERT(p2<p1);
    }

    void test() {
        PRONEST_TEST_CALL(test_construction());
        PRONEST_TEST_CALL(test_append());
        PRONEST_TEST_CALL(test_prepend());
        PRONEST_TEST_CALL(test_first_last_subpath());
        PRONEST_TEST_CALL(test_copy());
        PRONEST_TEST_CALL(test_less_equal());
    }
};

int main() {
    TestConfigurationPropertyPath().test();
    return PRONEST_TEST_FAILURES;
}
