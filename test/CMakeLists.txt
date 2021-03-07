set(source_files
  src/main.cpp
  src/delegate.test.cpp
)

add_executable(${PROJECT_NAME}.test
  ${source_files}
)
add_executable(${PROJECT_NAME}::test ALIAS ${PROJECT_NAME}.test)

target_link_libraries(${PROJECT_NAME}.test
  PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
  PRIVATE Catch2::Catch2
)

set_target_properties(${UNITTEST_TARGET_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

##############################################################################
# CTest
##############################################################################

include(Catch)
catch_discover_tests(${PROJECT_NAME}.test)
