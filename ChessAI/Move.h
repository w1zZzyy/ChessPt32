#pragma once

#include <memory>
#include "Bitboard.h"

constexpr int MOVES_CAPACITY = 218;

class Move
{
private:

	short move_value=0; // 4 - type 6 - to 6 - from
	short pieces=0; // 4 - castle 3 - prom 3 - pm 3 - pc


public:

	Move() = default;
	Move(int from, int to, int piece_moved);
	Move(int from, int to, MOVE_TYPE type);
	Move(int from, int to, MOVE_TYPE type, int piece_moved);
	Move(int from, int to, MOVE_TYPE type, int piece_moved, int piece_captured);

	void setProm(int piece_promoted) noexcept;
	void setType(MOVE_TYPE type) noexcept;
	void setCapture(int piece_captured) noexcept;
	void setCastle(int castle) noexcept;

	MOVE_TYPE getType() const noexcept;
	short getFrom() const noexcept;
	short getTo() const noexcept;
	PieceType getPieceMoved() const noexcept;
	PieceType getPieceCaptured() const noexcept;
	PieceType getPiecePromotion() const noexcept;
	short getCastle() const noexcept;

};


class MoveList
{
private:

	int size = 0;
	std::unique_ptr<Move[]> data;

public:

	MoveList();
	MoveList(MoveList&&) noexcept;
	MoveList(const MoveList&) = delete;

	MoveList& operator = (MoveList&&) noexcept;
	MoveList& operator = (const MoveList&) = delete;


	void push_back(const Move&) noexcept;
	void emplace_back(Move&&) noexcept;

	int getSize() const noexcept;
	bool empty() const noexcept;

	const Move* begin() const noexcept;
	const Move* end() const noexcept;

	const Move& operator [](int index) const noexcept;

	void sort() noexcept;

};