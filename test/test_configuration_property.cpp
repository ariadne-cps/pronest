/***************************************************************************
 *            test_configuration_property.cpp
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

#include "helper/test.hpp"
#include "helper/handle.hpp"
#include "searchable_configuration.hpp"
#include "configuration_property.tpl.hpp"

using namespace std;
using namespace ProNest;
using namespace Helper;

enum class LevelOptions { LOW, MEDIUM, HIGH };
std::ostream& operator<<(std::ostream& os, const LevelOptions level) {
    switch(level) {
        case LevelOptions::LOW: os << "LOW"; return os;
        case LevelOptions::MEDIUM: os << "MEDIUM"; return os;
        case LevelOptions::HIGH: os << "HIGH"; return os;
        default: HELPER_FAIL_MSG("Unhandled LevelOptions value");
    }
}

class TestInterface : public WritableInterface {
  public:
    virtual TestInterface* clone() const = 0;
};
class A : public TestInterface {
  public:
    ostream& _write(ostream& os) const override { return os << "A"; }
    TestInterface* clone() const override { return new A(*this); }

};
class B : public TestInterface {
public:
    ostream& _write(ostream& os) const override { return os << "B"; }
    TestInterface* clone() const override { return new B(*this); }
};

class TestHandle : public Handle<TestInterface> {
  public:
    using Handle<TestInterface>::Handle;
    TestInterface* clone() const { return _ptr->clone(); }
};

using DoubleConfigurationProperty = RangeConfigurationProperty<double>;
using LevelOptionsConfigurationProperty = EnumConfigurationProperty<LevelOptions>;
using TestHandleListConfigurationProperty = HandleListConfigurationProperty<TestHandle>;
using TestInterfaceListConfigurationProperty = InterfaceListConfigurationProperty<TestInterface>;
using Log10Converter = Log10SearchSpaceConverter<double>;
using Log2Converter = Log2SearchSpaceConverter<double>;

class TestConfiguration {
  public:

    void test_converters() {
        Log10SearchSpaceConverter<double> log10_x;
        HELPER_TEST_EQUALS(log10_x.to_int(0.001),-3);
        HELPER_TEST_PRINT(log10_x.from_int(-3));
        Log2SearchSpaceConverter<double> log2_x;
        HELPER_TEST_EQUALS(log2_x.to_int(0.03125),-5);
        HELPER_TEST_PRINT(log2_x.from_int(-5));
        LinearSearchSpaceConverter<double> lin_x;
        HELPER_TEST_EQUALS(lin_x.to_int(3.49),3);
        HELPER_TEST_EQUALS(lin_x.to_int(3.5),4);
        HELPER_TEST_EQUALS(lin_x.from_int(4),4);
        LinearSearchSpaceConverter<int> lin_int;
        HELPER_TEST_EQUALS(lin_int.to_int(-2),-2);
        HELPER_TEST_EQUALS(lin_int.from_int(4),4);
    }

    void test_boolean_configuration_property_construction() {
        BooleanConfigurationProperty p1;
        HELPER_TEST_PRINT(p1);
        HELPER_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        HELPER_TEST_ASSERT(not p1.is_specified());
        HELPER_TEST_ASSERT(not p1.is_single());
        HELPER_TEST_EQUALS(p1.cardinality(),0);
        BooleanConfigurationProperty p2(true);
        HELPER_TEST_PRINT(p2);
        HELPER_TEST_ASSERT(p2.is_specified());
        HELPER_TEST_ASSERT(p2.is_single());
        HELPER_TEST_EQUALS(p2.cardinality(),1);
    }

    void test_boolean_configuration_property_modification() {
        BooleanConfigurationProperty p;
        HELPER_TEST_PRINT(p);
        auto iv = p.integer_values();
        HELPER_TEST_EQUALS(iv.size(),1);
        HELPER_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(false);
        HELPER_TEST_PRINT(p);
        HELPER_TEST_EQUALS(p.get(),false);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),1);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        p.set(true);
        HELPER_TEST_PRINT(p);
        HELPER_TEST_EQUALS(p.get(),true);
        HELPER_TEST_EQUALS(p.cardinality(),1);
        p.set_both();
        HELPER_TEST_PRINT(p);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(not p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),2);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
    }

    void test_boolean_configuration_property_set_single() {
        BooleanConfigurationProperty p;
        p.set_both();
        p.set_single(ConfigurationPropertyPath(),0);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.get(),false);
        p.set_both();
        p.set_single(ConfigurationPropertyPath(),1);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.get(),true);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set_both();
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
    }

    void test_range_configuration_property_construction() {
        Log10Converter converter;
        DoubleConfigurationProperty p1(converter);
        HELPER_TEST_ASSERT(p1.is_metric(ConfigurationPropertyPath()));
        HELPER_TEST_ASSERT(not p1.is_specified());
        HELPER_TEST_ASSERT(not p1.is_single());
        HELPER_TEST_EQUALS(p1.cardinality(),0);
        DoubleConfigurationProperty p2(1e-2,converter);
        HELPER_TEST_ASSERT(p2.is_specified());
        HELPER_TEST_ASSERT(p2.is_single());
        HELPER_TEST_EQUALS(p2.cardinality(),1);
        DoubleConfigurationProperty p3(1e-10,1e-8,converter);
        HELPER_TEST_ASSERT(p3.is_specified());
        HELPER_TEST_ASSERT(not p3.is_single());
        HELPER_TEST_EQUALS(p3.cardinality(),3);
        HELPER_TEST_FAIL(DoubleConfigurationProperty(1e-8,1e-9,converter));
    }

    void test_range_configuration_property_modification() {
        Log10Converter converter;
        DoubleConfigurationProperty p(converter);
        HELPER_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        HELPER_TEST_EQUALS(iv.size(),1);
        HELPER_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(1e-2);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),1);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        p.set(1e-10,1e-8);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(not p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),3);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),3);
        HELPER_TEST_FAIL(p.set(1e-8,1e-9));
    }

    void test_range_configuration_property_set_single() {
        Log10Converter converter;
        DoubleConfigurationProperty p(0.001,0.1,converter);
        p.set_single(ConfigurationPropertyPath(),-3);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p);
        p.set(0.001,0.1);
        p.set_single(ConfigurationPropertyPath(),-1);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p);
        p.set(0.001,0.1);
        p.set_single(ConfigurationPropertyPath(),-2);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p.get());
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
        p.set(0.001,0.1);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-4));
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
    }

    void test_enum_configuration_property_construction() {
        LevelOptionsConfigurationProperty p1;
        HELPER_TEST_PRINT(p1);
        HELPER_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        HELPER_TEST_ASSERT(not p1.is_specified());
        HELPER_TEST_ASSERT(not p1.is_single());
        HELPER_TEST_EQUALS(p1.cardinality(),0);
        LevelOptionsConfigurationProperty p2(LevelOptions::LOW);
        HELPER_TEST_PRINT(p2);
        HELPER_TEST_ASSERT(p2.is_specified());
        HELPER_TEST_ASSERT(p2.is_single());
        HELPER_TEST_EQUALS(p2.cardinality(),1);
        LevelOptionsConfigurationProperty p3({LevelOptions::LOW,LevelOptions::HIGH});
        HELPER_TEST_PRINT(p3);
        HELPER_TEST_ASSERT(p3.is_specified());
        HELPER_TEST_ASSERT(not p3.is_single());
        HELPER_TEST_EQUALS(p3.cardinality(),2);
        HELPER_TEST_FAIL(new EnumConfigurationProperty<int>());
        HELPER_TEST_FAIL(new EnumConfigurationProperty<String>());
    }

    void test_enum_configuration_property_modification() {
        LevelOptionsConfigurationProperty p;
        HELPER_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        HELPER_TEST_EQUALS(iv.size(),1);
        HELPER_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(LevelOptions::MEDIUM);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),1);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        p.set({LevelOptions::MEDIUM,LevelOptions::HIGH});
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(not p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),2);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
        HELPER_TEST_FAIL(p.set(List<LevelOptions>()));
    }

    void test_enum_configuration_property_set_single() {
        LevelOptionsConfigurationProperty p({LevelOptions::MEDIUM,LevelOptions::HIGH});
        p.set_single(ConfigurationPropertyPath(),0);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.get(),LevelOptions::MEDIUM);
        p.set({LevelOptions::MEDIUM,LevelOptions::HIGH});
        p.set_single(ConfigurationPropertyPath(),1);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.get(),LevelOptions::HIGH);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set({LevelOptions::MEDIUM,LevelOptions::HIGH});
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));;
    }

    void test_list_configuration_property_construction() {
        TestHandleListConfigurationProperty p1;
        HELPER_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        HELPER_TEST_EQUALS(p1.cardinality(),0);
        HELPER_TEST_ASSERT(not p1.is_specified());
        A a;
        TestHandleListConfigurationProperty p2(a);
        HELPER_TEST_ASSERT(p2.is_specified());
        HELPER_TEST_ASSERT(p2.is_single());
        HELPER_TEST_EQUALS(p2.cardinality(),1);
        List<TestHandle> handles;
        HELPER_TEST_FAIL(new TestHandleListConfigurationProperty(handles));
        handles.push_back(A());
        handles.push_back(B());
        TestHandleListConfigurationProperty p3(handles);
        HELPER_TEST_ASSERT(p3.is_specified());
        HELPER_TEST_ASSERT(not p3.is_single());
        HELPER_TEST_EQUALS(p3.cardinality(),2);
    }

    void test_list_configuration_property_modification() {
        TestHandleListConfigurationProperty p;
        HELPER_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        HELPER_TEST_EQUALS(iv.size(),1);
        HELPER_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(A());
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),1);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        List<TestHandle> handles;
        HELPER_TEST_FAIL(p.set(handles));
        handles.push_back(A());
        handles.push_back(B());
        p.set(handles);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(not p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),2);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
    }

    void test_list_configuration_property_set_single() {
        List<TestHandle> handles;
        handles.push_back(A());
        handles.push_back(B());
        TestHandleListConfigurationProperty p(handles);
        p.set_single(ConfigurationPropertyPath(),0);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p);
        p.set(handles);
        p.set_single(ConfigurationPropertyPath(),1);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set(handles);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
    }

    void test_interface_configuration_property_construction() {
        TestInterfaceListConfigurationProperty p1;
        HELPER_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        HELPER_TEST_EQUALS(p1.cardinality(),0);
        HELPER_TEST_ASSERT(not p1.is_specified());
        A a;
        TestInterfaceListConfigurationProperty p2(a);
        HELPER_TEST_ASSERT(p2.is_specified());
        HELPER_TEST_ASSERT(p2.is_single());
        HELPER_TEST_EQUALS(p2.cardinality(),1);
        List<shared_ptr<TestInterface>> tests;
        HELPER_TEST_FAIL(new TestInterfaceListConfigurationProperty(tests));
        tests.push_back(shared_ptr<TestInterface>(new A()));
        tests.push_back(shared_ptr<TestInterface>(new B()));
        TestInterfaceListConfigurationProperty p3(tests);
        HELPER_TEST_ASSERT(p3.is_specified());
        HELPER_TEST_ASSERT(not p3.is_single());
        HELPER_TEST_EQUALS(p3.cardinality(),2);
    }

    void test_interface_configuration_property_modification() {
        TestInterfaceListConfigurationProperty p;
        HELPER_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        HELPER_TEST_EQUALS(iv.size(),1);
        HELPER_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(A());
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),1);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        List<shared_ptr<TestInterface>> tests;
        HELPER_TEST_FAIL(p.set(tests));
        tests.push_back(shared_ptr<TestInterface>(new A()));
        tests.push_back(shared_ptr<TestInterface>(new B()));
        p.set(tests);
        HELPER_TEST_ASSERT(p.is_specified());
        HELPER_TEST_ASSERT(not p.is_single());
        HELPER_TEST_EQUALS(p.cardinality(),2);
        HELPER_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
    }

    void test_interface_configuration_property_set_single() {
        List<shared_ptr<TestInterface>> tests;
        tests.push_back(shared_ptr<TestInterface>(new A()));
        tests.push_back(shared_ptr<TestInterface>(new B()));
        TestInterfaceListConfigurationProperty p(tests);
        p.set_single(ConfigurationPropertyPath(),0);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p);
        p.set(tests);
        p.set_single(ConfigurationPropertyPath(),1);
        HELPER_TEST_ASSERT(p.is_single());
        HELPER_TEST_PRINT(p);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set(tests);
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        HELPER_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
    }

    void test() {
        HELPER_TEST_CALL(test_converters());
        HELPER_TEST_CALL(test_boolean_configuration_property_construction());
        HELPER_TEST_CALL(test_boolean_configuration_property_modification());
        HELPER_TEST_CALL(test_boolean_configuration_property_set_single());
        HELPER_TEST_CALL(test_range_configuration_property_construction());
        HELPER_TEST_CALL(test_range_configuration_property_modification());
        HELPER_TEST_CALL(test_range_configuration_property_set_single());
        HELPER_TEST_CALL(test_enum_configuration_property_construction());
        HELPER_TEST_CALL(test_enum_configuration_property_modification());
        HELPER_TEST_CALL(test_enum_configuration_property_set_single());
        HELPER_TEST_CALL(test_list_configuration_property_construction());
        HELPER_TEST_CALL(test_list_configuration_property_modification());
        HELPER_TEST_CALL(test_list_configuration_property_set_single());
        HELPER_TEST_CALL(test_interface_configuration_property_construction());
        HELPER_TEST_CALL(test_interface_configuration_property_modification());
        HELPER_TEST_CALL(test_interface_configuration_property_set_single());
    }
};

int main() {

    TestConfiguration().test();
    return HELPER_TEST_FAILURES;
}