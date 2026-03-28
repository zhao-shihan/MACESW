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

file(GLOB_RECURSE MACESW_RECONSTRUCTION_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/MACE/*.c++
${CMAKE_CURRENT_SOURCE_DIR}/MACE/PhaseI/reconstruction/GenFitExtension/*.c++   
)

SET(MEASUREMENTS_DICTIONARY_SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/MACE/PhaseI/reconstruction/GenFitExtension/HelixMeasurement.h++
)
ROOT_GENERATE_DICTIONARY(
	ExtensionmeasurementsRootDict
	${MEASUREMENTS_DICTIONARY_SOURCES}
	LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/MACE/PhaseI/reconstruction/GenFitExtension/measurementsLinkDef.h)
SET(library_sources ${CMAKE_CURRENT_BINARY_DIR}/ExtensionmeasurementsRootDict.cxx ${library_sources})

add_library(MACEReconstruction STATIC ${MACESW_RECONSTRUCTION_SRC})
target_include_directories(MACEReconstruction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GENFIT_INCLUDE_DIRS})
target_link_libraries(MACEReconstruction PUBLIC MACEData MACEDetector Mustard::Mustard genfit2)
