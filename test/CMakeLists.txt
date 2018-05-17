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

find_package(PythonInterp)
set(STRUC_PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE STRING "Python executable used for testing")
option(STRUC_CHECK_PYTHON "Compare to pythoin" ON)
add_executable(test_struc test.cpp)
if(PYTHONINTERP_FOUND)
    target_compile_definitions(test_struc PRIVATE STRUC_PYTHON_EXECUTABLE="${STRUC_PYTHON_EXECUTABLE}")
endif()
if(STRUC_CHECK_PYTHON)
    target_compile_definitions(test_struc PRIVATE STRUC_CHECK_PYTHON)
endif()
target_include_directories(test_struc PRIVATE ../catch/single_include)
target_link_libraries(test_struc struc)
set_target_properties(test_struc PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_test(NAME test_struc COMMAND test_struc)
