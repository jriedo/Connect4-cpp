#include "ai.h"


Ai::Ai(int depth, int player):
    m_depth(depth),
    m_player(player),
    m_winScore(5000),
    m_looseScore(-5000),
    m_move(-1)
{
}

/**
 * @brief Ai::get_move  : only public method of this class, used to get a move as pair<move, score>
 * @param board         : current board, used to define next step
 * @return
 */
std::pair<int, int> Ai::get_move(const Board &board){

    int best_score = max_value(board, m_depth, -10000, 10000);
    auto result = std::max_element(m_score.begin(), m_score.end());

    return std::make_pair(std::distance(m_score.begin(), result), *result);
}

/**
 * @brief Ai::startFirstMove    : used as starting point for the threads
 * @param col                   : position to drop
 * @param board                 : current board
 * @param depth_to_go           : depth of minimax
 */
void Ai::startFirstMove(int col, Board board, int depth_to_go){
    Board m_tmp_board = Board(board.get_positions());
    m_tmp_board.drop(col, m_player);

    int s = min_value(m_tmp_board, depth_to_go - 1, -10000, 10000);

    if(s > m_score[col]){
        std::lock_guard<std::mutex> guard(mu);
        m_score[col] = s;
    }
}

/**
 * @brief Ai::max_value : max function of minimax algorithm, returns max value, starts drops.size threads
 * @param board         : current board (might be temporary from min function)
 * @param depth_to_go   : current depth, shrinks per iteration
 * @param alpha         : alpha value for alpha-beta-pruning
 * @param beta          : beta value for alpha-beta-pruning
 * @return              : max value from eval for this depth
 */
int Ai::max_value(Board board, int depth_to_go, int alpha, int beta){
    if(depth_to_go == 0 || board.is_game_over(3 - m_player)){
        return board.eval(m_player, m_winScore, m_looseScore, depth_to_go);
    }
    else{
        if(depth_to_go == m_depth){//only in first iteration i.e. the very next move
            std::vector<int> drops = board.possible_drops();
            m_score.fill(-10000);

            std::vector<std::thread> t(drops.size());

            for (int i = 0; i < drops.size(); ++i) {//make one thread per possible drop
                int col = drops[i];
                t[i] = std::thread(&Ai::startFirstMove, this, col, board, depth_to_go);
            }

            for (auto &thread: t) {
                if(thread.joinable()){
                    thread.join();}
            }

            return 5;
        }

        else{//all other moves
            std::vector<int> drops = board.possible_drops();
            int score = -10000;

            for(auto& col: drops){
                Board m_tmp_board = Board(board.get_positions());
                m_tmp_board.drop(col, m_player);
                int s = min_value(m_tmp_board, depth_to_go - 1, alpha, beta);
                if(s > score){
                    score = s;
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
}

/**
 * @brief Ai::min_value : min function of minimax algorithm, returns min value
 * @param board         : current board (might be temporary from max function)
 * @param depth_to_go   : current depth, shrinks per iteration
 * @param alpha         : alpha value for alpha-beta-pruning
 * @param beta          : beta value for alpha-beta-pruning
 * @return              : min value from eval for this depth
 */
int Ai::min_value(Board board, int depth_to_go, int alpha, int beta){
    if(depth_to_go == 0 || board.is_game_over(m_player)){
        return board.eval(m_player, m_winScore, m_looseScore, depth_to_go);
    }
    else{
        std::vector<int> drops = board.possible_drops();
        int score = 10000;
        for(const auto& col: drops){
            Board m_tmp_board = Board(board.get_positions());
            m_tmp_board.drop(col, 3 - m_player);
            int s = max_value(m_tmp_board, depth_to_go - 1, alpha, beta);
            if(s < score){
                score = s;
//                if(depth_to_go == _depth){_move = col;}
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
