set(LIBRARY_NAME PRONEST_SRC)

add_library(${LIBRARY_NAME} OBJECT
        searchable_configuration.cpp
        configuration_search_space.cpp
        configuration_property_path.cpp
        configuration_search_parameter.cpp
        configuration_search_point.cpp
        configuration_property.cpp
        )

if(COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

foreach(WARN ${LIBRARY_EXCLUSIVE_WARN})
    target_compile_options(${LIBRARY_NAME} PUBLIC -W${WARN})
endforeach()
