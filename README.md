# Kernel Proc Status

An educational Linux kernel module that creates `/proc/kernel_status`. Reading the file reports the running kernel release, configured timer frequency, and online CPU count. The project demonstrates module lifecycle management, the proc filesystem, `seq_file`, and safe read-only kernel information reporting.

## Compatibility

Targets Linux 5.6 or newer because it uses `struct proc_ops`. Build against headers matching the kernel where you plan to load the module. Kernel module loading may be disabled by system policy or Secure Boot configuration.

## Build

```sh
make
```

To use a non-default kernel build directory:

```sh
make KDIR=/path/to/kernel/build
```

## Load and read

```sh
sudo insmod kernel_status.ko
cat /proc/kernel_status
```

Example output (values depend on the machine):

```text
kernel_release: 6.x.y
timer_hz: 250
online_cpus: 8
```

Unload and clean up:

```sh
sudo rmmod kernel_status
make clean
```

## Files

- `kernel_status.c`: kernel module and proc entry.
- `Makefile`: out-of-tree module build.
- `index.html`: portfolio landing page for this project.

## What this project demonstrates

- Kernel module initialization and cleanup.
- A read-only `/proc` entry using the `seq_file` interface.
- Basic collection of kernel metadata via `init_uts_ns`, `HZ`, and CPU APIs.
- Out-of-tree module build workflow.

This is a learning project, not a supported monitoring interface. `/proc/kernel_status` is removed when the module unloads. Do not claim build or runtime verification until you have run it on a Linux system and recorded the kernel version.

## License

GPL-2.0-only. See the SPDX identifier in the source.
