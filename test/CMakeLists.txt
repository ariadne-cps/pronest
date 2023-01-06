include(CTest)

set(UNIT_TESTS
    test_configuration_property
    test_configuration_property_path
    test_searchable_configuration
)

foreach(TEST ${UNIT_TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} conclog)
    add_test(${TEST} ${TEST})
endforeach()

add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})
