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

driver_script_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
test_src_dir=$driver_script_dir
build_dir=$test_script_dir/..
regression_data_dir=$test_script_dir/regression_data_$(date --utc +%Y%m%d-%H%M%S)

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

if [ -f "$test_src_dir/macesw_regression_data.root" ]; then
    mkdir -p "$test_src_dir/old-regression-data/"
    mv "$test_src_dir/macesw_regression_data.root" "$script_dir/old-regression-data/outdated_at_$(date "+%Y%m%d-%H%M").root"
    echo "==> NOTICE: old version of macesw_regression_data.root moved to $script_dir/old-regression-data/"
fi

echo "Start simulation..."
source $test_src_dir/simulation/MACE/SimMACE/generate_regression_data.sh
source $test_src_dir/simulation/MACE/SimMMS/generate_regression_data.sh
source $test_src_dir/simulation/MACE/SimTTC/generate_regression_data.sh

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
