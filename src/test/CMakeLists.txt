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

set(SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/simulation"
    "${CMAKE_CURRENT_SOURCE_DIR}/source"
)
set(SOURCE_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_regression_data_dr.sh"
    "${CMAKE_CURRENT_SOURCE_DIR}/regression_test_all.sh"
)
set(DEST_DIR "${CMAKE_BINARY_DIR}/test")

file(COPY ${SOURCE_DIR} ${SOURCE_FILE}
     DESTINATION "${DEST_DIR}"
     FILES_MATCHING 
     REGEX ".*\\.template$" EXCLUDE
     REGEX ".*\\.cxx$" 
     REGEX ".*\\.sh$" 
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
