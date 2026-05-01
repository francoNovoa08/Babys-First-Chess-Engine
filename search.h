#pragma once
#include "position.h"
#include "movegen.h"
#include "evaluate.h"
#include <unordered_map>
#include <memory>

struct SearchResult {
	Move bestMove = NULL_MOVE;
	int score = 0;
	int depth = 0;
	uint64_t nodes = 0;
};

// Transposition table entry
enum TTFlag : uint8_t { TT_EXACT, TT_LOWER, TT_UPPER };

struct TTEntry {
	uint64_t hash = 0;
	int score = 0;
	int depth = 0;
	Move move = NULL_MOVE;
	TTFlag flag = TT_EXACT;
};

class Search {
public:
	Search();

	SearchResult findBestMove(Position& pos, int depth);

	bool stop = false;

	void resetStop() { stop = false; };
	bool isStopped() const { return stop; };
	void resetTT() { memset(tt.get(), 0, sizeof(TTEntry) * TT_SIZE); };

	SearchResult searchRoot(Position& pos, int depth);

private:
	uint64_t nodes = 0;

	static constexpr size_t TT_SIZE = 1 << 20;
	std::unique_ptr<TTEntry[]> tt;

	uint64_t zobristPiece[2][6][64];
	uint64_t zobristSide;
	uint64_t zobristCastle[16];
	uint64_t zobristEP[8];

	void initZobrist();
	uint64_t computeHash(const Position& pos) const;

	void storeTT(uint64_t hash, int score, int depth,
		Move move, TTFlag flag);
	bool probeTT(uint64_t hash, int depth, int alpha,
		int beta, int& score, Move& move) const;

	int quiescence(Position& pos, int alpha, int beta, uint64_t hash);

	/**
	* @brief Score moves to search the best ones first
	* @param pos The position
	* @param m The move
	* @return The score
	**/
	int scoreMoveForOrdering(const Position& pos, Move m, Move ttmove) const;

	void orderMoves(const Position& pos, MoveList& list, Move ttmove);

	int negamax(Position& pos, int depth, int alpha, int beta, uint64_t hash);
};