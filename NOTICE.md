# Attribution and provenance

The user-space programs and xv6 extensions in this repository were developed
by Kaiwen Yao as individual operating-systems coursework and subsequently
refactored for reproducibility and public presentation.

The xv6 patches target code maintained by the MIT Parallel and Distributed
Operating Systems Group:

- `xv6-riscv`, commit `e90b2575ae6efd40927fedb2425a1fc54ffa23df`
- `xv6-labs-2022`, `mmap` branch commit
  `9cc6b8345397c1f06cc93ed3fbaa20709cb1984e`

xv6 is distributed under the MIT License. Its upstream copyright remains with
Frans Kaashoek, Robert Morris, Russ Cox, and the other xv6 contributors. The
patches in this repository contain only the context required by Git to apply
the extensions; the complete upstream source is fetched separately.

The optional Linux kernel module under
`labs/01-processes-and-signals/experimental/kernel-module` is licensed under
GPL-2.0-only, as indicated in its source file. The repository-level MIT license
does not replace that file-specific license.

