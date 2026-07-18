# Получаем список всех исходных файлов в lib/src
get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" LIB_SOURCES)

# Создаем библиотеку (используем исходники и файл экспорта lib.def)
add_library(app_lib ${LIB_SOURCES} lib.def)

# Указываем путь к заголовочным файлам для всех, кто линкуется с этой библиотекой
target_include_directories(app_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")