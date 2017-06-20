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
	brd_type brd_move = mygame.brd.get_move(color);
	for(int i = 0;i != board::size2;++i){
		if(brd_move & (1ull << i))
		dc.DrawRectangle(bias + cell * (i & 7),bias + cell * (i >> 3),cell,cell);
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
			chssmn = brd.get(i + (j << 3));
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

float score(cbool color, cint stage){
	return mygame.score(color, stage);
}

object eval_ptn(cbool color){
	return mygame.eval_ptn(color);
}

void config(){
	return mygame.config();
} 
bool flip(cbool color,cint x,cint y){
	return mygame.flip(color,x,y);
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
	return mygame.undo();
}

bool redo(){
	return mygame.redo();
}

void mirror_h(){
	return mygame.mirror_h();
}
void mirror_v(){
	return mygame.mirror_v();
}
void reflect(){
	return mygame.reflect();
}
void rotate_l(){
	return mygame.rotate_l();
}
void rotate_r(){
	return mygame.rotate_r();
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
void grp_load(const string& filename,cint num_begin = 0,cint num = 100){
	return grp.load(filename,num_begin,num);
}
void grp_save(const string& filename){
	return grp.save(filename);
}
void grp_train(cint num, cint mthd, cint depth){
	for(int i = 0;i != num;++i){
		grp.train(method(mthd),depth);
	}
}

void use_ptn(cint num){
	set_ptn(grp.get(num));
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
		inter.def("config",::config);
		inter.def("flip",::flip);
		inter.def("play",::play);
		inter.def("plays",::plays);
		inter.def("undo",::undo);
		inter.def("redo",::redo);
		inter.def("mirror_h",::mirror_h);
		inter.def("mirror_v",::mirror_v);
		inter.def("reflect",::reflect);
		inter.def("rotate_l",::rotate_l);
		inter.def("rotate_r",::rotate_r);
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
		inter.def("score",::score);
		inter.def("eval_ptn",::eval_ptn);

		inter.def("load",::load);

		inter.def("grp_assign",::grp_assign);
		inter.def("grp_initial",::grp_initial);
		inter.def("grp_load",::grp_load);
		inter.def("grp_save",::grp_save);
		inter.def("grp_train",::grp_train);

		inter.def("set_ptn",set_ptn);
		inter.def("use_ptn",::use_ptn);
		inter.def("check_ptn",::check_ptn);

		inter.def("load_book",::load_book);

		inter.class_<board>("board");
//			.def("initial",board::tcl_initial)
//			.def("bget",board::tcl_bget);		

		inter.class_<pattern>("pattern")
			.def("initial",pattern::initial)
			.def("get1",pattern::get1)
			.def("get2",pattern::get2);

		inter.class_<group>("group")
			.def("assign",group::assign)
			.def("initial",group::initial)
			.def("load",group::load)
			.def("save",group::save)
			.def("get",group::get,factory("pattern"))
			//.def("train",group::train)
			.def("print_record",group::print_record);

		inter.eval(
			"set blank 0;"
			"set white 1;"
			"set black 2;"
			"set null 3;"

			"set false 0;"
			"set true 1;"

			"set mthd_rnd 0x0;"
			"set mthd_ab 0x1;"
			"set mthd_pvs 0x2;"
			"set mthd_trans 0x4;"
			"set mthd_kill 0x8;"
			"set mthd_mtdf 0x10;"
			"set mthd_ids 0x20;"
			"set mthd_ptn 0x40;"
			"set mthd_default $mthd_ab;"

			"config;"
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
