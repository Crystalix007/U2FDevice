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
