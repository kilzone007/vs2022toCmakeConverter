cmake_minimum_required(VERSION 3.16)

set(CONVERTER_LIB_NAME vs_converter)
add_project(LIBRARY STATIC TARGET ${CONVERTER_LIB_NAME} DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/converter LINK pugixml fmt)