#include "MACE/Data/Hit.h++"
#include "MACE/Data/MMSTrack.h++"
#include "MACE/Data/SimHit.h++"
#include "MACE/ReconMMSTrack/ReconMMSTrack.h++"
#include "MACE/Reconstruction/MMSTracking/Finder/TruthFinder.h++"
#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitDAFFitter.h++"
#include "MACE/Reconstruction/MMSTracking/Fitter/GenFitReferenceKalmanFitter.h++"
#include "MACE/Reconstruction/MMSTracking/Fitter/TruthFitter.h++"

#include "Mustard/Data/Output.h++"
#include "Mustard/Data/Processor.h++"
#include "Mustard/Data/Tuple.h++"
#include "Mustard/Env/MPIEnv.h++"
#include "Mustard/Parallel/ProcessSpecificPath.h++"
#include "Mustard/Utility/VectorArithmeticOperator.h++"

#include "ROOT/RDataFrame.hxx"
#include "TFile.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace MACE::ReconMMSTrack {

ReconMMSTrack::ReconMMSTrack() :
    Subprogram{"ReconMMSTrack", "Michel magnetic spectrometer (MMS) track reconstruction."} {}

auto ReconMMSTrack::Main(int argc, char* argv[]) const -> int {
    Mustard::Env::MPIEnv env{argc, argv, {}};

    std::vector<std::string> files;
    for (auto i{1}; i < argc; ++i) {
        files.emplace_back(argv[i]);
    }

    TFile file{Mustard::Parallel::ProcessSpecificPath("output.root").generic_string().c_str(), "RECREATE"};
    Mustard::Data::Output<Data::MMSTrack> reconTrack{"G4Run0/MMSTrack"};
    Mustard::Data::Output<Data::MMSTrack> reconTrackError{"G4Run0/MMSTrackError"};

    MMSTracking::TruthFinder finder;
    // MMSTracking::TruthFitter fitter;
    MMSTracking::GenFitDAFFitter fitter{0.2};
    // fitter.EnableEventDisplay(true);

    Mustard::Data::Processor processor;
    auto nextTrackID{0};
    processor.Process<Data::CDCSimHit>(
        ROOT::RDataFrame{"G4Run0/CDCSimHit", files}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            for (auto&& [trackID, good] : finder(event, nextTrackID).good) {
                const auto track{fitter(good.hitData, good.seed).track};
                if (track == nullptr) {
                    continue;
                }
                reconTrack.Fill(*track);

                using namespace Mustard::VectorArithmeticOperator;
                Mustard::Data::Tuple<Data::MMSTrack> trackError;
                Get<"EvtID">(trackError) = Get<"EvtID">(*track);
                Get<"TrkID">(trackError) = Get<"TrkID">(*track);
                Get<"HitID">(trackError) = Get<"HitID">(*track);
                Get<"chi2">(trackError) = Get<"chi2">(*track);
                Get<"t0">(trackError) = Get<"t0">(*track) - Get<"t0">(*good.seed);
                Get<"PDGID">(trackError) = Get<"PDGID">(*track) - Get<"PDGID">(*good.seed);
                Get<"x0">(trackError) = *Get<"x0">(*track) - *Get<"x0">(*good.seed);
                Get<"Ek0">(trackError) = Get<"Ek0">(*track) - Get<"Ek0">(*good.seed);
                Get<"p0">(trackError) = *Get<"p0">(*track) - *Get<"p0">(*good.seed);
                Get<"c0">(trackError) = *Get<"c0">(*track) - *Get<"c0">(*good.seed);
                Get<"r0">(trackError) = Get<"r0">(*track) - Get<"r0">(*good.seed);
                Get<"phi0">(trackError) = Get<"phi0">(*track) - Get<"phi0">(*good.seed);
                Get<"z0">(trackError) = Get<"z0">(*track) - Get<"z0">(*good.seed);
                Get<"theta0">(trackError) = Get<"theta0">(*track) - Get<"theta0">(*good.seed);
                reconTrackError.Fill(std::move(trackError));

                nextTrackID = trackID;
            }
        });

    reconTrack.Write();
    reconTrackError.Write();
    // fitter.OpenEventDisplay();

    return EXIT_SUCCESS;
}

} // namespace MACE::ReconMMSTrack
