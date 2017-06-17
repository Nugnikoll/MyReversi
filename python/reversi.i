%module reversi

%include <std_string.i>
%include <std_vector.i>
%include <cpointer.i>
%include <carrays.i>

%{
#include "../type.h"
#include "../reversi.h"
#include "../game.h"
#include "../pattern.h"
#include "../tree.h"
%}

%include "../type.h"
%include "../reversi.h"
%include "../pattern.h"
%include "../game.h"
%include "../tree.h"

%pointer_functions(pattern,ptn)

%template(shorts) std::vector<short>;
%template(ints) std::vector<int>;
%template(floats) std::vector<float>;
%template(boards) std::vector<board>;
%template(choices) std::vector<choice>;
%template(patterns) std::vector<pattern>;

%array_class(pos_type, pos_array);

%rename(__eq__) board::operator==;
%rename(__gt__) brd_val::operator>;
%rename(__lt__) brd_val::operator<;
%ignore board::operator<<(ostream& out,const board& brd);