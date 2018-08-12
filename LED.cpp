#include "LED.hpp"
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

void disableACTTrigger(bool nowDisabled)
{
	ofstream trigFile{ "/sys/class/leds/led0/trigger", ofstream::out | ofstream::trunc };

	if (!trigFile)
		throw runtime_error{ "Failed to retrieve led0 trigger file" };

	if (!static_cast<bool>(trigFile << (nowDisabled ? "none" : "mmc0")))
		throw runtime_error{ "Failed to write led0 trigger to file" };
}

void enableACTLED(bool nowOn)
{
	if (nowOn == getLEDState())
		return;

	ofstream enabledFile{ "/sys/class/leds/led0/brightness", ofstream::out | ofstream::trunc };

	if (!enabledFile)
		throw runtime_error{ "Failed to retrieve led0 brightness" };

	if (!static_cast<bool>(enabledFile << to_string(nowOn ? 0 : 1)))
		throw runtime_error{ "Failed to write led0 brightness to file" };

	ledState() = nowOn;
}

void toggleACTLED()
{
	enableACTLED(!getLEDState());
}
