#include "reversi_gui.h"
#include "pattern.h"

game_gui mygame;
group grp;

void game_gui::show(){
	wxClientDC dc(ptr_panel);
	mygame.do_show(dc);
}

void game_gui::do_show(wxDC& dc){

	//dc.DrawText(wxT("Testing"), 40, 60);

	dc.Clear();

	//draw valid moves
	dc.SetBrush(wxBrush(wxColor(23,95,0)));
	dc.SetPen(wxPen(wxColor(23,95,0),4));
	vector<choice> choices = game::get_choice(mthd_ab,color,0,0);
	for(choice& c:choices){
		dc.DrawRectangle(bias + cell * c.x,bias + cell * c.y,cell,cell);
	}

	//draw a board
	dc.SetPen(wxPen(*wxBLACK,4));
	for(int i = 0;i != num + 1;++i){
		dc.DrawLine(bias,bias + cell * i,bias + width,bias + cell * i);
	}
	for(int i = 0;i != num + 1;++i){
		dc.DrawLine(bias + cell * i,bias,bias + cell * i,bias + width);
	}

	chessman chssmn;

	for(int i = 0;i != num;++i){
		for(int j = 0;j != num;++j){
			chssmn = brd.get(i,j);
			if(chssmn == black){
				dc.SetBrush(wxBrush(wxColor(40,40,40)));
				dc.SetPen(wxPen(wxColor(20,20,20),4));
				dc.DrawCircle(wxPoint(cbias + cell * i,cbias + cell * j),radius);
			}else if(chssmn == white){
				dc.SetBrush(wxBrush(wxColor(210,210,210)));
				dc.SetPen(wxPen(wxColor(230,230,230),4));
				dc.DrawCircle(wxPoint(cbias + cell * i,cbias + cell * j),radius);
			}
		}
	}

	//show where is the last move
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.SetPen(wxPen(*wxYELLOW,4));
	dc.DrawCircle(wxPoint(cbias + cell * pos.x,cbias + cell * pos.y),radius);
}

void quit(){
	mygame.ptr_frame->Destroy();
}

void term_print(const string& str){
	mygame.ptr_term->AppendText(str + "\n");
}

void start(){
	mygame.start();
}

void auto_show(cbool flag){
	mygame.flag_auto_show = flag;
}

void auto_save(cbool flag){
	mygame.flag_auto_save = flag;
}

object bget(){
	return mygame.bget();
}

void assign(const object& obj){
	return mygame.assign(obj);
}

int visit(cint x, cint y){
	return mygame.get(pos_type(x),pos_type(y));
}

void set(cint x, cint y, cint chsman){
	return mygame.set(pos_type(x),pos_type(y),chessman(chsman));
}

int count(cbool color){
	return mygame.count(color);
}

int count_move(cbool color){
	return mygame.count_move(color);
}

void flip(cbool color,cint x,cint y){
	mygame.flip(color,x,y);
}

object play(cint mthd,cbool color,cint height){
	auto pos = mygame.play(method(mthd),color,height);
	object result;
	result.append(*mygame.ptr_inter,object(int(pos.x)));
	result.append(*mygame.ptr_inter,object(int(pos.y)));
	return result;
}

object plays(cint x,cint y,cint mthd){
	auto pos = mygame.play(coordinate(x,y),method(mthd));
	object result;
	result.append(*mygame.ptr_inter,object(int(pos.x)));
	result.append(*mygame.ptr_inter,object(int(pos.y)));
	return result;
}

bool undo(){
	return mygame.pop();
}

bool redo(){
	return mygame.redo();
}

void mirror(cbool is_horizontal){
	return mygame.mirror(is_horizontal);
}
void reflect(){
	return mygame.reflect();
}
void rotate(cbool is_clockwise){
	return mygame.rotate(is_clockwise);
}

bool get_color(){
	return mygame.color;
}

void set_color(cbool color){
	mygame.set_color(color);
}

bool get_is_lock(){
	return mygame.is_lock;
}

void set_is_lock(cbool is_lock){
	mygame.is_lock = is_lock;
}

object get_pos(){
	object result;
	result.append(*mygame.ptr_inter,object(int(mygame.pos.x)));
	result.append(*mygame.ptr_inter,object(int(mygame.pos.y)));
	return result;
}

void set_pos(cint x,cint y){
	mygame.set_pos(x,y);
}

