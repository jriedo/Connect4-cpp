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
    _iForm(iForm)
{
    _p1_is_ai = p1_is_ai;
    _p2_is_ai = p2_is_ai;
    _p1_depth = p1_depth;
    _p2_depth = p2_depth;
    _p1_time = 0;
    _p2_time = 0;
    _p_start = p_start;
    _current_player = p_start;
    game_over = false;

    //generate ais if necessary
    if(_p1_is_ai){
        _ai_1 = new Ai(_p1_depth, 1);
    }
    if(_p2_is_ai){
        _ai_2 = new Ai(_p2_depth, 2);
    }
}

Game::~Game(){
    delete _ai_1;
    delete _ai_2;
}

/**
 * @brief Game::start: Starts the game: calls ai if ai starts, otherwise do nothing (wait for user input)
 */
void Game::start(){
    if((_p_start == 1 && _p1_is_ai) || (_p_start==2 && _p2_is_ai))
    {
        ai_move();
    }
}

/**
 * @brief Game::ai_move: Get a move from the ai, execute it, evaluate board, proceed with ai or human
 */
void Game::ai_move(){

    //get the move, measure execution time
    std::pair<int, int> aipair;
    int t_delta;
    if(_current_player == 1){
        auto t_start = std::chrono::high_resolution_clock::now();
        aipair = _ai_1->get_move(_board);
        auto t_end = std::chrono::high_resolution_clock::now();
        t_delta = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_start).count();

        //add time to total execution time of concerning player
        _p1_time += t_delta;
    }
    else{
        auto t_start = std::chrono::high_resolution_clock::now();
        aipair = _ai_2->get_move(_board);
        auto t_end = std::chrono::high_resolution_clock::now();
        t_delta = std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_start).count();

        //add time to total execution time of concerning player
        _p2_time += t_delta;
    }

    //execute move and callback to form
    _board.drop(aipair.first, _current_player);
    _iForm->updatePositions(_board.get_positions());

    //write move and time to output list
    _iForm->writeToLog("player " + QString::number(_current_player) + ": " + QString::number(aipair.first));
    _iForm->writeToLog("score: " + QString::number(aipair.second));
    if(t_delta >= 1000){
        t_delta /= 1000;
        _iForm->writeToLog("time: " + QString::number(t_delta) + " s");
    }
    else{
        _iForm->writeToLog("time: " + QString::number(t_delta) + " ms");
    }

    _iForm->writeToLog("------------------");

    //eval board, if player won or game finish callback on form and write to output list
    if(_board.is_winner(_current_player)){
        _iForm->writeToLog("player " + QString::number(_current_player) + " wins");
        _iForm->writeToLog("------------------");
        final_time();
        game_over = true;
        _iForm->gameOver(_current_player);
        _iForm->setWinningLine(_board.get_winning_line(_current_player));
    }
    else if (_board.is_full()) {
        final_time();
        game_over = true;
        _iForm->gameOver(0);
    }
    else{//proceed in game with next player
          _current_player = 3 - _current_player;
         if((_current_player == 1 && _p1_is_ai) || (_current_player == 2 && _p2_is_ai)){
//             std::thread t(&Game::ai_move,this);
//             t.detach();
             ai_move();
         }
         else{//human move
             _iForm->updatePossibleDrops(_board.possible_drops());
         }
    }
}

/**
 * @brief Game::human_move: Execute a human move, evaluate board, proceed with ai or human
 * @param pos: defines the next move
 */
void Game::human_move(int pos){
    //execute move, callback on form
    _board.drop(pos, _current_player);
    _iForm->updatePositions(_board.get_positions());

    //write move to output list
    _iForm->writeToLog("P" + QString::number(_current_player) + " : " + QString::number(pos));
    _iForm->writeToLog("------------------");

    //evaluate board, if player won or game finish callback on form and write to output list
    if(_board.is_winner(_current_player)){
        _iForm->writeToLog("player " + QString::number(_current_player) + " wins");
        _iForm->writeToLog("------------------");
        final_time();
        game_over = true;
        _iForm->gameOver(_current_player);
        _iForm->setWinningLine(_board.get_winning_line(_current_player));
    }
    else if (_board.is_full()) {
        game_over = true;
        _iForm->gameOver(_current_player);
    }
    else{//proceed in game with next player
          _current_player = 3 - _current_player;
         if((_current_player == 1 && _p1_is_ai) || (_current_player == 2 && _p2_is_ai)){
//              std::thread t(&Game::ai_move,this);
//              t.detach();
//              if(t.joinable()){
//                  t.join();
//              }
              ai_move();
         }
         else{//human move
             _iForm->updatePossibleDrops(_board.possible_drops());
         }
    }
}

/**
 * @brief Game::get_current_player: get number of current player
 */
int Game::get_current_player(){
    return _current_player;
}

/**
 * @brief Game::final_time: callback on Form to write total computation time to output list
 */
void Game::final_time(){
    if(_p1_is_ai){
        _iForm->writeToLog("player 1 \ntotal time:");
        if(_p1_time >= 1000){
            _p1_time /= 1000;
            _iForm->writeToLog(QString::number(_p1_time) + " s\n");
        }
        else {
            _iForm->writeToLog(QString::number(_p1_time) + " ms\n");
        }
    }
    if (_p2_is_ai) {
        _iForm->writeToLog("player 2  \ntotal time:");
        if(_p2_time >= 1000){
            _p2_time /= 1000;
            _iForm->writeToLog(QString::number(_p2_time) + " s\n");
        }
        else {
            _iForm->writeToLog(QString::number(_p2_time) + " ms\n");
        }
    }
}



