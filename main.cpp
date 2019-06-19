#include "U2FDevice.hpp"
#include "Architecture.hpp"

int main(int argc, char **argv) {
	initialiseLights(argv[0]);
	int retCode = handleTransactions(argv[0], argc == 2 ? argv[1] : STORAGE_PREFIX);
	deinitialiseLights(argv[0]);
	return retCode;
}
