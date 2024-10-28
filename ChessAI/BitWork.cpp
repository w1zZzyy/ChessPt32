#include "BitWork.h"

UL BSF64(U64& pos) noexcept
{
	UL index;
	_BitScanForward64(&index, pos);
	pos ^= (1ULL << index);
	return index;
}

UL BSF64_safe(const U64& pos) noexcept
{
	UL index;
	_BitScanForward64(&index, pos);
	return index;
}

int BitsCount(U64 pos) noexcept
{
	return __popcnt64(pos);
}
