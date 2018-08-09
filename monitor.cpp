#include <iostream>
#include "Storage.hpp"
#include "Controller.hpp"
#include <signal.h>
#include <unistd.h>

using namespace std;

void signalCallback(int signum);

volatile bool contProc = true;

int main()
{
	signal(SIGINT, signalCallback);
	Storage::init();

	Controller ch{ 0xF1D00000 };

	while (contProc)
	{
		ch.handleTransaction();
		usleep(10000);
	}

	Storage::save();

	return EXIT_SUCCESS;
}

void signalCallback(int signum)
{
	contProc = false;
	clog << "Caught SIGINT signal" << endl;
}
