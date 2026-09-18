# Verification record

This file separates reproducible checks from historical coursework evidence.

| Component | Check | Current status |
| --- | --- | --- |
| Process and signal lab | Compile all programs; run normal and SIGTERM smoke tests | Passed in GitHub Actions on Ubuntu |
| Pthreads game | Compile with C++17 warnings enabled | Passed in GitHub Actions; interactive behavior is demonstrated in the included video |
| MLFQ patch | Apply to pinned `xv6-riscv` commit | Passed locally with `git apply --check` |
| MLFQ runtime | Boot xv6 and run the fast `mlfqtest` workload | Automated in GitHub Actions with QEMU |
| mmap patch | Apply to pinned `xv6-labs-2022` mmap commit | Passed locally with `git apply --check` |
| mmap runtime | Run the upstream mmap grade target | Automated in GitHub Actions with QEMU |

The original coursework report recorded a successful `mmaptest`, but this
repository will only mark the refactored implementation as runtime-verified
after the exact published patch has been executed again.
