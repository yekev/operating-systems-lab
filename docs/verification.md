# Verification record

This file separates reproducible checks from historical coursework evidence.

| Component | Check | Current status |
| --- | --- | --- |
| Process and signal lab | Compile all programs; run normal and SIGTERM smoke tests | Passed in GitHub Actions on Ubuntu 24.04 |
| Pthreads game | Compile with C++17 warnings enabled | Passed in GitHub Actions; interactive behavior is demonstrated in the included video |
| MLFQ patch | Apply to pinned `xv6-riscv` commit | Passed locally with `git apply --check` |
| MLFQ runtime | Boot xv6 and run the fast `mlfqtest` workload | Passed in GitHub Actions with QEMU |
| mmap patch | Apply to pinned `xv6-labs-2022` mmap commit | Passed locally with `git apply --check` |
| mmap runtime | Run all eight checks in the upstream grader's `mmaptest` group | Passed in GitHub Actions with QEMU |

The runtime jobs start from clean clones of the pinned upstream commits, apply
the published patches, build the resulting kernels, and run them in QEMU. The
mmap gate uses the official grader's `mmaptest` filter; course-time metadata and
unrelated baseline regression tests are intentionally outside its scope.
