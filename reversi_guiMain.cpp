/***************************************************************
 * Name:      reversi_guiMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Rick (1300012743@pku.edu.cn)
 * Created:   2016-04-16
 * Copyright: Rick ()
 * License:
 **************************************************************/

#include <algorithm>

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
const long reversi_guiFrame::id_text_term = wxNewId();
const long reversi_guiFrame::id_text_log = wxNewId();
const long reversi_guiFrame::id_book_tree = wxNewId();
const long reversi_guiFrame::id_notebook = wxNewId();
const long reversi_guiFrame::id_text_input = wxNewId();
const long reversi_guiFrame::id_panel_base = wxNewId();
const long reversi_guiFrame::id_menu_black = wxNewId();
const long reversi_guiFrame::id_menu_white = wxNewId();
const long reversi_guiFrame::id_menu_new = wxNewId();
const long reversi_guiFrame::id_menu_load = wxNewId();
const long reversi_guiFrame::id_menu_save = wxNewId();
const long reversi_guiFrame::id_menu_quit = wxNewId();
const long reversi_guiFrame::id_menu_undo = wxNewId();
const long reversi_guiFrame::id_menu_redo = wxNewId();
const long reversi_guiFrame::id_menu_mirror_h = wxNewId();
const long reversi_guiFrame::id_menu_mirror_v = wxNewId();
const long reversi_guiFrame::id_menu_reflect = wxNewId();
const long reversi_guiFrame::id_menu_rotate_r = wxNewId();
const long reversi_guiFrame::id_menu_rotate_l = wxNewId();
const long reversi_guiFrame::id_menu_reverse = wxNewId();
const long reversi_guiFrame::id_menu_trans = wxNewId();
const long reversi_guiFrame::id_menu_clear_log = wxNewId();
const long reversi_guiFrame::id_menu_clear_term = wxNewId();
const long reversi_guiFrame::id_menu_clear = wxNewId();
const long reversi_guiFrame::id_menu_alg_rnd = wxNewId();
const long reversi_guiFrame::id_menu_alg_ab = wxNewId();
const long reversi_guiFrame::id_menu_alg_pvs = wxNewId();
const long reversi_guiFrame::id_menu_alg_trans = wxNewId();
const long reversi_guiFrame::id_menu_alg_kill = wxNewId();
const long reversi_guiFrame::id_menu_alg_mtdf = wxNewId();
const long reversi_guiFrame::id_menu_alg_ids = wxNewId();
const long reversi_guiFrame::id_menu_alg_ptn = wxNewId();
const long reversi_guiFrame::id_menu_alg_mpc = wxNewId();
const long reversi_guiFrame::id_menu_alg = wxNewId();
const long reversi_guiFrame::id_menu_level1 = wxNewId();
const long reversi_guiFrame::id_menu_level2 = wxNewId();
const long reversi_guiFrame::id_menu_level3 = wxNewId();
const long reversi_guiFrame::id_menu_level4 = wxNewId();
const long reversi_guiFrame::id_menu_level5 = wxNewId();
const long reversi_guiFrame::id_menu_level6 = wxNewId();
const long reversi_guiFrame::id_menu_level7 = wxNewId();
const long reversi_guiFrame::id_menu_level8 = wxNewId();
const long reversi_guiFrame::id_menu_level9 = wxNewId();
const long reversi_guiFrame::id_menu_level10 = wxNewId();
const long reversi_guiFrame::id_menu_level = wxNewId();
const long reversi_guiFrame::id_menu_about = wxNewId();
const long reversi_guiFrame::id_statusbar = wxNewId();
//*)

BEGIN_EVENT_TABLE(reversi_guiFrame,wxFrame)
    //(*EventTable(reversi_guiFrame)
    //*)
END_EVENT_TABLE()

