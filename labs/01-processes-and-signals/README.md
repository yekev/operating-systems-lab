# Processes and signals

This lab launches a selected child program with `fork()` and `exec()`, observes
state changes with `SIGCHLD`, and decodes the status returned by `waitpid()`.
A parameterized child fixture exercises normal exit and fourteen common POSIX
signals without duplicating one source file per case.

The signal handler performs only an atomic flag update; all formatted output
and status processing happens in normal process context. A child stopped by
`SIGSTOP` is reported and resumed with `SIGCONT`, so the test does not leave a
stopped process behind.

## Build and run

```bash
make
./build/signal-runner ./build/signal-fixture normal
./build/signal-runner ./build/signal-fixture segmentation-fault
./build/signal-runner ./build/signal-fixture stop
```

`make smoke-test` runs non-destructive normal-exit and SIGTERM cases.

The [experimental kernel-module variant](experimental/kernel-module) is kept
separate because it requires matching Linux headers and administrator access.
It is not part of the default build.
