#!/usr/bin/env bash

end_time=$(date +%s)
total_time=$((end_time - start_time))

hours=$((total_time / 3600))
minutes=$(((total_time % 3600) / 60))
seconds=$((total_time % 60))

echo "################################################################################"
echo "# Start at: $(date --iso-8601=seconds -d @$start_time)"
echo "# End at: $(date --iso-8601=seconds -d @$end_time)"
echo "# Total running time: ${hours}h ${minutes}m ${seconds}s"
if [[ "${final_exit_code}" -eq 0 ]]; then
    echo "# ✅ All commands completed successfully!"
else
    echo "# ❌ final_exit_code is changed somehow, check it"
fi
echo "# Details in $test_dir"
echo "################################################################################"

 exit "${final_exit_code}"
