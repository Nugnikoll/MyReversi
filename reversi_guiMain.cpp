/***************************************************************
 * Name:      reversi_guiMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Rick (1300012743@pku.edu.cn)
 * Created:   2016-04-16
 * Copyright: Rick ()
 * License:
 **************************************************************/

#include "reversi_guiMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(reversi_guiFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include "reversi_gui.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(reversi_guiFrame)
const long reversi_guiFrame::id_panel_board = wxNewId();
const long reversi_guiFrame::id_text_label = wxNewId();
const long reversi_guiFrame::id_text_terminal = wxNewId();
const long reversi_guiFrame::ID_NOTEBOOK1 = wxNewId();
const long reversi_guiFrame::id_text_input = wxNewId();
const long reversi_guiFrame::ID_PANEL1 = wxNewId();
const long reversi_guiFrame::id_player_black = wxNewId();
const long reversi_guiFrame::id_player_white = wxNewId();
const long reversi_guiFrame::id_new_game = wxNewId();
const long reversi_guiFrame::id_load = wxNewId();
const long reversi_guiFrame::id_save = wxNewId();
const long reversi_guiFrame::id_quit = wxNewId();
const long reversi_guiFrame::id_undo = wxNewId();
const long reversi_guiFrame::id_redo = wxNewId();
const long reversi_guiFrame::id_clockwise = wxNewId();
const long reversi_guiFrame::id_counterclockwise = wxNewId();
const long reversi_guiFrame::ID_MENUITEM7 = wxNewId();
const long reversi_guiFrame::id_horizontal = wxNewId();
const long reversi_guiFrame::id_vertical = wxNewId();
const long reversi_guiFrame::ID_MENUITEM10 = wxNewId();
const long reversi_guiFrame::id_clear_log = wxNewId();
const long reversi_guiFrame::id_clear_term = wxNewId();
const long reversi_guiFrame::id_clear_all = wxNewId();
const long reversi_guiFrame::ID_MENUITEM1 = wxNewId();
const long reversi_guiFrame::ID_MENUITEM2 = wxNewId();
const long reversi_guiFrame::ID_MENUITEM3 = wxNewId();
const long reversi_guiFrame::ID_MENUITEM4 = wxNewId();
const long reversi_guiFrame::ID_MENUITEM5 = wxNewId();
const long reversi_guiFrame::ID_MENUITEM6 = wxNewId();
const long reversi_guiFrame::id_algorithm = wxNewId();
const long reversi_guiFrame::id_level = wxNewId();
const long reversi_guiFrame::id_about = wxNewId();
const long reversi_guiFrame::ID_STATUSBAR1 = wxNewId();
//*)

const long reversi_guiFrame::id_text_log = wxNewId();

BEGIN_EVENT_TABLE(reversi_guiFrame,wxFrame)
    //(*EventTable(reversi_guiFrame)
    //*)
END_EVENT_TABLE()

