#!/usr/bin/env bash

# Parse command line arguments
use_hwthreads=false
for arg in "$@"; do
    case $arg in
        --use-hwthreads)
            use_hwthreads=true
            ;;
        *)
            ;;
    esac
done

script_dir="$(dirname "$(readlink -f "$0")")"
build_dir=$script_dir/..
regression_data_dir=$script_dir/regression_data_$(date --utc +%Y%m%d-%H%M%S)

mkdir "$regression_data_dir" && cd "$regression_data_dir"
echo "Working directory: $(pwd)"

start_time=$(date +%s)

source $build_dir/data/macesw_offline_data.sh

final_exit_code=0

run_command() {
    local command=("$*")
    echo "################################################################################"
    echo "# Executing \"$command\""
    echo "################################################################################"
    if "$@"; then
        echo "################################################################################"
        echo "# ✅ \"$command\" successfully completed"
        echo "################################################################################"
    else
        local exit_code=$?
        echo "################################################################################"
        echo "# ❌ \"$command\" failed with exit code $exit_code"
        echo "################################################################################"
        final_exit_code=1
    fi
}

echo "Start simulation..."
if $use_hwthreads; then
    # Use hardware threads (hyperthreading included)
    n_proc=$(nproc)
else
    # Use physical cores (default)
    threads_per_core=$(env LC_ALL=C lscpu | grep "Thread(s) per core" | awk '{print $4}')
    if [[ -z "$threads_per_core" || "$threads_per_core" -eq 0 ]]; then
        threads_per_core=1  # Fallback to assuming 1 thread per core
    fi
    n_proc=$(echo "$(nproc) / $threads_per_core" | bc)
    if [[ "$n_proc" -lt 1 ]]; then
        n_proc=1  # Ensure at least 1 core
    fi
fi
run_command mpiexec -n $n_proc $build_dir/MACE SimMMS $build_dir/SimMMS/run_em_flat.mac
run_command mpiexec -n $n_proc $build_dir/MACE SimTTC $build_dir/SimTTC/run_em_flat.mac
run_command mpiexec -n $n_proc $build_dir/MACE SimMACE $build_dir/SimMACE/run_signal.mac

echo "Merging results..."
run_command hadd -ff SimMMS_em_flat_sample.root SimMMS_em_flat_test/*
run_command hadd -ff SimTTC_em_flat_sample.root SimTTC_em_flat_test/*
run_command hadd -ff SimMACE_signal_sample.root SimMACE_signal_test/*

echo "Generating regression histograms..."
if [ -f "$script_dir/macesw_regression_data.root" ]; then
    mkdir -p "$script_dir/old-regression-data/"
    mv "$script_dir/macesw_regression_data.root" "$script_dir/old-regression-data/macesw_regression_data_$(date "+%Y%m%d-%H%M").root"
    echo "==> NOTICE: old version of macesw_regression_data.root moved to $script_dir/old-regression-data/"
fi

echo "Draw & save regression histograms..."
run_command root -l -q "$script_dir/ReadCDCSimHit.cxx(\"SimMMS_em_flat\",\"SimMMS_em_flat_sample.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/ReadMMSSimTrack.cxx(\"SimMMS_em_flat\",\"SimMMS_em_flat_sample.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/ReadTTCSimHit.cxx(\"SimTTC_em_flat\",\"SimTTC_em_flat_sample.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/ReadMCPSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/ReadTTCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/ReadCDCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/ReadMMSSimTrack.cxx(\"SimMACE_signal\",\"SimMACE_signal_sample.root\",\"$script_dir/macesw_regression_data.root\")"

end_time=$(date +%s)
total_time=$((end_time - start_time))

hours=$((total_time / 3600))
minutes=$(( (total_time % 3600) / 60 ))
seconds=$((total_time % 60))

echo "################################################################################"
echo "# Start at: $(date --iso-8601=seconds -d @$start_time)"
echo "# End at: $(date --iso-8601=seconds -d @$end_time)"
echo "# Total running time: ${hours}h ${minutes}m ${seconds}s"
if [ $final_exit_code -eq 0 ]; then
    echo "# ✅ All commands completed successfully!"
else
    echo "# ❌ Some commands failed!"
fi
echo "# Details in $regression_data_dir"
echo "################################################################################"

exit $final_exit_code
