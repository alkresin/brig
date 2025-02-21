cmake_minimum_required(VERSION 2.8)

project(brig)
include_directories(include)
set(W source/winapi)
set(C source/common)
set(SOURCE_LIB
   ${W}/commond.cpp
   ${W}/control.cpp
   ${W}/draw.cpp
   ${W}/menu.cpp
   ${W}/message.cpp
   ${W}/misc.cpp
   ${W}/strings.cpp
   ${W}/table.cpp
   ${W}/window.cpp
   ${C}/brig_app.cpp
   ${C}/brig_base.cpp
   ${C}/brig_funcs.cpp
   ${C}/brig_menu.cpp
   ${C}/brig_style.cpp
   ${C}/brig_table.cpp
   ${C}/brig_widgets.cpp
   ${C}/brig_xml.cpp )

add_library(brig STATIC ${SOURCE_LIB})
