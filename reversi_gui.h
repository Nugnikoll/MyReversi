#ifndef REVERSI_GUI_H
#define REVERSI_GUI_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>

#include "game.h"

#include "cpptcl.h"

using namespace Tcl;

class game_gui:public game{
public:
	const int bias = 10;
	const int num = 8;
	const int cell = 50;
	const int width = num * cell;
	const int cbias = bias + cell / 2;
	const int radius = cell / 2 - 5;

	bool is_lock;

	struct pack{
		board brd;
		bool color;
		coordinate pos;
	};

	bool color;
	coordinate pos;

	wxFrame* frame;
	wxPanel* panel;
	wxTextCtrl* term;
	wxTextCtrl* log;
	wxTextCtrl* input;

	interpreter* ptr_inter;

	virtual void show();
	void do_show(wxDC& dc);

	void start(){
		color = true;
		pos.x = pos.y = -1;
		record.clear();
		game::start();
		log->AppendText(_("start a new game\n"));
		is_lock = false;
	}
	object bget(){
		object result;
		result.append(*ptr_inter,object((int)(brd.bget(true) >> 32)));
		result.append(*ptr_inter,object((int)(brd.bget(true))));
		result.append(*ptr_inter,object((int)(brd.bget(false) >> 32)));
		result.append(*ptr_inter,object((int)(brd.bget(false))));
		return result;
	}
	void assign(const object& obj){
		if(flag_auto_save){
			push();
		}
		brd_type temp_black, temp_white;
		temp_black = (unsigned int)obj.at(*ptr_inter,0).get<int>(*ptr_inter);
		temp_black <<= 32;
		temp_black |= (unsigned int)obj.at(*ptr_inter,1).get<int>(*ptr_inter);
		temp_white = (unsigned int)obj.at(*ptr_inter,2).get<int>(*ptr_inter);
		temp_white <<= 32;
		temp_white |= (unsigned int)obj.at(*ptr_inter,3).get<int>(*ptr_inter);
		brd.assign(temp_black,temp_white);
		if(flag_auto_show){
			show();
		}
	}
	void set(cint x, cint y, cint chsman){
		if(flag_auto_save){
			push();
		}
		brd.set(pos_type(x),pos_type(y),chessman(chsman));
		if(flag_auto_show){
			show();
		}
	}
	virtual void push(){
		pack temp = {brd,color,pos};
		record.push_back(temp);
		storage.clear();
	}
	virtual bool pop(){
		if(record.empty()){
			return false;
		}else{
			pack temp = {brd,color,pos};
			storage.push_back(temp);

			temp = record.back();
			record.pop_back();
			brd = temp.brd;
			color = temp.color;
			pos = temp.pos;

			if(flag_auto_show){
				show();
			}
			log->AppendText(_("undo\n"));
			is_lock = false;
			return true;
		}
	}
	bool redo(){
		if(storage.empty()){
			return false;
		}else{
			pack temp = {brd,color,pos};
			record.push_back(temp);

			temp = storage.back();
			storage.pop_back();
			brd = temp.brd;
			color = temp.color;
			pos = temp.pos;

			if(flag_auto_show){
				show();
			}
			log->AppendText(_("redo\n"));
			is_lock = false;
			return true;
		}
	}
	void mirror(cbool is_horizontal){
		if(flag_auto_save){
			push();
		}
		brd.mirror(is_horizontal);
		if(is_horizontal){
			pos.x = 7 - pos.x;
			log->AppendText(_("mirror horizontally\n"));
		}else{
			pos.y = 7 - pos.y;
			log->AppendText(_("mirror vertically\n"));
		}
		if(flag_auto_show){
			show();
		}
	}
	void rotate(cbool is_clockwise){
		if(flag_auto_save){
			push();
		}
		coordinate temp = pos;
		if(is_clockwise){
			pos.x = 7 - temp.y;
			pos.y = temp.x;
			brd.rotater(1);
			log->AppendText(_("rotate clockwise\n"));
		}else{
			pos.x = temp.y;
			pos.y = 7 - temp.x;
			brd.rotatel(1);
			log->AppendText(_("rotate counterclockwise\n"));
		}
		if(flag_auto_show){
			show();
		}
	}
	virtual bool flip(cbool color, cpos_type x, cpos_type y){
		if(flag_auto_save){
			push();
		}
		bool result = brd.flip(color,x,y);
		if(result){
			log->AppendText(
				(color? _("black") : _("white"))
				+ _(" place a stone at (")
				+ wxString::FromDouble(x)
				+ _(",")
				+ wxString::FromDouble(y)
				+ _(")\n")
			);
			this->color = !color;
			this->pos = coordinate(x,y);
			if(flag_auto_show){
				show();
			}
		}else{
			if(flag_auto_save){
				do_pop();
			}
			log->AppendText(
				(color? _("black") : _("white"))
				+ _(" cannot place a stone here\n")
			);
		}
		return result;
	}
	object get_choice(cint mthd,cbool color,cint height,cint stage = 0){
		vector<choice> choices = brd.get_choice(method(mthd),color,height,stage);
		object result;
		wxString str;
		wxClientDC dc(panel);
		dc.SetTextForeground(wxColor(200,20,20));
		dc.SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT));
		for(choice& c:choices){
			object obj;
			obj.append(*ptr_inter,object(double(c.val)));
			obj.append(*ptr_inter,object(int(c.x)));
			obj.append(*ptr_inter,object(int(c.y)));
			result.append(*ptr_inter,obj);
			str = wxString::FromDouble(c.val);
			dc.DrawText(str,bias + cell * c.x  + cell / 2 - 4 * str.size(),bias + cell * c.y + cell / 2 - 8);
		}
		return result;
	}
	object select_choice(object obj_choices){
		vector<choice> choices;
		choice temp;
		object obj;
		object result;
		//cout << obj_choices.get<string>(*ptr_inter) << " | ";

		int num = obj_choices.length(*ptr_inter);
		//cout << num << endl;
		for(int i = 0;i != num;++i){
			obj = obj_choices.at(*ptr_inter,i);
			temp.val = obj.at(*ptr_inter,0).get<double>(*ptr_inter);
			temp.x = obj.at(*ptr_inter,1).get<int>(*ptr_inter);
			temp.y = obj.at(*ptr_inter,2).get<int>(*ptr_inter);
			choices.push_back(temp);
		}
		temp = brd.select_choice(choices);
		result.append(*ptr_inter,object(double(temp.val)));
		result.append(*ptr_inter,object(int(temp.x)));
		result.append(*ptr_inter,object(int(temp.y)));
		return result;
	}
	virtual coordinate play(cmethod mthd,cbool color,cint height = -1){
//		if(flag_auto_save){
//			push();
//		}
		auto pos = brd.play(mthd,color,height);
		if(pos.x >= 0){
			log->AppendText(
				(color? _("black") : _("white"))
				+ _(" place a stone at (")
				+ wxString::FromDouble(pos.x)
				+ _(",")
				+ wxString::FromDouble(pos.y)
				+ _(")\n")
			);
			this->color = !color;
			this->pos = pos;
			if(flag_auto_show){
				show();
			}
		}else{
			log->AppendText(
				(color? _("black") : _("white"))
				+ _(" is unable to move.\n")
			);
//			if(flag_auto_save){
//				do_pop();
//			}
		}
		return pos;
	}
	virtual coordinate do_play(cmethod mthd,cbool color,cint height = -1){
		if(flag_auto_save){
			push();
		}
		auto pos = brd.play(mthd,color,height);
		if(pos.x >= 0){
			log->AppendText(
				(color? _("black") : _("white"))
				+ _(" place a stone at (")
				+ wxString::FromDouble(pos.x)
				+ _(",")
				+ wxString::FromDouble(pos.y)
				+ _(")\n")
			);
			this->color = !color;
			this->pos = pos;
			if(flag_auto_show){
				show();
			}
		}else{
			log->AppendText(
				(color? _("black") : _("white"))
				+ _(" is unable to move.\n")
			);
			if(flag_auto_save){
				do_pop();
			}
		}
		return pos;
	}
	coordinate play(ccoordinate _pos,cmethod mthd){
		bool flag = flip(color,_pos.x,_pos.y);
		if(!flag){
			return coordinate(-1,-1);
		}
		sts_type status = sts_again;
		coordinate pos;
		while(true){
			pos = play(mthd,color);
			status = brd.get_status(color);
			if(pos.x < 0){
				color = !color;
				break;
			}
			if(status & sts_again){
				color = !color;
			}else{
				break;
			}
		}
		if(status & sts_end){
			is_lock = true;
			log->AppendText(
				wxString::FromDouble(brd.count(true))
				+ _(" black stones and ")
				+ wxString::FromDouble(brd.count(false))
				+ _(" white stones remain\n")
			);
			if(status == sts_bwin){
				log->AppendText(_("black wins\n"));
			}else if(status == sts_wwin){
				log->AppendText(_("white wins\n"));
			}else{
				log->AppendText(_("a tie\n"));
			}
		}
		return pos;
	}
	void set_color(cbool _color){
		if(flag_auto_save){
			push();
		}
		color = _color;
		if(flag_auto_show){
			show();
		}
	}
	void set_pos(cint x,cint y){
		if(flag_auto_save){
			push();
		}
		pos.x = x;
		pos.y = y;
		if(flag_auto_show){
			show();
		}
	}

	void process(const string& str);

protected:
	vector<pack> record;
	vector<pack> storage;

	virtual bool do_pop(){
		if(record.empty()){
			return false;
		}else{
			pack temp = {brd,color,pos};
			storage.push_back(temp);

			temp = record.back();
			record.pop_back();
			brd = temp.brd;
			color = temp.color;
			pos = temp.pos;

			return true;
		}
	}
};

extern game_gui mygame;

#endif //REVERSI_GUI_H
