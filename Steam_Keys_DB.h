//---------------------------------------------------------------------------

#ifndef Steam_Keys_DBH
#define Steam_Keys_DBH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ADODB.hpp>
#include <DB.hpp>
#include <ExtCtrls.hpp>
#include <DBGrids.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Tabs.hpp>
#include <Vcl.WinXCalendars.hpp>
#include "SHDocVw_OCX.h"
#include <Vcl.OleCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Mask.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TADOConnection *ADOConnection;
	TDataSource *DataSourceDBGrid;
	TADOQuery *ADOQueryDBGrid;
	TADOQuery *ADOQueryListBox;
	TDBGrid *DBGridKeys_list;
	TGroupBox *Game_select;
	TListBox *GamesListBox;
	TADOQuery *ADOQuerySelect;
	TGroupBox *KeySelect;
	TADOQuery *ADOQueryDelete;
	TTimer *Timer;
	TADOQuery *ADOQueryUpdate;
	TGroupBox *SortingBox;
	TCheckBox *Multiselection;
	TMemo *Key_buffer;
	TButton *Copy_buffer;
	TCheckBox *Only_keys;
	TComboBox *SortBox;
	TBitBtn *Delete_key;
	TRadioGroup *SortGroup;
	TPageControl *PageControl1;
	TTabSheet *SKDB;
	TTabSheet *GaOpener;
	TListBox *Links_list;
	TButton *Add_link;
	TButton *Open_link;
	TButton *Add_group;
	TListBox *Groups_list;
	TButton *Open_groups;
	TGroupBox *Add_new_game;
	TLabel *Game_nameLabel;
	TLabel *Source_Label;
	TLabel *Label1;
	TComboBox *Game_name;
	TLabeledEdit *Key_link;
	TComboBox *Source;
	TCheckBox *Other;
	TCheckBox *Trading_cards;
	TCheckBox *DLC;
	TCheckBox *Already_used;
	TButton *Add_Key_Button;
	TDateTimePicker *Add_date;
	TMemo *Notes;
	TLabeledEdit *Number_keys;
	TButton *Update_key;
	TTabSet *TabSet1;
	TButton *Open_links;
	TButton *Open_group;
	TListBox *Browser;
	TRadioGroup *Device;
	TBitBtn *Delete_group;
	TBitBtn *Delete_link;
	TButton *Copy_from_clipboard;
	TButton *Form_StayOnTop;
	TButton *Form_Normal;
	TTrayIcon *TrayIcon;
	TButton *Form_hide;
	TComboBox *Activation_link;
	TCheckBox *BGR;
	TCheckBox *Trade;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall DBGridKeys_listCellClick(TColumn *Column);
	void __fastcall GamesListBoxClick(TObject *Sender);
	void __fastcall Delete_keyClick(TObject *Sender);
	void __fastcall Add_Key_ButtonClick(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall Update_keyClick(TObject *Sender);
	void __fastcall SortBoxChange(TObject *Sender);
	void __fastcall MultiselectionClick(TObject *Sender);
	void __fastcall Copy_bufferClick(TObject *Sender);
	void __fastcall Key_linkKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Game_nameKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall Key_linkClick(TObject *Sender);
	void __fastcall SortGroupClick(TObject *Sender);
	void __fastcall Open_linkClick(TObject *Sender);
	void __fastcall Add_groupClick(TObject *Sender);
	void __fastcall Open_groupsClick(TObject *Sender);
	void __fastcall Add_linkClick(TObject *Sender);
	void __fastcall Open_linksClick(TObject *Sender);
	void __fastcall Open_groupClick(TObject *Sender);
	void __fastcall DeviceClick(TObject *Sender);
	void __fastcall Delete_groupClick(TObject *Sender);
	void __fastcall Delete_linkClick(TObject *Sender);
	void __fastcall Copy_from_clipboardClick(TObject *Sender);
	void __fastcall Form_StayOnTopClick(TObject *Sender);
	void __fastcall Form_NormalClick(TObject *Sender);
	void __fastcall TrayIconClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall Form_hideClick(TObject *Sender);
	void __fastcall Activation_link_Change(TObject *Sender);
	void __fastcall Only_keysClick(TObject *Sender);
	void __fastcall BGRClick(TObject *Sender);
	void __fastcall TradeClick(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
