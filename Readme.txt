To make HID work

sudo install -m755 Scripts/Kernel_HID_Config.sh /usr/bin/Kernel_HID_Config.sh
sudoedit /etc/rc.local # Add '/usr/bin/Kernel_HID_Config.sh' on a new line before 'exit 0'

Things to install

To provide HW RNG facilities

sudo apt-get install rng-tools

To provide ECC facilities

git submodule update --init --recursive
cp Scripts/uECC-Makefile micro-ecc/Makefile
cp Scripts/cpp-base64-Makefile cpp-base64/Makefile

To change the Attestation certificate, see the Attestation readme

To build

make

To install

sudo make install

To run

sudo systemctl start U2FDevice.service

To run automatically at boot

sudo systmectl enable U2FDevice.service

Debug files at

Raw communication
/tmp/comdev.txt
/tmp/comhost.txt

Packets
/tmp/devPackets.html
/tmp/devAPDU.html
/tmp/hostPackets.html
/tmp/hostAPDU.html
