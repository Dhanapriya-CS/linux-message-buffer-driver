#!/usr/bin/env python3
"""Integration checks for the loaded /dev/kmsgbuf module."""

import errno
import os
import unittest

DEVICE = os.environ.get("KMSGBUF_DEVICE", "/dev/kmsgbuf")
CAPACITY = 4096


class MessageDeviceTests(unittest.TestCase):
    def setUp(self):
        self.fd = os.open(DEVICE, os.O_RDWR)
        os.write(self.fd, b"CLEAR\n")

    def tearDown(self):
        os.write(self.fd, b"CLEAR\n")
        os.close(self.fd)

    def test_write_then_read(self):
        payload = b"kernel buffer test"
        self.assertEqual(os.write(self.fd, payload), len(payload))
        self.assertEqual(os.read(self.fd, 128), payload)

    def test_writes_append(self):
        os.write(self.fd, b"first-")
        os.write(self.fd, b"second")
        self.assertEqual(os.read(self.fd, 128), b"first-second")

    def test_clear_command(self):
        os.write(self.fd, b"discard me")
        os.write(self.fd, b"CLEAR\n")
        self.assertEqual(os.read(self.fd, 128), b"")

    def test_full_buffer_returns_enospc(self):
        os.write(self.fd, b"x" * CAPACITY)
        with self.assertRaises(OSError) as error:
            os.write(self.fd, b"y")
        self.assertEqual(error.exception.errno, errno.ENOSPC)


if __name__ == "__main__":
    unittest.main(verbosity=2)
