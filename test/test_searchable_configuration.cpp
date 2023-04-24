/***************************************************************************
 *            test_searchable_configuration.cpp
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
#include "searchable_configuration.hpp"
#include "configuration_property.tpl.hpp"
#include "configuration_search_space.hpp"
#include "configurable.tpl.hpp"
#include "configuration_search_point.hpp"

using namespace std;
using namespace ProNest;
using namespace Helper;

class A;

enum class LevelOptions { LOW, MEDIUM, HIGH };
std::ostream& operator<<(std::ostream& os, const LevelOptions level) {
    switch(level) {
        case LevelOptions::LOW: os << "LOW"; return os;
        case LevelOptions::MEDIUM: os << "MEDIUM"; return os;
        case LevelOptions::HIGH: os << "HIGH"; return os;
        default: HELPER_FAIL_MSG("Unhandled LevelOptions value");
    }
}

class TestConfigurable;

namespace ProNest {

template<> struct Configuration<TestConfigurable> : public SearchableConfiguration {
  public:
    Configuration() { add_property("use_something",BooleanConfigurationProperty(true)); }
    bool const& use_use_something() const { return dynamic_cast<BooleanConfigurationProperty const&>(*properties().get("use_something")).get(); }
    void set_both_use_something() { dynamic_cast<BooleanConfigurationProperty&>(*properties().get("use_something")).set_both(); }
    void set_use_something(bool const& value) { dynamic_cast<BooleanConfigurationProperty&>(*properties().get("use_something")).set(value); }
};

}

class TestConfigurableInterface : public WritableInterface {
public:
    virtual TestConfigurableInterface* clone() const = 0;
    virtual void set_value(String value) = 0;
    virtual ~TestConfigurableInterface() = default;
};
class TestConfigurable : public TestConfigurableInterface, public Configurable<TestConfigurable> {
public:
    TestConfigurable(String value, Configuration<TestConfigurable> const& configuration) : TestConfigurable(configuration) { _value = value; }
    TestConfigurable(Configuration<TestConfigurable> const& configuration) : Configurable<TestConfigurable>(configuration) { }
    void set_value(String value) override { _value = value; }
    ostream& _write(ostream& os) const override { os << "TestConfigurable(value="<<_value<<",configuration=" << configuration() <<")"; return os; }
    TestConfigurableInterface* clone() const override { auto cfg = configuration(); return new TestConfigurable(_value,cfg); }
private:
    String _value;
};

using DoubleConfigurationProperty = RangeConfigurationProperty<double>;
using IntegerConfigurationProperty = RangeConfigurationProperty<int>;
using LevelOptionsConfigurationProperty = EnumConfigurationProperty<LevelOptions>;
using TestConfigurableConfigurationProperty = InterfaceListConfigurationProperty<TestConfigurableInterface>;
using Log10Converter = Log10SearchSpaceConverter<double>;
using Log2Converter = Log2SearchSpaceConverter<double>;

namespace ProNest {

    template<> struct Configuration<A> : public SearchableConfiguration {
    public:
        Configuration() {
            add_property("use_reconditioning",BooleanConfigurationProperty(false));
            add_property("maximum_order",IntegerConfigurationProperty(5));
            add_property("maximum_step_size",DoubleConfigurationProperty(std::numeric_limits<double>::infinity(),Log2Converter()));
            add_property("level",LevelOptionsConfigurationProperty(LevelOptions::LOW));
            add_property("test_configurable",TestConfigurableConfigurationProperty(TestConfigurable(Configuration<TestConfigurable>())));
        }

        bool const& use_reconditioning() const { return at<BooleanConfigurationProperty>("use_reconditioning").get(); }
        void set_both_use_reconditioning() { at<BooleanConfigurationProperty>("use_reconditioning").set_both(); }
        void set_use_reconditioning(bool const& value) { at<BooleanConfigurationProperty>("use_reconditioning").set(value); }

        int const& maximum_order() const { return at<IntegerConfigurationProperty>("maximum_order").get(); }
        void set_maximum_order(int const& value) { at<IntegerConfigurationProperty>("maximum_order").set(value); }
        void set_maximum_order(int const& lower, int const& upper) { at<IntegerConfigurationProperty>("maximum_order").set(lower,upper); }

        double const& maximum_step_size() const { return at<DoubleConfigurationProperty>("maximum_step_size").get(); }
        void set_maximum_step_size(double const& value) { at<DoubleConfigurationProperty>("maximum_step_size").set(value); }
        void set_maximum_step_size(double const& lower, double const& upper) { at<DoubleConfigurationProperty>("maximum_step_size").set(lower,upper); }

        LevelOptions const& level() const { return at<LevelOptionsConfigurationProperty>("level").get(); }
        void set_level(LevelOptions const& level) { at<LevelOptionsConfigurationProperty>("level").set(level); }
        void set_level(List<LevelOptions> const& levels) { at<LevelOptionsConfigurationProperty>("level").set(levels); }

        TestConfigurableInterface const& test_configurable() const { return at<TestConfigurableConfigurationProperty>("test_configurable").get(); }
        void set_test_configurable(TestConfigurableInterface const& test_configurable) { at<TestConfigurableConfigurationProperty>("test_configurable").set(test_configurable); }
        void set_test_configurable(shared_ptr<TestConfigurableInterface> const& test_configurable) { at<TestConfigurableConfigurationProperty>("test_configurable").set(test_configurable); }
    };

}

class A : public Configurable<A>, public WritableInterface {
  public:
    A() : Configurable<A>(Configuration<A>()) { }
    ostream& _write(ostream& os) const override { os << "configuration:" << configuration(); return os; }
};

class TestConfiguration {
  public:

    void test_configuration_construction() {
        Configuration<A> a;
        HELPER_TEST_PRINT(a);
        a.set_use_reconditioning(true);
        HELPER_TEST_ASSERT(a.use_reconditioning());
        a.set_use_reconditioning(false);
        HELPER_TEST_ASSERT(not a.use_reconditioning());
    }

    void test_configuration_at() {
        Configuration<A> ca;
        HELPER_TEST_EQUALS(ca.level(),LevelOptions::LOW);
        ca.set_level(LevelOptions::MEDIUM);
        HELPER_TEST_EQUALS(ca.level(),LevelOptions::MEDIUM);
        HELPER_TEST_EQUALS(ca.maximum_order(),5);
        ca.set_maximum_order(3);
        HELPER_TEST_EQUALS(ca.maximum_order(),3);
        HELPER_TEST_FAIL(ca.at<EnumConfigurationProperty<LevelOptions>>(ConfigurationPropertyPath("inexistent")));
        auto level_prop = ca.at<EnumConfigurationProperty<LevelOptions>>(ConfigurationPropertyPath("level"));
        HELPER_TEST_EQUALS(level_prop.get(),LevelOptions::MEDIUM);
        ca.at<LevelOptionsConfigurationProperty>(ConfigurationPropertyPath("level")).set(LevelOptions::HIGH);
        HELPER_TEST_EQUALS(ca.level(),LevelOptions::HIGH);
        auto use_something_prop = ca.at<BooleanConfigurationProperty>(ConfigurationPropertyPath("test_configurable").append("use_something"));
        HELPER_TEST_EQUALS(use_something_prop.get(),true);
        ca.at<BooleanConfigurationProperty>(ConfigurationPropertyPath("test_configurable").append("use_something")).set(false);
        auto use_something_prop_again = ca.at<BooleanConfigurationProperty>(ConfigurationPropertyPath("test_configurable").append("use_something"));
        HELPER_TEST_EQUALS(use_something_prop_again.get(),false);
    }

    void test_configuration_search_space() {
        Configuration<A> a;
        HELPER_TEST_FAIL(a.search_space());
        a.set_both_use_reconditioning();
        auto search_space = a.search_space();
        HELPER_TEST_EQUALS(search_space.dimension(),1);
    }

    void test_configuration_make_singleton() {
        Configuration<A> a;
        a.set_both_use_reconditioning();
        auto search_space = a.search_space();
        HELPER_TEST_PRINT(search_space);
        auto point = search_space.initial_point();
        HELPER_TEST_PRINT(point);
        bool use_reconditioning = (point.coordinates()[0] == 1 ? true : false);
        HELPER_TEST_PRINT(use_reconditioning);
        auto b = make_singleton(a,point);
        HELPER_TEST_ASSERT(not a.is_singleton());
        HELPER_TEST_ASSERT(b.is_singleton());
        HELPER_TEST_EQUALS(b.use_reconditioning(),use_reconditioning);

        a.set_maximum_step_size(1e-3,1e-1);
        auto search_space2 = a.search_space();
        HELPER_TEST_PRINT(search_space2);
        point = search_space2.initial_point();
        HELPER_TEST_PRINT(point);
        b = make_singleton(a,point);
        HELPER_TEST_ASSERT(not a.is_singleton());
        HELPER_TEST_ASSERT(b.is_singleton());
        HELPER_TEST_PRINT(b);

        ConfigurationSearchParameter p1(ConfigurationPropertyPath("use_reconditioning"), false, List<int>({0, 1}));
        ConfigurationSearchParameter p2(ConfigurationPropertyPath("maximum_step_size"), true, List<int>({-3, -1}));
        ConfigurationSearchParameter p3(ConfigurationPropertyPath("level"), false, List<int>({1, 2}));
        ConfigurationSearchParameter p4(ConfigurationPropertyPath("maximum_order"), true, List<int>({2, 4}));

        ConfigurationSearchSpace search_space3({p1, p2, p3, p4});
        HELPER_TEST_FAIL(make_singleton(a,search_space3.initial_point()));
        ConfigurationSearchSpace search_space4({p1, p2, p3});
        HELPER_TEST_FAIL(make_singleton(a,search_space4.initial_point()));
        a.set_use_reconditioning(false);
        ConfigurationSearchSpace search_space5({p1, p2});
        HELPER_TEST_FAIL(make_singleton(a,search_space5.initial_point()));
        a.set_both_use_reconditioning();
        ConfigurationSearchSpace search_space6({p1});
        HELPER_TEST_FAIL(make_singleton(a,search_space6.initial_point()))

        ConfigurationSearchParameter p5(ConfigurationPropertyPath("incorrect"), false, List<int>({2, 4}));
        ConfigurationSearchSpace search_space7({p5});
        HELPER_TEST_FAIL(make_singleton(a,search_space7.initial_point()));
    }

    void test_configuration_hierarchic_search_space() {
        Configuration<A> ca;
        Configuration<TestConfigurable> ctc;
        ctc.set_both_use_something();
        TestConfigurable tc(ctc);
        ca.set_test_configurable(tc);
        ca.set_both_use_reconditioning();
        auto search_space = ca.search_space();
        HELPER_TEST_PRINT(ca);
        HELPER_TEST_PRINT(search_space);
        HELPER_TEST_EQUALS(search_space.dimension(),2);
    }

    void test_configuration_hierarchic_make_singleton() {
        Configuration<A> ca;
        Configuration<TestConfigurable> ctc;
        ctc.set_both_use_something();
        TestConfigurable tc(ctc);
        ca.set_test_configurable(tc);
        ca.set_both_use_reconditioning();
        HELPER_TEST_PRINT(ca);
        auto search_space = ca.search_space();
        HELPER_TEST_PRINT(search_space);
        auto point = search_space.initial_point();
        HELPER_TEST_PRINT(point);
        auto singleton = make_singleton(ca,point);
        HELPER_TEST_PRINT(singleton);
    }

    void test() {
        HELPER_TEST_CALL(test_configuration_construction());
        HELPER_TEST_CALL(test_configuration_at());
        HELPER_TEST_CALL(test_configuration_search_space());
        HELPER_TEST_CALL(test_configuration_make_singleton());
        HELPER_TEST_CALL(test_configuration_hierarchic_search_space());
        HELPER_TEST_CALL(test_configuration_hierarchic_make_singleton());
    }
};

int main() {

    TestConfiguration().test();
    return HELPER_TEST_FAILURES;
}
