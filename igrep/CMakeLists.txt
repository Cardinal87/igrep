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

project ("igrep")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (igrep 
"src/igrep.cpp"
"src/utils/StringUtils.cpp" 
"src/indexer/Index.cpp"
"src/searcher/Searcher.cpp" 
"src/indexer/FileIndexer.cpp")

target_include_directories(igrep PRIVATE include)


