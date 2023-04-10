include(CTest)

set(UNIT_TESTS
    test_configuration_property
    test_configuration_property_path
    test_configuration_search_parameter
    test_searchable_configuration
)

foreach(TEST ${UNIT_TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} pronest)
    add_test(${TEST} ${TEST})
endforeach()

add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})
