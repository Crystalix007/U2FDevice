# U2FDevice

This program uses an [Android smartphone](https://www.android.com/intl/en_uk/) to act as a [U2F token](https://www.yubico.com/solutions/fido-u2f/). This permits any person with a compatible Android phone to try out U2F 2FA for themselves.

All testing has been performed on a [Nexus 5X](https://en.wikipedia.org/wiki/Nexus_5X).

# Required materials

![Android smartphone](https://upload.wikimedia.org/wikipedia/commons/4/40/Nexus_5X_%28White%29.jpg "Nexus 5X")

![Computer](https://upload.wikimedia.org/wikipedia/commons/f/fa/BackSlash_Linux_Olaf_Laptop_Dekstop.png "Preferably Linux computer")

![Confirmed-working USB data cable](Readme_Assets/Micro_USB_Cable.png "Micro USB Data Cable")

# OS Install

## Getting the ROM

Choose whichever [ROM](https://www.xda-developers.com/what-is-custom-rom-android/) you want **for your device**. Make sure that the kernel source is available for it.

For example, using the [Nexus 5X](https://en.wikipedia.org/wiki/Nexus_5X), I chose [CypherOS 7](https://forum.xda-developers.com/nexus-5x/development/rom-cypheros-7-0-0-poundcake-t3869446).

## Installing the ROM

Install TWRP. See [here](https://www.xda-developers.com/how-to-install-twrp/) for details.
Install ROM. See [here](https://www.xda-developers.com/how-to-install-custom-rom-android/) for details.
Install a root utility. Note that there are multiple options, but I recommend [Magisk](https://forum.xda-developers.com/apps/magisk/official-magisk-v7-universal-systemless-t3473445), with details on how to install [here](https://www.xda-developers.com/how-to-install-magisk/) also.

## Booting the device

Check the device boots now, by rebooting from TWRP. It is possible to skip this step, but don't. This gives you the assurance that the ROM actually works on your device / you performed the install steps correctly.

## Getting the kernel

Note, if using a Nexus 5X with the same ROM, you can skip ahead, as a pre-built boot image has already been prepared, from the source available [here](https://github.com/Crystalix007/kernel_lge_bullhead).

1. From the place where you got your ROM, there should be an explanation of where to get the corresponding kernel source. Most often, this is hosted on [Github](https://github.com/).
Alternatively, for stock ROMS, seek out your manufacturer's website for the source code, for example [LG Open Source](http://opensource.lge.com/index).

2. On a Linux (virtual) machine, [`git clone`](https://git-scm.com/book/en/v2/Git-Basics-Getting-a-Git-Repository#_git_cloning) this source / download it and unpack it.

3. Build the kernel from source to check the build works ([paraphrased from xda-developers](https://forum.xda-developers.com/android/software-hacking/reference-how-to-compile-android-kernel-t3627297)).
	* install a cross-compiler for [your device's **architecture**](https://wiki.lineageos.org/devices/).
	* setup environment to target cross-compiler: `export CROSS_COMPILE=<compiler-prefix->`, where if your cross-compiler's `gcc` is `/usr/bin/aarch64-linux-android-gcc`, then the prefix is `/usr/bin/aarch64-linux-android-`.
	* setup environment to target correct architecture: `export ARCH=<arch> && export SUBARCH=<arch>`. Here, `<arch>` can be `arm`, `arm64`, `x86`, etc.
	* setup a Python 2 virtualenv: `virtualenv2 kernelbuild && source kernelbuild/bin/activate`
	* find your defconfig. This will be inside `arch/<arch>/configs`, and have a name like `<codename>_defconfig`, e.g. `bullhead_defconfig` for the Nexus 5X. If you cannot find the correct file, look up your specific device.
	* run:
		* `make clean`
		* `make mrproper`
		* `make <defconfig_name>`
		* `make -j$(nproc --all)`
	* if, during the build, it fails due to a missing program or library, install, retry the command, and continue.

4. Build a replacement `boot.img` to flash in TWRP:
	* download the latest Android Image Kitchen (AIK) script from [this thread](https://forum.xda-developers.com/showthread.php?t=2073775), and unzip outside the kernel source.
	* take the existing `boot.img` from your chosen ROM (try looking inside the zip file), and copy it to the unpacked AIK folder.
	* run `./unpackimg.sh <your-boot-img>.img`.
	* copy your built kernel called `Image`, `Image-dtb`, `Image.gz` or `Image.gz-dtb`, from `arch/<arch>/boot` in the kernel folder, to the `split_img` subfolder inside the AIK folder.
	* navigate into the `split_img` folder, and replace the old `boot.img-zImage` with your compiled kernel.
	* navigate up a folder, to the AIK folder, and run `./repackimg.sh` to build an `image-new.img` image.

5. Install the compiled kernel, to check the compiled kernel still works:
	* reboot your device to TWRP.
	* copy the `image-new.img` to your device.
	* install from TWRP (you can press the `Install Image` button in the install menu to see images) to the boot partition.
	* re-install Magisk (or whichever root utility you choose), as it was likely lost when you flashed the new kernel

6. Reboot to ensure the device still works.

7. Find the required patch from [pelya/android-keyboard-gadget](https://github.com/pelya/android-keyboard-gadget). You will want to look in the subdirectory patches for a patch matching your device's description. If you cannot find your device, you will have to try applying one of the [generic patches according to your kernel version](https://github.com/pelya/android-keyboard-gadget/tree/master/patches/existing_tested/by-generic-kernel-version).

To apply a patch, download it to the kernel directory, and run `git apply name-of-file.patch` if you cloned the directory, or `patch -p1 < name-of-file.patch` if you downloaded the kernel.

8. Modify your kernel to support U2F:
	* using the previous patch for guidance, apply a U2F patch [also available here](Scripts/u2f-kernel.patch)

		```diff
		diff --git a/drivers/usb/gadget/android.c b/drivers/usb/gadget/android.c
		index ced63c82ae7..41ac7ac18ca 100644
		--- a/drivers/usb/gadget/android.c
		+++ b/drivers/usb/gadget/android.c
		@@ -55,20 +55,21 @@
		 #include "u_ctrl_hsic.c"
		 #include "u_data_hsic.c"
		 #include "u_ctrl_hsuart.c"
		 #include "u_data_hsuart.c"
		 #include "f_ccid.c"
		 #include "f_mtp.c"
		 #include "f_accessory.c"
		 #include "f_hid.h"
		 #include "f_hid_android_keyboard.c"
		 #include "f_hid_android_mouse.c"
		+#include "f_hid_android_u2f.c"
		 #include "f_rndis.c"
		 #include "rndis.c"
		 #include "f_qc_ecm.c"
		 #include "f_mbim.c"
		 #include "f_qc_rndis.c"
		 #include "u_data_ipa.c"
		 #include "u_bam_data.c"
		 #include "f_ecm.c"
		 #include "u_ether.c"
		 #include "u_qc_ether.c"
		@@ -2855,21 +2856,21 @@ static int uasp_function_bind_config(struct android_usb_function *f,

		 static struct android_usb_function uasp_function = {
		 	.name		= "uasp",
		 	.init		= uasp_function_init,
		 	.cleanup	= uasp_function_cleanup,
		 	.bind_config	= uasp_function_bind_config,
		 };

		 static int hid_function_init(struct android_usb_function *f, struct usb_composite_dev *cdev)
		 {
		-	return ghid_setup(cdev->gadget, 2);
		+	return ghid_setup(cdev->gadget, 3);
		 }

		 static void hid_function_cleanup(struct android_usb_function *f)
		 {
		 	ghid_cleanup();
		 }

		 static int hid_function_bind_config(struct android_usb_function *f, struct usb_configuration *c)
		 {
		 	int ret;
		@@ -2878,20 +2879,26 @@ static int hid_function_bind_config(struct android_usb_function *f, struct usb_c
		 	if (ret) {
		 		pr_info("%s: hid_function_bind_config keyboard failed: %d\n", __func__, ret);
		 		return ret;
		 	}
		 	printk(KERN_INFO "hid mouse\n");
		 	ret = hidg_bind_config(c, &ghid_device_android_mouse, 1);
		 	if (ret) {
		 		pr_info("%s: hid_function_bind_config mouse failed: %d\n", __func__, ret);
		 		return ret;
		 	}
		+	printk(KERN_INFO "hid u2f\n");
		+	ret = hidg_bind_config(c, &ghid_device_android_u2f, 2);
		+	if (ret) {
		+		pr_info("%s: hid_function_bind_config u2f failed: %d\n", __func__, ret);
		+		return ret;
		+	}
		 	return 0;
		 }

		 static struct android_usb_function hid_function = {
		 	.name		= "hid",
		 	.init		= hid_function_init,
		 	.cleanup	= hid_function_cleanup,
		 	.bind_config	= hid_function_bind_config,
		 };

		diff --git a/drivers/usb/gadget/f_hid.c b/drivers/usb/gadget/f_hid.c
		index 155d9fb3a08..117bc00feac 100644
		--- a/drivers/usb/gadget/f_hid.c
		+++ b/drivers/usb/gadget/f_hid.c
		@@ -57,21 +57,21 @@ struct f_hidg {
		 	int				minor;
		 	struct cdev			cdev;
		 	struct usb_function		func;

		 	struct usb_ep			*in_ep;
		 	struct usb_ep			*out_ep;
		 };

		 /* Hacky device list to fix f_hidg_write being called after device destroyed.
		    It covers only most common race conditions, there will be rare crashes anyway. */
		-enum { HACKY_DEVICE_LIST_SIZE = 4 };
		+enum { HACKY_DEVICE_LIST_SIZE = 6 };
		 static struct f_hidg *hacky_device_list[HACKY_DEVICE_LIST_SIZE];
		 static void hacky_device_list_add(struct f_hidg *hidg)
		 {
		 	int i;
		 	for (i = 0; i < HACKY_DEVICE_LIST_SIZE; i++) {
		 		if (!hacky_device_list[i]) {
		 			hacky_device_list[i] = hidg;
		 			return;
		 		}
		 	}
		diff --git a/drivers/usb/gadget/f_hid_android_u2f.c b/drivers/usb/gadget/f_hid_android_u2f.c
		new file mode 100644
		index 00000000000..950c244e032
		--- /dev/null
		+++ b/drivers/usb/gadget/f_hid_android_u2f.c
		@@ -0,0 +1,28 @@
		+#include <linux/platform_device.h>
		+#include <linux/usb/g_hid.h>
		+
		+/* HID descriptor for a mouse */
		+static struct hidg_func_descriptor ghid_device_android_u2f = {
		+	.subclass      = 0x01, /* Boot Interface Subclass */
		+	.protocol      = 0x00, /* Raw HID */
		+	.report_length = 512,
		+	.report_desc_length	= 34,
		+	.report_desc = {
		+		0x06, 0xD0, 0xF1, // Usage Page (FIDO Usage Page)
		+		0x09, 0x01,       // Usage (FIDO Usage U2F HID)
		+		0xA1, 0x01,       // Collection (Application)
		+		0x09, 0x20,       // Usage (FIDO Usage Data In)
		+		0x15, 0x00,       // Logical Minimum (0)
		+		0x26, 0xFF, 0x00, // Logical Maximum (255)
		+		0x75, 0x08,       // Report Size (8)
		+		0x95, 0x40,       // Report Count (HID Input Report Bytes)
		+		0x81, 0x02,       // Input (HID Data & HID Absolute & HID Variable)
		+		0x09, 0x21,       // Usage (FIDO Usage Data Out)
		+		0x15, 0x00,       // Logical Minimum (0)
		+		0x26, 0xFF, 0x00, // Logical Maximum (255)
		+		0x75, 0x08,       // Report Size (8)
		+		0x95, 0x40,       // Report Count (HID Output Report Bytes)
		+		0x91, 0x02,       // Output (HID Data & HID Absolute & HID Variable)
		+		0xC0              // End Collection
		+	}
		+};
		```

	* note that some of these settings are non-standard, and work around limitations experienced when debugging. See [warnings](#Warnings) below for more details.

9. Recompile the kernel
	* run `make -k$(nproc --all)` again.
	* copy kernel to split_img subfolder inside AIK folder.
	* rename the kernel image.
	* run `./repackimg.sh` from AIK folder.
	* reboot device to TWRP and copy `image-new.img` to device.
	* install new image to `boot` partition, and reinstall Magisk.
	* reboot device to check it still boots.

For Nexus 5X users, see the `precompiled` subdirectory for both the kernel, in the form of a precompiled `boot.img` and a precompiled form of this project. Using these, you may skip to [running the program](#Run-the-program).

## Setting up udev rules

For Linux only.

On most distributions of Linux, devices get automatically managed based upon certain tags they expose to the computer. This program exposes custom tags to uniquely identify it from other U2F keys. However, as a result, the automatic rules do not include it in the list of USB devices to mount as U2F keys.

To find the required details of your Android device, run `lsusb`. Look for something named `Bus 0?? Device 0??: ID 18d1:4ee2 Google Inc. Nexus Device (debug)` or similar. Keep track of the value in your output like **`18d1:4ee2`**.

On your Linux desktop, run the command `ls /etc/udev/rules.d/`. Look for anything which seems related to U2F.
For example, on my computer, I have the rules `/etc/udev/rules.d/70-u2f.rules`.

Inside this file, the contents are:

```
# Copyright (C) 2013-2015 Yubico AB
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.

# this udev file should be used with udev 188 and newer
ACTION!="add|change", GOTO="u2f_end"

# Yubico YubiKey
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="1050", ATTRS{idProduct}=="0113|0114|0115|0116|0120|0200|0402|0403|0406|0407|0410", TAG+="uaccess"

# Happlink (formerly Plug-Up) Security KEY
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="2581", ATTRS{idProduct}=="f1d0", TAG+="uaccess"

# Neowave Keydo and Keydo AES
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="1e0d", ATTRS{idProduct}=="f1d0|f1ae", TAG+="uaccess"

# HyperSecu HyperFIDO
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="096e|2ccf", ATTRS{idProduct}=="0880", TAG+="uaccess"

# Feitian ePass FIDO
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="096e", ATTRS{idProduct}=="0850|0852|0853|0854|0856|0858|085a|085b", TAG+="uaccess"

# JaCarta U2F
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="24dc", ATTRS{idProduct}=="0101", TAG+="uaccess"

# U2F Zero
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="8acf", TAG+="uaccess"

# VASCO SeccureClick
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="1a44", ATTRS{idProduct}=="00bb", TAG+="uaccess"

# Bluink Key
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="2abe", ATTRS{idProduct}=="1002", TAG+="uaccess"

# Thetis Key
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="1ea8", ATTRS{idProduct}=="f025", TAG+="uaccess"

# Nitrokey FIDO U2F
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="20a0", ATTRS{idProduct}=="4287", TAG+="uaccess"

# Google Titan U2F
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="18d1", ATTRS{idProduct}=="5026", TAG+="uaccess"

LABEL="u2f_end"
```

Basically, this file contains the same contents as [Yubico's udev rules](https://github.com/Yubico/libu2f-host/blob/master/70-u2f.rules).
If you don't have any rules, download the [raw file](https://raw.githubusercontent.com/Yubico/libu2f-host/master/70-u2f.rules), and copy it to the `/etc/udev/rules.d/` directory.

Then, add:

```
# Android Device U2F
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="18d1", ATTRS{idProduct}=="4ee2", TAG+="uaccess"
```
on lines just before `LABEL="u2f_end"`, replacing `18d1` and `4ee2` with their corresponding values from your previous `lsusb` output.
Then, reload the rules using `sudo udevadm control --reload-rules `

## Setting up to build

1. Grab all the submodules using `git submodule update --init --recursive`
2. Copy the `cpp-base64` Makefile using `cp Scripts/cpp-base64-Makefile cpp-base64/Makefile`
3. Copy the `micro-ecc` Makefile using `cp Scripts/uECC-Makefile micro-ecc/Makefile`
4. Make the object file directories using `mkdir obj && mkdir cpp-base64/obj && mkdir micro-ecc/obj`
5. Grab the required mbedtls library:
	* outside this directory, run `git clone --recursive https://github.com/ARMmbed/mbedtls.git`
	* enter the directory with `cd mbedtls`
	* build for cross-compile with `sudo make DESTDIR=/opt
		CXX=<cross-compiler-prefix>-g++ CC=<cross-compiler-prefix>-gcc install`,
		using the cross-compiler-prefix previously discussed in the [_Getting the
		kernel_](#Getting-the-kernel) phase.

## Build the program

Run `make CROSS_COMPILE=<cross-compiler-prefix>`, using the cross-compiler-prefix previously discussed in the [_Getting the kernel_ phase](#Getting-the-kernel).

## Run the program

1. If you haven't already, enable [device USB debugging](https://developer.android.com/studio/debug/dev-options#enable).
2. Copy the program to the device to test with `adb push U2FDevice /data/local/tmp`
3. Open up a shell to your phone, using `adb shell`.
4. Navigate to where the file is stored, running `cd /data/local/tmp`.
5. Get SuperUser access, with `su`. Note, you may need to open the manager
	 program of your root toolkit of choice to permit root.
6. Run the program with `./U2FDevice`.

# Warnings

## Logical issues

This project requests a 512-byte USB HID packet size. This contravenes the specification,
which states that 64 bytes should be used instead. This was chosen as, at least on the test
hardware, it was found that perfect multiples of the packet size caused packets to be
cached, and the device unable to read them, until four times the packet size was sent.

As a result, by maximising the packet size, unless a message of 512, 1024, 1536, ...
bytes needs to be sent, this caching issue will not arise. This obviously has a reduced
probability of occurring than with a 64-byte packet size. Additionally, 512 bytes is the
maximum limit of packet size on USB Version 2.0. Therefore, it cannot be made any larger.

If this issue does not arise with your hardware (i.e., you change `.report_length = 512`
in `drivers/usb/gadget/f_hid_android_u2f.c` to `.report_length = 64`, and U2F still works
after you reinstall the kernel), this is probably even better, and more compatible with
client U2F libraries.

## Security issues

This project is intended solely for the use in experimentation of the use of
U2F or as a backup for keys. It is _not_ intended for use as a regular
day-to-day key for several reasons.

* Private keys are stored in a freely accessible file (to all apps on the device) `/sdcard/U2F/U2F_Priv_Keys.txt`
* This program doesn't comply with the specification with regards to user
	interaction. There is a specific code sent to check for user interaction for
	registering or authenticating keys. This requirement is ignored by this
	implementation as currently, the program cannot natively interface with the
	Android device's buttons.
* Whilst this program is functional, it has the possibility of unintended
	crashes. I have tested to the limits I require, but you may require
	additional assurance.
* This solution is rather unwieldy - it requires a modified kernel and must be
	launched from the commandline.

For these reasons, if you want to use this as a way to backup your other U2F
devices against loss, this may be a very valid solution, but please don't rely
solely on this solution for U2F security.

## To change the Attestation certificate

This is probably highly advisable.
All registration requests use this private key, so you can be uniquely
identified by having a unique attestation certificate, however, it permits you
to have a secure attestation certificate, so still worth it.

See the [`Readme.AttestationCertificateGeneration.txt`](Readme.AttestationCertificateGeneration.txt)

# Running the program

At this point, the program should be tested using U2F demo websites. For
example, [Yubico's U2F demo](https://demo.yubico.com/u2f?tab=register),
[Yubico's WebAuthn demo](https://demo.yubico.com/webauthn-technical/registration),
or [DUO Lab's WebAuthn demo](https://webauthn.io/). First register the device, then test authentication.

If the program doesn't work on these - don't use as a backup device.

## Debug files

For those of you wishing to dig around in the actual protocol work, these are the files used by the application to log the activity. For this, you must enable the `DEBUG_STREAMS` flag in `Architecture.hpp`, and recompile.

The documents used for raw communication contain just that - the raw data sent to and from the device.

To view this data, I would recommend using `od` or `xxd`. For example, `cat comdev.txt | od -tx1 -Anone -v` in order  to print out the bytes sent from the Android device to the PC in hexadecimal form.

The documents used for packets show the higher-level structures used in the U2F protocol. The first level above the data sent in USB frames is the `U2F-HID` protocol. The most recent specification for these packets is available [here](https://fidoalliance.org/specs/fido-u2f-v1.2-ps-20170411/fido-u2f-hid-protocol-v1.2-ps-20170411.html) (as of 12/07/2018). This level details mainly how the PC performs the setup for talking to the device.

The next level above that is the actual `U2F` protocol. The most recent specification for these messages is available [here](https://fidoalliance.org/specs/fido-u2f-v1.2-ps-20170411/fido-u2f-raw-message-formats-v1.2-ps-20170411.html) (as of 12/07/2018). This level details how registration and authentication actually occurs.

These file paths assume that the U2FDevice program is being run from `/data/local/tmp/`, as `./U2FDevice`.

* __Raw communication__
	* /data/local/tmp/comdev.txt
	* /data/local/tmp/comhost.txt

* __Packets__
	* /data/local/tmp/devPackets.html
	* /data/local/tmp/devAPDU.html
	* /data/local/tmp/hostPackets.html
	* /data/local/tmp/hostAPDU.html
