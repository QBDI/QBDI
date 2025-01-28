---
name: Bug report
about: Create a bug report
title: ''
labels: ''
assignees: ''
---

** Description of the problem **
A clear description of what the bug is, including the expected and actual behavior.


**Compilation or execution log**
Any relevant log that may help to understand the bug.
Please use [distinct block]https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#quoting-code) to format this section.

**Minimal code/script to reproduce the problem**
Insert your script/code that use QBDI here.

**Code of the target of the instrumentation**
Insert your script/code that you try to instrument with QBDI.
If the instrumented code is a public library/binary, you may provide a link to download it.

**Specific command to reproduce your issue**
  Can you list all the command (if possible shell) that should be run to reproduce the bug.

**Information about the environment**
Version of your environment and any tools that is needed to reproduce the bug
If you use multiples devices (like a computer and a smartphone), please provide the information for each of them.

- OS: (with version) [e.g. Windows 11, Debian 12.4, Android 14, ...]
- Architecture: [e.g. X86, X64, AARCH64, ARM32]
- QBDI version (returned by getVersion) and commit hash (if not a release version)
- Frida version on the Host and on the target (in case of android device) (for frida/QBDI issue)
- Python version (for PyQBDI issue)

**Additional context or informations**
Any other context about the problem here.

Thanks for taking the time to fill out this report!
