Things to install

//To provide HW RNG facilities
sudo apt-get install rng-tools

//To provide ECC facilities
git submodule update --init --recursive
cp Scripts/uECC-Makefile micro-ecc/Makefile

To build

make

//Currently no install command is provided
