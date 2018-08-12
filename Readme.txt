Things to install

//To provide HW RNG facilities
sudo apt-get install rng-tools

//To provide ECC facilities
git submodule update --init --recursive
cp Scripts/uECC-Makefile micro-ecc/Makefile
cp Scripts/cpp-base64-Makefile cpp-base64/Makefile

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
