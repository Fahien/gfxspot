#pragma once

#include <cstdint>

namespace spot::gfx
{
struct ValidationLayers
{
	uint32_t     count = 0;
	const char** names = nullptr;
};


struct RequiredExtensions
{
	uint32_t     count = 0;
	const char** names = nullptr;
};

}  // namespace spot::gfx