#include "reversi_gui.h"

wxFrame* ptr_frame;
wxPanel* ptr_panel;
wxTextCtrl* ptr_term;
wxTextCtrl* ptr_log;
wxTextCtrl* ptr_input;
wxTreeCtrl* ptr_book;

void show_choice(const vector<choice>& choices){
	wxClientDC dc(ptr_panel);
	dc.SetTextForeground(wxColor(255,0,150));
	dc.SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT));
	wxString str;
	pos_type x,y;
	for(const choice& c:choices){
		x = c.pos & 7;
		y = c.pos >> 3;
		str = wxString::FromDouble(c.val);
		dc.DrawText(str,bias + cell * x  + cell / 2 - 4 * str.size(),bias + cell * y + cell / 2 - 8);
	}
}

void do_show(wxDC& dc){

	dc.Clear();

	//draw valid moves
	dc.SetBrush(wxBrush(wxColor(23,95,0)));
	dc.SetPen(wxPen(wxColor(23,95,0),4));
	brd_type brd_move = mygame.brd.get_move(mygame.color);
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
			chssmn = mygame.brd.get(i + (j << 3));
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
	dc.DrawCircle(wxPoint(cbias + cell * mygame.pos.x,cbias + cell * mygame.pos.y),radius);
}

void game_gui::show(){
	wxClientDC dc(ptr_panel);
	do_show(dc);
}

void game_gui::log_print(const string& str){
	ptr_log->AppendText(str);
}

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

void quit(){
	ptr_frame->Destroy();
}

void term_print(const string& str){
	ptr_term->AppendText(str + "\n");
}

void log_print(const string& str){
	ptr_log->AppendText(str);
}

void load_script(const string& path){
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
