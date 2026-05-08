#!/usr/bin/env bash

source_dir="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
test_src_dir=$source_dir/..
build_dir=$test_src_dir/..

module_name=$1
shift
test_dir=$test_src_dir/test_${module_name}_$(date --utc +%Y%m%d-%H%M%S)
mkdir "$test_dir" && cd "$test_dir"
echo "Working directory: $(pwd)"

source $source_dir/run_command-source.sh
source $source_dir/parexec-source.sh "$@"

start_time=$(date +%s)

golden_file="$test_src_dir/macesw_regression_data.root"
if [[ ! -f "$golden_file" ]]; then
    echo "ERROR: $golden_file not found."
    exit 1
fi

source $build_dir/data/macesw_offline_data.sh
