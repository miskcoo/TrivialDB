cmake_minimum_required (VERSION 2.6)

project(trivial_db)

add_subdirectory(src/parser)

set(CMAKE_BINARY_DIR build)
set(EXECUTABLE_OUTPUT_PATH build)
set(SOURCE
	src/btree/btree.cpp
	src/fs/page_fs.cpp
	src/page/variant_page.cpp
	src/table/record.cpp
	src/table/table.cpp
)

set(HEADERS
	${HEADERS}
)

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE} ${HEADERS})
add_executable(test_table test/test_table.cpp)
target_link_libraries(test_table sql_parser ${CMAKE_PROJECT_NAME}_lib)

