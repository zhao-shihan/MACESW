#!/usr/bin/env bash
# --SimMMS regression test script--

module_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

source $module_dir/../../../source/init-source.sh 'SimMMS' "$@"

echo "Start SimMMS"
run_command parexec $build_dir/MACE SimMMS --seed 0 $build_dir/SimMMS/run_em_flat.mac

echo "Merging results..."
if [ -d "SimMMS_em_flat_test" ]; then
    run_command hadd -ff SimMMS_em_flat_test.root SimMMS_em_flat_test/*.root
else
    echo "Single file already present."
fi

echo "Generating regression report..."
run_command root -l -q "$module_dir/TestCDCSimHit.cxx(\"SimMMS_em_flat\",\"SimMMS_em_flat_test.root\",\"$golden_file\")"
run_command root -l -q "$module_dir/TestMMSSimTrack.cxx(\"SimMMS_em_flat\",\"SimMMS_em_flat_test.root\",\"$golden_file\")"

source $module_dir/../../../source/summarize-source.sh
