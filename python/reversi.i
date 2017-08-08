%module(directors="1") reversi

%include <std_string.i>
%include <std_vector.i>
%include <std_unordered_set.i>
%include <cpointer.i>
%include <carrays.i>

%feature("director") game;

%{
#include "../type.h"
#include "../asm.h"
#include "../reversi.h"
#include "../game.h"
#include "../matrix.h"
#include "../pattern.h"
#include "../tree.h"
%}

%include "../type.h"
%include "../reversi.h"
%include "../matrix.h"
%include "../pattern.h"
%include "../game.h"
%include "../tree.h"

%template(shorts) std::vector<short>;
%template(ints) std::vector<int>;
%template(floats) std::vector<float>;
%template(boards) std::vector<board>;
%template(choices) std::vector<choice>;
%template(patterns) std::vector<pattern>;
%rename(samples) std::unordered_set<board>;
%template(mat_i) matrix<int>;
%template(mat_f) matrix<float>;
%template(mat_brd) matrix<board>;

%array_class(pos_type, pos_array);

%rename(__eq__) board::operator==;
%ignore board::operator<<(ostream& out,const board& brd);