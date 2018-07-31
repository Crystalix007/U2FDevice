#pragma once

#define FIELD(name) reinterpret_cast<uint8_t*>(&name), (reinterpret_cast<uint8_t*>(&name) + sizeof(name))
