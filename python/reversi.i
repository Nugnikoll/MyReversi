// -*- c++ -*-

%module(directors="1") reversi

//%feature("autodoc", "3");

%include <std_string.i>
%include <std_vector.i>
%include <std_pair.i>
%include <cpointer.i>
%include <carrays.i>

%feature("director") game;

%{
	#define SWIG_FILE_WITH_INIT
	#include "../cpp/type.h"
	#include "../cpp/asm.h"
	#include "../cpp/board.h"
	#include "../cpp/pattern.h"
	#include "../cpp/log.h"
	#include "../cpp/tree.h"
%}

%include "numpy.i"

%init %{
	import_array();
%}

#define ARRAY_1D(TYPE) \
	%apply (TYPE* INPLACE_ARRAY1, int DIM1) \
	{(TYPE* ptri, int i1)}; \
	%apply (TYPE* INPLACE_ARRAY1, int DIM1) \
	{(TYPE* ptrj, int j1)}; \
	%apply (TYPE** ARGOUTVIEW_ARRAY1, int* DIM1) \
	{(TYPE** ptro, int* o1)}; \
	%apply (TYPE** ARGOUTVIEWM_ARRAY1, int* DIM1) \
	{(TYPE** ptrm, int* m1)};

#define ARRAY_2D(TYPE) \
	%apply (TYPE* INPLACE_ARRAY2, int DIM1, int DIM2) \
	{(TYPE* ptri, int i1, int i2)}; \
	%apply (TYPE* INPLACE_ARRAY2, int DIM1, int DIM2) \
	{(TYPE* ptrj, int j1, int j2)}; \
	%apply (TYPE** ARGOUTVIEW_ARRAY2, int* DIM1, int* DIM2) \
	{(TYPE** ptro, int* o1, int* o2)}; \
	%apply (TYPE** ARGOUTVIEWM_ARRAY2, int* DIM1, int* DIM2) \
	{(TYPE** ptrm, int* m1, int* m2)};

#define ARRAY_3D(TYPE) \
	%apply (TYPE* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) \
	{(TYPE* ptri, int i1, int i2, int i3)}; \
	%apply (TYPE* INPLACE_ARRAY3, int DIM1, int DIM2, int DIM3) \
	{(TYPE* ptrj, int j1, int j2, int j3)}; \
	%apply (TYPE** ARGOUTVIEW_ARRAY3, int* DIM1, int* DIM2, int* DIM3) \
	{(TYPE** ptro, int* o1, int* o2, int* o3)}; \
	%apply (TYPE** ARGOUTVIEWM_ARRAY3, int* DIM1, int* DIM2, int* DIM3) \
	{(TYPE** ptrm, int* m1, int* m2, int* m3)}; \

#define ARRAY_ND(TYPE) \
	ARRAY_1D(TYPE) \
	ARRAY_2D(TYPE) \
	ARRAY_3D(TYPE)

ARRAY_ND(bool);
ARRAY_ND(int);
ARRAY_ND(unsigned long long);
ARRAY_ND(float);
ARRAY_ND(double);

%include "../cpp/type.h"
%include "../cpp/board.h"
%include "../cpp/pattern.h"
%include "../cpp/log.h"
%include "../cpp/tree.h"

%template(shorts) std::vector<short>;
%template(ints) std::vector<int>;
%template(floats) std::vector<float>;
%template(boards) std::vector<board>;
%template(choices) std::vector<choice>;
%template(patterns) std::vector<pattern>;
%template(pair_method) std::pair<method, short>;

%array_class(pos_type, pos_array);

%rename(__eq__) board::operator==;
%rename(__neq__) board::operator!=;
%ignore board::operator<<(ostream& out,const board& brd);
