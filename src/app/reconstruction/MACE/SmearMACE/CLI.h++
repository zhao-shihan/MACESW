// -*- C++ -*-
//
// Copyright (C) 2020-2025  MACESW developers
//
// This file is part of MACESW, Muonium-to-Antimuonium Conversion Experiment
// offline software.
//
// MACESW is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// MACESW is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// MACESW. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Mustard/CLI/CLI.h++"
#include "Mustard/CLI/Module/BasicModule.h++"
#include "Mustard/CLI/Module/ModuleBase.h++"
#include "Mustard/CLI/Module/MonteCarloModule.h++"

#include "muc/hash_map"

#include "gsl/gsl"

#include <filesystem>
#include <string>
#include <utility>

namespace MACE::SmearMACE {

class CLIModule : public Mustard::CLI::ModuleBase {
public:
    explicit CLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli);

    auto InputFilePath() const -> auto { return TheCLI()->get<std::vector<std::string>>("input"); }
    auto OutputFileMode() const -> auto { return TheCLI()->present("-m").value_or("NEW"); }
    auto OutputFilePath() const -> std::filesystem::path;

    auto DatasetIndexRange() const -> std::pair<gsl::index, gsl::index>;

    auto CDCSimHitSmearingConfig() const -> auto { return ParseSmearingConfig("--cdc-hit"); }
    auto CDCSimHitIdentity() const -> bool { return TheCLI()->get<bool>("--cdc-hit-id"); }
    auto CDCSimHitNameFormat() const -> auto { return TheCLI()->present("--cdc-hit-name").value_or("G4Run{}/CDCSimHit"); }

    auto TTCSimHitSmearingConfig() const -> auto { return ParseSmearingConfig("--ttc-hit"); }
    auto TTCSimHitIdentity() const -> bool { return TheCLI()->get<bool>("--ttc-hit-id"); }
    auto TTCSimHitNameFormat() const -> auto { return TheCLI()->present("--ttc-hit-name").value_or("G4Run{}/TTCSimHit"); }

    auto MMSSimTrackSmearingConfig() const -> auto { return ParseSmearingConfig("--mms-track"); }
    auto MMSSimTrackIdentity() const -> bool { return TheCLI()->get<bool>("--mms-track-id"); }
    auto MMSSimTrackNameFormat() const -> auto { return TheCLI()->present("--mms-track-name").value_or("G4Run{}/MMSSimTrack"); }

    auto MCPSimHitSmearingConfig() const -> auto { return ParseSmearingConfig("--mcp-hit"); }
    auto MCPSimHitIdentity() const -> bool { return TheCLI()->get<bool>("--mcp-hit-id"); }
    auto MCPSimHitNameFormat() const -> auto { return TheCLI()->present("--mcp-hit-name").value_or("G4Run{}/MCPSimHit"); }

    auto ECALSimHitSmearingConfig() const -> auto { return ParseSmearingConfig("--ecal-hit"); }
    auto ECALSimHitIdentity() const -> bool { return TheCLI()->get<bool>("--ecal-hit-id"); }
    auto ECALSimHitNameFormat() const -> auto { return TheCLI()->present("--ecal-hit-name").value_or("G4Run{}/ECALSimHit"); }

private:
    auto ParseSmearingConfig(std::string_view arg) const -> muc::flat_hash_map<std::string, std::string>;
};

using CLI = Mustard::CLI::CLI<Mustard::CLI::BasicModule,
                              Mustard::CLI::MonteCarloModule,
                              CLIModule>;

} // namespace MACE::SmearMACE
