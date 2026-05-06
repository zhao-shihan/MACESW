# MACE 离线软件 / MACE offline software (MACESW)

[![GitHub License](https://img.shields.io/github/license/zhao-shihan/MACESW?color=red)](COPYING)
![GitHub Created At](https://img.shields.io/github/created-at/zhao-shihan/MACESW?color=blue)
![GitHub top language](https://img.shields.io/github/languages/top/zhao-shihan/MACESW?color=f34b7d)
![GitHub last commit](https://img.shields.io/github/last-commit/zhao-shihan/MACESW)
[![GitHub contributors](https://img.shields.io/github/contributors/zhao-shihan/MACESW?style=flat)](https://github.com/zhao-shihan/MACESW/graphs/contributors)
![GitHub repo size](https://img.shields.io/github/repo-size/zhao-shihan/MACESW)

[![Build (AMD64 GNU/Linux GCC)](https://github.com/zhao-shihan/MACESW/actions/workflows/build-with-gcc.yml/badge.svg)](https://github.com/zhao-shihan/MACESW/actions/workflows/build-with-gcc.yml)
[![Build (AMD64 GNU/Linux Clang, unity build)](https://github.com/zhao-shihan/MACESW/actions/workflows/build-with-clang.yml/badge.svg)](https://github.com/zhao-shihan/MACESW/actions/workflows/build-with-clang.yml)
[![Static code analysis](https://github.com/zhao-shihan/MACESW/actions/workflows/static-code-analysis.yml/badge.svg)](https://github.com/zhao-shihan/MACESW/actions/workflows/static-code-analysis.yml)

<!--
  The align attribute on img is obsolete in HTML5, but is used here because
  GitHub strips inline styles from README.md. This is the only way to right-align
  the image in GitHub READMEs. Do not replace with inline styles.
-->
<img src="docs/picture/MACE_logo_v2.svg" alt="MACE logo" align="right" width=300/>

**正反缪子素转化实验（MACE）** 是下一代粒子物理实验，旨在探索缪子素（ $\mathrm{M}$ ，即 $\mu^+$ 与 $e^-$ 的束缚态）向反缪子素（ $\overline{\mathrm{M}}$ ，即 $\mu^-$ 与 $e^+$ 的束缚态）的自发转化过程——这是一种带电轻子味破坏（cLFV）过程。观测到这种稀有转化现象将为超越标准模型的新物理提供明确证据，对理解中微子质量起源、正反物质不对称性等基本物理现象具有重要意义。  
The **Muonium-to-Antimuonium Conversion Experiment (MACE)** is a next-generation particle physics experiment designed to search for the spontaneous conversion of muonium ($\mathrm{M}$, a bound state of $\mu^+$ and $e^-$) to antimuonium ($\overline{\mathrm{M}}$, a bound state of $\mu^-$ and $e^+$) — a charged lepton flavor violation (cLFV) process. Observing this rare conversion would provide clear evidence of new physics beyond the Standard Model, with implications for understanding neutrino masses, matter-antimatter asymmetry, and other fundamental phenomena.

**MACE 离线软件（MACESW）** 为实验提供从模拟、重建到数据分析的全周期、全方位支持，助力开展正反缪子素转化、缪子稀有过程及其他相关物理的高精度研究。  
The **MACE offline software (MACESW)** provides comprehensive support for the experiment's full lifecycle—from simulation and reconstruction to data analysis—enabling high-precision studies of muonium-to-antimuonium conversion, rare muon processes, and other relevant physics.

MACESW 基于 [**Mustard**](https://github.com/zhao-shihan/Mustard) 框架开发。  
MACESW is developed based on the [**Mustard**](https://github.com/zhao-shihan/Mustard) framework.

- [MACE 离线软件 / MACE offline software (MACESW)](#mace-离线软件--mace-offline-software-macesw)
  - [构建指南 / How to build](#构建指南--how-to-build)
    - [面向基础用户：使用 RGB 镜像构建 / For basic users: Build with RGB image](#面向基础用户使用-rgb-镜像构建--for-basic-users-build-with-rgb-image)
      - [1. 安装 Apptainer / Install Apptainer](#1-安装-apptainer--install-apptainer)
      - [2. 下载 RGB 镜像 / Download RGB image](#2-下载-rgb-镜像--download-rgb-image)
      - [3. 构建 MACESW / Build MACESW](#3-构建-macesw--build-macesw)
      - [4. 运行 MACESW / Run MACESW](#4-运行-macesw--run-macesw)
    - [面向高级用户或开发者：从零开始构建 / For advanced users or developers: Build from scratch](#面向高级用户或开发者从零开始构建--for-advanced-users-or-developers-build-from-scratch)
      - [工具链 / Toolchain](#工具链--toolchain)
      - [依赖 / Dependencies](#依赖--dependencies)
      - [可选依赖 / Optional Dependencies](#可选依赖--optional-dependencies)
  - [引用 / Citation](#引用--citation)
  - [文献列表（按公开日期排序）/ Literatures list (sort by disclosure date)](#文献列表按公开日期排序-literatures-list-sort-by-disclosure-date)

## 构建指南 / How to build

### 面向基础用户：使用 [RGB](https://github.com/zhao-shihan/RGB) 镜像构建 / For basic users: Build with [RGB](https://github.com/zhao-shihan/RGB) image

推荐基础用户使用 RGB Apptainer/Docker 镜像来构建 MACESW，该镜像包含了所有必要的依赖项。  
The recommended approach for basic user to build MACESW is using the RGB Apptainer/Docker image, which contains all necessary dependencies.

#### 1. 安装 Apptainer / Install Apptainer

对于 Debian 系 GNU/Linux 系统，可直接从 APT 安装：  
Install Apptainer via APT on Debian-based systems:
```bash
sudo apt install apptainer squashfuse fuse2fs gocryptfs
```
您也可从[官方发布页](https://github.com/apptainer/apptainer/releases)下载并安装 Apptainer。  
You can also download and install Apptainer from the [official releases](https://github.com/apptainer/apptainer/releases).

#### 2. 下载 RGB 镜像 / Download RGB image

拉取 RGB Apptainer 镜像：  
Pull the RGB Apptainer image:
```bash
apptainer pull oras://ghcr.io/zhao-shihan/rgb
```
这将在您当前目录下创建一个镜像文件 `rgb_latest.sif`。  
This creates an image file `rgb_latest.sif` in your current directory.

#### 3. 构建 MACESW / Build MACESW

在 RGB 镜像内启动一个交互式 shell：  
Start an interactive shell within the RGB image:
```bash
apptainer shell ./rgb_latest.sif
```
（请留意：并非必须打开 shell，您也可以通过 `./rgb_latest.sif <command>` 使用镜像运行命令，但为了简单起见，这里我们在容器 shell 内进行操作）  
(Please note that opening a shell is not necessary, you can also run something with the image by `./rgb_latest.sif <command>`, but here we work inside the container shell for simplicity)

然后，在容器 shell 中，克隆并构建 MACESW。  
Then, in the container shell, clone and build MACESW.
```bash
git clone https://github.com/zhao-shihan/MACESW.git
cd MACESW
mkdir build && cd build
cmake ..
make -j8
```
（请留意：现在您所在的目录为您进入容器 shell 时的目录，进入 Apptainer 容器只相当于“替换”了一部分系统目录，您的个人目录和容器外是一致的。）  
(Please note that the directory you are in now is the same as the one outside the container, entering the Apptainer container is like "replacing" part of the system directories, your home directory is consistent with outside the container.)

#### 4. 运行 MACESW / Run MACESW

成功编译后，测试可执行文件：  
After successful compilation, test the executable:
```bash
./MACE --help
```

### 面向高级用户或开发者：从零开始构建 / For advanced users or developers: Build from scratch

适用于从源代码构建或为 MACESW 做贡献的高级用户或开发者：  
For advanced users or developers building from source or contributing to MACESW:

#### 工具链 / Toolchain

- **C++ 编译器 / C++ compiler:** GCC ≥ 13 或 LLVM Clang ≥ 16 或同等工具 / GCC ≥ 13 or LLVM Clang ≥ 16 or equivalent
- **C++ 标准库 / C++ standard library:** libstdc++ ≥ 13 或同等标准库 / libstdc++ ≥ 13 or equivalent
- **构建系统 / Build system:** CMake ≥ 3.21，以及 GNU Make 或 Ninja 或同等工具 / CMake ≥ 3.21, and GNU Make or Ninja or equivalent

#### 依赖 / Dependencies

[Mustard](https://github.com/zhao-shihan/Mustard) 需要您系统上已安装这些[外部库](https://github.com/zhao-shihan/Mustard?tab=readme-ov-file#dependencies)。  
[Mustard](https://github.com/zhao-shihan/Mustard) requires these [external libraries](https://github.com/zhao-shihan/Mustard?tab=readme-ov-file#dependencies) to be installed on your system.

#### 可选依赖 / Optional Dependencies

以下依赖是可选的。如果在配置过程中在您的系统上未找到它们，CMake 将自动下载并构建它们。  
The following dependencies are optional. If they are not found on your system during configuration, CMake will automatically download and build them.

| 库 Library                                                      | 版本 Version | 描述 Description                                                                                                         |
| :-------------------------------------------------------------- | :----------- | :----------------------------------------------------------------------------------------------------------------------- |
| [**Mustard**](https://github.com/zhao-shihan/Mustard)           | ≥ 0.26.315   | 一个现代的、高性能的高能物理实验离线软件框架 / A modern, high-performance offline software framework for HEP experiments |
| [**PMP Library**](https://www.pmp-library.org/)                 | ≥ 3.0.0      | 多边形网格处理库 / The Polygon Mesh Processing Library                                                                   |
| [**zhao-shihan/GenFit**](https://github.com/zhao-shihan/GenFit) | main         | 一个通用的径迹拟合工具包 / A generic track-fitting toolkit                                                               |

| 数据 Data           | 版本 Version | 描述 Description                      |
| :------------------ | :----------- | :------------------------------------ |
| macesw_offline_data | 0.26.010616  | MACESW 离线数据 / MACESW offline data |
| macesw_test_data    | 0.26.010616  | MACESW 测试数据 / MACESW test data    |

## 引用 / Citation

如果您在研究中使用 MACESW，请引用以下论文：  
If you use MACESW in your research, please cite the following papers:

**BAI Ai-Yu, CAI Hanjie, CHEN Chang-Lin, et al. (MACE working group).**  
**Conceptual Design of the Muonium-to-Antimuonium Conversion Experiment (MACE)** [J].  
*Nucl. Sci. Tech.*, 2026, 37: 57. DOI: 10.1007/s41365-025-01876-0. arXiv: 2410.18817 [hep-ex].  
https://doi.org/10.1007/s41365-025-01876-0.

## 文献列表（按公开日期排序）/ Literatures list (sort by disclosure date)

**LU Guihao, ZHAO Shihan, CHEN Siyuan, et al.**  
**Positron transport system for muonium-to-antimuonium conversion experiment** [J].  
*Phys. Rev. Accel. Beams*, 2026, 29(3): 031602. DOI: 10.1103/2yb7-zv76. arXiv: 2508.07922 [hep-ex].  
https://doi.org/10.1103/2yb7-zv76.

**AN FengPeng, BAI Dong, CHEN Siyuan, et al.**  
**High-Precision Physics Experiments at Huizhou Large-Scale Scientific Facilities** [J].  
*Chin. Phys. Lett.*, 2025, 42(11): 110102. DOI: 10.1088/0256-307X/42/11/110102.  
https://doi.org/10.1088/0256-307X/42/11/110102.

**BAI Ai-Yu, CAI Hanjie, CHEN Chang-Lin, et al. (MACE working group).**  
**Conceptual Design of the Muonium-to-Antimuonium Conversion Experiment (MACE)** [J].  
*Nucl. Sci. Tech.*, 2026, 37: 57. DOI: 10.1007/s41365-025-01876-0. arXiv: 2410.18817 [hep-ex].  
https://doi.org/10.1007/s41365-025-01876-0.

**CHEN Siyuan, ZHAO Shihan, XIONG Weizhi, et al.**  
**Design of a CsI(Tl) calorimeter for muonium-to-antimuonium conversion experiment** [J].  
*Front. Phys.*, 2025, 20: 035202. DOI: 10.15302/frontphys.2025.035202. arXiv: 2408.17114 [physics.ins-det].  
https://doi.org/10.15302/frontphys.2025.035202.

**ZHAO Shihan, TANG Jian.**  
**Progress of muonium-to-antimuonium conversion experiment** [J].  
*Nucl. Part. Phys. Proc.*, 2024, 345: 24-28. DOI: 10.1016/j.nuclphysbps.2024.05.005.  
https://doi.org/10.1016/j.nuclphysbps.2024.05.005.

**ZHAO Shihan, TANG Jian**  
**Optimization of muonium yield in perforated silica aerogel** [J].  
*Phys. Rev. D*, 2024, 109(7): 072012. DOI: 10.1103/PhysRevD.109.072012. arXiv: 2401.00222 [hep-ex].  
https://doi.org/10.1103/PhysRevD.109.072012.

**CORRODI S., OKSUZIAN Y., EDMONDS A., et al.**  
**Workshop on a future muon program at FNAL** [C].  
*arXiv preprint*, 2023: 2309.05933 [hep-ex].  
https://arxiv.org/abs/2309.05933.

**BAI Ai-Yu, CHEN Yu, CHEN Yukai, et al. (MACE working group).**  
**Snowmass2021 Whitepaper: Muonium to Antimuonium Conversion** [DB/OL].  
*arXiv preprint*, 2022: 2203.11406 [hep-ph].  
https://arxiv.org/abs/2203.11406.

**HAN Chengcheng, HUANG Da, TANG Jian, et al.**  
**Probing the doubly charged Higgs boson with a muonium to antimuonium conversion experiment** [J].  
*Phys. Rev. D*, 2021, 103(5): 055023. DOI: 10.1103/PhysRevD.103.055023. arXiv: 2102.00758 [hep-ph].  
https://doi.org/10.1103/PhysRevD.103.055023.