reversi_guiFrame::reversi_guiFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(reversi_guiFrame)
    wxBoxSizer* BoxSizer4;
    wxMenuItem* menu_about;
    wxMenuBar* menubar;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;
    wxMenu* menu;
    wxMenuItem* menu_quit;

    Create(parent, wxID_ANY, _("Reversi"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    {
    wxIcon FrameIcon;
    FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("E:\\LLK\\ComputerSci\\Cplusplus\\reversi\\image\\Reversi.bmp"))));
    SetIcon(FrameIcon);
    }
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    panel_base = new wxPanel(this, id_panel_base, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id_panel_base"));
    panel_base->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    panel_board = new wxPanel(panel_base, id_panel_board, wxDefaultPosition, wxSize(424,424), wxSUNKEN_BORDER|wxTAB_TRAVERSAL, _T("id_panel_board"));
    panel_board->SetBackgroundColour(wxColour(43,155,0));
    BoxSizer3->Add(panel_board, 1, wxALL, 5);
    BoxSizer2->Add(BoxSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    text_label = new wxStaticText(panel_base, id_text_label, _("Reversi"), wxDefaultPosition, wxSize(154,51), 0, _T("id_text_label"));
    text_label->SetForegroundColour(wxColour(200,200,200));
    wxFont text_labelFont(26,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Segoe Script"),wxFONTENCODING_DEFAULT);
    text_label->SetFont(text_labelFont);
    BoxSizer4->Add(text_label, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    notebook = new wxNotebook(panel_base, id_notebook, wxDefaultPosition, wxSize(448,296), 0, _T("id_notebook"));
    wxFont notebookFont(12,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    notebook->SetFont(notebookFont);
    text_term = new wxTextCtrl(notebook, id_text_term, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("id_text_term"));
    text_term->SetForegroundColour(wxColour(200,200,200));
    text_term->SetBackgroundColour(wxColour(32,32,32));
    wxFont text_termFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    text_term->SetFont(text_termFont);
    text_log = new wxTextCtrl(notebook, id_text_log, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("id_text_log"));
    text_log->SetForegroundColour(wxColour(200,200,200));
    text_log->SetBackgroundColour(wxColour(32,32,32));
    wxFont text_logFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    text_log->SetFont(text_logFont);
    book_tree = new wxTreeCtrl(notebook, id_book_tree, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("id_book_tree"));
    book_tree->SetForegroundColour(wxColour(200,200,200));
    book_tree->SetBackgroundColour(wxColour(32,32,32));
    wxFont book_treeFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    book_tree->SetFont(book_treeFont);
    notebook->AddPage(text_term, _("terminal"), false);
    notebook->AddPage(text_log, _("log"), false);
    notebook->AddPage(book_tree, _("book"), false);
    BoxSizer4->Add(notebook, 1, wxALL|wxEXPAND, 5);
    text_input = new wxTextCtrl(panel_base, id_text_input, wxEmptyString, wxDefaultPosition, wxSize(266,30), wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB, wxDefaultValidator, _T("id_text_input"));
    text_input->SetForegroundColour(wxColour(200,200,200));
    text_input->SetBackgroundColour(wxColour(32,32,32));
    wxFont text_inputFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Consolas"),wxFONTENCODING_DEFAULT);
    text_input->SetFont(text_inputFont);
    BoxSizer4->Add(text_input, 0, wxALL|wxEXPAND, 5);
    BoxSizer2->Add(BoxSizer4, 1, wxALL|wxEXPAND, 5);
    panel_base->SetSizer(BoxSizer2);
    BoxSizer2->Fit(panel_base);
    BoxSizer2->SetSizeHints(panel_base);
    BoxSizer1->Add(panel_base, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    menubar = new wxMenuBar();
    menu = new wxMenu();
    menu_new = new wxMenu();
    menu_black = new wxMenuItem(menu_new, id_menu_black, _("Player &Black\tCtrl-B"), wxEmptyString, wxITEM_NORMAL);
    menu_new->Append(menu_black);
    menu_white = new wxMenuItem(menu_new, id_menu_white, _("Player &White\tCtrl-W"), wxEmptyString, wxITEM_NORMAL);
    menu_new->Append(menu_white);
    menu->Append(id_menu_new, _("New Game"), menu_new, wxEmptyString);
    menu_load = new wxMenuItem(menu, id_menu_load, _("&Load\tCtrl-L"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(menu_load);
    menu_save = new wxMenuItem(menu, id_menu_save, _("&Save\tCtrl-S"), wxEmptyString, wxITEM_NORMAL);
    menu->Append(menu_save);
    menu_quit = new wxMenuItem(menu, id_menu_quit, _("&Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    menu->Append(menu_quit);
    menubar->Append(menu, _("&File"));
    menu_edit = new wxMenu();
    menu_undo = new wxMenuItem(menu_edit, id_menu_undo, _("&Undo\tCtrl-Z"), wxEmptyString, wxITEM_NORMAL);
    menu_edit->Append(menu_undo);
    menu_redo = new wxMenuItem(menu_edit, id_menu_redo, _("&Redo\tCtrl-Y"), wxEmptyString, wxITEM_NORMAL);
    menu_edit->Append(menu_redo);
    menu_trans = new wxMenu();
    menu_mirror_h = new wxMenuItem(menu_trans, id_menu_mirror_h, _("Mirror &Horizontally\tAlt+H"), wxEmptyString, wxITEM_NORMAL);
    menu_trans->Append(menu_mirror_h);
    menu_mirror_v = new wxMenuItem(menu_trans, id_menu_mirror_v, _("Mirror &Vertically\tAlt+V"), wxEmptyString, wxITEM_NORMAL);
    menu_trans->Append(menu_mirror_v);
    menu_reflect = new wxMenuItem(menu_trans, id_menu_reflect, _("&Reflect\tAlt+R"), wxEmptyString, wxITEM_NORMAL);
    menu_trans->Append(menu_reflect);
    menu_rotate_r = new wxMenuItem(menu_trans, id_menu_rotate_r, _("Rotate &Clockwise\tAlt+C"), wxEmptyString, wxITEM_NORMAL);
    menu_trans->Append(menu_rotate_r);
    menu_rotate_l = new wxMenuItem(menu_trans, id_menu_rotate_l, _("Rotate Coun&terclockwise\tAlt+T"), wxEmptyString, wxITEM_NORMAL);
    menu_trans->Append(menu_rotate_l);
    menu_reverse = new wxMenuItem(menu_trans, id_menu_reverse, _("Reverse\tAlt+S"), wxEmptyString, wxITEM_NORMAL);
    menu_trans->Append(menu_reverse);
    menu_edit->Append(id_menu_trans, _("&Transform"), menu_trans, wxEmptyString);
    menu_clear = new wxMenu();
    menu_clear_log = new wxMenuItem(menu_clear, id_menu_clear_log, _("&Log"), wxEmptyString, wxITEM_NORMAL);
    menu_clear->Append(menu_clear_log);
    menu_clear_term = new wxMenuItem(menu_clear, id_menu_clear_term, _("&Terminal"), wxEmptyString, wxITEM_NORMAL);
    menu_clear->Append(menu_clear_term);
    menu_edit->Append(id_menu_clear, _("&Clear"), menu_clear, wxEmptyString);
    menubar->Append(menu_edit, _("&Edit"));
    menu_set = new wxMenu();
    menu_alg = new wxMenu();
    menu_alg_rnd = new wxMenuItem(menu_alg, id_menu_alg_rnd, _("&Random"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_rnd);
    menu_alg->AppendSeparator();
    menu_alg_ab = new wxMenuItem(menu_alg, id_menu_alg_ab, _("&Alpha-Beta Pruning"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_ab);
    menu_alg_ab->Check(true);
    menu_alg_pvs = new wxMenuItem(menu_alg, id_menu_alg_pvs, _("Principal &Variation Search"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_pvs);
    menu_alg_pvs->Check(true);
    menu_alg_trans = new wxMenuItem(menu_alg, id_menu_alg_trans, _("&Transposition Table"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_trans);
    menu_alg_trans->Check(true);
    menu_alg_kill = new wxMenuItem(menu_alg, id_menu_alg_kill, _("&Killer Heuristic"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_kill);
    menu_alg_kill->Check(true);
    menu_alg_mtdf = new wxMenuItem(menu_alg, id_menu_alg_mtdf, _("&Memory-Enhanced Test Driver"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_mtdf);
    menu_alg_mtdf->Check(true);
    menu_alg_ids = new wxMenuItem(menu_alg, id_menu_alg_ids, _("&Iterative Deepening Search"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_ids);
    menu_alg_ptn = new wxMenuItem(menu_alg, id_menu_alg_ptn, _("&Pattern"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_ptn);
    menu_alg_ptn->Check(true);
    menu_alg_mpc = new wxMenuItem(menu_alg, id_menu_alg_mpc, _("Multi-Probability &Cut"), wxEmptyString, wxITEM_CHECK);
    menu_alg->Append(menu_alg_mpc);
    menu_set->Append(id_menu_alg, _("&Algorithm"), menu_alg, wxEmptyString);
    menu_level = new wxMenu();
    menu_level1 = new wxMenuItem(menu_level, id_menu_level1, _("Level 1"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level1);
    menu_level2 = new wxMenuItem(menu_level, id_menu_level2, _("Level 2"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level2);
    menu_level3 = new wxMenuItem(menu_level, id_menu_level3, _("Level 3"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level3);
    menu_level4 = new wxMenuItem(menu_level, id_menu_level4, _("Level 4"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level4);
    menu_level5 = new wxMenuItem(menu_level, id_menu_level5, _("Level 5"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level5);
    menu_level6 = new wxMenuItem(menu_level, id_menu_level6, _("Level 6"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level6);
    menu_level7 = new wxMenuItem(menu_level, id_menu_level7, _("Level 7"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level7);
    menu_level8 = new wxMenuItem(menu_level, id_menu_level8, _("Level 8"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level8);
    menu_level8->Check(true);
    menu_level9 = new wxMenuItem(menu_level, id_menu_level9, _("Level 9"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level9);
    menu_level10 = new wxMenuItem(menu_level, id_menu_level10, _("Level 10"), wxEmptyString, wxITEM_CHECK);
    menu_level->Append(menu_level10);
    menu_set->Append(id_menu_level, _("&Level"), menu_level, wxEmptyString);
    menubar->Append(menu_set, _("&Settings"));
    Menu2 = new wxMenu();
    menu_about = new wxMenuItem(Menu2, id_menu_about, _("&About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(menu_about);
    menubar->Append(Menu2, _("&Help"));
    SetMenuBar(menubar);
    statusbar = new wxStatusBar(this, id_statusbar, 0, _T("id_statusbar"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    statusbar->SetFieldsCount(1,__wxStatusBarWidths_1);
    statusbar->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(statusbar);
    dialog_load = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, _("*.tcl"), wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    panel_board->Connect(wxEVT_PAINT,(wxObjectEventFunction)&reversi_guiFrame::on_panel_board_paint,0,this);
    panel_board->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&reversi_guiFrame::on_panel_board_leftdown,0,this);
    Connect(id_text_input,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&reversi_guiFrame::on_text_input_textenter);
    Connect(id_menu_black,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_black);
    Connect(id_menu_white,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_white);
    Connect(id_menu_load,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_load);
    Connect(id_menu_quit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::OnQuit);
    Connect(id_menu_undo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_undo);
    Connect(id_menu_redo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_redo);
    Connect(id_menu_mirror_h,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_transform);
    Connect(id_menu_mirror_v,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_transform);
    Connect(id_menu_reflect,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_transform);
    Connect(id_menu_rotate_r,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_transform);
    Connect(id_menu_rotate_l,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_transform);
    Connect(id_menu_reverse,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_transform);
    Connect(id_menu_clear_log,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clear_log);
    Connect(id_menu_clear_term,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clear_term);
    Connect(id_menu_clear,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_clear_all);
    Connect(id_menu_alg_rnd,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_ab,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_pvs,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_trans,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_kill,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_mtdf,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_ids,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_ptn,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_alg_mpc,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_alg);
    Connect(id_menu_level1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_level10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::on_menu_level);
    Connect(id_menu_about,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&reversi_guiFrame::OnAbout);
    //*)

	Connect(id_book_tree,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,(wxObjectEventFunction)&reversi_guiFrame::on_tree_item_select);
	panel_board->Connect(wxEVT_CONTEXT_MENU,wxContextMenuEventHandler(reversi_guiFrame::on_context_menu),NULL,this); 

	ptr_frame = this;
	ptr_panel = panel_board;
	ptr_term = text_term;
	ptr_input = text_input;
	ptr_log = text_log;
	ptr_book = book_tree;
	mygame.flag_lock = true;
	flag_text_term = false;

	#ifdef DEBUG_SEARCH
		out.open("out.html");
	#endif
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
	#ifdef DEBUG_SEARCH
		out.close();
	#endif
}

void reversi_guiFrame::on_panel_board_paint(wxPaintEvent& event)
{
	wxClientDC dc(panel_board);
	do_show(dc);
}

void reversi_guiFrame::on_black(wxCommandEvent& event){
	process("start");
}

void reversi_guiFrame::on_white(wxCommandEvent& event){
	process("start");
	process("puts [play [get_method] $true 0]");
}

void reversi_guiFrame::on_undo(wxCommandEvent& event){
	process("undo");
}

void reversi_guiFrame::on_redo(wxCommandEvent& event){
	process("redo");
}

void reversi_guiFrame::on_load(wxCommandEvent& event){
	if(dialog_load->ShowModal() == wxID_OK){
		string path = dialog_load->GetPath().ToStdString();
		string::size_type pos = 0;
		while((pos = path.find("\\",pos + 2)) != path.npos){
			path.replace(pos,1,"\\\\");
		}
		process(string("load ") + path);
	}
}

void reversi_guiFrame::on_transform(wxCommandEvent& event){
	long id = event.GetId();
	if(id == id_menu_rotate_r){
		process("rotate_r");
	}else if(id == id_menu_rotate_l){
		process("rotate_l");
	}else if(id == id_menu_mirror_h){
		process("mirror_h");
	}else if(id == id_menu_mirror_v){
		process("mirror_v");
	}else if(id == id_menu_reflect){
		process("reflect");
	}else if(id == id_menu_reverse){
		process("reverse");
	}
}

void reversi_guiFrame::on_clear_log(wxCommandEvent& event){
	text_log->Clear();
}

void reversi_guiFrame::on_clear_term(wxCommandEvent& event){
	text_term->Clear();
}

void reversi_guiFrame::on_clear_all(wxCommandEvent& event){
	text_log->Clear();
	text_term->Clear();
}

void reversi_guiFrame::on_menu_alg(wxCommandEvent& event){
	long id = event.GetId();
	wxMenuItem* item = menu_alg->FindItem(id);

	if(id == id_menu_alg_rnd){
		if(menu_alg_rnd->IsChecked()){
			for(wxMenuItem* ptr:menu_alg->GetMenuItems()){
				if(ptr->IsCheckable()){
					ptr->Check(false);
				}
			}
			process("set_method $mthd_rnd");
		}
		menu_alg_rnd->Check(true);
	}else{
		if(item->IsChecked()){
			item->Check(true);
			menu_alg_rnd->Check(false);

			if(id == id_menu_alg_ab){
				process("set_method [expr [get_method] | $mthd_ab]");
			}else if(id == id_menu_alg_kill){
				process("set_method [expr [get_method] | $mthd_kill]");
			}else if(id == id_menu_alg_pvs){
				process("set_method [expr [get_method] | $mthd_pvs]");
			}else if(id == id_menu_alg_trans){
				process("set_method [expr [get_method] | $mthd_trans]");
			}else if(id == id_menu_alg_mtdf){
				process("set_method [expr [get_method] | $mthd_mtdf]");
			}else if(id == id_menu_alg_ids){
				process("set_method [expr [get_method] | $mthd_ids]");
			}else if(id == id_menu_alg_ptn){
				process("set_method [expr [get_method] | $mthd_ptn]");
			}else if(id == id_menu_alg_mpc){
				process("set_method [expr [get_method] | $mthd_mpc]");
			}
		}else{
			bool flag = false;
			for(wxMenuItem* ptr:menu_alg->GetMenuItems()){
				if(ptr->IsChecked()){
					flag = true;
				}
			}
			if(!flag){
				menu_alg_rnd->Check(true);
			}

			if(id == id_menu_alg_ab){
				process("set_method [expr [get_method] & ~ $mthd_ab]");
			}else if(id == id_menu_alg_kill){
				process("set_method [expr [get_method] & ~ $mthd_kill]");
			}else if(id == id_menu_alg_pvs){
				process("set_method [expr [get_method] & ~ $mthd_pvs]");
			}else if(id == id_menu_alg_trans){
				process("set_method [expr [get_method] & ~ $mthd_trans]");
			}else if(id == id_menu_alg_mtdf){
				process("set_method [expr [get_method] & ~ $mthd_mtdf]");
			}else if(id == id_menu_alg_ids){
				process("set_method [expr [get_method] & ~ $mthd_ids]");
			}else if(id == id_menu_alg_ptn){
				process("set_method [expr [get_method] & ~ $mthd_ptn]");
			}else if(id == id_menu_alg_mpc){
				process("set_method [expr [get_method] & ~ $mthd_mpc]");
			}
		}
	}
}

void reversi_guiFrame::on_menu_level(wxCommandEvent& event){
	for(wxMenuItem* ptr:menu_level->GetMenuItems()){
		ptr->Check(false);
	}

	int pos;
	wxMenuItem* item = menu_level->FindChildItem(event.GetId(),(size_t*)&pos);
	item->Check(true);
	if(pos == 7){
		pos = -1;
	}else if(pos == 8){
		pos = -2;
	}else if(pos == 9){
		pos = -3;
	}
	process((_("set_depth ") + to_string(pos)).ToStdString());
}

void reversi_guiFrame::on_panel_board_leftdown(wxMouseEvent& event)
{
	if(mygame.flag_lock){
		return;
	}
	wxPoint pos = event.GetPosition();
	int x = pos.x < bias ? -1 : (pos.x - bias) / cell;
	int y = pos.y < bias ? -1 : (pos.y - bias) / cell;
	process(
		(
			_("puts [plays ") + to_string(x) + " "
			+ to_string(y) + _(" [get_method] [get_depth]]")
		).ToStdString()
	);
}

void reversi_guiFrame::on_text_input_textenter(wxCommandEvent& event)
{
	process(text_input->GetValue().ToStdString());
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

void reversi_guiFrame::on_tree_item_select(wxTreeEvent& event){
	wxTreeItemId item = event.GetItem();
	vector<node*> vec;
	board brd;

	while(item != book_tree->GetRootItem()){
		vec.push_back(((myTreeItemData*)(book_tree->GetItemData(item)))->ptr);
		item = book_tree->GetItemParent(item);
	}
	reverse(vec.begin(),vec.end());
	if(!vec.empty()){
		mygame.color = vec.back()->dat.color;
	}else{
		mygame.color = true;
	}
	brd.initial();
	for(node *& ptr:vec){
		brd.flip(!ptr->dat.color,ptr->dat.pos);
	}
	mygame.brd = brd;
	mygame.show();
}