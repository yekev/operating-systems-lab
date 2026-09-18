# Verification record

This file separates reproducible checks from historical coursework evidence.

| Component | Check | Current status |
| --- | --- | --- |
| Process and signal lab | Compile all programs; run normal and SIGTERM smoke tests | Defined in CI; local Linux run pending |
| Pthreads game | Compile with C++17 warnings enabled | Defined in CI; interactive behavior previously demonstrated in the included video |
| MLFQ patch | Apply to pinned `xv6-riscv` commit | Passed locally with `git apply --check` |
| MLFQ runtime | Boot xv6 and run `mlfqtest` / `mlfqtest stress` | Pending a RISC-V/QEMU environment |
| mmap patch | Apply to pinned `xv6-labs-2022` mmap commit | Passed locally with `git apply --check` |
| mmap runtime | Run `mmaptest` and the upstream grade target | Pending a RISC-V/QEMU environment |

The original coursework report recorded a successful `mmaptest`, but this
repository will only mark the refactored implementation as runtime-verified
after the exact published patch has been executed again.

