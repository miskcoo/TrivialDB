cmake_minimum_required (VERSION 2.6)

project(trivial_db)

add_subdirectory(src/parser)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BINARY_DIR build)
set(EXECUTABLE_OUTPUT_PATH build)
set(SOURCE
	${SOURCE}
	src/btree/btree.cpp
	src/fs/page_fs.cpp
	src/page/variant_page.cpp
	src/table/record.cpp
	src/table/table.cpp
	src/table/table_header.cpp
	src/database/database.cpp
	src/database/dbms.cpp
	src/expression/expression.cpp
	src/expression/serialization.cpp
	src/index/index.cpp
)

set(HEADERS
	${HEADERS}
)

add_library(${CMAKE_PROJECT_NAME}_lib ${SOURCE} ${HEADERS})
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} sql_parser ${CMAKE_PROJECT_NAME}_lib)

#add_executable(test_table test/test_table.cpp)
#target_link_libraries(test_table sql_parser ${CMAKE_PROJECT_NAME}_lib)

