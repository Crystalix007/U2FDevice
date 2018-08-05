#include "Field.hpp"

using namespace std;

vector<uint8_t> beEncode(const uint8_t* val, const size_t byteCount)
{
	return { reverse_iterator<const uint8_t *>(val + byteCount), reverse_iterator<const uint8_t *>(val) };
}
