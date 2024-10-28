#pragma once

#include "BitWork.h"



/// 
/// b . . . . . . .
/// . . . . . . . . 
/// . . P . . . . . 
/// . . . K . . . . 
/// 
/// у Pinner должны быть
///		поля по прямой : KING - Pinner
///		фигура, которая связана
/// 
/// U64 table[64]
/// key = blockerPOS
/// table[key] = поля по прямой : (kingINDEX, pinnerINDEX]
/// 

struct Threats
{
	Threats() = default;
	Threats(const Threats&) = delete;

	void refresh() noexcept;
	void insert(UL index, U64 possible) noexcept;

	int checks_counter = 0;			// количество шахов
	U64 pin_table[64]{};			// хеш таблица для связанных фигур (поля на которые может пойти связанные фигура)
	U64 sqrs_to_deffend = 0;		// при шахе это поля, по которым можно защититься
	U64 opp_attacks = 0;			// атаки вражеских фигур
	U64 pinned = 0;					// связанные фигуры
};