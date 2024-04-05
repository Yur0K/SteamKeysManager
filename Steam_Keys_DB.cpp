// Author: Yuriy Tyutyunnik
// Date: 2/5/2018


#include <vcl.h>
#include <conio.h>
#include <stdio.h>
#include <Clipbrd.hpp>
#include "Steam_Keys_DB.h"
#pragma hdrstop
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma resource "*.dfm"
TMainForm *MainForm;
int count_record;
AnsiString TextSQL, TextDelete, TextSelect, TextUpdate, Key_cache, TextSQLList;
AnsiString URL, BOT, Path;

__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TMainForm::FormShow(TObject *Sender)
{
    try
	{
		MainForm->Caption="Steam Keys Database";
		// Defining ConnectionString to SteamDB.mdb database
		ADOConnection->ConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Password="";Data Source=SteamDB.mdb;Persist Security Info=True";
		// Starting ADOConnction
		ADOConnection->Connected="true";
		// ADOQuerySelect query using ADOConnection connection
		ADOQuerySelect->Connection=ADOConnection;
		// Selecting all rows from Keys table
		TextSQL="SELECT * FROM Keys";
		ADOQuerySelect->SQL->Clear();
		ADOQuerySelect->SQL->Add(TextSQL);
		ADOQuerySelect->Active=true;
		// Adjusting iterface elements
		Delete_key->Enabled=false;
		Add_new_game->Visible=true;
		Key_buffer->Visible=false;
		Copy_buffer->Enabled=false;
		Only_keys->Visible=false;
		Only_keys->Checked=false;
		BGR->Visible=false;
		BGR->Checked=false;
		Trade->Visible=false;
		Trade->Checked=false;
		Multiselection->Left=548;
		Multiselection->Top=23;
		Key_link->MaxLength=71;
		Game_name->MaxLength=60;
		Source->MaxLength=25;
		SortBox->ItemIndex=0;
		Device->ItemIndex = 0;
		SKDB->Show();

		// Filling up a list of browsers
		Browser->Items->Clear();
		Activation_link->Text="Open activation link";
		TSearchRec Rec;
		Path = ExtractFileDir(Application->ExeName);

			// Filling up a list of devices
		switch (Device->ItemIndex)
		{
			case 0:
			Path = Path + "\\PC\\";
			break;

			case 1:
			Path = Path + "\\Notebook\\";
			break;

			default:
				;
		}

			// Filling up a list of browsers
		if(FindFirst(Path+"\\*.lnk", faAnyFile , Rec) == 0)
		{
			do
			{
				Browser->Items->Add(Rec.Name.SubString(1, Rec.Name.Length() - 4 ));
				Activation_link->Items->Add(Rec.Name.SubString(1, Rec.Name.Length() - 4 ));
			}
				while(FindNext(Rec) == 0);
		}
		FindClose(Rec);

		// Disable some interface items and fill up test fields with messages if database is empty
		if (ADOQuerySelect->RecordCount==0)
		{
			Number_keys->Text="0";
			Add_date->DateTime.CurrentDateTime();
			Key_link->Text="Enter a key or Indiegala link";
			Game_name->Text="Enter a name of the game";
			Source->Text="Enter source";
			Notes->Text="You can write down some notes here. Delete this text for now :)";

			KeySelect->Visible=false;
			Game_select->Visible=false;
			SortingBox->Visible=false;
			Update_key->Enabled=false;
			MainForm->Height=245;
			MainForm->Width=663;
			GaOpener->TabVisible=false;
		}
		// Selecting all keys and filling up lists sorted by date if DB is not empty
		else
		{
			KeySelect->Visible=true;
			Game_select->Visible=true;
			SortingBox->Visible=true;
			Update_key->Enabled=true;
			MainForm->Height=565;
			MainForm->Width=1138;
			GaOpener->TabVisible=true;

			TextSQL="SELECT * FROM Keys ORDER BY Add_date ASC";
			ADOQuerySelect->SQL->Clear();
			ADOQuerySelect->SQL->Add(TextSQL);
			ADOQuerySelect->Active=true;
			ADOQuerySelect->FindLast();
			Add_date->DateTime=ADOQuerySelect->FieldByName("Add_date")->AsString;
			Key_link->Text=ADOQuerySelect->FieldByName("Key_link")->AsString;
			Game_name->Text=ADOQuerySelect->FieldByName("Game_name")->AsString;
			Source->Text=ADOQuerySelect->FieldByName("Source")->AsString;
			Notes->Text=ADOQuerySelect->FieldByName("Notes")->AsString;
				if(ADOQuerySelect->FieldByName("Trading_cards")->AsString=="True")
				{
					Trading_cards->Checked=true;
				}
				else
				{
					Trading_cards->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("DLC")->AsString=="True")
				{
					DLC->Checked=true;
				}
				else
				{
					DLC->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("Other")->AsString=="True")
				{
					Other->Checked=true;
				}
				else
				{
					Other->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("Already_used")->AsString=="True")
				{
					Already_used->Checked=true;
				}
				else
				{
					Already_used->Checked=false;
				}
			// Filling up table with list of keys
			TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
			TextSQL+=Game_name->Text;
			TextSQL+="' ORDER BY Add_date ASC";
			ADOQueryDBGrid->SQL->Clear();
			ADOQueryDBGrid->SQL->Add(TextSQL);
			ADOQueryDBGrid->Active=true;
			Number_keys->Text=ADOQueryDBGrid->RecordCount;
			// Filling up list of key's sources
			TextSQL="SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
			ADOQueryListBox->SQL->Clear();
			ADOQueryListBox->SQL->Add(TextSQL);
			ADOQueryListBox->Active=true;
			Source->Items->Clear();
				for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
				{
					Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
					ADOQueryListBox->FindNext();
				}
			// Filling up games list
			TextSQL="SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
			ADOQueryListBox->SQL->Clear();
			ADOQueryListBox->SQL->Add(TextSQL);
			ADOQueryListBox->Active=true;
			GamesListBox->Items->Clear();
			Game_name->Items->Clear();
				for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
				{
					GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
					Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
					ADOQueryListBox->FindNext();
				}
			// Set selected item in games list corresponding to game name field
			int index = GamesListBox->Items->IndexOf(Game_name->Text);
			if (index >= 0)
			{
				GamesListBox->ItemIndex = index;
			}
		Key_cache=Key_link->Text.Trim();
		}
	}
    catch(...)
	{
		ShowMessage(L"Database file SteamDB.mdb is not found!");
		MainForm->Close();
	}
}

