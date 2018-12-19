#include "ai.h"

/**
 * @brief Ai::Ai    : Constructor initializes all member variables
 * @param depth     : defines the depth for this ai
 * @param player    : defines the player for this ai
 */
Ai::Ai(int depth, int player): m_depth(depth), m_player(player), m_winScore(5000), m_looseScore(-5000), m_move(-1)
{}


Ai::~Ai()
{}

/**
 * @brief Ai::get_move  : only public method of this class, used to get a move as pair<move, score>
 * @param board         : current board, used to define next step
 * @return
 */
std::pair<int, int> Ai::get_move(const Board &board){
    int best_score = max_value(board, m_depth, -10000, 10000);
    return std::make_pair(m_move, best_score);
}

/**
 * @brief Ai::max_value : max function of minimax algorithm, returns max value
 * @param board         : current board (might be temporary from min function)
 * @param depth_to_go   : current depth, shrinks per iteration
 * @param alpha         : alpha value for alpha-beta-pruning
 * @param beta          : beta value for alpha-beta-pruning
 * @return
 */
int Ai::max_value(Board board, int depth_to_go, int alpha, int beta){
    if(depth_to_go == 0 || board.is_game_over(3 - m_player)){
        return board.eval(m_player, m_winScore, m_looseScore, depth_to_go);
    }
    else{
        std::vector<int> drops = board.possible_drops();
        int score = -10000;
        for(auto& col: drops){
            m_tmp_board = Board(board.get_positions());
            m_tmp_board.drop(col, m_player);
            int s = min_value(m_tmp_board, depth_to_go - 1, alpha, beta);
            if(s > score){
                score = s;
                if(depth_to_go == m_depth){m_move = col;}
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

/**
 * @brief Ai::max_value : min function of minimax algorithm, returns min value
 * @param board         : current board (might be temporary from max function)
 * @param depth_to_go   : current depth, shrinks per iteration
 * @param alpha         : alpha value for alpha-beta-pruning
 * @param beta          : beta value for alpha-beta-pruning
 * @return
 */
int Ai::min_value(Board board, int depth_to_go, int alpha, int beta){
    if(depth_to_go == 0 || board.is_game_over(m_player)){
        return m_tmp_board.eval(m_player, m_winScore, m_looseScore, depth_to_go);
    }
    else{
        std::vector<int> drops = board.possible_drops();
        int score = 10000;
        for(const auto& col: drops){
            m_tmp_board = Board(board.get_positions());
            m_tmp_board.drop(col, 3 - m_player);
            int s = max_value(m_tmp_board, depth_to_go - 1, alpha, beta);
            if(s < score){
                score = s;
                if(depth_to_go == m_depth){m_move = col;}
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
