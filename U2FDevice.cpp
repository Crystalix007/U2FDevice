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

#include "Architecture.hpp"
#include <iostream>
#include "Storage.hpp"
#include "Controller.hpp"
#include "LED.hpp"
#include <csignal>
#include <unistd.h>

using namespace std;

void signalCallback(int signum);

volatile bool contProc = true;

bool initialiseLights(const string& prog) {
	try
	{
		disableACTTrigger(true);
		enableACTLED(false);
	}
	catch (runtime_error &e)
	{
		cerr << e.what() << endl;

		return false;
	}

	return true;
}

int handleTransactions(const string& prog, const string& privKeyDir)
{
	signal(SIGINT, signalCallback);
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

			raise(SIGINT);
			return EXIT_FAILURE;
		}
		usleep(10000);
	}

	Storage::save();
	return EXIT_SUCCESS;
}

bool deinitialiseLights(const string& prog) {
	try
	{
		disableACTTrigger(false);
		enableACTLED(true);
	}
	catch (runtime_error &e)
	{
		cerr << e.what() << endl;

		return false;
	}

	return true;
}

void signalCallback([[maybe_unused]] int signum)
{
	contProc = false;
	clog << "\nClosing" << endl;
}
