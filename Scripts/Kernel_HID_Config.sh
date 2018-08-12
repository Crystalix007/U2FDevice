#!/usr/bin/env bash

# Directly from http://isticktoit.net/?p=1383

# Load libcomposite
modprobe libcomposite

cd /sys/kernel/config/usb_gadget/

# Create gadget
mkdir -p /sys/kernel/config/usb_gadget/hidraw
cd /sys/kernel/config/usb_gadget/hidraw

# Add basic information
echo 0x0100 > bcdDevice       # Version 1.0.0
echo 0x0200 > bcdUSB          # USB 2.0

# For this part, used the Linux idVendor
# Of course, no official spec for Raspberry Pi 0 acting as U2F device, so self defined product here we go
echo 0x1d6b > idVendor        # Linux Foundation
echo 0x0400 > idProduct       # Self defined U2F device

echo 0x0000 > bDeviceClass    # RAW HID?
echo 0x0000 > bDeviceSubClass # 
echo 0x0040 > bMaxPacketSize0 # Max Packet Size

# Create English locale
mkdir -p strings/0x409

# Obviously, feel free to change these however you want
echo "Michael Kuc"         > strings/0x409/manufacturer
echo "Raspberry U2f"       > strings/0x409/product
echo "0123456789"          > strings/0x409/serialnumber

# Create HIDRAW function
mkdir -p functions/hid.usb0

echo 0 > functions/hid.usb0/protocol
echo 8 > functions/hid.usb0/report_length
echo 1 > functions/hid.usb0/subclass
printf '\x06\xD0\xF1\x09\x01\xA1\x01\x09\x20\x15\x00\x26\xFF\x00\x75\x08\x95\x40\x81\x02\x09\x21\x15\x00\x26\xFF\x00\x75\x08\x95\x40\x91\x02\xC0' > functions/hid.usb0/report_desc

# Create ECM function
mkdir -p functions/ecm.usb0

# An attempt at keeping addresses constant
echo "DD:03:52:23:18:4C" > functions/ecm.usb0/host_addr
echo "62:30:54:93:06:DE" > functions/ecm.usb0/dev_addr

# Create configuration
mkdir -p configs/c.1
mkdir -p configs/c.1/strings/0x409

echo 0x80    > configs/c.1/bmAttributes
echo 200     > configs/c.1/MaxPower
echo "HIDRAW" > configs/c.1/strings/0x409/configuration

# Link HID function to configuration
ln -s functions/hid.usb0 configs/c.1

# Link g_ether function to configuration
ln -s functions/ecm.usb0 configs/c.1

# Sleep for 1 second
sleep 1

# Enable gadget
ls /sys/class/udc > UDC
