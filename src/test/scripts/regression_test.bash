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
test_dir=$script_dir/test_$(date --utc +%Y%m%d-%H%M%S)

mkdir "$test_dir" && cd "$test_dir"
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
        return 0
    else
        local exit_code=$?
        echo "################################################################################"
        echo "# ❌ \"$command\" failed with exit code $exit_code"
        echo "################################################################################"

        end_time=$(date +%s)
        total_time=$((end_time - start_time))
        hours=$((total_time / 3600))
        minutes=$(( (total_time % 3600) / 60 ))
        seconds=$((total_time % 60))

        echo "#################################################################################"
        echo "# Start at: $(date --iso-8601=seconds -d @$start_time)"
        echo "# End at: $(date --iso-8601=seconds -d @$end_time)"
        echo "# Total running time: ${hours}h ${minutes}m ${seconds}s"
        echo "# ❌ Command failed! Stopping execution."
        echo "# Details in $test_dir"
        echo "#################################################################################"

        exit $exit_code
    fi
}

parexec() {
    if $use_hwthreads; then
        # Use hardware threads (hyperthreading included)
        if [[ -n "$(echo $(mpiexec --version 2>/dev/null) | grep "Open MPI")" ]]; then
            mpiexec --allow-run-as-root --use-hwthread-cpus $@
        else
            mpiexec -n $(nproc) $@
        fi
    else
        # Use physical cores (default)
        local threads_per_core=$(env LC_ALL=C lscpu | grep "Thread(s) per core" | awk '{print $4}')
        if [[ -z "$threads_per_core" || "$threads_per_core" -eq 0 ]]; then
            threads_per_core=1  # Fallback to assuming 1 thread per core
        fi
        local n_physical_cores=$(echo "$(nproc) / $threads_per_core" | bc)
        if [[ "$n_physical_cores" -lt 1 ]]; then
            n_physical_cores=1  # Ensure at least 1 core
        fi
        if [[ -n "$(echo $(mpiexec --version 2>/dev/null) | grep "Open MPI")" ]]; then
            mpiexec --allow-run-as-root -n $n_physical_cores $@
        else
            mpiexec -n $n_physical_cores $@
        fi
    fi
}

echo "Start simulation..."
run_command parexec $build_dir/MACE SimMMS --seed 0 $build_dir/SimMMS/run_em_flat.mac
run_command parexec $build_dir/MACE SimTTC --seed 0 $build_dir/SimTTC/run_em_flat.mac
run_command parexec $build_dir/MACE SimMACE --seed 0 $build_dir/SimMACE/run_signal.mac

echo "Merging results..."
run_command hadd -ff SimMMS_em_flat_test.root SimMMS_em_flat_test/*
run_command hadd -ff SimTTC_em_flat_test.root SimTTC_em_flat_test/*
run_command hadd -ff SimMACE_signal_test.root SimMACE_signal_test/*

echo "Generating regression report..."
run_command root -l -q "$script_dir/TestCDCSimHit.cxx(\"SimMMS_em_flat\",\"SimMMS_em_flat_test.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/TestMMSSimTrack.cxx(\"SimMMS_em_flat\",\"SimMMS_em_flat_test.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/TestTTCSimHit.cxx(\"SimTTC_em_flat\",\"SimTTC_em_flat_test.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/TestMCPSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/TestTTCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/TestCDCSimHit.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$script_dir/macesw_regression_data.root\")"
run_command root -l -q "$script_dir/TestMMSSimTrack.cxx(\"SimMACE_signal\",\"SimMACE_signal_test.root\",\"$script_dir/macesw_regression_data.root\")"

end_time=$(date +%s)
total_time=$((end_time - start_time))

hours=$((total_time / 3600))
minutes=$(( (total_time % 3600) / 60 ))
seconds=$((total_time % 60))

echo "################################################################################"
echo "# Start at: $(date --iso-8601=seconds -d @$start_time)"
echo "# End at: $(date --iso-8601=seconds -d @$end_time)"
echo "# Total running time: ${hours}h ${minutes}m ${seconds}s"
echo "# ✅ All commands completed successfully!"
echo "# Details in $test_dir"
echo "################################################################################"

exit 0
