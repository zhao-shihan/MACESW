# 参与 MACESW 开发 / Contributing to MACESW

感谢您对 MACE 离线软件 (MACESW) 的开发感兴趣。您的帮助将使该项目更进一步，并将为前沿科学研究做出贡献。  
Thank you for your interest in contributing to MACE offline software (MACESW). Your help makes this project better and contributes to exploring cutting-edge science for everyone.

本文档说明了如何报告问题、提出更改以及准备贡献，以便它们能够被快速审查和合并。  
This document explains how to report issues, propose changes, and prepare contributions so they can be reviewed and merged quickly.

- [参与 MACESW 开发 / Contributing to MACESW](#参与-macesw-开发--contributing-to-macesw)
  - [工作语言 / Working Language](#工作语言--working-language)
  - [报告问题 / Reporting issues](#报告问题--reporting-issues)
  - [提出更改（拉取请求）/ Proposing changes (pull requests)](#提出更改拉取请求-proposing-changes-pull-requests)
  - [代码风格与测试 / Code style and tests](#代码风格与测试--code-style-and-tests)
  - [分支与提交信息 / Branching and commit messages](#分支与提交信息--branching-and-commit-messages)
  - [拉取请求检查清单 / Pull request checklist](#拉取请求检查清单--pull-request-checklist)
  - [审阅与维护 / Reviewing and maintaining](#审阅与维护--reviewing-and-maintaining)
  - [交流与行为准则 / Communication and Code of Conduct](#交流与行为准则--communication-and-code-of-conduct)
  - [获取帮助 / Getting help](#获取帮助--getting-help)
  - [维护者 / Maintainer](#维护者--maintainer)

## 工作语言 / Working Language

可用的工作语言：**中文**、**英文**。  
Acceptable working language: **Chinese** or **English**.

我们优先使用**英文**作为工作语言以确保与国际同行的顺利沟通。为促进更广泛的社区参与，我们推荐以下做法：  
We prioritize **English** as the working language to ensure smooth communication with international peers. To facilitate broader community participation, we recommend:

- **代码与注释:** 必须使用英文  
  **Code & comments:** Must use English
- **提交信息:** 必须使用英文  
  **Commit messages:** Must use English
- **文档:** 优先提供英文版本  
  **Documentation:** Provide English version first
- **讨论、议题与拉取请求:** 鼓励使用英文，中文也可接受  
  **Discussions, issues & PR:** English encouraged, Chinese acceptable

## 报告问题 / Reporting issues

- 在开启新议题前，请先搜索现有议题以避免重复。  
  Search existing issues before opening a new one to avoid duplicates.
- 开启议题时请包含：  
  When opening an issue include:
  - 清晰的问题或功能请求标题和描述。  
    A clear title and description of the problem or feature request.
  - 环境信息（操作系统、编译器版本、任何相关的依赖版本）。  
    Environment information (OS, compiler version, any relevant dependency versions).
  - 重现步骤（如果可能，提供最小可重现示例）。  
    Steps to reproduce (minimum reproducible example if possible).
  - 预期行为与实际行为。  
    Expected vs actual behavior.
  - 日志或堆栈跟踪，以及重现问题所需的任何输入文件。  
    Logs or stack traces, and any input files needed to reproduce the issue.
  - 如相关，请附上能重现问题的小测试文件或简短脚本。  
    If relevant, attach small test files or a short script that reproduces the problem.

## 提出更改（拉取请求）/ Proposing changes (pull requests)

- Fork 本仓库并从 `main` 分支创建一个具有描述性名称的分支，例如 `feature/add-xyz`、`fix/issue-123` 或 `docs/update-abc`。  
  Fork the repository and create a branch from `main` with a descriptive name, e.g. `feature/add-xyz`, `fix/issue-123` or `docs/update-abc`.
- 进行小型而专一的提交，并附上清晰的信息。  
  Make small, focused commits with clear messages.
- 将最新的 `main` 变基或合并到您的分支中，以保持历史记录清晰。并在本地解决合并冲突。  
  Rebase or merge the latest `main` into your branch to keep history clean and resolve conflicts locally.
- 开启一个目标为 `main` 分支的拉取请求（PR）。在 PR 描述中，请包含：  
  Open a pull request targeting `main`. In the PR description, include:
  - 更改的简要摘要以及为何需要此更改。  
    A short summary of the change and why it is needed.
  - 相关议题编号（如有，例如 "Fixes #123"）。  
    Related issue number(s) (if relevant, e.g. "Fixes #123").
  - 任何设计决策或权衡。  
    Any design decisions or trade-offs.
  - 您如何测试此更改以及重现测试的说明。  
    How you tested the change and instructions to reproduce the test.

## 代码风格与测试 / Code style and tests

- 遵循项目的[代码风格指南](STYLE_GUIDE.md)。在提交代码前始终运行代码格式化工具和 Linter。  
  Follow the project's [coding style guide](STYLE_GUIDE.md). Always run code formatter and linter before submitting code.
- 在适用的情况下，为新功能和错误修复添加单元测试或集成测试。  
  Add unit or integration tests for new features and bug fixes where applicable.
- 确保所有测试在本地通过。  
  Ensure all tests pass locally.

## 分支与提交信息 / Branching and commit messages

- 使用简短、描述性的分支名称。使用 `feature/`、`fix/`、`chore/`、`docs/` 前缀有助于审阅变更。  
  Use short, descriptive branch names. Use `feature/`, `fix/`, `chore/`, `docs/` prefixes help review changes.
- 提交信息格式：  
  Commit message format:
  - 一行摘要（50 个字符或更少）。  
    One-line summary (50 characters or less).
  - 空行。  
    Blank line.
  - 如果需要，更详细的描述（约 72 个字符换行）。  
    More detailed description, if necessary (wrap at ~72 characters).
- 使用现在时和祈使语气，句首字母大写，例如 "Add command-line option for X"。  
  Use present tense and imperative mood, with the first letter of the sentence capitalized, e.g. "Add command-line option for X".

## 拉取请求检查清单 / Pull request checklist

在请求审阅前，请确保：  
Before requesting review, ensure:
- [ ] 我已阅读[贡献指南](CONTRIBUTING.md)。  
      I have read the [contributing guidelines](CONTRIBUTING.md)
- [ ] 此 PR 将一个工作分支（非 `main`）合并到 `main` 分支。  
      The PR will merge a working branch (other than `main`) into the `main` branch.
- [ ] 我在 PR 描述中链接了相关议题并提供了上下文。  
      I linked related issues and provided context in the PR description.
- [ ] 我已运行代码格式化工具和 Linter（方法请参见[此章节](STYLE_GUIDE.md#tooling)）。  
      I have run the code formatter and linter (see [this section](STYLE_GUIDE.md#tooling) for instructions).
- [ ] 我已清除了所有编译器警告、格式化工具警告和 Linter 警告。  
      I cleared all compiler warnings, formatter warnings, and linter warnings.
- [ ] 我的代码遵循项目的[代码风格指南](STYLE_GUIDE.md)。  
      My code follows the project's [coding style guide](STYLE_GUIDE.md).
- [ ] 我在适用的情况下添加/更新了单元测试。  
      I added/updated unit tests where applicable.
- [ ] 我更新了相关文档（README、Doxygen 或设计文档）。  
      I updated relevant documentation (README, Doxygen, or design docs).
- [ ] 我在本地运行了测试套件且所有测试均通过。  
      I ran the test-suite locally and all tests pass.
- [ ] 所有 CI 检查均通过。  
      All CI checks pass.

## 审阅与维护 / Reviewing and maintaining

- 对审阅评论及时响应：更新您的分支，推送更改，并回复审阅评论。  
  Be responsive to review comments: update your branch, push changes, and add replies.
- 尽可能保持 PR 专一和小型——这样审阅更快。  
  Keep PRs focused and small where possible — they review faster.
- 维护者可能要求在合并前压缩或重做提交。请保持合作态度。  
  Maintainers may ask to squash or rework commits before merging. Maintain a cooperative attitude.

## 交流与行为准则 / Communication and Code of Conduct

- 在议题和 PR 中保持尊重和礼貌。建设性的反馈对大家都有帮助。  
  Be respectful and courteous in issues and PRs. Constructive feedback helps everyone.
- 在实施大型更改前，如果您对某事不确定，请先询问维护者或其他开发者。  
  If you are unsure about something, ask maintainers or developers before implementing a large change.

## 获取帮助 / Getting help

- 如果您在入门时需要帮助，请开启一个以 `Help:` 为前缀的议题，维护者或开发者将尽力提供帮助。  
  If you need assistance getting started, open an issue prefixed with `Help:` and maintainers or developers will try to help.

## 维护者 / Maintainer

[**@zhao-shihan**](https://github.com/zhao-shihan)

感谢您帮助改进 MACESW！  
Thank you for helping improve MACESW!
