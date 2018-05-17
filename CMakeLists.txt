#
#   struc, A C++11 implementation of python's struct module.
#
#   struc is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#   Copyright (c) 2018, emJay Software Consulting AB, See AUTHORS for details.
#

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(struc CXX)

find_package(Boost 1.58 REQUIRED)
add_library(struc INTERFACE)
target_include_directories(struc INTERFACE include ${Boost_INCLUDE_DIRS})
target_link_libraries(struc INTERFACE m)
install(FILES include/struc.hpp DESTINATION include)

option(STRUC_BUILD_TESTS "Build tests" OFF)

if(STRUC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
