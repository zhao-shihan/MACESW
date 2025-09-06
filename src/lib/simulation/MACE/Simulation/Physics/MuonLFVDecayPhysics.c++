// -*- C++ -*-
//
// Copyright 2020-2024  The Mustard development team
//
// This file is part of Mustard, an offline software framework for HEP experiments.
//
// Mustard is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Mustard is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Mustard. If not, see <https://www.gnu.org/licenses/>.

#include "MACE/Simulation/Physics/MuonLFVDecayPhysics.h++"

#include "Mustard/Geant4X/Physics/MuonNLODecayPhysics.h++"

#include "G4DecayTable.hh"
#include "G4PhaseSpaceDecayChannel.hh"

namespace MACE::inline Simulation::inline Physics {

MuonLFVDecayPhysics::MuonLFVDecayPhysics(G4int verbose) :
    MuonNLODecayPhysics{verbose},
    fDoubleRadiativeDecayBR{},
    fMessengerRegister{this} {}

auto MuonLFVDecayPhysics::InsertDecayChannel(const G4String& parentName, gsl::not_null<G4DecayTable*> decay) -> void {
    MuonNLODecayPhysics::InsertDecayChannel(parentName, decay);
    if (parentName == "mu+") {
        decay->Insert(new G4PhaseSpaceDecayChannel{parentName, 1e-4, 3, "e+", "gamma", "gamma"});
    } else if (parentName == "mu-") {
        decay->Insert(new G4PhaseSpaceDecayChannel{parentName, 1e-4, 3, "e-", "gamma", "gamma"});
    } else {
#ifdef G4VERBOSE
        if (GetVerboseLevel() > 0) {
            G4cout << "MuonInternalConversionDecayChannel::(Constructor) says\n"
                      "\tParent particle is not mu+ or mu- but "
                   << parentName << G4endl;
        }
#endif
    }
}

auto MuonLFVDecayPhysics::AssignMinorDecayBR(gsl::not_null<G4DecayTable*> decay) -> void {
    MuonNLODecayPhysics::AssignMinorDecayBR(decay);
    decay->GetDecayChannel(3)->SetBR(fDoubleRadiativeDecayBR);
}

} // namespace MACE::inline Simulation::inline Physics
