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

#include "searchable_configuration.hpp"
#include "configuration_property.tpl.hpp"
#include "test.hpp"

using namespace ProNest;

enum class LevelOptions { LOW, MEDIUM, HIGH };
std::ostream& operator<<(std::ostream& os, const LevelOptions level) {
    switch(level) {
        case LevelOptions::LOW: os << "LOW"; return os;
        case LevelOptions::MEDIUM: os << "MEDIUM"; return os;
        case LevelOptions::HIGH: os << "HIGH"; return os;
        default: PRONEST_FAIL_MSG("Unhandled LevelOptions value");
    }
}

class TestInterface : public WritableInterface {
  public:
    virtual TestInterface* clone() const = 0;
};
class A : public TestInterface {
  public:
    OutputStream& _write(OutputStream& os) const override { return os << "A"; }
    TestInterface* clone() const override { return new A(*this); }
};
class B : public TestInterface {
public:
    OutputStream& _write(OutputStream& os) const override { return os << "B"; }
    TestInterface* clone() const override { return new B(*this); }
};

using ExactDoubleConfigurationProperty = RangeConfigurationProperty<ExactDouble>;
using LevelOptionsConfigurationProperty = EnumConfigurationProperty<LevelOptions>;
using SweeperConfigurationProperty = HandleListConfigurationProperty<Sweeper<FloatDP>>;
using TestInterfaceListConfigurationProperty = InterfaceListConfigurationProperty<TestInterface>;
using Log10Converter = Log10SearchSpaceConverter<ExactDouble>;
using Log2Converter = Log2SearchSpaceConverter<ExactDouble>;

class TestConfiguration {
  public:

    void test_converters() {
        Log10SearchSpaceConverter<ExactDouble> log10_x;
        PRONEST_TEST_EQUALS(log10_x.to_int(cast_exact(0.001)),-3);
        PRONEST_TEST_PRINT(log10_x.from_int(-3).get_d());
        Log2SearchSpaceConverter<ExactDouble> log2_x;
        PRONEST_TEST_EQUALS(log2_x.to_int(0.03125_x),-5);
        PRONEST_TEST_PRINT(log2_x.from_int(-5).get_d());
        LinearSearchSpaceConverter<ExactDouble> lin_x;
        PRONEST_TEST_EQUALS(lin_x.to_int(cast_exact(3.49)),3);
        PRONEST_TEST_EQUALS(lin_x.to_int(3.5_x),4);
        PRONEST_TEST_EQUALS(lin_x.from_int(4).get_d(),4);
        LinearSearchSpaceConverter<int> lin_int;
        PRONEST_TEST_EQUALS(lin_int.to_int(-2),-2);
        PRONEST_TEST_EQUALS(lin_int.from_int(4),4);
    }

    void test_boolean_configuration_property_construction() {
        BooleanConfigurationProperty p1;
        PRONEST_TEST_PRINT(p1);
        PRONEST_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        PRONEST_TEST_ASSERT(not p1.is_specified());
        PRONEST_TEST_ASSERT(not p1.is_single());
        PRONEST_TEST_EQUALS(p1.cardinality(),0);
        BooleanConfigurationProperty p2(true);
        PRONEST_TEST_PRINT(p2);
        PRONEST_TEST_ASSERT(p2.is_specified());
        PRONEST_TEST_ASSERT(p2.is_single());
        PRONEST_TEST_EQUALS(p2.cardinality(),1);
        PRONEST_TEST_PRINT(p2.get());
    }

