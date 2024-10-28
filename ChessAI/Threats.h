#pragma once

#include "BitWork.h"



/// 
/// b . . . . . . .
/// . . . . . . . . 
/// . . P . . . . . 
/// . . . K . . . . 
/// 
/// � Pinner ������ ����
///		���� �� ������ : KING - Pinner
///		������, ������� �������
/// 
/// U64 table[64]
/// key = blockerPOS
/// table[key] = ���� �� ������ : (kingINDEX, pinnerINDEX]
/// 

struct Threats
{
	Threats() = default;
	Threats(const Threats&) = delete;

	void refresh() noexcept;
	void insert(UL index, U64 possible) noexcept;

	int checks_counter = 0;			// ���������� �����
	U64 pin_table[64]{};			// ��� ������� ��� ��������� ����� (���� �� ������� ����� ����� ��������� ������)
	U64 sqrs_to_deffend = 0;		// ��� ���� ��� ����, �� ������� ����� ����������
	U64 opp_attacks = 0;			// ����� ��������� �����
	U64 pinned = 0;					// ��������� ������
};