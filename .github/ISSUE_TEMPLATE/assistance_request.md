---
name: Assistance Request
about: Ask for help for using QBDI
title: ''
labels: ''
assignees: ''
---

**Description of the problem**
A clear description of what your problem is.

**Compilation or execution log**
Any relevant log that illustrate your problem.
Please use [distinct block]https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#quoting-code) to format this section.

**QBDI/PyQBDI/Frida script**
Insert your script/code that use QBDI here.

**Code of the target of the instrumentation**
Insert the script/code that you try to instrument with QBDI.
If the instrumented code is a public library/binary, you may provide a link to download it.

**Specific command to reproduce your problem on other laptop**
List all the command (if possible shell) that should be run the get the same output as you.

**Information about the environment**
Version of your environment and any tools that is needed to reproduce your problem

If you use multiples devices (like a computer and a smartphone), please provide the information for each of them.

- OS: (with version) [e.g. Windows 11, Debian 12.4, Android 14, ...]
- Architecture: [e.g. X86, X64, AARCH64, ARM32]
- QBDI version (returned by getVersion) and commit hash (if not a release version)
- Frida version on the Host and on the target (in case of android device) (for frida/QBDI issue)
- Python version (for PyQBDI issue)

**Additional context or informations**
Any other context about the problem here.

Thanks for taking the time to fill out all information! This will help us to reproduce your issue on our side.
