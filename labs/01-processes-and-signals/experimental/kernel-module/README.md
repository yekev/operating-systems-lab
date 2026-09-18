# Experimental kernel-module variant

This optional experiment starts a Linux kernel worker, invokes a user-mode
signal test with `call_usermodehelper()`, and records the helper's wait status
in the kernel log.

It is intentionally excluded from automated CI because loading third-party
kernel modules requires administrator privileges and matching kernel headers.
Use a disposable Linux virtual machine, not a production host.

```bash
cc -O2 -Wall -Wextra test.c -o /tmp/csc3150-signal-test
make
sudo insmod program2.ko
sudo dmesg | tail -n 20
sudo rmmod program2
```

The original coursework targeted Linux 5.15. Compatibility with newer kernel
APIs is not guaranteed.
