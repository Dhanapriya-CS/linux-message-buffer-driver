# Linux Message Buffer Character Device

A small out-of-tree Linux kernel module that exposes a bounded message buffer through a character device. It is designed as a learning portfolio project for Linux kernel fundamentals: module lifecycle, character-device registration, user/kernel copies, locking, and reproducible testing.

## Features

- Registers `/dev/kmsgbuf` dynamically when loaded.
- Supports `read`, `write`, and `clear` using the device file.
- Stores up to 4096 bytes and returns `-ENOSPC` when a write would exceed capacity.
- Uses a mutex to protect buffer contents and length.
- Handles partial reads and writes and reports standard Linux errors.
- Includes a command-line client, a Python integration test, and a QEMU-based test guide.

This is an educational example, not a production driver. It does not persist data, implement poll/ioctl, or provide a stable ABI promise.

## Requirements

- Linux host or VM with kernel headers matching the running kernel.
- `make`, GCC, and Python 3 for the test client.
- Root privileges to load/unload the module and create the device node (normally udev creates it).

## Build

```sh
make
```

If the running kernel's build directory is not the default, specify it:

```sh
make KDIR=/path/to/kernel/build
```

## Load and use

```sh
sudo insmod kmsgbuf.ko
dmesg | tail
ls -l /dev/kmsgbuf
```

When udev does not create the device node automatically, the module prints its major number. Create the node with that number:

```sh
sudo mknod /dev/kmsgbuf c MAJOR 0
sudo chmod 666 /dev/kmsgbuf
```

Build and run the client:

```sh
make client
./kmsgbufctl write 'hello from userspace'
./kmsgbufctl read
./kmsgbufctl clear
```

Unload and clean up:

```sh
sudo rmmod kmsgbuf
sudo rm -f /dev/kmsgbuf
make clean
```

## Test

With the module loaded and `/dev/kmsgbuf` accessible:

```sh
python3 tests/test_device.py
```

The test checks write/read round-tripping, append behavior, clear behavior, and capacity handling. Run it only in a disposable development VM; it writes to and clears the device buffer.

For an isolated environment, see [docs/qemu.md](docs/qemu.md). A QEMU guest with a matching kernel build tree is the recommended place to load an experimental module.

## Design notes

- `register_chrdev()` assigns a dynamic major number; the module logs the value for manual device-node creation.
- The buffer and its length are protected by a mutex. File operations can sleep while holding it, so a mutex is used instead of a spinlock.
- `copy_from_user()` and `copy_to_user()` are used for all user memory transfers.
- A read consumes bytes from the front of the buffer. A write appends bytes. This intentionally simple queue-like behavior keeps the interface small.
- `clear` is exposed as writing the exact command `CLEAR\n`; all other writes append to the buffer.

## Project structure

```text
kmsgbuf.c             Kernel module
kmsgbufctl.c          Small userspace client
Makefile              Out-of-tree module and client build
tests/test_device.py  Integration checks
docs/qemu.md          Isolated QEMU workflow
```

## Portfolio follow-up

Good next improvements include an ioctl-based clear operation, `poll()` support, a ring buffer, KUnit tests for helper logic, and a patch formatted with `checkpatch.pl`. Describe only work you have completed and verified on your resume.

## License

GPL-2.0-only. See the SPDX identifiers in the source files.
