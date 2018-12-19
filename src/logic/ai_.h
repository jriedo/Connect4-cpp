#ifndef AI_H
#define AI_H

#include "board.h"

/**
 * @brief The Ai class represents an ai for a certain player, with defined depth
 */
class Ai
{
public:
    Ai(int depth, int player);
    ~Ai();
    std::pair<int, int> get_move(const Board &board);

private:
    int m_depth;
    int m_player;
    int m_winScore;
    int m_looseScore;
    int m_move;

    Board m_tmp_board;


    int max_value(Board board, int depth_to_go, int alpha, int beta);
    int min_value(Board board, int depth_to_go, int alpha, int beta);
};

#endif // AI_H
