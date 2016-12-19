/***************************************************************
 * Name:      reversi_guiMain.h
 * Purpose:   Defines Application Frame
 * Author:    Rick (1300012743@pku.edu.cn)
 * Created:   2016-04-16
 * Copyright: Rick ()
 * License:
 **************************************************************/

#ifndef REVERSI_GUIMAIN_H
#define REVERSI_GUIMAIN_H

//(*Headers(reversi_guiFrame)
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class reversi_guiFrame: public wxFrame
{
    public:

        reversi_guiFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~reversi_guiFrame();

    private:

        //(*Handlers(reversi_guiFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnClose1(wxCloseEvent& event);
        void on_panel_board_paint(wxPaintEvent& event);
        void on_panel_board_leftdown(wxMouseEvent& event);
        void on_text_input_textenter(wxCommandEvent& event);
        //*)
		void on_black(wxCommandEvent& event);
		void on_white(wxCommandEvent& event);
		void on_undo(wxCommandEvent& event);
		void on_redo(wxCommandEvent& event);
		void on_load(wxCommandEvent& event);
		void on_clear_log(wxCommandEvent& event);
		void on_clear_term(wxCommandEvent& event);
		void on_clear_all(wxCommandEvent& event);
		void on_clockwise(wxCommandEvent& event);
		void on_counterclockwise(wxCommandEvent& event);
		void on_horizontal(wxCommandEvent& event);
		void on_vertical(wxCommandEvent& event);
		void on_reflect(wxCommandEvent& event);
		void on_context_menu(wxContextMenuEvent& event);

        //(*Identifiers(reversi_guiFrame)
        static const long id_panel_board;
        static const long id_text_label;
        static const long id_text_terminal;
        static const long ID_NOTEBOOK1;
        static const long id_text_input;
        static const long ID_PANEL1;
        static const long id_player_black;
        static const long id_player_white;
        static const long id_new_game;
        static const long id_load;
        static const long id_save;
        static const long id_quit;
        static const long id_undo;
        static const long id_redo;
        static const long id_horizontal;
        static const long id_vertical;
        static const long id_reflect;
        static const long id_clockwise;
        static const long id_counterclockwise;
        static const long ID_MENUITEM7;
        static const long id_clear_log;
        static const long id_clear_term;
        static const long id_clear_all;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM3;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM5;
        static const long ID_MENUITEM6;
        static const long id_algorithm;
        static const long id_level;
        static const long id_about;
        static const long ID_STATUSBAR1;
        //*)

        static const long id_text_log;

        //(*Declarations(reversi_guiFrame)
        wxMenuItem* MenuItem8;
        wxMenuItem* MenuItem26;
        wxMenuItem* MenuItem7;
        wxNotebook* Notebook1;
        wxMenuItem* MenuItem5;
        wxFileDialog* dialog_load;
        wxMenu* Menu3;
        wxMenu* MenuItem3;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem14;
        wxTextCtrl* text_input;
        wxMenuItem* MenuItem11;
        wxMenuItem* MenuItem15;
        wxMenuItem* MenuItem22;
        wxPanel* Panel1;
        wxMenuItem* MenuItem17;
        wxMenuItem* MenuItem13;
        wxMenuItem* MenuItem10;
        wxMenuItem* MenuItem24;
        wxStaticText* text_label;
        wxMenuItem* MenuItem20;
        wxMenu* MenuItem12;
        wxStatusBar* StatusBar1;
        wxMenuItem* MenuItem6;
        wxMenuItem* MenuItem23;
        wxTextCtrl* text_terminal;
        wxMenu* MenuItem25;
        wxPanel* panel_board;
        wxMenuItem* MenuItem21;
        wxMenuItem* MenuItem16;
        wxMenuItem* MenuItem9;
        wxMenu* MenuItem18;
        wxMenu* Menu4;
        wxMenuItem* MenuItem19;
        //*)

        wxTextCtrl* text_log;

        DECLARE_EVENT_TABLE()
};
 
#endif // REVERSI_GUIMAIN_H
