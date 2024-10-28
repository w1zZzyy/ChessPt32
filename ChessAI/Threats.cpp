#include "Threats.h"



void Threats::refresh() noexcept
{
	while (pinned)
	{
		UL index = BSF64(pinned);
		pin_table[index] = 0;
	}

	checks_counter = 0;
	sqrs_to_deffend = 0;
	opp_attacks = 0;
}

void Threats::insert(UL index, U64 possible) noexcept
{
	pinned |= (1ULL << index);
	pin_table[index] = possible;
}