    void test_boolean_configuration_property_modification() {
        BooleanConfigurationProperty p;
        PRONEST_TEST_PRINT(p);
        auto iv = p.integer_values();
        PRONEST_TEST_EQUALS(iv.size(),1);
        PRONEST_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(false);
        PRONEST_TEST_PRINT(p);
        PRONEST_TEST_EQUALS(p.get(),false);
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),1);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        p.set(true);
        PRONEST_TEST_PRINT(p);
        PRONEST_TEST_EQUALS(p.get(),true);
        PRONEST_TEST_EQUALS(p.cardinality(),1);
        p.set_both();
        PRONEST_TEST_PRINT(p);
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(not p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),2);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
    }

    void test_boolean_configuration_property_set_single() {
        BooleanConfigurationProperty p;
        p.set_both();
        p.set_single(ConfigurationPropertyPath(),0);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.get(),false);
        p.set_both();
        p.set_single(ConfigurationPropertyPath(),1);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.get(),true);
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set_both();
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
    }

    void test_range_configuration_property_construction() {
        Log10Converter converter;
        ExactDoubleConfigurationProperty p1(converter);
        PRONEST_TEST_ASSERT(p1.is_metric(ConfigurationPropertyPath()));
        PRONEST_TEST_ASSERT(not p1.is_specified());
        PRONEST_TEST_ASSERT(not p1.is_single());
        PRONEST_TEST_EQUALS(p1.cardinality(),0);
        ExactDoubleConfigurationProperty p2(cast_exact(1e-2),converter);
        PRONEST_TEST_ASSERT(p2.is_specified());
        PRONEST_TEST_ASSERT(p2.is_single());
        PRONEST_TEST_PRINT(p2.get());
        PRONEST_TEST_EQUALS(p2.cardinality(),1);
        ExactDoubleConfigurationProperty p3(cast_exact(1e-10),cast_exact(1e-8),converter);
        PRONEST_TEST_ASSERT(p3.is_specified());
        PRONEST_TEST_ASSERT(not p3.is_single());
        PRONEST_TEST_EQUALS(p3.cardinality(),3);
        PRONEST_TEST_FAIL(ExactDoubleConfigurationProperty(cast_exact(1e-8),cast_exact(1e-9),converter));
    }

    void test_range_configuration_property_modification() {
        Log10Converter converter;
        ExactDoubleConfigurationProperty p(converter);
        PRONEST_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        PRONEST_TEST_EQUALS(iv.size(),1);
        PRONEST_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(cast_exact(1e-2));
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),1);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        p.set(cast_exact(1e-10),cast_exact(1e-8));
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(not p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),3);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),3);
        PRONEST_TEST_FAIL(p.set(cast_exact(1e-8),cast_exact(1e-9)));
    }

    void test_range_configuration_property_set_single() {
        Log10Converter converter;
        ExactDoubleConfigurationProperty p(cast_exact(0.001),cast_exact(0.1),converter);
        p.set_single(ConfigurationPropertyPath(),-3);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p);
        p.set(cast_exact(0.001),cast_exact(0.1));
        p.set_single(ConfigurationPropertyPath(),-1);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p);
        p.set(cast_exact(0.001),cast_exact(0.1));
        p.set_single(ConfigurationPropertyPath(),-2);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p.get().get_d());
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
        p.set(cast_exact(0.001),cast_exact(0.1));
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-4));
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
    }

    void test_enum_configuration_property_construction() {
        LevelOptionsConfigurationProperty p1;
        PRONEST_TEST_PRINT(p1);
        PRONEST_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        PRONEST_TEST_ASSERT(not p1.is_specified());
        PRONEST_TEST_ASSERT(not p1.is_single());
        PRONEST_TEST_EQUALS(p1.cardinality(),0);
        LevelOptionsConfigurationProperty p2(LevelOptions::LOW);
        PRONEST_TEST_PRINT(p2);
        PRONEST_TEST_ASSERT(p2.is_specified());
        PRONEST_TEST_ASSERT(p2.is_single());
        PRONEST_TEST_EQUALS(p2.cardinality(),1);
        LevelOptionsConfigurationProperty p3({LevelOptions::LOW,LevelOptions::HIGH});
        PRONEST_TEST_PRINT(p3);
        PRONEST_TEST_ASSERT(p3.is_specified());
        PRONEST_TEST_ASSERT(not p3.is_single());
        PRONEST_TEST_EQUALS(p3.cardinality(),2);
        PRONEST_TEST_FAIL(new EnumConfigurationProperty<int>());
        PRONEST_TEST_FAIL(new EnumConfigurationProperty<Integer>());
    }

    void test_enum_configuration_property_modification() {
        LevelOptionsConfigurationProperty p;
        PRONEST_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        PRONEST_TEST_EQUALS(iv.size(),1);
        PRONEST_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(LevelOptions::MEDIUM);
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),1);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        p.set({LevelOptions::MEDIUM,LevelOptions::HIGH});
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(not p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),2);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
        PRONEST_TEST_FAIL(p.set(List<LevelOptions>()));
    }

    void test_enum_configuration_property_set_single() {
        LevelOptionsConfigurationProperty p({LevelOptions::MEDIUM,LevelOptions::HIGH});
        p.set_single(ConfigurationPropertyPath(),0);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.get(),LevelOptions::MEDIUM);
        p.set({LevelOptions::MEDIUM,LevelOptions::HIGH});
        p.set_single(ConfigurationPropertyPath(),1);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.get(),LevelOptions::HIGH);
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set({LevelOptions::MEDIUM,LevelOptions::HIGH});
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));;
    }

    void test_list_configuration_property_construction() {
        SweeperConfigurationProperty p1;
        PRONEST_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        PRONEST_TEST_EQUALS(p1.cardinality(),0);
        PRONEST_TEST_ASSERT(not p1.is_specified());
        ThresholdSweeper<FloatDP> sweeper(DoublePrecision(),1e-6);
        SweeperConfigurationProperty p2(sweeper);
        PRONEST_TEST_ASSERT(p2.is_specified());
        PRONEST_TEST_ASSERT(p2.is_single());
        PRONEST_TEST_EQUALS(p2.cardinality(),1);
        PRONEST_TEST_PRINT(p2.get());
        List<Sweeper<FloatDP>> sweepers;
        PRONEST_TEST_FAIL(new SweeperConfigurationProperty(sweepers));
        sweepers.append(ThresholdSweeper<FloatDP>(DoublePrecision(),1e-6));
        sweepers.append(GradedSweeper<FloatDP>(DoublePrecision(),Order(6)));
        SweeperConfigurationProperty p3(sweepers);
        PRONEST_TEST_ASSERT(p3.is_specified());
        PRONEST_TEST_ASSERT(not p3.is_single());
        PRONEST_TEST_EQUALS(p3.cardinality(),2);
    }

    void test_list_configuration_property_modification() {
        SweeperConfigurationProperty p;
        PRONEST_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        PRONEST_TEST_EQUALS(iv.size(),1);
        PRONEST_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(ThresholdSweeper<FloatDP>(DoublePrecision(),1e-6));
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),1);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        List<Sweeper<FloatDP>> sweepers;
        PRONEST_TEST_FAIL(p.set(sweepers));
        sweepers.append(ThresholdSweeper<FloatDP>(DoublePrecision(),1e-6));
        sweepers.append(GradedSweeper<FloatDP>(DoublePrecision(),Order(6)));
        p.set(sweepers);
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(not p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),2);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
    }

    void test_list_configuration_property_set_single() {
        List<Sweeper<FloatDP>> sweepers;
        sweepers.append(ThresholdSweeper<FloatDP>(DoublePrecision(),1e-6));
        sweepers.append(GradedSweeper<FloatDP>(DoublePrecision(),Order(6)));
        SweeperConfigurationProperty p(sweepers);
        p.set_single(ConfigurationPropertyPath(),0);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p);
        p.set(sweepers);
        p.set_single(ConfigurationPropertyPath(),1);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p);
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set(sweepers);
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
    }

    void test_interface_configuration_property_construction() {
        TestInterfaceListConfigurationProperty p1;
        PRONEST_TEST_ASSERT(not p1.is_metric(ConfigurationPropertyPath()));
        PRONEST_TEST_EQUALS(p1.cardinality(),0);
        PRONEST_TEST_ASSERT(not p1.is_specified());
        A a;
        TestInterfaceListConfigurationProperty p2(a);
        PRONEST_TEST_ASSERT(p2.is_specified());
        PRONEST_TEST_ASSERT(p2.is_single());
        PRONEST_TEST_EQUALS(p2.cardinality(),1);
        PRONEST_TEST_PRINT(p2.get());
        List<SharedPointer<TestInterface>> tests;
        PRONEST_TEST_FAIL(new TestInterfaceListConfigurationProperty(tests));
        tests.append(SharedPointer<TestInterface>(new A()));
        tests.append(SharedPointer<TestInterface>(new B()));
        TestInterfaceListConfigurationProperty p3(tests);
        PRONEST_TEST_ASSERT(p3.is_specified());
        PRONEST_TEST_ASSERT(not p3.is_single());
        PRONEST_TEST_EQUALS(p3.cardinality(),2);
    }

    void test_interface_configuration_property_modification() {
        TestInterfaceListConfigurationProperty p;
        PRONEST_TEST_EQUALS(p.cardinality(),0);
        auto iv = p.integer_values();
        PRONEST_TEST_EQUALS(iv.size(),1);
        PRONEST_TEST_EQUALS(iv.begin()->second.size(),0);
        p.set(A());
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),1);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),1);
        List<SharedPointer<TestInterface>> tests;
        PRONEST_TEST_FAIL(p.set(tests));
        tests.append(SharedPointer<TestInterface>(new A()));
        tests.append(SharedPointer<TestInterface>(new B()));
        p.set(tests);
        PRONEST_TEST_ASSERT(p.is_specified());
        PRONEST_TEST_ASSERT(not p.is_single());
        PRONEST_TEST_EQUALS(p.cardinality(),2);
        PRONEST_TEST_EQUALS(p.integer_values().begin()->second.size(),2);
    }

    void test_interface_configuration_property_set_single() {
        List<SharedPointer<TestInterface>> tests;
        tests.append(SharedPointer<TestInterface>(new A()));
        tests.append(SharedPointer<TestInterface>(new B()));
        TestInterfaceListConfigurationProperty p(tests);
        p.set_single(ConfigurationPropertyPath(),0);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p);
        p.set(tests);
        p.set_single(ConfigurationPropertyPath(),1);
        PRONEST_TEST_ASSERT(p.is_single());
        PRONEST_TEST_PRINT(p);
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),0));
        p.set(tests);
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),2));
        PRONEST_TEST_FAIL(p.set_single(ConfigurationPropertyPath(),-1));
    }

    void test() {
        PRONEST_TEST_CALL(test_converters());
        PRONEST_TEST_CALL(test_boolean_configuration_property_construction());
        PRONEST_TEST_CALL(test_boolean_configuration_property_modification());
        PRONEST_TEST_CALL(test_boolean_configuration_property_set_single());
        PRONEST_TEST_CALL(test_range_configuration_property_construction());
        PRONEST_TEST_CALL(test_range_configuration_property_modification());
        PRONEST_TEST_CALL(test_range_configuration_property_set_single());
        PRONEST_TEST_CALL(test_enum_configuration_property_construction());
        PRONEST_TEST_CALL(test_enum_configuration_property_modification());
        PRONEST_TEST_CALL(test_enum_configuration_property_set_single());
        PRONEST_TEST_CALL(test_list_configuration_property_construction());
        PRONEST_TEST_CALL(test_list_configuration_property_modification());
        PRONEST_TEST_CALL(test_list_configuration_property_set_single());
        PRONEST_TEST_CALL(test_interface_configuration_property_construction());
        PRONEST_TEST_CALL(test_interface_configuration_property_modification());
        PRONEST_TEST_CALL(test_interface_configuration_property_set_single());
    }
};

int main() {

    TestConfiguration().test();
    return PRONEST_TEST_FAILURES;
}