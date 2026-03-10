# 安全策略 / Security policy

感谢您抽出时间帮助确保 MACESW 的安全性。本文档说明了如何负责任地报告安全漏洞以及我们如何处理报告。  
Thank you for taking the time to help keep the MACESW project secure. This document explains how to responsibly report security vulnerabilities and how we handle reports.

## 报告漏洞 / Reporting a vulnerability

**首选方法：**  
**Preferred method:**
- **为此仓库创建 GitHub 安全公告：https://github.com/zhao-shihan/MACESW/security/advisories/new**  
  **Create a GitHub Security Advisory for this repository: https://github.com/zhao-shihan/MACESW/security/advisories/new**

报告时，请尽可能包含以下内容：  
When reporting, please include as much of the following as you can:
- 受影响的组件和版本（提交 SHA、标签或分支）。  
  Affected component(s) and version(s) (commit SHA, tag, or branch).
- 问题的简要描述和潜在影响。  
  A concise description of the issue and potential impact.
- 重现步骤（最小重现步骤将非常有帮助）。  
  Steps to reproduce (minimal reproduction is extremely helpful).
- 概念验证代码或测试用例（如有）。  
  Proof-of-concept code or test case, if available.
- 任何建议的修复或缓解措施。  
  Any suggested remediation or mitigation.
- 您的联系信息以及您希望如何被致谢（或要求匿名）。  
  Your contact information and how you prefer to be credited (or request anonymity).

如果因任何原因无法使用 GitHub 安全公告工作流程，请私下联系项目所有者，或者开启一个议题但在修复可用前避免包含任何漏洞利用细节。  
If you are unable to use the GitHub Security Advisory workflow for any reason, please contact the project owner privately, or open an issue but avoid including any exploit details until a fix is available.

## 受支持的版本 / Supported releases

我们将 `main` 分支和最新的标签版本视为受支持的版本。如果您报告漏洞，请指明受影响的版本（标签或提交 SHA）。我们将优先修复当前稳定版本和 `main` 分支。  
We consider the `main` branch and the latest tagged release as supported. If you report a vulnerability, please indicate the affected versions (tags or commit SHAs). We will prioritize fixes for the current stable release and the `main` branch.

## 不在范围内的项目 / Out-of-scope

第三方依赖中的安全问题应报告给这些依赖的维护者，并在适当时也报告给我们。供应链攻击或外部构建工具中的漏洞应在其各自的项目中向上游报告。  
Security issues in third-party dependencies should be reported to the maintainers of those dependencies as well as here when appropriate. Supply-chain attacks or vulnerabilities in external build tools should be reported upstream in their respective projects.

## 致谢 / Credit

除非您另有要求，我们将在发布说明或公告中致谢报告安全问题的人员。  
Unless you request otherwise, we will credit people who report security issues in release notes or advisories.

## 法律声明 / Legal

我们鼓励安全研究人员遵循负责任的披露实践。对于遵循上述指导原则的善意安全研究，我们不会采取法律行动。我们希望报告者避免故意破坏行为，并避免访问或修改与演示问题无关的数据。  
We encourage security researchers to follow responsible disclosure practices. We will not pursue legal action against good-faith security research that follows the above guidance. We expect reporters to avoid intentionally destructive behavior and to avoid accessing or modifying data not needed to demonstrate the issue.

感谢您帮助提高 MACESW 的安全性。  
Thank you for helping improve the security of MACESW.
