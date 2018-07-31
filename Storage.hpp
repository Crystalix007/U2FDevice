#include <map>
#include <array>
#include <string>
#include <fstream>

class Storage
{
public:
	using KeyHandle = uint32_t;
	using AppParam  = std::array<uint8_t, 32>;
	using PrivKey   = std::array<uint8_t, 32>;
	using PubKey    = std::array<uint8_t, 65>;

	protected:
		Storage() = default;

		static std::string filename;

	public:
		static void init(const std::string &dirPrefix = "");
		static void save();
		static std::map<KeyHandle, AppParam> appParams;
		static std::map<KeyHandle, PrivKey>  privKeys;
		static std::map<KeyHandle, PubKey>   pubKeys;
};
