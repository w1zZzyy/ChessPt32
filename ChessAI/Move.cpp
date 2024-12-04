#include "Move.h"
#include <algorithm>
#include "Evaluate.h"

Move::Move(int from, int to, int piece_moved)
{
    move_value = from | to << 6;
    pieces = piece_moved << 3;
}

Move::Move(int from, int to, MOVE_TYPE type)
{
    move_value = from | to << 6 | type << 12;
}

Move::Move(int from, int to, MOVE_TYPE type, int piece_moved) : Move(from, to, type)
{
    pieces = piece_moved << 3;
}

Move::Move(int from, int to, MOVE_TYPE type, int piece_moved, int piece_captured) : Move(from, to, type)
{
    pieces = piece_moved << 3 | piece_captured;
}

void Move::setProm(int piece_promoted) noexcept
{
    pieces &= 0b000111111;
    pieces |= piece_promoted << 6;
}

void Move::setType(MOVE_TYPE type) noexcept
{
    move_value |= type << 12;
}
void Move::setCapture(int piece_captured) noexcept
{
    pieces |= piece_captured;
}

void Move::setCastle(int castle) noexcept
{
    pieces |= castle << 9;
}

short Move::getFrom() const noexcept
{
    return move_value & 0b111111;
}

short Move::getTo() const noexcept
{
    return (move_value & 0b111111000000) >> 6;
}

MOVE_TYPE Move::getType() const noexcept
{
    short type = (move_value & 0b111111000000000000) >> 12;
    return static_cast<MOVE_TYPE>(type);
}

PieceType Move::getPieceMoved() const noexcept
{
    return static_cast<PieceType>((pieces & 0b111000) >> 3);
}

PieceType Move::getPieceCaptured() const noexcept
{
    return static_cast<PieceType>(pieces & 0b111);
}

PieceType Move::getPiecePromotion() const noexcept
{
    return static_cast<PieceType>((pieces & 0b111000000) >> 6);
}

short Move::getCastle() const noexcept
{
    return (pieces & 0b1111000000000) >> 9;
}






MoveList& MoveList::operator=(MoveList&& moves) noexcept
{
    data = std::move(moves.data);
    size = moves.size;
    return *this;
}

MoveList::MoveList()
{
    data = std::make_unique<Move[]>(MOVES_CAPACITY);
}

MoveList::MoveList(MoveList&& moves) noexcept
{
    data = std::move(moves.data);
    size = moves.size;
}

void MoveList::push_back(const Move& move) noexcept
{
    data[size] = move;
    size++;
}

void MoveList::emplace_back(Move&& move) noexcept
{
    data[size] = move;
    size++;
}

int MoveList::getSize() const noexcept
{
    return size;
}

bool MoveList::empty() const noexcept
{
    return size == 0;
}

const Move* MoveList::begin() const noexcept
{
    return &data[0];
}

const Move* MoveList::end() const noexcept
{
    return &data[size];
}

const Move& MoveList::operator[](int index) const noexcept
{
    return data[index];
}

void MoveList::sort() noexcept
{
    if (size <= 1)
        return;
    auto iter = data.get();
    std::sort(iter, iter + size,
        [](const Move& m1, const Move& m2) 
        {
            MOVE_TYPE mt1 = m1.getType(), mt2 = m2.getType();
            bool capture1 = mt1 == MOVE_TYPE::CAPTURE, capture2 = mt2 == MOVE_TYPE::CAPTURE;
            if (capture1 && capture2)
                return PIECE_VALUES[m1.getPieceCaptured()] > PIECE_VALUES[m2.getPieceCaptured()];
            return capture1 && !capture2;
        }
    );
}

//void MoveList::memory() noexcept
//{
//    Move* _data = new Move[size];
//    std::memcpy(_data, data, size * sizeof(Move));
//    delete[] data;
//    data = _data;
//}
