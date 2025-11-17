# Copyright (C) 2020-2025  MACESW developers
#
# This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
# offline software.
#
# MACESW is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# MACESW. If not, see <https://www.gnu.org/licenses/>.

file(GLOB_RECURSE MACESW_DATA_MODEL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++)

add_library(MACEData STATIC ${MACESW_DATA_MODEL_SRC})
target_include_directories(MACEData PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MACEData PUBLIC Mustard::Mustard)
