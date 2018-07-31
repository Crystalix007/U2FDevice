#pragma once
#include <cstdio>
#include <memory>

std::shared_ptr<FILE> getHostStream();
std::shared_ptr<FILE> getComHostStream();
std::shared_ptr<FILE> getHostPacketStream();
std::shared_ptr<FILE> getComDevStream();
std::shared_ptr<FILE> getDevPacketStream();
