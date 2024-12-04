#include "Transposition.h"

TransPositionTable::TransPositionTable()
{
	this->TABLESIZE = 89475072;
	table = new Entry[TABLESIZE];
	_mtx_ = new std::shared_mutex[1024];
}

void TransPositionTable::store(const Zobrist& key, Entry&& entry) noexcept
{
	size_t _key = key.getKey() % TABLESIZE;
	size_t mtx_key = _key % 1024;
	std::lock_guard<std::shared_mutex>l(_mtx_[mtx_key]);
	if (table[_key].depth < entry.depth)
		table[_key] = entry;
}

std::optional<Entry> TransPositionTable::find(const Zobrist& key) const noexcept
{
	size_t _key = key.getKey() % TABLESIZE;
	size_t mtx_key = _key % 1024;
	std::lock_guard<std::shared_mutex>l(_mtx_[mtx_key]);
	if (table[_key].key != key)
		return std::nullopt;
	return table[_key];
}

TransPositionTable::~TransPositionTable()
{
	delete[] table;
	delete[] _mtx_;
}
