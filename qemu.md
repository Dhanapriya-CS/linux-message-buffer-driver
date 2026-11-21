# Testing in a QEMU Linux Guest

Use a disposable Linux guest for loading an out-of-tree kernel module. Do not load an experimental module on a machine with important work or data.

## Prepare the guest

Install a Linux distribution in QEMU and install the build tools and headers matching the guest's running kernel. On Ubuntu, for example:

```sh
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r) python3
```

Copy this project into the guest, then build it:

```sh
make
make client
```

## Load and exercise

```sh
sudo insmod kmsgbuf.ko
dmesg | tail
```

Use the major number in the module log if udev did not create `/dev/kmsgbuf`:

```sh
sudo mknod /dev/kmsgbuf c MAJOR 0
sudo chmod 666 /dev/kmsgbuf
```

Run the sample client and integration checks:

```sh
./kmsgbufctl write 'QEMU kernel module check'
./kmsgbufctl read
python3 tests/test_device.py
```

Then unload the module and remove the manually created node if needed:

```sh
sudo rmmod kmsgbuf
sudo rm -f /dev/kmsgbuf
```

Record the guest distribution, kernel version (`uname -r`), build result, and test output in your project notes. These are the details to cite on your resume only after you have run the project yourself.
