cmake_minimum_required (VERSION 2.6)

PROJECT(trivial_db)
ADD_DEFINITIONS(-std=c++11 -O2 -Wall)

SET(SRC_LIST
	src/btree/btree.cpp
	src/fs/page_fs.cpp
	src/page/variant_page.cpp
	src/table/record.cpp
	src/table/table.cpp
)

ADD_EXECUTABLE(build/test_table ${SRC_LIST} test/test_table.cpp)

