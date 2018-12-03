#ifndef GAME_H
#define GAME_H

#include <QListWidget>
#include <thread>
#include <chrono>
#include "board.h"
#include "ai.h"
#include "observer.h"


/**
 * @brief The Game class manages the whole game procedure, gives callbacks to form to update the GUI
 */
class Game
{

public:
    Game(Observer* iForm, bool p1_is_ai, bool p2_is_ai, int p1_depth, int p2_depth, int p_start);
    ~Game();

    bool game_over;

    void start();
    int get_current_player();
    void human_move(int pos);

private:
    Board _board;
    Ai* _ai_1;
    Ai* _ai_2;
    Observer* _iForm;

    bool _p1_is_ai;
    bool _p2_is_ai;
    int _p1_depth;
    int _p2_depth;
    unsigned _p1_time;
    unsigned _p2_time;
    int _p_start;
    int _current_player;

    void ai_move();
    void final_time();
};

#endif // GAME_H
