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

file(GLOB SimTTC_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*)
foreach(_scripts ${SimTTC_SCRIPTS})
    set(SimTTC_SCRIPTS_COPY_DIR SimTTC)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${SimTTC_SCRIPTS_COPY_DIR})
    configure_file(${_scripts} ${CMAKE_BINARY_DIR}/${SimTTC_SCRIPTS_COPY_DIR} COPYONLY)
    install(FILES ${_scripts} DESTINATION ${MACESW_DATAROOTDIR}/${SimTTC_SCRIPTS_COPY_DIR})
endforeach()
