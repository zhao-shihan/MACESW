# --SimMACE regression data generation script--
# IMPORTANT: This file is not a standalone executable.
# 			 It must be sourced by the main test driver script, which is responsible
# 			 for defining the 'run_command' helper function and the required
# 			 environment variables (n_proc, build_dir, test_src_dir).

simmace_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

echo "Start SimMACE"
run_command mpiexec -n $n_proc $build_dir/MACE SimMACE $build_dir/SimMACE/run_signal.mac

echo "Merging results..."
run_command hadd -ff SimMACE_signal_sample.root SimMACE_signal_test/*

echo "Drawing & saving regression histograms..."
run_command root -l -q "$simmace_dir/ReadMCPSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$test_src_dir/macesw_regression_data.root\")"
run_command root -l -q "$simmace_dir/ReadTTCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$test_src_dir/macesw_regression_data.root\")"
run_command root -l -q "$simmace_dir/ReadCDCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$test_src_dir/macesw_regression_data.root\")"
run_command root -l -q "$simmace_dir/ReadMMSSimTrack.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$test_src_dir/macesw_regression_data.root\")"
