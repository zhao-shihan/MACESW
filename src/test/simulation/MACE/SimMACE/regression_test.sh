#!/usr/bin/env bash
# --SimMACE regression test script--

module_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

source $module_dir/../../../source/init-source.sh 'SimMACE' "$@"

echo "Start SimMACE"
run_command parexec $build_dir/MACE SimMACE --seed 0 $build_dir/SimMACE/run_signal.mac

echo "Merging results..."
if [ -d "SimMACE_signal_test" ]; then
    run_command hadd -ff SimMACE_signal_test.root SimMACE_signal_test/*.root
else
    echo "Single file already present."
fi

echo "Generating regression report..."
run_command root -l -q "$module_dir/TestMCPSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$golden_file\")"
run_command root -l -q "$module_dir/TestTTCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$golden_file\")"
run_command root -l -q "$module_dir/TestCDCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$golden_file\")"
run_command root -l -q "$module_dir/TestMMSSimTrack.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$golden_file\")"

source $module_dir/../../../source/summarize-source.sh
