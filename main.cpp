#include "Architecture.hpp"
#include "U2FDevice.hpp"
#include <execinfo.h>
#include <iostream>

#ifdef DEBUG_MSGS
// Courtesy StackOverflow answer https://stackoverflow.com/a/3356421
void terminateHandler() {
	void* trace_elems[20];
	int trace_elem_count(backtrace(trace_elems, 20));
	char** stack_syms(backtrace_symbols(trace_elems, trace_elem_count));
	for (int i = 0; i < trace_elem_count; ++i) {
		std::cout << stack_syms[i] << "\n";
	}
	free(stack_syms);

	exit(1);
}
#endif

int main(int argc, char** argv) {
#ifdef DEBUG_MSGS
	std::set_terminate(terminate_handler);
#endif
	int retCode = handleTransactions(argv[0], argc == 2 ? argv[1] : STORAGE_PREFIX);

	try {
		initialiseLights(argv[0]);
		deinitialiseLights(argv[0]);
	} catch (std::exception& e) {
		std::cerr << "Exception in code: " << e.what() << std::endl;
		throw;
	}

	return retCode;
}
