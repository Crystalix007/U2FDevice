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

#pragma once

enum APDU : uint8_t { U2F_REG = 0x01, U2F_AUTH = 0x02, U2F_VER = 0x03 };

enum APDU_STATUS : uint16_t {
	SW_NO_ERROR = 0x9000,
	SW_WRONG_LENGTH = 0x6700,
	SW_CONDITIONS_NOT_SATISFIED = 0x6985,
	SW_WRONG_DATA = 0x6A80,
	SW_INS_NOT_SUPPORTED = 0x6D00,
	SW_COMMAND_NOT_ALLOWED = 0x6E00,
};
