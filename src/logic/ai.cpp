#include "ai.h"

Ai::Ai(int depth, int player): _depth(depth), _player(player), _winScore(5000), _looseScore(-5000)
{
    _move=-1;
}

std::pair<int, int> Ai::get_move(const Board &board){
    int best_score = max_value(board, _depth, -10000, 10000);
    return std::make_pair(_move, best_score);
}


int Ai::max_value(Board board, int depth_to_go, int alpha, int beta){
    if(depth_to_go == 0 || board.is_game_over(3 - _player)){
        return board.eval(_player, _winScore, _looseScore, depth_to_go);
    }
    else{
        std::vector<int> drops = board.possible_drops();
        int score = -10000;
        for(auto& col: drops){
            _tmp_board = Board(board.get_positions());
            _tmp_board.drop(col, _player);
            int s = min_value(_tmp_board, depth_to_go - 1, alpha, beta);
            if(s > score){
                score = s;
                if(depth_to_go == _depth){_move = col;}
            }
            if(alpha < s){
                alpha = s;
            }
            if(beta <= alpha){
                break;
            }
        }
        return score;
    }
}

int Ai::min_value(Board board, int depth_to_go, int alpha, int beta){
    if(depth_to_go == 0 || board.is_game_over(_player)){
        return _tmp_board.eval(_player, _winScore, _looseScore, depth_to_go);
    }
    else{
        std::vector<int> drops = board.possible_drops();
        int score = 10000;
        for(const auto& col: drops){
            _tmp_board = Board(board.get_positions());
            _tmp_board.drop(col, 3 - _player);
            int s = max_value(_tmp_board, depth_to_go - 1, alpha, beta);
            if(s < score){
                score = s;
                if(depth_to_go == _depth){_move = col;}
            }
            if(beta > s){
                beta = s;
            }
            if(alpha >= beta){
                break;
            }
        }
        return score;
    }
}