void __fastcall TMainForm::DBGridKeys_listCellClick(TColumn *Column)
{
	TextSelect="SELECT * FROM Keys WHERE Key_link='";
	TextSelect+=DBGridKeys_list->SelectedField->AsString;
	TextSelect+=" ' ";
	ADOQuerySelect->SQL->Clear();
	ADOQuerySelect->SQL->Add(TextSelect);
	ADOQuerySelect->Active=true;
	Add_date->DateTime=ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
	Key_link->Text=ADOQuerySelect->FieldByName("Key_link")->AsString;
	Game_name->Text=ADOQuerySelect->FieldByName("Game_name")->AsString;
	Source->Text=ADOQuerySelect->FieldByName("Source")->AsString;
	Notes->Text=ADOQuerySelect->FieldByName("Notes")->AsString;

		if(ADOQuerySelect->FieldByName("Trading_cards")->AsString=="True")
		{
			Trading_cards->Checked=true;
		}
		else
		{
			Trading_cards->Checked=false;
		}

		if(ADOQuerySelect->FieldByName("DLC")->AsString=="True")
		{
			DLC->Checked=true;
		}
		else
		{
			DLC->Checked=false;
		}

		if(ADOQuerySelect->FieldByName("Other")->AsString=="True")
		{
			Other->Checked=true;
		}
		else
		{
			Other->Checked=false;
		}

		if(ADOQuerySelect->FieldByName("Already_used")->AsString=="True")
		{
			Already_used->Checked=true;
		}
		else
		{
			Already_used->Checked=false;
		}

		if (Multiselection->Checked==false)
		{
			Clipboard()->AsText = ADOQuerySelect->FieldByName("Game_name")->AsString + " - " + ADOQuerySelect->FieldByName("Key_link")->AsString;
		}
		else
		{
			if (Only_keys->Checked==true)
			{
				if (Key_buffer->Lines->Text.IsEmpty())
				{
					Key_buffer->Lines->Add("!redeem " + ADOQuerySelect->FieldByName("Key_link")->AsString);
				}
				else
				{
					Key_buffer->Lines->Text=Key_buffer->Lines->Text.Trim() + "," + ADOQuerySelect->FieldByName("Key_link")->AsString;
				}
			}
			if (BGR->Checked==true)
			{
				Key_buffer->Lines->Add(ADOQuerySelect->FieldByName("Game_name")->AsString + "\t" + ADOQuerySelect->FieldByName("Key_link")->AsString);
			}
			if (Trade->Checked==true)
			{
			Key_buffer->Lines->Add(ADOQuerySelect->FieldByName("Game_name")->AsString + " - " + ADOQuerySelect->FieldByName("Key_link")->AsString);
			}
		}

	TextSelect="SELECT * FROM Keys WHERE Game_name='";
	TextSelect+=Game_name->Text;
	TextSelect+=" ' ";
	ADOQuerySelect->SQL->Clear();
	ADOQuerySelect->SQL->Add(TextSelect);
	ADOQuerySelect->Active=true;
	// Set selected item in games list corresponding to game name field
	int index = GamesListBox->Items->IndexOf(Game_name->Text);
	if (index >= 0)
	{
		GamesListBox->ItemIndex = index;
	}
	Number_keys->Text=ADOQuerySelect->RecordCount;
	Delete_key->Caption="Delete key";
	Delete_key->Enabled=true;
	if (Multiselection->Checked==true)
	{
		Copy_buffer->Enabled=true;
	}
	Key_cache=Key_link->Text.Trim();
	if (Game_name->Text.IsEmpty())
	{
		Add_date->DateTime.CurrentDateTime();
		Key_link->Text="Click on a key or link";
		Game_name->Text="Click on a key or link";
	}
}

