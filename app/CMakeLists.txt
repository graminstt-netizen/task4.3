# Получаем список всех исходных файлов в app/src (наш main.c)
get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" APP_SOURCES)

# Создаем исполняемый файл нашей программы
add_executable(my_program ${APP_SOURCES})

# Связываем программу с библиотекой app_lib
target_link_libraries(my_program PRIVATE app_lib)