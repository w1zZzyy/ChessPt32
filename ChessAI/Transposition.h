#pragma once
#include "BitWork.h"
#include <unordered_map>

enum class CUT_FLAG
{
	EXACT,
	ALPHA,
	BETA
};

struct Position
{
	float eval;
	int depth;
	CUT_FLAG flag;
};

struct Hash
{
	size_t operator() (const U64& pos) const noexcept
	{
		return (pos * 239017l) % (1ULL << 32);
	}
};

class Transposition
{
private:

	inline static std::unordered_map<U64, Position, Hash> data;

public:

	static void Setup() noexcept;
	static const Position* find(U64 hash) noexcept;
	static void store(U64 key, Position&& p) noexcept;

};