void __fastcall TMainForm::GamesListBoxClick(TObject *Sender)
{
	TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
	TextSQL+=GamesListBox->Items->Strings[GamesListBox->ItemIndex];
	TextSQL+="' ORDER BY Add_date";
	ADOQueryDBGrid->SQL->Clear();
	ADOQueryDBGrid->SQL->Add(TextSQL);
	ADOQueryDBGrid->Active=true;
	Number_keys->Text=ADOQueryDBGrid->RecordCount;

	TextSelect="SELECT * FROM Keys WHERE Game_name='";
	TextSelect+=GamesListBox->Items->Strings[GamesListBox->ItemIndex];
	TextSelect+=" ' ORDER BY Add_date";
	ADOQuerySelect->SQL->Clear();
	ADOQuerySelect->SQL->Add(TextSelect);
	ADOQuerySelect->Active=true;
	ADOQuerySelect->FindFirst();
	Add_date->DateTime=ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
	Key_link->Text=ADOQuerySelect->FieldByName("Key_link")->AsString;
	Game_name->Text=ADOQuerySelect->FieldByName("Game_name")->AsString;
	Source->Text=ADOQuerySelect->FieldByName("Source")->AsString;
	Notes->Text=ADOQuerySelect->FieldByName("Notes")->AsString;

	if(ADOQuerySelect->FieldByName("Trading_cards")->AsString=="True")
	{
		Trading_cards->Checked=true;
	}
	else
	{
		Trading_cards->Checked=false;
	}

	if(ADOQuerySelect->FieldByName("DLC")->AsString=="True")
	{
		DLC->Checked=true;
	}
	else
	{
		DLC->Checked=false;
	}

	if(ADOQuerySelect->FieldByName("Other")->AsString=="True")
	{
		Other->Checked=true;
	}
	else
	{
		Other->Checked=false;
	}

	if(ADOQuerySelect->FieldByName("Already_used")->AsString=="True")
	{
		Already_used->Checked=true;
	}
	else
	{
		Already_used->Checked=false;
	}

	Delete_key->Enabled=false;
	Key_cache=Key_link->Text.Trim();
}

