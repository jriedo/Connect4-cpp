#ifndef AI_H
#define AI_H

#include <thread>
#include <future>
#include <mutex>
#include "board.h"

class Ai
{
public:
    Ai(int depth, int player);
    std::pair<int, int> get_move(const Board &board);

private:
    int m_depth;
    int m_player;
    int m_winScore;
    int m_looseScore;
    int m_move;
    std::array<int, 7> m_score;
    std::mutex mu;

    void startFirstMove(int col, Board board, int depth_to_go);
    int max_value(Board board, int depth_to_go, int alpha, int beta);
    int min_value(Board board, int depth_to_go, int alpha, int beta);
};

#endif // AI_H
