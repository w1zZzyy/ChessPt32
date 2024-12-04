#pragma once

#include "BitWork.h"
#include "Zobrist.h"

#include <optional>
#include <shared_mutex>

enum EntryType
{
	EXACT,
	LOWER_BOUND, // alpha_cut 
	UPPER_BOUND // beta_cut
};

struct Entry
{
	Zobrist key;
	int depth;
	float eval;
	EntryType type;

	inline explicit Entry() : depth(0), eval(0), type(EXACT) {}
	inline explicit Entry(const Zobrist& k, int d, float e, EntryType t) :
		key(k),
		depth(d),
		eval(e),
		type(t)
	{}
};

class TransPositionTable
{
private:

	size_t TABLESIZE;
	Entry* table;
	std::shared_mutex* _mtx_;

public:

	TransPositionTable();

	void store(const Zobrist& key, Entry&& entry) noexcept;
	std::optional<Entry> find(const Zobrist& key) const noexcept;

	~TransPositionTable();

};