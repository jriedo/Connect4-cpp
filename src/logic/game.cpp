#include "game.h"

/**
 * @brief Game::Game: Constructor for the Game class, setting up a game environment
 * @param iForm     : observer for callbaks on form
 * @param p1_is_ai  : defines if player 1 is an ai
 * @param p2_is_ai  : defines if player 2 is an ai
 * @param p1_depth  : defines depth of player 1 (if it is an ai)
 * @param p2_depth  : defines depth of player 2 (if it is an ai)
 * @param p_start   : defines which player to start
 */
Game::Game(Observer* iForm, bool p1_is_ai, bool p2_is_ai, int p1_depth, int p2_depth, int p_start):
    m_iForm(iForm)
{
    m_p1_is_ai = p1_is_ai;
    m_p2_is_ai = p2_is_ai;
    m_p1_depth = p1_depth;
    m_p2_depth = p2_depth;
    m_p1_time = 0;
    m_p2_time = 0;
    m_p_start = p_start;
    m_current_player = p_start;
    game_over = false;

    //generate ais if necessary
    if(m_p1_is_ai){
        m_ai_1.reset(new Ai(m_p1_depth, 1));
    }
    if(m_p2_is_ai){
        m_ai_2.reset(new Ai(m_p2_depth, 2));
    }
}

/**
 * @brief Game::~Game   : empty destructor
 */
Game::~Game()
{}

/**
 * @brief Game::start: Starts the game: calls ai if ai starts, otherwise do nothing (wait for user input)
 */
void Game::start(){
    if((m_p_start == 1 && m_p1_is_ai) || (m_p_start==2 && m_p2_is_ai))
    {
        ai_move();
    }
}

/**
 * @brief Game::ai_move: Get a move from the ai, execute it, evaluate board, proceed with ai or human
 */
void Game::ai_move(){
    // Wait for human move to finish
    std::mutex huMu;
    std::unique_lock<std::mutex> aiLock(huMu);
    m_aiPlayer.wait(aiLock,[this]{return (m_current_player == 1 && m_p1_is_ai) || (m_current_player == 2 && m_p2_is_ai);});

    //get the move, measure execution time
    std::pair<int, int> aipair;
    int t_delta;
    if(m_current_player == 1){
        auto t_start = std::chrono::high_resolution_clock::now();
        aipair = m_ai_1->get_move(m_board);
        auto t_end = std::chrono::high_resolution_clock::now();
        t_delta = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_start).count();

        //add time to total execution time of concerning player
        m_p1_time += t_delta;
    }
    else{
        auto t_start = std::chrono::high_resolution_clock::now();
        aipair = m_ai_2->get_move(m_board);
        auto t_end = std::chrono::high_resolution_clock::now();
        t_delta = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_start).count();

        //add time to total execution time of concerning player
        m_p2_time += t_delta;
    }

    //execute move and callback to form
    m_board.drop(aipair.first, m_current_player);
    m_iForm->updatePositions(m_board.get_positions());

    //write move and time to output list
    m_iForm->writeToLog("player " + QString::number(m_current_player) + ": " + QString::number(aipair.first));
    m_iForm->writeToLog("score: " + QString::number(aipair.second));
    if(t_delta >= 1000){
        t_delta /= 1000;
        m_iForm->writeToLog("time: " + QString::number(t_delta) + " s");
    }
    else{
        m_iForm->writeToLog("time: " + QString::number(t_delta) + " ms");
    }

    m_iForm->writeToLog("------------------");

    //eval board, if player won or game finish callback on form and write to output list
    if(m_board.is_winner(m_current_player)){
        m_iForm->writeToLog("player " + QString::number(m_current_player) + " wins");
        m_iForm->writeToLog("------------------");
        final_time();
        game_over = true;
        m_iForm->gameOver(m_current_player);
        m_iForm->setWinningLine(m_board.get_winning_line(m_current_player));
    }
    else if (m_board.is_full()) {
        final_time();
        game_over = true;
        m_iForm->gameOver(0);
    }
    else{//proceed in game with next player
          m_current_player = 3 - m_current_player;
         if((m_current_player == 1 && m_p1_is_ai) || (m_current_player == 2 && m_p2_is_ai)){
             std::thread t(&Game::ai_move,this);
             t.detach();
         }
         else{//human move
             m_iForm->updatePossibleDrops(m_board.possible_drops());
         }
    }
    // ai move done
    m_aiPlayer.notify_one();
}

/**
 * @brief Game::human_move: Execute a human move, evaluate board, proceed with ai or human
 * @param pos: defines the next move
 */
void Game::human_move(int pos){
    // Wait for ai_move to finish
    std::mutex aiMu;
    std::unique_lock<std::mutex> aiLock(aiMu);
    m_aiPlayer.wait(aiLock,[this]{return !((m_current_player == 1 && m_p1_is_ai) || (m_current_player == 2 && m_p2_is_ai));});

    //execute move, callback on form
    m_board.drop(pos, m_current_player);
    m_iForm->updatePositions(m_board.get_positions());

    //write move to output list
    m_iForm->writeToLog("P" + QString::number(m_current_player) + " : " + QString::number(pos));
    m_iForm->writeToLog("------------------");

    //evaluate board, if player won or game finish callback on form and write to output list
    if(m_board.is_winner(m_current_player)){
        m_iForm->writeToLog("player " + QString::number(m_current_player) + " wins");
        m_iForm->writeToLog("------------------");
        final_time();
        game_over = true;
        m_iForm->gameOver(m_current_player);
        m_iForm->setWinningLine(m_board.get_winning_line(m_current_player));
    }
    else if (m_board.is_full()) {
        game_over = true;
        m_iForm->gameOver(m_current_player);
    }
    else{//proceed in game with next player
          m_current_player = 3 - m_current_player;
         if((m_current_player == 1 && m_p1_is_ai) || (m_current_player == 2 && m_p2_is_ai)){
              std::thread t(&Game::ai_move,this);
              t.detach();
         }
         else{//human move
             m_iForm->updatePossibleDrops(m_board.possible_drops());
         }
    }
    // human move done
    m_aiPlayer.notify_one();
}

/**
 * @brief Game::get_current_player: get number of current player
 */
int Game::get_current_player(){
    return m_current_player;
}

/**
 * @brief Game::final_time: callback on Form to write total computation time to output list
 */
void Game::final_time(){
    if(m_p1_is_ai){
        m_iForm->writeToLog("player 1 \ntotal time:");
        if(m_p1_time >= 1000){
            m_p1_time /= 1000;
            m_iForm->writeToLog(QString::number(m_p1_time) + " s\n");
        }
        else {
            m_iForm->writeToLog(QString::number(m_p1_time) + " ms\n");
        }
    }
    if (m_p2_is_ai) {
        m_iForm->writeToLog("player 2  \ntotal time:");
        if(m_p2_time >= 1000){
            m_p2_time /= 1000;
            m_iForm->writeToLog(QString::number(m_p2_time) + " s\n");
        }
        else {
            m_iForm->writeToLog(QString::number(m_p2_time) + " ms\n");
        }
    }
}



