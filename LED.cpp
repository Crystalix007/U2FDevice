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

#include "LED.hpp"
#include "Architecture.hpp"
#include <fstream>
#include <stdexcept>
#include <string>

using namespace std;

bool& ledState()
{
	static bool state = true;
	return state;
}

bool getLEDState()
{
	return ledState();
}

void disableACTTrigger([[maybe_unused]] bool nowDisabled)
{
#ifdef LEDS
	ofstream trigFile{ "/sys/class/leds/led0/trigger", ofstream::out | ofstream::trunc };

	if (!trigFile)
		throw runtime_error{ "Failed to retrieve led0 trigger file" };

	if (!static_cast<bool>(trigFile << (nowDisabled ? "none" : "mmc0")))
		throw runtime_error{ "Failed to write led0 trigger to file" };
#endif
}

void enableACTLED([[maybe_unused]] bool nowOn)
{
#ifdef LEDS
	if (nowOn == getLEDState())
		return;

	ofstream enabledFile{ "/sys/class/leds/led0/brightness", ofstream::out | ofstream::trunc };

	if (!enabledFile)
		throw runtime_error{ "Failed to retrieve led0 brightness" };

	if (!static_cast<bool>(enabledFile << to_string(nowOn ? 0 : 1)))
		throw runtime_error{ "Failed to write led0 brightness to file" };

	ledState() = nowOn;
#endif
}

void toggleACTLED()
{
	enableACTLED(!getLEDState());
}
