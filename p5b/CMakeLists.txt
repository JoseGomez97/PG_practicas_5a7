cmake_minimum_required(VERSION 2.8)

project(p5)

add_executable(p5 main.cpp)
target_link_libraries(p5 PGUPV)

include(../PGUPV/pgupv.cmake)

set_target_properties( p5 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin 
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

install(TARGETS p5 DESTINATION ${PG_SOURCE_DIR}/bin)
