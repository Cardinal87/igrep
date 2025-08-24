# CMakeList.txt: проект CMake для igrep; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.10)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("igrep")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (igrep "igrep.cpp" "igrep.h" 
"include/utils/StringUtils.h" "src/utils/StringUtils.cpp" 
"include/indexer/Index.h" "src/indexer/Index.cpp"
"include/indexer/Position.h" "include/indexer/FileIndexer.h" 
"include/searcher/Searcher.h" "include/searcher/SearchResult.h" 
"src/searcher/Searcher.cpp" "src/indexer/FileIndexer.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET igrep PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
