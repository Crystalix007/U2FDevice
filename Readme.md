For the Android version of this, please see [the android branch](../../tree/android), as there is a completely different build procedure and resources.
This program uses a Raspberry Pi 0 to act as a [U2F token](https://www.yubico.com/solutions/fido-u2f/). This permits any person with a Raspberry Pi 0 to try out U2F 2FA for themselves.
![Raspberry Pi Zero](https://www.raspberrypi.org/app/uploads/2017/05/Raspberry-Pi-Zero-462x322.jpg "Raspberry Pi 0")

![SD Card with capacity > 2GB](https://upload.wikimedia.org/wikipedia/commons/d/da/MicroSD_cards_2GB_4GB_8GB.jpg "SD Card with capacity > 2GB")
![Confirmed-working micro USB data cable](Readme_Assets/Micro_USB_Cable.png "Micro USB Data Cable")
## Getting the image

Go [here](https://www.raspberrypi.org/downloads/raspbian/) and download the Raspbian Stretch _Lite_ image.<br />If you download the zip, make sure you extract it.

## Installing the image

Once you have the `*.img` file, you must write it to an SD card. <br />For this, I suggest the use of [etcher.io](https://etcher.io/) which works cross-platfrom. Alternatively, use whatever image burner you want.

## Mounting the SD Card

After the image burning has completed, most OSs should automatically mount the SD card. We only need access to the `boot` partition initially.

![Storage Monitor](Readme_Assets/Storage.png)

## Setting up SSH

Since this device can be set up without a monitor, we shall. <br />First, we need to enable SSH.
1. Open the boot partition - it should look like so: <br />![Boot Partition](Readme_Assets/Boot_partition.png)

2. Create a `ssh` file. Note that this doesn't have any extension. For Linux users, you can open a console in this directory, and `touch ssh` to create the file. This file enables `ssh` on the latest versions of Raspbian. <br />![ssh file](Readme_Assets/Create_ssh.png)

3. Edit `config.txt`. Using your preferred editor, add the line `dtoverlay=dwc2` to the end. <br />![Edited config.txt](Readme_Assets/Edit_config_txt.png)

4. Edit `cmdline.txt`. Again, using your preferred editor, between `rootwait` and the next word, insert `modules-load=dwc2,g_ether` leaving only one space after `rootwait` and one before the next word. <br />![Edited cmdline.txt](Readme_Assets/Edit_cmdline_txt.png)

5. Eject the SD card safely. <br />![Eject SD card](Readme_Assets/Eject_SD.png)

6. Insert the SD card into the Raspberry Pi. <br />![Inserted SD card](Readme_Assets/Insert_SD.png)

7. Use a known USB micro data cable to connect the Raspberry Pi to a computer.

   * If possible, check this cable can transfer data by connecting another device and attempting to transfer a file.
   * If possible, use a Linux computer for the next step, one with a graphical network manager.
   * If possible, use a USB 3 or higher port instead of a USB 2 port. This is because the USB port will supply the entire power for the Raspberry Pi, and some USB 2 ports may not be able to deliver the required power.

8. Let the Raspberry Pi fully boot up (once the LED stops blinking frequently, it is probably booted). Then you need to configure your network settings.

   * Linux:

     * Use your preferred network manager:

     * Create a new connection profile <br />![Creating connection profile](Readme_Assets/Add_connection.png)

     * Set it's type to be 'Wired Ethernet (shared)' if available, else, 'Wired Ethernet'

     * Name it <br />![Naming new connection](Readme_Assets/Configure_Connection_Name.png)

     * Ensure the method is set to 'Shared to other computers', or similar. <br />![Sharing connection to other computers](Readme_Assets/Configure_Connection_Method.png)

     * Save and connect on the connection not currently used. <br />![Connecting to new connection](Readme_Assets/Connect_to_connection.png)

     * Check your IP address on this network. <br />![Checking IP address](Readme_Assets/Checking_IP_Address.png)

       * This shows that the host computer (Linux) has the IP address `10.42.0.1`

       * Therefore, the Raspberry Pi has an IP address of `10.42.0.*`, where `*` can be any number 2-255.

       * Find the Raspberry Pi's address by using the tool [`nmap`](https://nmap.org/), installable from your preferred package manager.

         `nmap -sn "10.42.0.*"` and look for the IP address which the host doesn't have (i.e. look for the ip address other than 10.42.0.1 in this example).

         For example, when I ran the command, the output was <br />![10.42.0.172](Readme_Assets/Nmap_IP_scan.png)

         so the IP address I am looking for is `10.42.0.172`.

9. Connecting

  * Linux users
    * Now finally we can SSH into the Raspberry Pi:

         `ssh pi@RASPBERRY_PI_IP`

         So in my example, the command would be:

         `ssh pi@10.42.0.172`

  * Other OSs

    * For users of windows, see [ssh using PUTTY](https://desertbot.io/blog/headless-pi-zero-ssh-access-over-usb-windows#step-8-install-putty), and for users of OS X, you can simply ssh in using `ssh pi@raspberrypi.local` in a terminal.

  * This should then ask you if you want to continue connecting, displaying the ECDSA key.  Type `yes` and hit enter to continue.<br />![Continue connecting](Readme_Assets/SSH_connecting_ECDSA.png)

  * Then, when the password is asked for, type `raspberry` - the default password in Raspbian.

10. Then, change your password using `passwd`. Enter `raspberry` as the current password, and a memorable password for the new password.

## Setting up USB config for multiple drivers simultaneously

Past this point, do not reboot / power off unless you wish to start all over again.

1. Edit `/boot/cmdline.txt` by using `sudoedit /boot/cmdline.txt`
   * Remove the `modules-load=dwc2,g_ether` and ensure there is no trailing space.
   * Close and save by pressing `ctrl-x`.
2. Edit `/etc/modules` by using `sudoedit /etc/modules`
   * Add `libcomposite` at the end of the file (i.e. not on a line beggining with `#`).

## Setting up the libcomposite config script

### Getting Git

1. Update package list using `sudo apt-get update`
2. Get git by running `sudo apt-get install git`

### Using this respository

1. Grab the contents of this repository using `git clone https://github.com/Crystalix007/U2FDevice.git`
2. Enter the cloned directory by running `cd U2FDevice`
3. Install the config script using `sudo install -m755 Scripts/Kernel_HID_Config.sh /usr/bin/Kernel_HID_Config.sh`
4. Install the config script startup service by editing `/etc/rc.local`
   * `sudoedit /etc/rc.local`
   * Add `/usr/bin/Kernel_HID_Config.sh`, on a new line, before `'exit 0'`
5. At this point you are once again free to reboot / shutdown.
On your Linux desktop, run the command `ls /etc/udev/rules.d/`. Look for anything which seems related to U2F. <br />For example, on my computer, I have the rules `/etc/udev/rules.d/70-u2f.rules`. <br />Inside this file, the contents are:
Basically, this file contains the same contents as [Yubico's udev rules](https://github.com/Yubico/libu2f-host/blob/master/70-u2f.rules). <br />
If you don't have any rules, download the [raw file](https://raw.githubusercontent.com/Yubico/libu2f-host/master/70-u2f.rules), and copy it to the `/etc/udev/rules.d/` directory. <br />
Then, add: <br />
```# Rapsberry Pi U2F```<br /> ```KERNEL=="hidraw*", SUBSYSTEM=="hidraw", ATTRS{idVendor}=="1209", ATTRS{idProduct}=="2900", TAG+="uaccess"``` <br />
on lines just before `LABEL="u2f_end"`.
5. Grab the required library using `sudo apt-get install libmbedtls-dev`
1. Run `make`
## Install the program
1. Run `sudo make install`
## To shut down
This device cannot be powered off without running a command in SSH (for now). If the device has its power interrupted by a sudden poweroff, it is likely there will be corruption which will render all data on the SD card useless.
So, to power off currently, SSH into the device as shown above, then run the command `sudo poweroff ; exit`
This project is intended solely for the use in experimentation of the use of U2F or as a backup for keys. It is _not_ intended for use as a regular day-to-day key for several reasons.

* Private keys are stored in a freely accessible file (to users of the pi) `/usr/share/U2FDevice/U2F_Priv_Keys.txt`
* This program doesn't comply with the specification with regards to user interaction. There is a specific code sent to check for user interaction for registering or authenticating keys. This requirement is ignored by this implementation as there are no pre-existing buttons on the Pi.
* Whilst this program is functional, it has the possibility of unintended crashes. I have tested to the limits I require, but you may require additional assurance.
* This program's private keys are stored on an SD card. This is an incredibly volatile medium (and yes, regularly I mean that in the computing sense - SD cards regularly do cleanup / maintenance routines that can cause complete corruption if the power is lost suddenly). I would not consider these keys safe under very regular use (infrequent use should be fine though).
* This solution is rather unwieldy - it requires a long boot time and must be shutdown (for now) with a command using SSH.

For these reasons, if you want to use this as a way to backup your other U2F devices against loss, this may be a very valid solution, but please don't rely solely on this solution for U2F security.

## To improve RNG (improve crypto security)

1. Install `rng-tools` with `sudo apt-get install rng-tools`
This may be highly advisable, or inadvisable - I am currently unsure. <br />All registration requests use this private key, so likely advisable. <br/>However, you can be uniquely identified by having a unique attestation certificate.
See the `Readme.AttestationCertificateGeneration.txt`
## To run

Run `sudo systemctl start U2FDevice.service`

At this point, the program should be tested using U2F demo websites. For example, [Yubico's U2F demo](https://demo.yubico.com/u2f?tab=register), or [appspot's U2F demo](https://crxjs-dot-u2fdemo.appspot.com/). First register the device, then test authentication.
## To run automatically at boot
Once the program runs successfully, you can enable automatic startup at boot.
Run `sudo systemctl enable U2FDevice.service`
## Debug files

For those of you wishing to dig around in the actual protocol work, these are the files used by the application to log the activity.

The documents used for raw communication contain just that - the raw data sent to and from the device. <br />To view this data, I would recommend using `od` . For example, `cat /tmp/comdev.txt | od -tx1 -Anone -v` in order  to print out the bytes sent from the Raspberry Pi to the PC in hexadecimal form.
  * /tmp/comdev.txt
  * /tmp/comhost.txt
  * /tmp/devPackets.html
  * /tmp/devAPDU.html
  * /tmp/hostPackets.html
  * /tmp/hostAPDU.html