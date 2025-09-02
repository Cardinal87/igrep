# CMakeList.txt: проект CMake для igrep; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("igrep" VERSION 1.1.0)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (igrep 
"src/igrep.cpp"
"src/utils/StringUtils.cpp" 
"src/indexer/Index.cpp"
"src/searcher/Searcher.cpp" 
"src/indexer/FileIndexer.cpp")

target_include_directories(igrep PRIVATE include)
target_compile_definitions(igrep PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")

#installation
install(TARGETS igrep DESTINATION bin)
set(CPACK_PACKAGE_NAME "igrep")
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Indexed version of base Unix grep")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Cardinal87 <googl.acc.2651@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/cardinal87/igrep")
include(CPack)

#tests
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(igrep_tests
  tests/test_index.cpp
  tests/test_file_indexer.cpp
  tests/test_string_utils.cpp
  tests/test_searcher.cpp
  src/indexer/Index.cpp
  src/indexer/FileIndexer.cpp
  src/utils/StringUtils.cpp
  src/searcher/Searcher.cpp
)

target_link_libraries(igrep_tests PRIVATE gtest gtest_main)
target_include_directories(igrep_tests PRIVATE include)
target_compile_definitions(igrep_tests PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_test(NAME AllTests COMMAND igrep_tests)
