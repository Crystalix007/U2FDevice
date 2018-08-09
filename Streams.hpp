#pragma once
#include <cstdio>
#include <memory>

std::shared_ptr<int>  getHostDescriptor();
std::shared_ptr<FILE> getComHostStream();
std::shared_ptr<FILE> getHostPacketStream();
std::shared_ptr<FILE> getHostAPDUStream();
std::shared_ptr<FILE> getComDevStream();
std::shared_ptr<FILE> getDevPacketStream();
std::shared_ptr<FILE> getDevAPDUStream();
