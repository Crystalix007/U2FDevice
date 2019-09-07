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

#include "Streams.hpp"
#include "Architecture.hpp"
#include "IO.hpp"
#include <android/log.h>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

using std::cerr;
using std::clog;
using std::endl;
using std::runtime_error;
using std::shared_ptr;
using std::string;

shared_ptr<int> initialiseHostDescriptor();

#ifdef DEBUG_STREAMS

FILE* initHTML(FILE* fPtr, const string& title);

void closeHTML(FILE* fPtr);

#endif

shared_ptr<int> getHostDescriptor() {
	static shared_ptr<int> descriptor{ initialiseHostDescriptor() };
	return descriptor;
}

#ifdef DEBUG_STREAMS

shared_ptr<FILE> getComHostStream() {
	static shared_ptr<FILE> stream{ fopen((cacheDirectory + "comhost.txt").c_str(), "wb"),
		                            [](FILE* f) {
		                                clog << "Closing comhost stream" << endl;
		                                fclose(f);
		                            } };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getHostPacketStream() {
	static shared_ptr<FILE> stream{
		initHTML(fopen((cacheDirectory + "hostpackets.html").c_str(), "wb"), "Host Packets"),
		[](FILE* f) {
		    clog << "Closing hostPackets stream" << endl;
		    closeHTML(f);
		}
	};

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getHostAPDUStream() {
	static shared_ptr<FILE> stream{
		initHTML(fopen((cacheDirectory + "hostAPDU.html").c_str(), "wb"), "Host APDU"),
		[](FILE* f) {
		    clog << "Closing host APDU stream" << endl;
		    closeHTML(f);
		}
	};

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getComDevStream() {
	static shared_ptr<FILE> stream{ fopen((cacheDirectory + "comdev.txt").c_str(), "wb"),
		                            [](FILE* f) {
		                                clog << "Closing comdev stream" << endl;
		                                fclose(f);
		                            } };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getDevPacketStream() {
	static shared_ptr<FILE> stream{
		initHTML(fopen((cacheDirectory + "devpackets.html").c_str(), "wb"), "Dev Packets"),
		[](FILE* f) {
		    clog << "Closing devPackets stream" << endl;
		    closeHTML(f);
		}
	};

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getDevAPDUStream() {
	static shared_ptr<FILE> stream{ initHTML(fopen((cacheDirectory + "devAPDU.html").c_str(), "wb"),
		                                     "Dev APDU"),
		                            [](FILE* f) {
		                                clog << "Closing dev APDU stream" << endl;
		                                closeHTML(f);
		                            } };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

FILE* initHTML(FILE* fPtr, const string& title) {
	fprintf(fPtr,
	        "<html>\n"
	        "\t<head>\n"
	        "\t\t<title>%s</title>\n"
	        "\t\t<style>\n"
	        "\t\t\ttable {\n"
	        "\t\t\t\tdisplay: table;\n"
	        "\t\t\t\twidth: 100%%;\n"
	        "\t\t\t\tborder-collapse: collapse;\n"
	        "\t\t\t\tbox-sizing: border-box;\n"
	        "\t\t\t}\n"
	        "\n"
	        "\t\t\tth.data {\n"
	        "\t\t\t\ttext-align: left;\n"
	        "\t\t\t}\n"
	        "\n"
	        "\t\t\ttd {\n"
	        "\t\t\t\tfont-family: \"Courier New\", Courier, monospace;\n"
	        "\t\t\t\twhite-space: pre;\n"
	        "\t\t\t}\n"
	        "\n"
	        "\t\t\ttd.data {\n"
	        "\t\t\t\toverflow: hidden;\n"
	        "\t\t\t\ttext-overflow: ellipsis;\n"
	        "\t\t\t\tmax-width:1px;\n"
	        "\t\t\t\twidth:100%%;\n"
	        "\t\t\t}\n"
	        "\n"
	        "\t\t\ttd.data:hover {\n"
	        "\t\t\t\twhite-space: pre-wrap;\n"
	        "\t\t\t}\n"
	        "\n"
	        "\t\t\ttable, th, td {\n"
	        "\t\t\t\tborder: 1px solid black;\n"
	        "\t\t\t}\n"
	        "\t\t</style>\n"
	        "\t</head>\n"
	        "\n"
	        "\t<body>",
	        title.c_str());

	return fPtr;
}

void closeHTML(FILE* fPtr) {
	fprintf(fPtr, "\t</body>\n"
	              "</html>");
	fclose(fPtr);
}

#endif

shared_ptr<int> initialiseHostDescriptor() {
	int descriptor;

#ifdef HID_SOCKET
	if (access(clientSocket.c_str(), F_OK)) {
		remove(clientSocket.c_str());
	}

	descriptor = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
	if (descriptor == -1)
		throw runtime_error{ "Unable to open client socket" };

	sockaddr_un serverSockAddr{}, clientSockAddr{};

	clientSockAddr.sun_family = AF_UNIX;
	strncpy(clientSockAddr.sun_path, clientSocket.c_str(), sizeof(clientSockAddr.sun_path) - 1);

	// Attempt to remove existing
	unlink(clientSocket.c_str());

	int result = ::bind(descriptor, (sockaddr*)&clientSockAddr, sizeof(clientSockAddr));
	if (result == -1)
		throw runtime_error{ "Unable to bind to client socket: " + clientSocket };

	serverSockAddr.sun_family = AF_UNIX;
	strncpy(serverSockAddr.sun_path, HID_DEV, sizeof(serverSockAddr.sun_path) - 1);

	for (size_t connectCount = 0; connectCount < 100; connectCount++) {
		result = connect(descriptor, (sockaddr*)&serverSockAddr, sizeof(serverSockAddr));
		if (result != -1)
			break;
		usleep(100'000);
	}

	if (result == -1)
		throw runtime_error{ "Unable to connect to server socket: " + string{ HID_DEV } };

	__android_log_print(ANDROID_LOG_DEBUG, "U2FDevice", "Connected to server");

	return shared_ptr<int>{ new int{ descriptor }, [](const int* fd) {
		                  close(*fd);
		                  remove(clientSocket.c_str());
		                  delete fd;
		              } };
#else
	descriptor = open(HID_DEV, O_RDWR | O_NONBLOCK | O_APPEND);

	if (descriptor == -1)
		throw runtime_error{ "Descriptor is unavailable" };

	return shared_ptr<int>{ new int{ descriptor }, [](const int* fd) {
		                  close(*fd);
		                  delete fd;
		              } };
#endif
}
