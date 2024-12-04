#pragma once

#include "Position.h"
#include "Evaluate.h"
#include "ThreadPool.h"
#include "Transposition.h"

const float MATE = 10000.0f;

class Engine
{
private:

	Position* position_ptr;
	TransPositionTable TPT;
	const int DEPTH;
	ThreadPool<float> workers;

	float alpha_beta_MAX(Evaluate& position, int depth, float alpha, float beta) noexcept;
	float alpha_beta_MIN(Evaluate& position, int depth, float alpha, float beta) noexcept;

public:

	inline Engine(Position& pos, int depth) : position_ptr(&pos), DEPTH(depth) {}
	inline ~Engine() { workers.join(); }
	void Search();


};
