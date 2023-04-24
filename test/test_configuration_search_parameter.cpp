/***************************************************************************
 *            test_configuration_search_parameter.cpp
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
#include "configuration_search_point.hpp"
#include "configuration_search_space.hpp"

using namespace ProNest;

class TestConfigurationSearchParameter {
  public:

    void test_parameter_creation() {
        ConfigurationSearchParameter p(ConfigurationPropertyPath("use_subdivisions"), false, List<int>({0, 1}));
        HELPER_TEST_PRINT(p);
    }

    void test_parameter_randomise() {
        ConfigurationSearchParameter p(ConfigurationPropertyPath("use_subdivisions"), false, List<int>({0, 1}));
        HELPER_TEST_PRINT(p);
        List<int> values;
        for (unsigned int i=0; i<16; ++i) values.push_back(p.random_value());
        HELPER_TEST_PRINT(values);
    }

    void test_metric_parameter_shift() {
        ConfigurationSearchParameter metric(ConfigurationPropertyPath("sweep_threshold"), true, List<int>({8, 9, 10, 11}));
        HELPER_TEST_EQUALS(metric.shifted_value_from(8),9);
        HELPER_TEST_EQUALS(metric.shifted_value_from(11),10);
        auto from_1 = metric.shifted_value_from(10);
        HELPER_TEST_ASSERT(from_1 == 9 or from_1 == 11);
    }

    void test_parameter_space() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5}));
        ConfigurationSearchSpace space({bp, mp});
        HELPER_TEST_PRINT(space);
        HELPER_TEST_PRINT(space.parameters());
        HELPER_TEST_EQUALS(space.dimension(),2);
        HELPER_TEST_EQUALS(space.index(bp),1);
        HELPER_TEST_EQUALS(space.index(mp),0);
        HELPER_TEST_EQUALS(space.total_points(),6);
    }

    void test_parameter_point_creation() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5}));
        ConfigurationSearchSpace space({bp, mp});
        HELPER_TEST_PRINT(space.initial_point());
        Map<ConfigurationPropertyPath,int> bindings = {{use_subdivisions,1},{sweep_threshold,5}};
        HELPER_TEST_PRINT(bindings);
        ConfigurationSearchPoint point = space.make_point(bindings);
        HELPER_TEST_PRINT(point);
        HELPER_TEST_PRINT(point.space());
    }

    void test_parameter_point_equality() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5}));
        ConfigurationSearchSpace space({bp, mp});

        ConfigurationSearchPoint point1 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 2}});
        ConfigurationSearchPoint point2 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 2}});
        ConfigurationSearchPoint point3 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 3}});
        HELPER_TEST_EQUAL(point1,point2);
        HELPER_TEST_NOT_EQUAL(point1,point3);
    }

    void test_parameter_point_distance() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5}));
        ConfigurationSearchSpace space({bp, mp});

        ConfigurationSearchPoint point1 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 2}});
        ConfigurationSearchPoint point2 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 2}});
        ConfigurationSearchPoint point3 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 3}});
        ConfigurationSearchPoint point4 = space.make_point({{use_subdivisions, 0}, {sweep_threshold, 5}});
        HELPER_TEST_EQUALS(point1.distance(point2),0);
        HELPER_TEST_EQUALS(point1.distance(point3),1);
        HELPER_TEST_EQUALS(point3.distance(point4),3);
    }

    void test_parameter_point_adjacent_shift() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5}));
        ConfigurationSearchSpace space({bp, mp});

        ConfigurationSearchPoint point1 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 5}});
        HELPER_TEST_PRINT(point1);
        auto point2 = point1.make_adjacent_shifted();
        HELPER_TEST_PRINT(point2);
        HELPER_TEST_NOT_EQUAL(point1,point2);
    }

    void test_parameter_point_random_shift() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5, 6, 7}));
        ConfigurationSearchSpace space({bp, mp});

        ConfigurationSearchPoint point = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 5}});
        auto points = point.make_random_shifted(1);
        HELPER_TEST_EQUALS(points.size(),1);
        points = point.make_random_shifted(3);
        HELPER_TEST_EQUALS(points.size(),3);
        points = point.make_random_shifted(static_cast<unsigned int>(space.total_points()));
        HELPER_TEST_EQUALS(points.size(),space.total_points());
    }

    void test_parameter_point_adjacent_set_shift() {
        ConfigurationPropertyPath use_subdivisions("use_subdivisions");
        ConfigurationPropertyPath sweep_threshold("sweep_threshold");
        ConfigurationSearchParameter bp(use_subdivisions, false, List<int>({0, 1}));
        ConfigurationSearchParameter mp(sweep_threshold, true, List<int>({3, 4, 5, 6, 7, 8}));
        ConfigurationSearchSpace space({bp, mp});
        HELPER_TEST_PRINT(space.total_points());

        ConfigurationSearchPoint point = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 5}});
        Set<ConfigurationSearchPoint> points = point.make_random_shifted(3);
        HELPER_TEST_PRINT(points);
        auto all_points = make_extended_set_by_shifting(points, 5);
        HELPER_TEST_EQUALS(all_points.size(),5);
        HELPER_TEST_PRINT(all_points);

        ConfigurationSearchPoint point1 = space.make_point({{use_subdivisions, 1}, {sweep_threshold, 8}});
        ConfigurationSearchPoint point2 = space.make_point({{use_subdivisions, 0}, {sweep_threshold, 3}});
        Set<ConfigurationSearchPoint> border_points = {point1, point2};
        HELPER_TEST_PRINT(border_points);
        HELPER_PRINT_TEST_COMMENT("Checking maximum number of single shift points including the original border points");
        auto six_points = make_extended_set_by_shifting(border_points, 6);
        HELPER_TEST_PRINT(six_points);
        HELPER_PRINT_TEST_COMMENT("Checking 1 point over the number of possible adjacent shiftings");
        auto seven_points = make_extended_set_by_shifting(border_points, 7);
        HELPER_TEST_PRINT(seven_points);
        HELPER_PRINT_TEST_COMMENT("Checking up to the maximum number");
        auto twelve_points = make_extended_set_by_shifting(border_points, 12);
        HELPER_TEST_PRINT(twelve_points);
        HELPER_PRINT_TEST_COMMENT("Checking 1 point over the maximum number");
        HELPER_TEST_FAIL(make_extended_set_by_shifting(points, point.space().total_points()+1));
    }

    void test() {
        HELPER_TEST_CALL(test_parameter_creation());
        HELPER_TEST_CALL(test_parameter_randomise());
        HELPER_TEST_CALL(test_metric_parameter_shift());
        HELPER_TEST_CALL(test_parameter_space());
        HELPER_TEST_CALL(test_parameter_point_creation());
        HELPER_TEST_CALL(test_parameter_point_equality());
        HELPER_TEST_CALL(test_parameter_point_distance());
        HELPER_TEST_CALL(test_parameter_point_adjacent_shift());
        HELPER_TEST_CALL(test_parameter_point_random_shift());
        HELPER_TEST_CALL(test_parameter_point_adjacent_set_shift());
    }
};

int main() {
    TestConfigurationSearchParameter().test();
    return HELPER_TEST_FAILURES;
}
