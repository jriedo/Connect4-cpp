#ifndef GAME_H
#define GAME_H

#include <thread>
#include <chrono>
#include <future>

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
    Board m_board;


    std::unique_ptr<Ai> m_ai_1;
    std::unique_ptr<Ai> m_ai_2;

    Observer* m_iForm;

    bool m_p1_is_ai;
    bool m_p2_is_ai;
    int m_p1_depth;
    int m_p2_depth;
    unsigned m_p1_time;
    unsigned m_p2_time;
    int m_p_start;
    int m_current_player;

    void ai_move();
    void final_time();

    std::condition_variable m_aiPlayer;
};

#endif // GAME_H
