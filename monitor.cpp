#include <iostream>
#include "Storage.hpp"
#include "Controller.hpp"
#include "LED.hpp"
#include <signal.h>
#include <unistd.h>

using namespace std;

void signalCallback(int signum);

volatile bool contProc = true;

int main()
{
	try
	{
		disableACTTrigger(true);
		enableACTLED(false);
	}
	catch (runtime_error &e)
	{
		cerr << e.what() << endl;
	}

	signal(SIGINT, signalCallback);
	Storage::init();

	Controller ch{ 0xF1D00000 };

	while (contProc)
	{
		ch.handleTransaction();
		usleep(10000);
	}

	Storage::save();

	try
	{
		disableACTTrigger(false);
		enableACTLED(true);
	}
	catch (runtime_error &e)
	{
		cerr << e.what() << endl;
	}

	return EXIT_SUCCESS;
}

void signalCallback([[maybe_unused]] int signum)
{
	contProc = false;
	clog << "\nCaught SIGINT signal" << endl;
}
