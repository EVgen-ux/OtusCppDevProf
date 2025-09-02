#pragma once

#include <cstddef>
#include <string>

namespace AsyncBulk {

using ContextID = size_t;

ContextID connect(size_t block_size);
void receive(ContextID context_id, const std::string& command);
void disconnect(ContextID context_id);
void shutdown();

} // namespace AsyncBulk