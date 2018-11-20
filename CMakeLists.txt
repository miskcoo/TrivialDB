cmake_minimum_required (VERSION 2.6)

project(trivial_db)

set(CMAKE_BINARY_DIR build)
set(EXECUTABLE_OUTPUT_PATH build)
set(SRC_LIST
	src/btree/btree.cpp
	src/fs/page_fs.cpp
	src/page/variant_page.cpp
	src/table/record.cpp
	src/table/table.cpp
)

add_executable(test_table ${SRC_LIST} test/test_table.cpp)

