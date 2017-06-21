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
#include <wx/textfile.h>
#include <wx/treectrl.h>

#include "game.h"

#include "cpptcl.h"

using namespace Tcl;



class myTreeItemData: public wxTreeItemData{
public:
	myTreeItemData(node*const& _ptr): ptr(_ptr){}
	node* ptr;
};

class game_gui:public game{
public:
	const int bias = 10;
	const int num = 8;
	const int cell = 50;
	const int width = num * cell;
	const int cbias = bias + cell / 2;
	const int radius = cell / 2 - 5;

	wxFrame* ptr_frame;
	wxPanel* ptr_panel;
	wxTextCtrl* ptr_term;
	wxTextCtrl* ptr_log;
	wxTextCtrl* ptr_input;
	wxTreeCtrl* ptr_book;

	interpreter* ptr_inter;

	virtual void show();
	void do_show(wxDC& dc);

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

	object eval_ptn(cbool color){
		object result;
		vector<float> vec = brd.eval_ptn(color);
		for(float& i:vec){
			result.append(*ptr_inter,object(i));
		}
		return result;
	}

	object get_choice(cint mthd,cbool color,cint height,cint stage = 0){
		vector<choice> choices = brd.get_choice(method(mthd),color,height,stage);
		object result;
		wxString str;
		wxClientDC dc(ptr_panel);
		dc.SetTextForeground(wxColor(255,0,150));
		dc.SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT));

		pos_type x,y;
		for(choice& c:choices){
			x = c.pos & 7;
			y = c.pos >> 3;
			object obj;
			obj.append(*ptr_inter,object(double(c.val)));
			obj.append(*ptr_inter,object(int(x)));
			obj.append(*ptr_inter,object(int(y)));
			result.append(*ptr_inter,obj);
			str = wxString::FromDouble(c.val);
			dc.DrawText(str,bias + cell * x  + cell / 2 - 4 * str.size(),bias + cell * y + cell / 2 - 8);
		}
		return result;
	}
	object select_choice(object obj_choices){
		vector<choice> choices;
		choice temp;
		object obj;
		object result;

		int num = obj_choices.length(*ptr_inter);

		for(int i = 0;i != num;++i){
			obj = obj_choices.at(*ptr_inter,i);
			temp.val = obj.at(*ptr_inter,0).get<double>(*ptr_inter);
			temp.pos = obj.at(*ptr_inter,1).get<int>(*ptr_inter)
				+ (obj.at(*ptr_inter,2).get<int>(*ptr_inter) << 3);
			choices.push_back(temp);
		}
		temp = brd.select_choice(choices);
		result.append(*ptr_inter,object(double(temp.val)));
		result.append(*ptr_inter,object(int(temp.pos & 7)));
		result.append(*ptr_inter,object(int(temp.pos >> 3)));
		return result;
	}

	void process(const string& str);

	void load(const string& path);

	void load_book(const string& path){
		book.load(path);
		ptr_book->DeleteAllItems();
		wxTreeItemId item_root = ptr_book->AddRoot(_("Root"));
		load_node(item_root,book.root);
	}

	void load_node(const wxTreeItemId& item, node* ptr){
		wxTreeItemId item_branch;
		ostringstream out;

		for(ptr = ptr->child;ptr;ptr = ptr->sibling){
			out << "x:" << (ptr->dat.pos & 7) << " y:" << (ptr->dat.pos >> 3)
				<< " " << (ptr->dat.color ? "black" : "white")
				<< " win:" << ptr->dat.win
				<< " lose:" << ptr->dat.lose;
			item_branch = ptr_book->AppendItem(item,out.str(),-1,-1,new myTreeItemData(ptr));
			out.str("");
			load_node(item_branch,ptr);
		}
	}

protected:

};

extern game_gui mygame;

#endif //REVERSI_GUI_H
