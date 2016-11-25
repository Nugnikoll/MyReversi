%module reversi

%include <std_string.i>
%include <std_vector.i>
%include <cpointer.i>

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

%template(choices) std::vector<choice>;
%template(patterns) std::vector<pattern>;

%rename(__eq__) board::operator==;
%rename(__gt__) brd_val::operator>;
%rename(__lt__) brd_val::operator<;
%ignore board::operator<<(ostream& out,const board& brd);