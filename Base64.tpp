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

#include "Base64.hpp"
#include <sstream>
#include "cpp-base64/base64.h"
#include <array>
#include <vector>
#include <algorithm>
#include "Field.hpp"

template <typename InContainer, typename OutContainer>
void b64encode(const InContainer &iContainer, OutContainer &oContainer)
{
	b64encode(std::begin(iContainer), std::end(iContainer), std::back_inserter(oContainer));
}

template <typename InContainer, typename OutContainer>
void b64decode(const InContainer &iContainer, OutContainer &oContainer)
{
	b64decode(std::begin(iContainer), std::end(iContainer), std::back_inserter(oContainer));
}

template <typename InContainer, typename Elem, size_t count>
void b64encode(const InContainer &iContainer, std::array<Elem, count> &oArr)
{
	b64encode(std::begin(iContainer), std::end(iContainer), std::begin(oArr));
}

template <typename InContainer, typename Elem, size_t count>
void b64decode(const InContainer &iContainer, std::array<Elem, count> &oArr)
{
	b64decode(std::begin(iContainer), std::end(iContainer), std::begin(oArr));
}

template <typename InContainerIter, typename OutContainerIter>
void b64encode(const InContainerIter beginIter, const InContainerIter endIter, OutContainerIter oBeginIter)
{
	std::vector<uint8_t> rawBytes{};

	for (auto iter = beginIter; iter != endIter; iter++)
		rawBytes.insert(rawBytes.end(), FIELD(*iter));

	auto encoded = base64_encode(rawBytes.data(), rawBytes.size());
	std::copy(encoded.begin(), encoded.end(), oBeginIter);
}

template <typename InContainerIter, typename OutContainerIter>
void b64decode(const InContainerIter beginIter, const InContainerIter endIter, OutContainerIter oBeginIter)
{
	auto decoded = base64_decode(std::string{ beginIter, endIter });
	std::copy(decoded.begin(), decoded.end(), oBeginIter);
}