object get_choice(cint mthd,cbool color,cint height,cint stage){
	return mygame.get_choice(mthd,color,height,stage);
}

object select_choice(object obj_choices){
	return mygame.select_choice(obj_choices);
}

void load(const string& path){
	return mygame.load(path);
};

void load_book(const string& path){
	return mygame.load_book(path);
};

void grp_assign(cint size){
	return grp.assign(size);
}
void grp_initial(){
	return grp.initial();
}
void grp_load(
	const string& filename,cbool is_compress = true,cint num_begin = 0,cint num = 100
	,cbool is_compatible = true){
	return grp.load(filename,is_compress,num_begin,num,is_compatible);
}
void grp_save(const string& filename,const bool& is_compress = true){
	return grp.save(filename,is_compress);
}
void grp_train(cint num){
	for(int i = 0;i != num;++i){
		grp.train();
	}
}

void game_gui::process(const string& str){

	static interpreter inter;
	static bool flag = true;

	if(flag){
		flag = false;
		ptr_inter = &inter;

		inter.def("quit",::quit);
		inter.def("exit",::quit);
		inter.def("puts",::term_print);
		inter.def("print",::term_print);
		inter.def("start",::start);
		inter.def("flip",::flip);
		inter.def("play",::play);
		inter.def("plays",::plays);
		inter.def("undo",::undo);
		inter.def("redo",::redo);
		inter.def("mirror",::mirror);
		inter.def("reflect",::reflect);
		inter.def("rotate",::rotate);
		inter.def("auto_show",::auto_show);
		inter.def("auto_save",::auto_save);
		inter.def("bget",::bget);
		inter.def("assign",::assign);
		inter.def("visit",::visit);
		inter.def("place",::set);
		inter.def("get_color",::get_color);
		inter.def("set_color",::set_color);
		inter.def("get_pos",::get_pos);
		inter.def("set_pos",::set_pos);
		inter.def("get_is_lock",::get_is_lock);
		inter.def("set_is_lock",::set_is_lock);
		inter.def("get_choice",::get_choice);
		inter.def("select_choice",::select_choice);
		inter.def("count",::count);
		inter.def("count_move",::count_move);
		inter.def("load",::load);
		inter.def("grp_assign",::grp_assign);
		inter.def("grp_initial",::grp_initial);
		inter.def("grp_load",::grp_load);
		inter.def("grp_save",::grp_save);
		inter.def("grp_train",::grp_train);
		inter.def("load_book",::load_book);

		inter.class_<board>("board");
//			.def("initial",board::tcl_initial)
//			.def("bget",board::tcl_bget);

		inter.class_<element>("element")
			.def("get_cnt",element::get_cnt)
			.def("to_str",element::to_str);			

		inter.class_<pattern>("pattern")
			.def("initial",pattern::initial)
			.def("get1",pattern::get1,factory("element"))
			.def("get2",pattern::get2,factory("element"));

		inter.class_<group>("group")
			.def("assign",group::assign)
			.def("initial",group::initial)
			.def("load",group::load)
			.def("save",group::save)
			.def("get",group::get,factory("pattern"))
			.def("train",group::train)
			.def("print_record",group::print_record);

		inter.def("set_ptn",set_ptn);

		inter.eval(
			"set blank 0;"
			"set white 1;"
			"set black 2;"
			"set null 3;"

			"set false 0;"
			"set true 1;"

			"set mthd_ab 0x1;"
			"set mthd_pvs 0x2;"
			"set mthd_trans 0x4;"
			"set mthd_kill 0x8;"
			"set mthd_mtdf 0x10;"
			"set mthd_ids 0x20;"
			"set mthd_ptn 0x40;"
			"set mthd_default $mthd_ab;"
		);
	}

	ptr_term->AppendText(prompt + str + "\n");
	try{
		inter.eval(str);
	}catch(const tcl_error& err){
		ptr_term->AppendText(string(err.what()) + "\n");
	}
}

void game_gui::load(const string& path){
	if(wxFileExists(path)){
		ptr_log->AppendText(_("open the file \"") + path + "\"\n");
		wxTextFile fileopen(path);
		fileopen.Open(wxConvLocal);
		wxString str;
		for(str = fileopen.GetFirstLine();!fileopen.Eof();str += fileopen.GetNextLine()){
			str += _("\n");
		};
		process(str.ToStdString());
		fileopen.Close();
	}else{
		ptr_term->AppendText(_("cannot find the file \"") + path + "\"\n");
	}
}