void __fastcall TMainForm::Delete_keyClick(TObject *Sender)
{
	try
	{
		TextDelete="DELETE FROM Keys WHERE Key_link = '";
		TextDelete+=DBGridKeys_list->SelectedField->AsString;
		TextDelete+="'";
		ADOQueryDelete->Active=false;
		ADOQueryDelete->SQL->Clear();
		ADOQueryDelete->SQL->Add(TextDelete);
		ADOQueryDelete->ExecSQL();

		Timer->Enabled=true;
		KeySelect->Color=clLime;
		Delete_key->Caption="KEY DELETED";
		Delete_key->Enabled=false;

		TextSQL="SELECT * FROM Keys";
		ADOQuerySelect->SQL->Clear();
		ADOQuerySelect->SQL->Add(TextSQL);
		ADOQuerySelect->Active=true;

		if (ADOQuerySelect->RecordCount!=0)
		{
			TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
			TextSQL+=Game_name->Text;
			TextSQL+="' ORDER BY Add_date";
			ADOQueryDBGrid->SQL->Clear();
			ADOQueryDBGrid->SQL->Add(TextSQL);
			ADOQueryDBGrid->Active=true;
			Number_keys->Text=ADOQueryDBGrid->RecordCount;
			if (Number_keys->Text=="0")
			{
				ADOQueryDBGrid->Active=false;
				TextSQL="SELECT * FROM Keys";
				ADOQuerySelect->SQL->Clear();
				ADOQuerySelect->SQL->Add(TextSQL);
				ADOQuerySelect->Active=true;
				ADOQuerySelect->FindLast();
				Add_date->DateTime=ADOQuerySelect->FieldByName("Add_date")->AsString;
				Key_link->Text=ADOQuerySelect->FieldByName("Key_link")->AsString;
				Game_name->Text=ADOQuerySelect->FieldByName("Game_name")->AsString;
				Source->Text=ADOQuerySelect->FieldByName("Source")->AsString;
				Notes->Text=ADOQuerySelect->FieldByName("Notes")->AsString;
				Number_keys->Text=ADOQuerySelect->RecordCount;

				if(ADOQuerySelect->FieldByName("Trading_cards")->AsString=="True")
				{
					Trading_cards->Checked=true;
				}
				else
				{
					Trading_cards->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("DLC")->AsString=="True")
				{
					DLC->Checked=true;
				}
				else
				{
					DLC->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("Other")->AsString=="True")
				{
					Other->Checked=true;
				}
				else
				{
					Other->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("Already_used")->AsString=="True")
				{
					Already_used->Checked=true;
				}
				else
				{
					Already_used->Checked=false;
				}

				TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
				TextSQL+=Game_name->Text;
				TextSQL+="' ORDER BY Add_date";
				ADOQueryDBGrid->SQL->Clear();
				ADOQueryDBGrid->SQL->Add(TextSQL);
				ADOQueryDBGrid->Active=true;
				Number_keys->Text=ADOQueryDBGrid->RecordCount;

				TextSQL="SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
				ADOQueryListBox->SQL->Clear();
				ADOQueryListBox->SQL->Add(TextSQL);
				ADOQueryListBox->Active=true;
				Source->Items->Clear();

				for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
				{
					Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
					ADOQueryListBox->FindNext();
				}

				TextSQL="SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
				ADOQueryListBox->SQL->Clear();
				ADOQueryListBox->SQL->Add(TextSQL);
				ADOQueryListBox->Active=true;
				GamesListBox->Items->Clear();
				Game_name->Items->Clear();

				for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
				{
					GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
					Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
					ADOQueryListBox->FindNext();
				}
				// Set selected item in games list corresponding to game name field
				int index = GamesListBox->Items->IndexOf(Game_name->Text);
				if (index >= 0)
				{
					GamesListBox->ItemIndex = index;
				}
			}
			else
			{
				// Set selected item in games list corresponding to game name field
				int index = GamesListBox->Items->IndexOf(Game_name->Text);
				if (index >= 0)
				{
					GamesListBox->ItemIndex = index;
				}

				TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
				TextSQL+=GamesListBox->Items->Strings[GamesListBox->ItemIndex];
				TextSQL+="' ORDER BY Add_date";
				ADOQueryDBGrid->SQL->Clear();
				ADOQueryDBGrid->SQL->Add(TextSQL);
				ADOQueryDBGrid->Active=true;
				Number_keys->Text=ADOQueryDBGrid->RecordCount;

				TextSelect="SELECT * FROM Keys WHERE Game_name='";
				TextSelect+=GamesListBox->Items->Strings[GamesListBox->ItemIndex];
				TextSelect+=" ' ORDER BY Add_date";
				ADOQuerySelect->SQL->Clear();
				ADOQuerySelect->SQL->Add(TextSelect);
				ADOQuerySelect->Active=true;
				ADOQuerySelect->FindFirst();
				Add_date->DateTime=ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
				Key_link->Text=ADOQuerySelect->FieldByName("Key_link")->AsString;
				Game_name->Text=ADOQuerySelect->FieldByName("Game_name")->AsString;
				Source->Text=ADOQuerySelect->FieldByName("Source")->AsString;
				Notes->Text=ADOQuerySelect->FieldByName("Notes")->AsString;

				if(ADOQuerySelect->FieldByName("Trading_cards")->AsString=="True")
				{
					Trading_cards->Checked=true;
				}
				else
				{
					Trading_cards->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("DLC")->AsString=="True")
				{
					DLC->Checked=true;
				}
				else
				{
					DLC->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("Other")->AsString=="True")
				{
					Other->Checked=true;
				}
				else
				{
					Other->Checked=false;
				}

				if(ADOQuerySelect->FieldByName("Already_used")->AsString=="True")
				{
					Already_used->Checked=true; }
				else
				{
					Already_used->Checked=false;
				}

				Delete_key->Enabled=false;
				Key_cache=Key_link->Text.Trim();
			}
		}
		else
		{
            		Number_keys->Text="0";
			Add_date->DateTime.CurrentDateTime();
			Key_link->Text="Enter a key or Indiegala link";
			Game_name->Text="Enter a name of the game";
			Source->Text="Enter source";
			Notes->Text="You can write down some notes here. Delete this text for now :)";

			KeySelect->Visible=false;
			Game_select->Visible=false;
			SortingBox->Visible=false;
			Update_key->Enabled=false;
			MainForm->Height=245;
			MainForm->Width=663;
			GaOpener->TabVisible=false;
        	}
	}
	catch(...)
	{
		KeySelect->Color=clRed;
		Delete_key->Caption="ERROR";
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Add_Key_ButtonClick(TObject *Sender)
{
	if(Key_link->Text.IsEmpty()||Game_name->Text.IsEmpty()||Source->Text.IsEmpty())
	{
		Add_Key_Button->Caption="SOME FIELDS ARE EMPTY";
		Add_new_game->Color=clYellow;
		Timer->Enabled=true;
	}

	if (Key_link->Text=="Click on a key or link")
	{
		Add_Key_Button->Caption="Check your key or link ;)";
		Add_new_game->Color=clYellow;
		Key_link->Color=clRed;
		Add_date->Date.CurrentDate();
		Add_date->Time.CurrentTime();
		Timer->Enabled=true;
	}
	else
	{
		ADOQuerySelect->SQL->Clear();
		AnsiString TextSQL;
		TextSQL="INSERT INTO Keys(Add_date, Key_link, Game_name, Source, Trading_cards, DLC, Other, Already_used, Notes)";
		TextSQL+="values(:Add_date, :Key_link, :Game_name, :Source, :Trading_cards, :DLC, :Other, :Already_used, :Notes)";
		ADOQuerySelect->SQL->Add(TextSQL);
		ADOQuerySelect->Parameters->Items[0]->Value=Add_date->DateTime.CurrentDateTime();
		ADOQuerySelect->Parameters->Items[1]->Value=Key_link->Text.Trim();
		ADOQuerySelect->Parameters->Items[2]->Value=Game_name->Text.Trim();
		ADOQuerySelect->Parameters->Items[3]->Value=Source->Text.Trim();
		ADOQuerySelect->Parameters->Items[8]->Value=Notes->Text.Trim();

		if(Trading_cards->Checked==true)
		{
			ADOQuerySelect->Parameters->Items[4]->Value=1;
		}
		else
		{
			ADOQuerySelect->Parameters->Items[4]->Value=0;
		}

		if(DLC->Checked==true)
		{
			ADOQuerySelect->Parameters->Items[5]->Value=1;
		}
		else
		{
			ADOQuerySelect->Parameters->Items[5]->Value=0;
		}

		if(Other->Checked==true)
		{
			ADOQuerySelect->Parameters->Items[6]->Value=1;
		}
		else
		{
			ADOQuerySelect->Parameters->Items[6]->Value=0;
		}

		if(Already_used->Checked==true)
		{
			ADOQuerySelect->Parameters->Items[7]->Value=1;
		}
		else
		{
			ADOQuerySelect->Parameters->Items[7]->Value=0;
		}

		try
		{
			ADOQuerySelect->ExecSQL();
			ADOQuerySelect->SQL->Clear();

			Add_Key_Button->Caption="KEY ADDED";
			Add_new_game->Color=clLime;
			Timer->Enabled=true;
			Key_link->Clear();
			Delete_key->Enabled=false;

			TextSQL="SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
			ADOQueryListBox->SQL->Clear();
			ADOQueryListBox->SQL->Add(TextSQL);
			ADOQueryListBox->Active=true;
			Source->Items->Clear();

			for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
			{
				Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
				ADOQueryListBox->FindNext();
			}

			TextSQL="SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
			ADOQueryListBox->SQL->Clear();
			ADOQueryListBox->SQL->Add(TextSQL);
			ADOQueryListBox->Active=true;
			GamesListBox->Items->Clear();
			Game_name->Items->Clear();

			for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
			{
				GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
				Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
				ADOQueryListBox->FindNext();
			}

			// Set selected item in games list corresponding to game name field
			int index = GamesListBox->Items->IndexOf(Game_name->Text);
			if (index >= 0)
			{
				GamesListBox->ItemIndex = index;
			}

			KeySelect->Visible=true;
			Game_select->Visible=true;
			SortingBox->Visible=true;
			Update_key->Enabled=true;
			MainForm->Height=555;
			MainForm->Width=1123;
			GaOpener->TabVisible=true;
		}

		catch(...)
		{
			Add_Key_Button->Caption="ERROR";
			Add_new_game->Color=clRed;
			Timer->Enabled=true;
		}
	}

	TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
	TextSQL+=Game_name->Text;
	TextSQL+="' ORDER BY Add_date";
	ADOQueryDBGrid->SQL->Clear();
	ADOQueryDBGrid->SQL->Add(TextSQL);
	ADOQueryDBGrid->Active=true;
	Number_keys->Text=ADOQueryDBGrid->RecordCount;
	Key_cache=Key_link->Text.Trim();
}

void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
	Add_Key_Button->Caption="Add key";
	Copy_buffer->Caption="Copy";
	Add_new_game->Color=clWhite;
	KeySelect->Color=clWhite;
	SortingBox->Color=clWhite;
	Key_link->Color=clWindow;
	Delete_key->Caption="Delete key";
	Update_key->Caption="Update info";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Update_keyClick(TObject *Sender)
{
	Delete_key->Enabled=false;

	if(Key_link->Text.IsEmpty()||Game_name->Text.IsEmpty()||Source->Text.IsEmpty())
	{
		Update_key->Caption="SOME FIELDS ARE EMPTY";
		Add_new_game->Color=clYellow;
		Timer->Enabled=true;
	}
	else
	{
		TextUpdate="UPDATE Keys SET Add_date='";
		TextUpdate+=Add_date->DateTime.DateTimeString();
		TextUpdate+="', Key_link='";
		TextUpdate+=Key_link->Text.Trim();
		TextUpdate+="', Game_name='";
		TextUpdate+=Game_name->Text.Trim();
		TextUpdate+="', Source='";
		TextUpdate+=Source->Text.Trim();
		TextUpdate+="', Notes='";
		TextUpdate+=Notes->Text.Trim();
		TextUpdate+="', Trading_cards='";
		if(Trading_cards->Checked==true)
		{
			TextUpdate+=1;
		}
		else
		{
			TextUpdate+=0;
		}
		TextUpdate+="', DLC='";
		if(DLC->Checked==true)
		{
			TextUpdate+=1;
		}
		else
		{
			TextUpdate+=0;
		}
		TextUpdate+="', Other='";
		if(Other->Checked==true)
		{
			TextUpdate+=1;
		}
		else
		{
			TextUpdate+=0;
		}
		TextUpdate+="', Already_used='";
		if(Already_used->Checked==true)
		{
			TextUpdate+=1;
		}
		else
		{
			TextUpdate+=0;
		}
		TextUpdate+="'";
		TextUpdate+=" WHERE Key_link='";
		TextUpdate+=Key_cache;
		TextUpdate+=" ' ";
			try
			{
				ADOQueryUpdate->Active=false;
				ADOQueryUpdate->SQL->Clear();
				ADOQueryUpdate->SQL->Add(TextUpdate);
				ADOQueryUpdate->ExecSQL();

				Update_key->Caption="INFO UPDATED";
				Add_new_game->Color=clLime;
				Timer->Enabled=true;

				GamesListBox->Items->Clear();
				Source->Items->Clear();
				Game_name->Items->Clear();

				TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
				TextSQL+=Game_name->Text;
				TextSQL+="' ORDER BY Add_date ASC";
				ADOQueryDBGrid->SQL->Clear();
				ADOQueryDBGrid->SQL->Add(TextSQL);
				ADOQueryDBGrid->Active=true;
				Number_keys->Text=ADOQueryDBGrid->RecordCount;

				TextSQL="SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
				ADOQueryListBox->SQL->Clear();
				ADOQueryListBox->SQL->Add(TextSQL);
				ADOQueryListBox->Active=true;
				Source->Items->Clear();

				for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
				{
					Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
					ADOQueryListBox->FindNext();
				}

				TextSQL="SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
				ADOQueryListBox->SQL->Clear();
				ADOQueryListBox->SQL->Add(TextSQL);
				ADOQueryListBox->Active=true;
				GamesListBox->Items->Clear();
				Game_name->Items->Clear();

				for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
				{
					GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
					Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
					ADOQueryListBox->FindNext();
				}

				// Set selected item in games list corresponding to game name field
			   	int index = GamesListBox->Items->IndexOf(Game_name->Text);
			   	if (index >= 0)
			   	{
					GamesListBox->ItemIndex = index;
			   	}
			}
		catch (...)
		{
			Update_key->Caption="ERROR";
			Add_new_game->Color=clRed;
			Timer->Enabled=true;
		}
	}
	Key_cache=Key_link->Text.Trim();
}

void __fastcall TMainForm::SortBoxChange(TObject *Sender)
{
	switch (SortBox->ItemIndex)
	{
		case 0:
		TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys ORDER BY Add_date ASC, Game_name ASC";
		TextSQLList="SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name ASC";
		break;

		case 1:
		TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys ORDER BY Add_date DESC, Game_name DESC";
		TextSQLList="SELECT Game_name FROM Keys GROUP BY Game_name, Add_date ORDER BY Add_date DESC";
		break;

		case 2:
		TextSQL="SELECT Count(*) AS [Quantity],  Game_name AS [Game name] FROM Keys WHERE Game_name=Game_name GROUP BY Game_name ORDER BY Count(*) ASC";
		TextSQLList="SELECT Game_name FROM Keys WHERE Game_name=Game_name GROUP BY Game_name ORDER BY Count(*) ASC";
		break;

		case 3:
		TextSQL="SELECT Count(*) AS [Quantity],  Game_name AS [Game name] FROM Keys WHERE Game_name=Game_name GROUP BY Game_name ORDER BY Count(*) DESC";
		TextSQLList="SELECT Game_name FROM Keys WHERE Game_name=Game_name GROUP BY Game_name ORDER BY Count(*) DESC";
		break;

		case 4:
		TextSQL="SELECT Add_date, Key_link, Game_name, Source FROM Keys ORDER BY Source ASC";
		TextSQLList="SELECT Game_name FROM Keys GROUP BY Game_name,Source ORDER BY Source ASC";
		break;

		case 5:
		TextSQL="SELECT Add_date, Key_link, Game_name, Source FROM Keys ORDER BY Source DESC, Game_name DESC";
		TextSQLList="SELECT Game_name FROM Keys GROUP BY Game_name,Source ORDER BY Source DESC, Game_name DESC";
		break;

		case 6:
		TextSQL="SELECT Count(*) AS [Quantity],  Game_name AS [Game name] FROM Keys WHERE Game_name=Game_name GROUP BY Game_name ORDER BY Game_name ASC";
		break;

		case 7:
		TextSQL="SELECT Count(*) AS [Quantity],  Game_name AS [Game name] FROM Keys WHERE Game_name=Game_name GROUP BY Game_name ORDER BY Game_name DESC";
		break;

		default:
			;
	}

	ADOQueryDBGrid->SQL->Clear();
	ADOQueryDBGrid->SQL->Add(TextSQL);
	ADOQueryDBGrid->Active=true;

	ADOQueryListBox->SQL->Clear();
	ADOQueryListBox->SQL->Add(TextSQLList);
	ADOQueryListBox->Active=true;

	GamesListBox->Items->Clear();

	for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
	{
		GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
		ADOQueryListBox->FindNext();
	}
	// Set selected item in games list corresponding to game name field
	int index = GamesListBox->Items->IndexOf(Game_name->Text);
	if (index >= 0)
	{
		GamesListBox->ItemIndex = index;
	}

	Number_keys->Text=ADOQueryDBGrid->RecordCount;
	Delete_key->Enabled=false;
	Key_cache=Key_link->Text.Trim();
}

void __fastcall TMainForm::MultiselectionClick(TObject *Sender)
{
	if (Multiselection->Checked==true)
	{
		Add_new_game->Visible=false;
		Key_buffer->Lines->Clear();
		Key_buffer->Visible=true;
		Key_buffer->Left=0;
		Key_buffer->Top=1;
		Key_buffer->Height=182;
		Key_buffer->Width=645;
		Only_keys->Visible=true;
		BGR->Visible=true;
		Trade->Visible=true;
		Multiselection->Top=36;
		Only_keys->Top=13;
		Only_keys->Left=548;
	}
	else
	{
		Add_new_game->Visible=true;
		Key_buffer->Visible=false;
		Copy_buffer->Enabled=false;
		Only_keys->Visible=false;
		Only_keys->Checked=false;
		BGR->Visible=false;
		BGR->Checked=false;
		Trade->Visible=false;
		Trade->Checked=false;
		Multiselection->Top=23;
	}
	Delete_key->Enabled=false;
}

void __fastcall TMainForm::Copy_bufferClick(TObject *Sender)
{
	if (Key_buffer->Text.IsEmpty())
	{
		Copy_buffer->Caption="Empty";
		Timer->Enabled=true;
	}
	else
	{
		SortingBox->Color=clLime;
		Copy_buffer->Caption="Copied";
		Timer->Enabled=true;
		Clipboard()->AsText = Key_buffer->Text.Trim();
	}
}

void __fastcall TMainForm::Key_linkKeyPress(TObject *Sender, wchar_t &Key)
{
	if(Key==13)
	{
		Add_Key_ButtonClick(Sender);
	}
}

void __fastcall TMainForm::Game_nameKeyPress(TObject *Sender, wchar_t &Key)
{
	if(Key==13)
	{
		Add_Key_ButtonClick(Sender);
	}
}

void __fastcall TMainForm::Key_linkClick(TObject *Sender)
{
	if (Key_link->Text.IsEmpty())
	{
		Key_link->Text=Clipboard()->AsText;
	}
	else
	{
		Key_link->SelectAll();
		Key_link->SetFocus();
	}
}

void __fastcall TMainForm::SortGroupClick(TObject *Sender)
{
	switch (SortGroup->ItemIndex)
	{
		case 0:
		TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys WHERE Trading_cards= True ORDER BY Add_date ASC";
		TextSQLList="SELECT Game_name FROM Keys WHERE Trading_cards = True GROUP BY Game_name";
		TextSelect="SELECT * FROM Keys WHERE Trading_cards = True ORDER BY Add_date ASC";
		break;

		case 1:
		TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys WHERE DLC = True ORDER BY Add_date ASC";
		TextSQLList="SELECT Game_name FROM Keys WHERE DLC = True GROUP BY Game_name";
		TextSelect="SELECT * FROM Keys WHERE DLC = True ORDER BY Add_date ASC";
		break;

		case 2:
		TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys WHERE Other = True ORDER BY Add_date ASC";
		TextSQLList="SELECT Game_name FROM Keys WHERE Other = True GROUP BY Game_name";
		TextSelect="SELECT * FROM Keys WHERE Other = True ORDER BY Add_date ASC";
		break;

		case 3:
		TextSQL="SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys WHERE Already_used = True ORDER BY Add_date ASC";
		TextSQLList="SELECT Game_name FROM Keys WHERE Already_used = True GROUP BY Game_name";
		TextSelect="SELECT * FROM Keys WHERE Already_used = True ORDER BY Add_date ASC";
		break;

		default:
			;
	}

	ADOQuerySelect->SQL->Clear();
	ADOQuerySelect->SQL->Add(TextSelect);
	ADOQuerySelect->Active=true;
	ADOQuerySelect->FindFirst();
	Add_date->DateTime=ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
	Key_link->Text=ADOQuerySelect->FieldByName("Key_link")->AsString;
	Game_name->Text=ADOQuerySelect->FieldByName("Game_name")->AsString;
	Source->Text=ADOQuerySelect->FieldByName("Source")->AsString;
	Notes->Text=ADOQuerySelect->FieldByName("Notes")->AsString;
	if(ADOQuerySelect->FieldByName("Trading_cards")->AsString=="True")
	{
		Trading_cards->Checked=true;
	}
	else
	{
		Trading_cards->Checked=false;
	}

	if(ADOQuerySelect->FieldByName("DLC")->AsString=="True")
	{
		DLC->Checked=true; }
	else
	{
		DLC->Checked=false;
	}

	if(ADOQuerySelect->FieldByName("Other")->AsString=="True")
	{
		Other->Checked=true;
	}
	else
	{
		Other->Checked=false;
	}

	if(ADOQuerySelect->FieldByName("Already_used")->AsString=="True")
	{
		Already_used->Checked=true;
	}
	else
	{
		Already_used->Checked=false;
	}

	ADOQueryDBGrid->SQL->Clear();
	ADOQueryDBGrid->SQL->Add(TextSQL);
	ADOQueryDBGrid->Active=true;

	ADOQueryListBox->SQL->Clear();
	ADOQueryListBox->SQL->Add(TextSQLList);
	ADOQueryListBox->Active=true;

	GamesListBox->Items->Clear();

	for (count_record = -1; count_record < ADOQueryListBox->RecordCount-1 ; count_record++)
	{
		GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
		ADOQueryListBox->FindNext();
	}
	// Set selected item in games list corresponding to game name field
	int index = GamesListBox->Items->IndexOf(Game_name->Text);
	if (index >= 0)
	{
		GamesListBox->ItemIndex = index;
	}

Number_keys->Text=ADOQueryDBGrid->RecordCount;
Delete_key->Enabled=false;
Key_cache=Key_link->Text.Trim();
}

//--------------- GA OPENER ------------------------------

void __fastcall TMainForm::Open_linkClick(TObject *Sender)
{
	if (Links_list->ItemIndex!=-1)
	{
		URL=Links_list->Items->Strings[Links_list->ItemIndex];
		BOT = Path + Browser->Items->Strings[Browser->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}

}

void __fastcall TMainForm::Add_groupClick(TObject *Sender)
{
	Groups_list->Items->Add(Clipboard()->AsText);
}

void __fastcall TMainForm::Open_groupsClick(TObject *Sender)
{
	if (Groups_list->ItemIndex!=-1)
	{
		BOT = Path + Browser->Items->Strings[Browser->ItemIndex];

		for (Groups_list->ItemIndex = 0; Groups_list->ItemIndex < Groups_list->Items->Count-1; Groups_list->ItemIndex++)
		{
			URL=Groups_list->Items->Strings[Groups_list->ItemIndex];
			ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
		}
		URL=Groups_list->Items->Strings[Groups_list->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}
}

void __fastcall TMainForm::Add_linkClick(TObject *Sender)
{
	Links_list->Items->Add(Clipboard()->AsText);
}

void __fastcall TMainForm::Open_linksClick(TObject *Sender)
{
	if (Links_list->ItemIndex!=-1)
	{
		BOT = Path + Browser->Items->Strings[Browser->ItemIndex];
		for (Links_list->ItemIndex = 0; Links_list->ItemIndex < Links_list->Items->Count-1; Links_list->ItemIndex++)
		{
			URL=Links_list->Items->Strings[Links_list->ItemIndex];
			ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
		}
		URL=Links_list->Items->Strings[Links_list->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}
}

void __fastcall TMainForm::Open_groupClick(TObject *Sender)
{
	if (Groups_list->ItemIndex!=-1)
	{
		URL = Groups_list->Items->Strings[Groups_list->ItemIndex];
		BOT = Path + Browser->Items->Strings[Browser->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}

}

void __fastcall TMainForm::DeviceClick(TObject *Sender)
{
	Browser->Items->Clear();
	TSearchRec Rec;

	switch (Device->ItemIndex)
	{
		case 0:
		Path = Path + "\\PC\\";
		break;

		case 1:
		Path = Path + "\\Notebook\\";
		break;

		default:
		;
	}

	if(FindFirst(Path + "\\*.lnk", faAnyFile , Rec) == 0)
	{
		do
		{
			Browser->Items->Add(Rec.Name.SubString(1, Rec.Name.Length() - 4 ));
		}
		while(FindNext(Rec) == 0);
	}
	FindClose(Rec);
}

void __fastcall TMainForm::Delete_groupClick(TObject *Sender)
{
	Groups_list->Items->Delete(Groups_list->ItemIndex);
}

void __fastcall TMainForm::Delete_linkClick(TObject *Sender)
{
	Links_list->Items->Delete(Links_list->ItemIndex);
}

void __fastcall TMainForm::Copy_from_clipboardClick(TObject *Sender)
{
	Key_link->Text=Clipboard()->AsText;
	Add_Key_ButtonClick(Sender);
}

void __fastcall TMainForm::Form_StayOnTopClick(TObject *Sender)
{
	MainForm->FormStyle=fsStayOnTop;
}

void __fastcall TMainForm::Form_NormalClick(TObject *Sender)
{
	MainForm->FormStyle=fsNormal;
}

void __fastcall TMainForm::TrayIconClick(TObject *Sender)
{
	TrayIcon->Visible=false;
	ShowWindow(MainForm->Handle, SW_SHOW);
}

void __fastcall TMainForm::FormHide(TObject *Sender)
{
	TrayIcon->Visible=true;
}

void __fastcall TMainForm::Form_hideClick(TObject *Sender)
{
	TrayIcon->Visible = true;
	ShowWindow(MainForm->Handle, SW_HIDE);
}

void __fastcall TMainForm::Activation_link_Change(TObject *Sender)
{
	if (Activation_link->ItemIndex!=-1)
	{
		URL="https://store.steampowered.com/account/registerkey?key=" + Key_link->Text.Trim();
		BOT = Path + Activation_link->Items->Strings[Activation_link->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}
}
void __fastcall TMainForm::Only_keysClick(TObject *Sender)
{
	if (Only_keys->Checked==true)
	{
		BGR->Enabled=false;
		Trade->Enabled=false;
		BGR->Checked=false;
		Trade->Checked=false;
	}
	else
	{
		Trade->Enabled=true;
		BGR->Enabled=true;
    }

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BGRClick(TObject *Sender)
{
if (BGR->Checked==true)
	{
		Trade->Enabled=false;
		Trade->Checked=false;
		Only_keys->Enabled=false;
		Only_keys->Checked=false;
	}
	else
	{
		Trade->Enabled=true;
		Only_keys->Enabled=true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TradeClick(TObject *Sender)
{
if (Trade->Checked==true)
	{
		BGR->Enabled=false;
		Only_keys->Enabled=false;
		BGR->Checked=false;
		Only_keys->Checked=false;
	}
	else
	{
		Only_keys->Enabled=true;
		BGR->Enabled=true;
	}
}
//---------------------------------------------------------------------------

