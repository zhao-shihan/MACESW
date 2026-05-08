#!/usr/bin/env bash
# --SimTTC regression test script--

module_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

source $module_dir/../../../source/init-source.sh 'SimTTC' "$@"

echo "Start SimTTC"
run_command parexec $build_dir/MACE SimTTC --seed 0 $build_dir/SimTTC/run_em_flat.mac

echo "Merging results..."
if [ -d "SimTTC_em_flat_test" ]; then
    run_command hadd -ff SimTTC_em_flat_test.root SimTTC_em_flat_test/*.root
else
    echo "Single file already present."
fi

echo "Generating regression report..."
run_command root -l -q "$module_dir/TestTTCSimHit.cxx(\"SimTTC_em_flat\",\"SimTTC_em_flat_test.root\",\"$golden_file\")"

source $module_dir/../../../source/summarize-source.sh
