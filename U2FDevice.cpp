#include <iostream>
#include "Storage.hpp"
#include "Controller.hpp"
#include "LED.hpp"
#include <signal.h>
#include <unistd.h>

using namespace std;

void signalCallback(int signum);

volatile bool contProc = true;

int main(int argc, char **argv)
{
	try
	{
		disableACTTrigger(true);
		enableACTLED(false);
	}
	catch (runtime_error &e)
	{
		cerr << e.what() << endl;

		if (getuid() != 0)
			cerr << "Try running as root, using \'sudo " << argv[0] << "\'" << endl;
	}

	signal(SIGINT, signalCallback);

	string privKeyDir = (argc == 2 ? argv[1] : "/usr/share/U2FDevice/");

	Storage::init(privKeyDir);

	Controller ch{ 0xF1D00000 };

	while (contProc)
	{
		try
		{
			ch.handleTransaction();
		}
		catch (const runtime_error &e)
		{
			cerr << e.what() << endl;

			if (getuid() != 0)
				cerr << "Try running as root, using \'sudo " << argv[0] << "\'" << endl;
			raise(SIGINT);
			return EXIT_FAILURE;
		}
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
	clog << "\nClosing" << endl;
}
