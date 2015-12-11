#include "reversi.h"

board::calc_type board::table_count[board::enum_num];
board::calc_type board::table_eval[board::stage_num][board::pos_num][board::enum_num];
unordered_map<board::lline_type,board::line_type> board::table_valid;
unordered_map<board::exline_type,board::lline_type> board::table_flip;