# MACESW Regression Test Guide

This guide describes how to do regression tests locally and how to add regression test models in CI step-by-step.

For instructions on following the regression testing workflow, see [this section](#add-regression-test-units-in-ci) and those preceding it. The remaining sections detail the mechanisms of MACESW regression testing, serving as a reference for developers who wish to understand or enhance this workflow.

## Table of Contents
- [MACESW Regression Test Guide](#macesw-regression-test-guide)
  - [Table of Contents](#table-of-contents)
  - [Naming Convention](#naming-convention)
  - [Do Regression Test Locally](#do-regression-test-locally)
  - [Add Regression Test Units in CI](#add-regression-test-units-in-ci)
    - [Test a New Datatuple](#test-a-new-datatuple)
    - [Test a New Model](#test-a-new-model)
  - [Regression Test Mechanism of MACESW](#regression-test-mechanism-of-macesw)
    - [High-level Workflow](#high-level-workflow)
    - [Data and Histogram Conventions](#data-and-histogram-conventions)
    - [Files Produced](#files-produced)
  - [Notice About Details](#notice-about-details)
  - [Edge Cases and Notes](#edge-cases-and-notes)

## Naming Convention
The MACESW program has different simulation/generation/reconstruction units, each of which produce data based on the data model of MACESW.   
In this guide
- Each subprogram (or unit) of MACESW (e.g. `SimMACE`, `ReconECAL`, `GenM2ENNE`...) is denoted by the term "model".
- Each TTree of data (with multiple columns of data) in the output `*.root` files (e.g. `SimCDCHit`, `SimDecayVertex`, `SimMMSTrack`...) is called a "datatuple".

## Do Regression Test Locally
If no new datatuples or modules are to be tested in your update: 
- Build program MACE
- Run `build/test/regression_test.sh` with bash, a directory, named by the date and time `test_YYYYMMDD-HHMMSS` and created under `build/test/` will be the working directory of regression test.
- Check the standard output of each data's verdict of regression test.
- For more information about regression test, check the file `regression_report.root` in the working directory of regression test. 

If you need to test a new datatuple or a new model, refer to the section [Add Regression Test Units in CI](#add-regression-test-units-in-ci) below.

## Add Regression Test Units in CI

### Test a New Datatuple
- First of all, determine:
  - Which models will generate this new datatuple and which of them need to be tested.
  - Which columns in this new datatuple you want to test for those models as described above (e.g. `d`, `Edep`, `unitID`...).
- Based on the data distribution characteristics of these columns, set an appropriate range for regression testing. Generally there are 3 typical range settings:
    - $[min(x), max(x)]$ if the distribution is a broad spectrum.
    - $[\bar{x}-a\sigma_x, \bar{x}+a\sigma_x]$ if the distribution is a characteristic peak.
    - A limited set of integers if the data consists of a limited set of integer values (e.g. unitID).
- Write a `Read<datatuple_name>.cxx` macro file for generating golden data and a `Test<datatuple_name>.cxx` macro file for testing in directory `src/test/scripts/`, based on the provided template file `ReadSomeDataTuple.cxx.template` and `TestSomeDataTuple.cxx.template` (*copy the template file and modify according to the guide text in it*).

### Test a New Model
- If the datatuples you want to test in this new model are already provided, just add extra `Read*.cxx(...)` lines in file `src/test/scripts/generate_regression_data.bash` and `Test*.cxx(...)` lines in file `src/test/scripts/regression_test.bash`.
- If there are new datatuples you want to test in this new model, follow the section above: [Test a new datatuple](#test-a-new-datatuple)


---

> [!NOTE]
> Below details mechanism of regression test of MACESW, provided for those who have ideas about improving this workflow.


## Regression Test Mechanism of MACESW
<img src="picture/Regression_test_mechanism.svg" alt="Intuitive flowchart showing the MACESW regression test workflow.">

This section explains how MACESW generates "golden" regression data (the left branch shown in the image above) and how it verifies new outputs against those golden data (the right branch shown in the image above). The repository provides two main driver scripts under `src/test/scripts`: `generate_regression_data.bash` (create/update the golden data) and `regression_test.bash` (run a test and compare current outputs to the golden data). The comparison itself is implemented in a set of ROOT macros (`Read*.cxx` to produce golden histograms and `Test*.cxx` to run comparisons and write a report).

### High-level Workflow
- Data generation (create golden / sample data)  
  driver script: `generate_regression_data.bash`
  1. Run selected units (e.g., `SimMMS`, `SimTTC`, `SimMACE`). The script launches the binaries with the configured macros (e.g., `run_em_flat.mac`, `run_signal.mac`) using MPI (`mpiexec`).
  2. Merge per-process ROOT outputs into a single sample file per unit using `hadd -ff`. Example output names created by the script:
     - `SimMMS_em_flat_sample.root`
     - `SimTTC_em_flat_sample.root`
     - `SimMACE_signal_sample.root`
  3. Run the `Read*.cxx` ROOT macros (e.g., ReadCDCSimHit.cxx, `ReadMMSSimTrack.cxx`, `ReadTTCSimHit.cxx`) to produce histogram summaries from the sample ROOT files. These macros create and append histograms into `macesw_regression_data.root` (the golden data file), placed in the scripts directory. Each macro writes under a TDirectory named after the Module (e.g., `SimMACE_signal/`) and a subdirectory matching the datatuple (e.g.`CDCSimHit/`), which contains the histograms of selected column of data to be tested (e.g. `Edep`, `d`...).
   
- Regression testing (compare current output to golden)  
  driver script: `regression_test.bash`
  1. Run the same simulation units as the generation process, merges outputs into `*_test.root` files:
     - `SimMMS_em_flat_test.root`
     - `SimTTC_em_flat_test.root`
     - `SimMACE_signal_test.root`
  2. Run the `Test*.cxx` ROOT macros (e.g., `TestCDCSimHit.cxx`, `TestMMSSimTrack.cxx`) which:
     - Open the golden file `macesw_regression_data.root` and read the list of stored histograms for each Module/data-tuple.
     - Build corresponding histograms from the test ROOT file using ***identical bin counts and ranges*** (the test histograms are produced in real time in `Test*.cxx` from the test file using the same expression/column names).
     - Perform a statistical comparison using `TH1::Chi2Test` with the `"P"` option to obtain a p-value.
     - Based on the p-value, evaluate the homogeneity between the tested data and golden data and print per-histogram verdicts to stdout (**PASS**, **SUSPICIOUS**, **FAIL**, **IDENTICAL**).
     - Save visual comparisons and a pull histogram into `regression_report.root` (canvases overlaying sample vs test and the pull distribution). 

### Data and Histogram Conventions
- Histogram storage location:
  - Golden histograms are stored in `macesw_regression_data.root` under `/<ModuleName>/<DataTupleName>/` (each histogram named by the column/expression).
- Bin count and ranges:
  - The read macros create histograms with a fixed bin count of 100 by default (see `nBinsValueType` constant).
  - Histogram ranges are computed from the sample data at generation time. The macros compute a small margin around the computed range so the test histograms use identical bin edges.
- Normalization and pulls:
  - During comparison the test code normalizes both sample and test histograms (scale to unit integral) before computing pulls.
  - A pull histogram (difference divided by combined error per bin) is produced, plotted, and written into `regression_report.root`.

### Files Produced
- Golden/sample creation:
  - `macesw_regression_data.root` — the canonical golden dataset containing histograms used for comparisons.
  - `*_sample.root` — merged sample ROOT files created during generation.
- Test run:
  - `*_test.root` — merged test ROOT files from the test run.
  - `regression_report.root` — per-test visual report with canvases and pull histograms.
  - The scripts create a timestamped working directory for logs and results (e.g., `test_YYYYMMDD-HHMMSS`).

## Notice About Details
Important operational details and assumptions you must follow so the test scripts run reliably and produce meaningful comparisons.

- Required tools and environment
  - The scripts expect: ROOT (with RDataFrame), `hadd` (part of ROOT utilities), MPI (`mpiexec`), standard GNU tools used in the scripts (`bc`, `lscpu`, `date`, etc.). Make sure these are available on your PATH.
  - The scripts source `data/macesw_offline_data.sh` from the build tree. Ensure the build directory used by the scripts points to a valid built tree with `MACE` and the `data/` helper script available.

- Working directories and artifacts
  - Both driver scripts create timestamped working directories under which they run and store temporary outputs; the script prints the working directory at start. Inspect those directories when debugging.
  - generate_regression_data.bash moves an existing `macesw_regression_data.root` into `old-regression-data/` before writing a new golden file. Check that folder if you need the previous golden data.

- Naming conventions (recommended and used by the scripts)
  - `*_sample.root` — merged sample files produced by the generation script. These are the outputs of `hadd` when creating the golden dataset (e.g., `SimMMS_em_flat_sample.root`).
  - `*_test.root` — merged outputs produced for a test run (the test-run `.mac` files in this repository are configured to produce output filenames containing `_test`; regression_test.bash merges per-rank outputs into these files).
  - `macesw_regression_data.root` — the canonical golden dataset (contains TDirectories per Module and per data-tuple with the stored histograms).
  - `regression_report.root` — the visual report produced by the `Test*.cxx` macros (canvases and pull histograms); the macros open it in update mode and append results.

- Important rules about Read / Test macros and TDirectory layout
  - A single `macesw_regression_data.root` must not be written twice for the same `<Module>/<DataTuple>` by repeated `Read*` calls. Otherwise the `Read*` macros returns with an error. 
  - Before running any `Read<DataTuple>.cxx` macro, ensure the destination golden file is writable and that there is no conflicting `<Module>/<DataTuple>` directory inside it (or intentionally archive the old golden file). The script `generate_regression_data.bash` already moves the old golden file to `old-regression-data/` to avoid this conflict when regenerating.
  - Before running any `Test<DataTuple>.cxx` macro, confirm that `macesw_regression_data.root` contains the expected `<Module>/<DataTuple>` directory and histograms. If the sample folder or the expected histograms are missing, the test macro will print an error and exit.

- How histograms and binning are handled (consequences)
  - The `Read*` macros compute histogram ranges and create histograms with fixed bin count (default `nBinsValueType = 100`). 
  - Histogram ranges are computed from the sample and the code adds a small margin. Because bin counts and edges are fixed at golden-creation time, the `Test*` macros reconstruct test histograms with the identical number of bins and exact min/max edges read from the golden histograms. Therefore:
    - If you change the histogram expressions, column names, bin count or range logic, the golden dataset (`macesw_regression_data.root`) must be regenerated.
    - Avoid manually change bin edges in the golden file as possible.
  - The range of each histogram is specified by the distributional characteristics of the data. Generally, for a tested data column $x$ (named "X" for example), there are 3 main range settings:
    - $[min(x), max(x)]$ if the distribution is a broad spectrum.
    - $[\bar{x}-a\sigma_x, \bar{x}+a\sigma_x]$ if the distribution is a characteristic peak.
    - A limited set of integers if the data consists of a limited set of integer values (e.g. unitID).
  - The `Test*` macros normalize sample and test histograms to unit integral before computing pulls. If a histogram has zero integral, the test macro prints a warning and skips scaling for that histogram to avoid division-by-zero.

- Practical pre-check list before running scripts
  - Ensure `MACE` binary in the build directory is built and executable.
  - Ensure `ROOT` and `hadd` are usable from the shell.
  - Confirm `data/macesw_offline_data.sh` exists and is reachable from `build_dir`.
  - If running regression_test.bash, make sure `build/test/macesw_regression_data.root` (the golden file) contains the Module/data-tuple you are testing.
  - If regenerating golden data, back up existing `macesw_regression_data.root` or follow the script’s `old-regression-data/` move behavior.
  - Do not re-run a `Read*` macro for the same `<Module>/<DataTuple>` inside the same `macesw_regression_data.root` unless you first remove or archive the old entry.

## Edge Cases and Notes
- Zero-integral histograms: the test macros handle zero-integral histograms by printing a warning and skipping scaling to avoid division-by-zero errors.
- Missing golden file: regression_test.bash requires `macesw_regression_data.root` to exist in the script directory; if missing, tests that rely on it will fail when attempting to open the sample directory.
- Binning/range mismatch: because ranges are computed when creating the golden sample, always regenerate golden data if upstream data schemas or distributions change (e.g., new columns or different units).
- Determinism: regression_test.bash launches simulations with `--seed 0`, which instructs the simulation to auto-generate a random seed for each run (i.e., the seed is not fixed and will differ every time the test is executed). This means outputs are non-deterministic and will vary run-by-run, which helps to reduce statistical variance across repeated tests. If strict reproducibility is required, specify a fixed nonzero seed (e.g., `--seed 12345`). Note that even with randomized seeding, some runs may still exhibit statistical fluctuations — watch for `SUSPICIOUS` flags.