#include "board.h"
//type boardarray represents positions of board
using boardarray = std::array<std::array<int, 6>, 7>;

/**
 * @brief Board::Board Constructor used for the one "real" board. called by Game
 */
Board::Board(){
    reset();
}

/**
 * @brief Board::Board Constructor used for temporary boards created by ai (no graphics)
 * @param positions current state of game, copied to new board
 */
Board::Board(boardarray positions)
{
    _positions=positions;
}

Board::~Board(){}

/**
 * @brief Board::drop   : Execute drop on boardarray positions
 * @param col           : defines move
 * @param player        : represents player who plays the move
 */
void Board::drop(int col, int player){
        std::size_t row = std::distance(_positions[col].begin(), std::find_if(_positions[col].begin(), _positions[col].end(), [](int val) { return val != 0; }))-1;
        _positions[col][row] = player;
}

/**
 * @brief Board::is_game_over   : checks if player won or board full
 * @param player                : player for which the win criteria is checked
 * @return                      : true if game is over
 */
bool Board::is_game_over(int player){
    return (possible_drops().size()==0 || is_winner(player));
}

/**
 * @brief Board::is_full    : checks if game is over
 * @return
 */
bool Board::is_full(){
    return (possible_drops().size()==0)? true : false;
}

/**
 * @brief Board::is_winner  : checks if player won
 * @param player            : player for which the win criteria is checked
 * @return
 */
bool Board::is_winner(int player){
//    vertical
    for(auto col : _positions){
        if(std::search_n(col.begin(), col.end(), 4, player) != col.end()){return true;};//could be faster with hints somehow
    }
//    horizontal
    for(int j = 0; j < 6; ++j){
        for(int i = 0; i < 4; ++i){
            if(_positions[i][j] == player && _positions[i+1][j] == player && _positions[i+2][j] == player && _positions[i+3][j] == player){
                return true;
            }
        }
    }
//    diagonal down
        for(int j = 0; j < 3; ++j){
            for(int i = 0; i < 4; ++i){
                if(_positions[i][j] == player && _positions[i+1][j+1] == player && _positions[i+2][j+2] == player && _positions[i+3][j+3] == player){
                    return true;
                }
            }
        }
//    diagonal up
        for(int j = 3; j < 6; ++j){
            for(int i = 0; i < 4; ++i){
                if(_positions[i][j] == player && _positions[i+1][j-1] == player && _positions[i+2][j-2] == player && _positions[i+3][j-3] == player){
                    return true;
                }
            }
        }
    return false;
}

/**
 * @brief Board::eval   : Evaluates positions of player, gives back nummeric value of how good the positions are
 * @param player        : Player for which the evaluation is carried out
 * @param win           : Value of a win situation
 * @param loose         : Value of a loose situation
 * @param depth         : Depth of current board evaluation
 * @return
 */
int Board::eval(int player, int win, int loose, int depth){
    if(is_winner(player)){//return +depth so faster wins are better
        return win+depth;
    }
    else if(is_winner(3-player)){
        return loose;
    }
    else{//was unable to put the variable somewhere meaningful (i.e. init in i, allocate in constructors
        const boardarray _weights = { 3, 4, 5, 5, 4, 3,
                                      4, 6, 8, 8, 6, 4,
                                      5, 8, 11, 11, 8, 5,
                                      7, 10, 13, 13, 10, 7,
                                      5, 8, 11, 11, 8, 5,
                                      4, 6, 8, 8, 6, 4,
                                      3, 4, 5, 5, 4, 3};
        int sum = 0;
        for(int col = 0; col<7; ++col){
            for(int row = 0; row<6; ++row){
                if(_positions[col][row] == player){
                    sum += _weights[col][row];
                }
            }
        }
        return sum;
    }
}

/**
 * @brief Board::possible_drops : returns all possible positions to drop in a vector
 * @return
 */
std::vector<int> Board::possible_drops(){
    std::vector<int> drops;
    for(int i=0; i<7; ++i){
        if(_positions[i][0]==0){
            drops.push_back(i);
        }
    }
    return drops;
}

/**
 * @brief Board::reset  :
 */
void Board::reset(){
    std::array<int, 6> dummy;
    dummy.fill(0);
    _positions.fill(dummy);
}

/**
 * @brief Board::get_positions  : return current positions
 * @return
 */
boardarray Board::get_positions(){
    return _positions;
}

/**
 * @brief Board::get_winning_line   : return pair of start and endpoint of the 4 connected winner coins
 * @param player                    : winning player
 * @return
 */
std::pair<std::pair<int, int>, std::pair<int, int> > Board::get_winning_line(int player)
{
    //returns pair <start, end> with start = <x,y> and end = <x,y>
    //    vertical
        for(int i = 0; i < 7; ++i){
            for(int j = 0; j < 3; ++j){
                if(_positions[i][j] == player && _positions[i][j+1] == player && _positions[i][j+2] == player && _positions[i][j+3] == player){
                    return std::make_pair(std::make_pair(i, j), std::make_pair(i, j+3));
                }
            }
        }
    //    horizontal
        for(int j = 0; j < 6; ++j){
            for(int i = 0; i < 4; ++i){
                if(_positions[i][j] == player && _positions[i+1][j] == player && _positions[i+2][j] == player && _positions[i+3][j] == player){
                    return std::make_pair(std::make_pair(i, j), std::make_pair(i+3, j));
                }
            }
        }
    //    diagonal down
            for(int j = 0; j < 3; ++j){
                for(int i = 0; i < 4; ++i){
                    if(_positions[i][j] == player && _positions[i+1][j+1] == player && _positions[i+2][j+2] == player && _positions[i+3][j+3] == player){
                        return std::make_pair(std::make_pair(i, j), std::make_pair(i+3, j+3));
                    }
                }
            }
    //    diagonal up
            for(int j = 3; j < 6; ++j){
                for(int i = 0; i < 4; ++i){
                    if(_positions[i][j] == player && _positions[i+1][j-1] == player && _positions[i+2][j-2] == player && _positions[i+3][j-3] == player){
                        return std::make_pair(std::make_pair(i, j), std::make_pair(i+3, j-3));
                    }
                }
            }
}

