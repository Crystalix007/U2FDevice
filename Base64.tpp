#include "Base64.hpp"
#include <sstream>
#include <b64/encode.h>
#include <b64/decode.h>
#include <array>
#include <algorithm>

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
	std::stringstream oss, iss{ std::string{ beginIter, endIter } };
	base64::encoder encoder{};
	encoder.encode(iss, oss);

	auto oStr = oss.str();
	oStr.erase(std::remove_if(oStr.begin(), oStr.end(), [](auto charac){ return charac == '\n'; }), oStr.end());
	copy(oStr.begin(), oStr.end(), oBeginIter);
}

template <typename InContainerIter, typename OutContainerIter>
void b64decode(const InContainerIter beginIter, const InContainerIter endIter, OutContainerIter oBeginIter)
{
	std::stringstream oss, iss{ std::string{ beginIter, endIter } };
	base64::decoder decoder{};
	decoder.decode(iss, oss);

	auto oStr = oss.str();
	copy(oStr.begin(), oStr.end(), oBeginIter);
}