reversi_guiFrame::reversi_guiFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(reversi_guiFrame)
    wxBoxSizer* BoxSizer4;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Reversi"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    {
    wxIcon FrameIcon;
    FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("E:\\LLK\\ComputerSci\\Cplusplus\\reversi\\icon\\Reversi.bmp"))));
    SetIcon(FrameIcon);
    }
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    panel_board = new wxPanel(Panel1, id_panel_board, wxDefaultPosition, wxSize(424,424), wxSUNKEN_BORDER|wxTAB_TRAVERSAL, _T("id_panel_board"));
    panel_board->SetBackgroundColour(wxColour(43,155,0));
    BoxSizer3->Add(panel_board, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    text_label = new wxStaticText(Panel1, id_text_label, _("Reversi"), wxDefaultPosition, wxSize(154,51), 0, _T("id_text_label"));
    text_label->SetForegroundColour(wxColour(200,200,200));
    wxFont text_labelFont(26,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Segoe Script"),wxFONTENCODING_DEFAULT);
    text_label->SetFont(text_labelFont);
    BoxSizer4->Add(text_label, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Notebook1 = new wxNotebook(Panel1, ID_NOTEBOOK1, wxDefaultPosition, wxSize(448,323), 0, _T("ID_NOTEBOOK1"));
    wxFont Notebook1Font(12,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    Notebook1->SetFont(Notebook1Font);
    text_terminal = new wxTextCtrl(Notebook1, id_text_terminal, wxEmptyString, wxPoint(-81,23), wxSize(283,296), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("id_text_terminal"));
    text_terminal->SetForegroundColour(wxColour(200,200,200));
    text_terminal->SetBackgroundColour(wxColour(32,32,32));
    wxFont text_terminalFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    text_terminal->SetFont(text_terminalFont);
    Notebook1->AddPage(text_terminal, wxEmptyString, false);
    BoxSizer4->Add(Notebook1, 1, wxALL|wxEXPAND, 5);
    text_input = new wxTextCtrl(Panel1, id_text_input, wxEmptyString, wxDefaultPosition, wxSize(266,30), wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB, wxDefaultValidator, _T("id_text_input"));
    text_input->SetForegroundColour(wxColour(200,200,200));
    text_input->SetBackgroundColour(wxColour(32,32,32));
    wxFont text_inputFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    text_input->SetFont(text_inputFont);
    BoxSizer4->Add(text_input, 0, wxALL|wxEXPAND, 5);
    BoxSizer2->Add(BoxSizer4, 1, wxALL|wxEXPAND, 5);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    BoxSizer1->Add(Panel1, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenu();
    MenuItem4 = new wxMenuItem(MenuItem3, id_player_black, _("Player &Black\tCtrl-B"), wxEmptyString, wxITEM_NORMAL);
    MenuItem3->Append(MenuItem4);
    MenuItem5 = new wxMenuItem(MenuItem3, id_player_white, _("Player &White\tCtrl-W"), wxEmptyString, wxITEM_NORMAL);
    MenuItem3->Append(MenuItem5);
    Menu1->Append(id_new_game, _("New Game"), MenuItem3, wxEmptyString);
    MenuItem7 = new wxMenuItem(Menu1, id_load, _("&Load\tCtrl-O"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu1, id_save, _("&Save\tCtrl-S"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem8);
    MenuItem1 = new wxMenuItem(Menu1, id_quit, _("&Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu4 = new wxMenu();
    MenuItem10 = new wxMenuItem(Menu4, id_undo, _("&Undo\tCtrl-Z"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem10);
    MenuItem11 = new wxMenuItem(Menu4, id_redo, _("&Redo\tCtrl-Y"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem11);
    MenuItem18 = new wxMenu();
    MenuItem19 = new wxMenuItem(MenuItem18, id_clockwise, _("&Clockwise"), wxEmptyString, wxITEM_NORMAL);
    MenuItem18->Append(MenuItem19);
    MenuItem20 = new wxMenuItem(MenuItem18, id_counterclockwise, _("C&ounterclockwise"), wxEmptyString, wxITEM_NORMAL);
    MenuItem18->Append(MenuItem20);
    Menu4->Append(ID_MENUITEM7, _("R&otate"), MenuItem18, wxEmptyString);
    MenuItem21 = new wxMenu();
    MenuItem22 = new wxMenuItem(MenuItem21, id_horizontal, _("&Horizontal"), wxEmptyString, wxITEM_NORMAL);
    MenuItem21->Append(MenuItem22);
    MenuItem23 = new wxMenuItem(MenuItem21, id_vertical, _("&Vertical"), wxEmptyString, wxITEM_NORMAL);
    MenuItem21->Append(MenuItem23);
    Menu4->Append(ID_MENUITEM10, _("&Mirror"), MenuItem21, wxEmptyString);
    MenuItem25 = new wxMenu();
    MenuItem24 = new wxMenuItem(MenuItem25, id_clear_log, _("&Log"), wxEmptyString, wxITEM_NORMAL);
    MenuItem25->Append(MenuItem24);
    MenuItem26 = new wxMenuItem(MenuItem25, id_clear_term, _("&Terminal"), wxEmptyString, wxITEM_NORMAL);
    MenuItem25->Append(MenuItem26);
    Menu4->Append(id_clear_all, _("&Clear"), MenuItem25, wxEmptyString);
    MenuBar1->Append(Menu4, _("&Edit"));
    Menu3 = new wxMenu();
    MenuItem12 = new wxMenu();
    MenuItem9 = new wxMenuItem(MenuItem12, ID_MENUITEM1, _("&Alpha-Beta Pruning"), wxEmptyString, wxITEM_CHECK);
    MenuItem12->Append(MenuItem9);
    MenuItem9->Enable(false);
    MenuItem9->Check(true);
    MenuItem13 = new wxMenuItem(MenuItem12, ID_MENUITEM2, _("&Principal Variation Search"), wxEmptyString, wxITEM_CHECK);
    MenuItem12->Append(MenuItem13);
    MenuItem13->Enable(false);
    MenuItem14 = new wxMenuItem(MenuItem12, ID_MENUITEM3, _("&Transition Table"), wxEmptyString, wxITEM_CHECK);
    MenuItem12->Append(MenuItem14);
    MenuItem14->Enable(false);
    MenuItem15 = new wxMenuItem(MenuItem12, ID_MENUITEM4, _("&Killer Heuristic"), wxEmptyString, wxITEM_CHECK);
    MenuItem12->Append(MenuItem15);
    MenuItem15->Enable(false);
    MenuItem15->Check(true);
    MenuItem16 = new wxMenuItem(MenuItem12, ID_MENUITEM5, _("&Memory-Enhanced Test Driver"), wxEmptyString, wxITEM_CHECK);
    MenuItem12->Append(MenuItem16);
    MenuItem16->Enable(false);
    MenuItem17 = new wxMenuItem(MenuItem12, ID_MENUITEM6, _("&Iterative Deepening Search"), wxEmptyString, wxITEM_CHECK);
    MenuItem12->Append(MenuItem17);
    MenuItem17->Enable(false);
    Menu3->Append(id_algorithm, _("&Algorithm"), MenuItem12, wxEmptyString);
    MenuItem6 = new wxMenuItem(Menu3, id_level, _("&Level"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem6);
    MenuBar1->Append(Menu3, _("&Settings"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, id_about, _("&About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    dialog_load = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, _("*.tcl"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    panel_board->Connect(wxEVT_PAINT,(wxObjectEventFunction)&reversi_guiFrame::on_panel_board_paint,0,this);
    panel_board->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&reversi_guiFrame::on_panel_board_leftdown,0,this);
    Connect(id_text_input,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&reversi_guiFrame::on_text_input_textenter);
    Connect(id_quit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::OnQuit);
    Connect(id_about,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::OnAbout);
    //*)

	Notebook1->SetPageText(0,_("terminal"));
	text_log = new wxTextCtrl(Notebook1, id_text_log, wxEmptyString, wxPoint(-81,23), wxSize(283,296), wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("id_text_log"));
    text_log->SetForegroundColour(wxColour(200,200,200));
    text_log->SetBackgroundColour(wxColour(32,32,32));
    wxFont text_logFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    text_log->SetFont(text_logFont);
    Notebook1->AddPage(text_log, _("log"), false);
    
	mygame.frame = this;
	mygame.panel = panel_board;
	mygame.term = text_terminal;
	mygame.input = text_input;
	mygame.log = text_log;
	mygame.is_lock = true;

	Connect(id_player_black,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_black);
	Connect(id_player_white,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_white);
	Connect(id_undo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_undo);
	Connect(id_redo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_redo);
	Connect(id_load,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_load);
	Connect(id_clear_log,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clear_log);
	Connect(id_clear_term,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clear_term);
	Connect(id_clear_all,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clear_all);
	Connect(id_clockwise,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clockwise);
	Connect(id_counterclockwise,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_counterclockwise);
	Connect(id_horizontal,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_horizontal);
	Connect(id_vertical,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_vertical);

	panel_board->Connect(wxEVT_CONTEXT_MENU,wxContextMenuEventHandler(reversi_guiFrame::on_context_menu),NULL,this); 
}

reversi_guiFrame::~reversi_guiFrame()
{
    //(*Destroy(reversi_guiFrame)
    //*)
}

void reversi_guiFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void reversi_guiFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = _("Reversi Game\nBy Rick\n1300012743@pku.edu.cn");
    wxMessageBox(msg, _("About"));
}

void reversi_guiFrame::OnClose(wxCloseEvent& event)
{
}

void reversi_guiFrame::on_panel_board_paint(wxPaintEvent& event)
{
	wxClientDC dc(panel_board);
	mygame.do_show(dc);
}

void reversi_guiFrame::on_black(wxCommandEvent& event){
	mygame.process("start");
}

void reversi_guiFrame::on_white(wxCommandEvent& event){
	mygame.process("start");
	mygame.process("puts [play $mthd_ab $true 7]");
}

void reversi_guiFrame::on_undo(wxCommandEvent& event){
	mygame.process("undo");
}

void reversi_guiFrame::on_redo(wxCommandEvent& event){
	mygame.process("redo");
}

void reversi_guiFrame::on_load(wxCommandEvent& event){
	if(dialog_load->ShowModal() == wxID_OK){
		string path = dialog_load->GetPath().ToStdString();
		string::size_type pos = 0;
		while((pos = path.find("\\",pos + 2)) != path.npos){
			path.replace(pos,1,"\\\\");
		}
		mygame.process(string("load ") + path);
	}
}

void reversi_guiFrame::on_clockwise(wxCommandEvent& event){
	mygame.process("rotate $true");
}

void reversi_guiFrame::on_counterclockwise(wxCommandEvent& event){
	mygame.process("rotate $false");
}

void reversi_guiFrame::on_horizontal(wxCommandEvent& event){
	mygame.process("mirror $true");
}

void reversi_guiFrame::on_vertical(wxCommandEvent& event){
	mygame.process("mirror $false");
}

void reversi_guiFrame::on_clear_log(wxCommandEvent& event){
	text_log->Clear();
}

void reversi_guiFrame::on_clear_term(wxCommandEvent& event){
	text_terminal->Clear();
}

void reversi_guiFrame::on_clear_all(wxCommandEvent& event){
	text_log->Clear();
	text_terminal->Clear();
}

void reversi_guiFrame::on_panel_board_leftdown(wxMouseEvent& event)
{
	if(mygame.is_lock){
		return;
	}
	wxPoint pos = event.GetPosition();
	int x = (pos.x - mygame.bias) / mygame.cell;
	int y = (pos.y - mygame.bias) / mygame.cell;
	mygame.process(
		(
			_("puts [plays ") + wxString::FromDouble(x) + " "
			+ wxString::FromDouble(y) + _(" $mthd_ab]")
		).ToStdString()
	);
}

void reversi_guiFrame::on_text_input_textenter(wxCommandEvent& event)
{
	mygame.process(text_input->GetValue().ToStdString());
}

void reversi_guiFrame::on_context_menu(wxContextMenuEvent& event){
	//wxMenu* menu = new wxMenu();

	//menu->Append(id_undo, _("&Undo"));  
//	menu->Append(wxID_REDO, _("&Redo"));  
//	menu->AppendSeparator();  
//	menu->Append(wxID_CUT, _("Cu&t"));  
//	menu->Append(wxID_COPY, _("&Copy"));  
//	menu->Append(wxID_PASTE, _("&Paste"));  
//	menu->Append(wxID_CLEAR, _("&Delete"));  
//	menu->AppendSeparator();  
//	menu->Append(wxID_SELECTALL, _("Select &All"));  
//  
	//PopupMenu(menu);
}