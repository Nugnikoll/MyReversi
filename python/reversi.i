%module reversi

%include <std_string.i>
%include <std_vector.i>
%include <cpointer.i>

%{
#include "../game.h"
#include "../pattern.h"
%}

%pointer_functions(pattern,ptn)

%include "../reversi.h"
%include "../pattern.h"
%include "../game.h"

%template(MyStructVector) std::vector<choice>;
%template(MyStructVector) std::vector<pattern>;