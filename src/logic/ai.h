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
    std::pair<int, int> get_move(const Board &board);

private:
    int _depth;
    int _player;
    int _winScore;
    int _looseScore;
    int _move;

    Board _tmp_board;


    int max_value(Board board, int depth_to_go, int alpha, int beta);
    int min_value(Board board, int depth_to_go, int alpha, int beta);
};

#endif // AI_H
