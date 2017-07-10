#ifndef SEARCH_H
#define SEARCH_H

typedef unordered_map<board,board::interval> trans_type;
extern calc_type table_val[board::size2][board::size2];
extern const calc_type table_val_init[board::size2];
extern trans_type table_trans[2];

#endif //SEARCH_H