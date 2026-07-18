cmake_minimum_required(VERSION 3.15)

project(MyProject C)

# Устанавливаем стандарт C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Настройки для компилятора MSVC (если собираем под Windows)
if(MSVC)
    add_compile_options(/wd5045)
    
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    link_libraries(msvcrt.lib)
    link_libraries(legacy_stdio_definitions.lib)
endif()

# Подключаем кастомные функции cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MyFunctions)

# Опция для сборки динамической библиотеки (по умолчанию выключена — собираем статическую)
option(BUILD_SHARED_LIBS "Build libraries as shared (dynamic)" OFF)

# Подключаем поддиректории библиотеки и самого приложения
add_subdirectory(lib)
add_subdirectory(app)