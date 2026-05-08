# --SimTTC regression data generation script--
# IMPORTANT: This file is not a standalone executable.
# 			 It must be sourced by the main test driver script, which is responsible
# 			 for defining the 'run_command' helper function and the required
# 			 environment variables (n_proc, build_dir, test_src_dir).

simttc_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

echo "Start SimTTC"
run_command mpiexec -n $n_proc $build_dir/MACE SimTTC $build_dir/SimTTC/run_em_flat.mac

echo "Merging results..."
run_command hadd -ff SimTTC_em_flat_sample.root SimTTC_em_flat_test/*

echo "Drawing & saving regression histograms..."
run_command root -l -q "$simttc_dir/ReadTTCSimHit.cxx(\"SimTTC_em_flat\",\"SimTTC_em_flat_sample.root\",\"$test_src_dir/macesw_regression_data.root\")"

