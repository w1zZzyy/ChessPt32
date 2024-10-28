#include "Transposition.h"
#include "shared_mutex"

std::shared_mutex mtx;

void Transposition::Setup() noexcept
{
	data.reserve(1048576);
	data.max_load_factor(0.25);
}

const Position* Transposition::find(U64 hash) noexcept
{
	std::shared_lock<std::shared_mutex>lock(mtx);
	auto it = data.find(hash);
	if (it == data.end())
		return nullptr;
	return &it->second;
}

void Transposition::store(U64 key, Position&& p) noexcept
{
	{
		std::shared_lock<std::shared_mutex>lock(mtx);
		auto it = data.find(key);
		if (it != data.end())
		{
			if (p.depth > it->second.depth)
				it->second = p;
			return;
		}
	}
	std::lock_guard<std::shared_mutex>lock(mtx);
	data.insert({ key, p });
}