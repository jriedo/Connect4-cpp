#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QWidget>
#include <QScrollArea>
#include <vector>
#include <iostream>
#include <algorithm>

#include "observer.h"

/**
 * @brief The Board class represents the board and provides evaluation functions on it
 */
class Board
{
    using boardarray = std::array<std::array<int, 6>, 7>;

public:

    Board();
    Board(boardarray);
    ~Board();

    void drop(int col, int player);
    std::vector<int> possible_drops();
    void reset();

    bool is_game_over(int player);
    bool is_winner(int player);

    bool is_full();
    int eval(int player, int win, int loose, int depth);
    void celebration(int player);

    boardarray get_positions();
    std::pair<std::pair<int, int>, std::pair<int, int>> get_winning_line(int player);

private:
    boardarray m_positions;


};

#endif // BOARD_H
