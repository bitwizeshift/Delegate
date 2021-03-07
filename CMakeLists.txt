cmake_minimum_required(VERSION 3.6)

if (PROJECT_NAME)
  set(IS_SUBPROJECT TRUE)
endif ()

set(CATCH_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/third_party/catch/contrib")
set(DELEGATE_CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(DELEGATE_CMAKE_TEMPLATE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/templates")

set(CMAKE_MODULE_PATH "${DELEGATE_CMAKE_MODULE_PATH}" "${CATCH_MODULE_PATH}" "${CMAKE_MODULE_PATH}")

option(DELEGATE_COMPILE_UNIT_TESTS "Compile and run the unit tests for this library" ON)

if (NOT CMAKE_TESTING_ENABLED AND DELEGATE_COMPILE_UNIT_TESTS)
  enable_testing()
endif ()

project(Delegate
  VERSION "0.0.1"
  LANGUAGES CXX
)

set(DELEGATE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "Major version of Delegate")
set(DELEGATE_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "Minor version of Delegate")
set(DELEGATE_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "Patch version of Delegate")
set(DELEGATE_VERSION ${PROJECT_VERSION} CACHE INTERNAL "Version of Delegate")

##############################################################################
# Targets
##############################################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(header_files
  include/delegate.hpp
)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  INTERFACE $<INSTALL_INTERFACE:include>
)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  # clang-cl does not appear to implement '-pedantic' or 'pedantic-errors',
  # so this case needs to be handled specifically
  add_compile_options(-Wall -Werror -Wextra)

  # clang-cl treats '-Wall' like '-Weverything' currently; so there are a few
  # warnings we need to manually disable.

  # Disable unnecessary compatibility and 'newline-eof'. This is a modern C++
  # library -- so these serve no purpose
  add_compile_options(-Wno-c++98-compat -Wno-c++98-compat-pedantic)

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wall -Werror -Wextra -pedantic -pedantic-errors)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  add_compile_options(/W4 /WX)
endif ()

include(AddSelfContainmentTest)

if (DELEGATE_COMPILE_UNIT_TESTS)
  add_subdirectory("third_party/catch")

  add_self_containment_test(${PROJECT_NAME}.SelfContainmentTest
    TARGET ${PROJECT_NAME}
    HEADERS ${header_files}
  )

  add_subdirectory("test")
endif ()

##############################################################################
# Installation
##############################################################################

if (IS_SUBPROJECT)
  return()
endif ()


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(DELEGATE_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# The generated *ConfigVersion is strongly tied to the architecture
# it was generated on, and sets variables such as 'SIZEOF_VOID_P'.
# Since this library is header-only, the host architecture does not
# actually affect the target that consumes this project, so we fake
# the variable to be empty, but reset it after.
#
# Otherwise a 64-bit creator would cause a 32-bit consumer to fail to
# use this library, even though it's header-only.
set(DELEGATE_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
set(CMAKE_SIZEOF_VOID_P "")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY "SameMajorVersion"
)
set(CMAKE_SIZEOF_VOID_P ${DELEGATE_CMAKE_SIZEOF_VOID_P})

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${DELEGATE_CMAKE_CONFIG_DESTINATION}"
)

# Targets
install(
  TARGETS "${PROJECT_NAME}"
  EXPORT "${PROJECT_NAME}Targets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}Targets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${DELEGATE_CMAKE_CONFIG_DESTINATION}"
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  DESTINATION "${DELEGATE_CMAKE_CONFIG_DESTINATION}"
)

# Includes
install(
  DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
