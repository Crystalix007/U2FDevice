/*
U2FDevice - A program to allow Raspberry Pi Zeros to act as U2F tokens
Copyright (C) 2018  Michael Kuc

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
