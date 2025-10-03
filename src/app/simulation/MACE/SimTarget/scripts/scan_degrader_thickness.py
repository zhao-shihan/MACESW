#!/usr/bin/env python3

import argparse
import numpy as np
import subprocess
import sys
import yaml


def write_yaml_config(thickness, filename="scan_degrader_thickness.yaml"):
    config = {
        "BeamDegrader": {
            "Thickness": thickness
        }
    }

    with open(filename, 'w') as file:
        yaml.dump(config, file, default_flow_style=False)

    print(f"Written config: {filename}, Thickness: {thickness}")


def run_simulation():
    try:
        # Run MACE simulation with mpirun and capture output in real-time
        process = subprocess.Popen(
            ["mpirun", "MACE", "SimTarget", "scan_degrader_thickness.mac"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,  # Combine stdout and stderr
            universal_newlines=True,
            bufsize=1
        )

        # Print output in real-time
        for line in process.stdout:
            print(line, end='')  # Print each line as it comes

        # Wait for process to complete
        process.wait()

        # Check return code
        if process.returncode == 0:
            print("Simulation completed successfully")
            return True
        else:
            print(f"Simulation failed with return code: {process.returncode}")
            return False

    except FileNotFoundError:
        print(
            "Error: MACE or mpirun command not found. Please ensure they are in your PATH")
        return False
    except Exception as e:
        print(f"Unexpected error during simulation: {e}")
        return False


def main():
    parser = argparse.ArgumentParser(
        description="Degrader thickness scan script")
    parser.add_argument("start", type=float, help="Starting thickness value")
    parser.add_argument("end", type=float, help="Ending thickness value")
    parser.add_argument("step", type=float, help="Thickness step size")

    args = parser.parse_args()

    # Validate parameters
    if args.step == 0:
        print("Error: Step size cannot be zero")
        return 1

    if (args.end - args.start) * args.step <= 0:
        print("Error: Invalid thickness range or step direction")
        return 1

    # Generate thickness values
    thickness_values = np.arange(args.start, args.end, args.step)
    if thickness_values[-1] != args.end:
        thickness_values = np.append(thickness_values, args.end)
    thickness_values = thickness_values.tolist()

    print(
        f"Will scan {len(thickness_values)} thickness values: {thickness_values}")

    # Execute simulations for each thickness
    successful_runs = 0
    for i, thickness in enumerate(thickness_values):
        print(
            f"\n=== Run {i+1}/{len(thickness_values)}: Thickness = {thickness} ===")

        # Write configuration file
        write_yaml_config(thickness)

        # Execute simulation
        if run_simulation():
            successful_runs += 1

    print(f"\n=== Scan Complete ===")
    print(f"Successful runs: {successful_runs}/{len(thickness_values)}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
