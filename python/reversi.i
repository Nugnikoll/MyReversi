%module(directors="1") reversi

%include <std_string.i>
%include <std_vector.i>
%include <std_pair.i>
%include <std_unordered_set.i>
%include <cpointer.i>
%include <carrays.i>

%feature("director") game;

%{
#include "../cpp/type.h"
#include "../cpp/asm.h"
#include "../cpp/reversi.h"
#include "../cpp/game.h"
#include "../cpp/matrix.h"
#include "../cpp/pattern.h"
#include "../cpp/tree.h"
%}

%include "../cpp/type.h"
%include "../cpp/reversi.h"
%include "../cpp/matrix.h"
%include "../cpp/pattern.h"
%include "../cpp/game.h"
%include "../cpp/tree.h"

%template(shorts) std::vector<short>;
%template(ints) std::vector<int>;
%template(floats) std::vector<float>;
%template(boards) std::vector<board>;
%template(choices) std::vector<choice>;
%template(patterns) std::vector<pattern>;
//%template(samples) std::unordered_set<board>;
%template(pair_method) std::pair<method,short>;
%template(mat_i) matrix<int>;
%template(mat_f) matrix<float>;
%template(mat_brd) matrix<board>;

%array_class(pos_type, pos_array);

%rename(__eq__) board::operator==;
%rename(__neq__) board::operator!=;
%ignore board::operator<<(ostream& out,const board& brd);
