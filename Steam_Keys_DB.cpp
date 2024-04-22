// Author: Yuriy Tyutyunnik
// Date: 05.04.2024


#include <vcl.h>
#include <conio.h>
#include <stdio.h>
#include <Clipbrd.hpp>
#include <FileCtrl.hpp>
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
        // Set the caption of the main form
        MainForm->Caption = "Steam Keys Database";

        // Establish connection to the database
        ADOConnection->ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Password=\"\";Data Source=SteamDB.mdb;Persist Security Info=True";
        ADOConnection->Connected = true;
        ADOQuerySelect->Connection = ADOConnection;

        // Execute SQL query to select all rows from the 'Keys' table
        ADOQuerySelect->SQL->Text = "SELECT * FROM Keys";
        ADOQuerySelect->Active = true;

        // Adjust interface elements
        Delete_key->Enabled = false;
        Add_new_game->Visible = true;
        Key_buffer->Visible = false;
        Copy_buffer->Enabled = false;
        Only_keys->Visible = false;
        Only_keys->Checked = false;
        BGR->Visible = false;
        BGR->Checked = false;
        Trade->Visible = false;
        Trade->Checked = false;
        Multiselection->Left = 548;
        Multiselection->Top = 23;
        Key_link->MaxLength = 71;
        Game_name->MaxLength = 60;
        Source->MaxLength = 25;
        SortBox->ItemIndex = 0;
        Device->ItemIndex = 0;
        SKDB->Show();

        // Filling up a list of browsers
        Browser->Items->Clear();
        Activation_link->Text = "Open activation link";

		AnsiString path = ExtractFilePath(Application->ExeName);
		switch (Device->ItemIndex)
		{
			case 0:
				path = IncludeTrailingBackslash(path + "PC");
				break;
			case 1:
				path = IncludeTrailingBackslash(path + "Notebook");
				break;
			default:
				break;
		}

        TSearchRec Rec;
        if (FindFirst(path + "*.lnk", faAnyFile, Rec) == 0)
        {
            do
            {
                Browser->Items->Add(ChangeFileExt(Rec.Name, ""));
                Activation_link->Items->Add(ChangeFileExt(Rec.Name, ""));
            }
            while (FindNext(Rec) == 0);
        }
        FindClose(Rec);

        // Disable interface items and fill test fields if database is empty
        if (ADOQuerySelect->RecordCount == 0)
        {
            Number_keys->Text = "0";
            Add_date->Date = Date();
            Key_link->Text = "Enter a key or Indiegala link";
            Game_name->Text = "Enter a name of the game";
            Source->Text = "Enter source";
            Notes->Text = "You can write down some notes here. Delete this text for now :)";

            KeySelect->Visible = false;
            Game_select->Visible = false;
            SortingBox->Visible = false;
            Update_key->Enabled = false;
            MainForm->Height = 245;
            MainForm->Width = 663;
            GaOpener->TabVisible = false;
        }
        else
        {
            // Handle non-empty database
            KeySelect->Visible = true;
            Game_select->Visible = true;
            SortingBox->Visible = true;
            Update_key->Enabled = true;
            MainForm->Height = 565;
            MainForm->Width = 1138;
            GaOpener->TabVisible = true;

            ADOQuerySelect->Last();
            Add_date->Date = ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
            Key_link->Text = ADOQuerySelect->FieldByName("Key_link")->AsString;
            Game_name->Text = ADOQuerySelect->FieldByName("Game_name")->AsString;
            Source->Text = ADOQuerySelect->FieldByName("Source")->AsString;
            Notes->Text = ADOQuerySelect->FieldByName("Notes")->AsString;

            Trading_cards->Checked = ADOQuerySelect->FieldByName("Trading_cards")->AsBoolean;
            DLC->Checked = ADOQuerySelect->FieldByName("DLC")->AsBoolean;
            Other->Checked = ADOQuerySelect->FieldByName("Other")->AsBoolean;
            Already_used->Checked = ADOQuerySelect->FieldByName("Already_used")->AsBoolean;

            // Fill up table with list of keys
            ADOQueryDBGrid->SQL->Text = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='" + Game_name->Text + "' ORDER BY Add_date ASC";
            ADOQueryDBGrid->Active = true;
            Number_keys->Text = IntToStr(ADOQueryDBGrid->RecordCount);

            // Fill up list of key's sources
            ADOQueryListBox->SQL->Text = "SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
            ADOQueryListBox->Active = true;
            Source->Items->Clear();
            while (!ADOQueryListBox->Eof)
            {
                Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
                ADOQueryListBox->Next();
            }

            // Fill up games list
            ADOQueryListBox->SQL->Text = "SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
            ADOQueryListBox->Active = true;
            GamesListBox->Items->Clear();
            Game_name->Items->Clear();
            while (!ADOQueryListBox->Eof)
            {
                GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
                Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
                ADOQueryListBox->Next();
            }

            int index = GamesListBox->Items->IndexOf(Game_name->Text);
            if (index >= 0)
            {
                GamesListBox->ItemIndex = index;
            }

            Key_cache = Key_link->Text.Trim();
        }
    }
    catch(Exception &e)
    {
        ShowMessage("Error: " + e.Message);
        MainForm->Close();
    }
}

void __fastcall TMainForm::DBGridKeys_listCellClick(TColumn *Column)
{
    TextSelect = "SELECT * FROM Keys WHERE Key_link='";
    TextSelect += DBGridKeys_list->SelectedField->AsString;
    TextSelect += "' ";
    ADOQuerySelect->SQL->Clear();
    ADOQuerySelect->SQL->Add(TextSelect);
    ADOQuerySelect->Active = true;

    Add_date->DateTime = ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
    Key_link->Text = ADOQuerySelect->FieldByName("Key_link")->AsString;
    Game_name->Text = ADOQuerySelect->FieldByName("Game_name")->AsString;
    Source->Text = ADOQuerySelect->FieldByName("Source")->AsString;
    Notes->Text = ADOQuerySelect->FieldByName("Notes")->AsString;

    Trading_cards->Checked = ADOQuerySelect->FieldByName("Trading_cards")->AsBoolean;
    DLC->Checked = ADOQuerySelect->FieldByName("DLC")->AsBoolean;
    Other->Checked = ADOQuerySelect->FieldByName("Other")->AsBoolean;
    Already_used->Checked = ADOQuerySelect->FieldByName("Already_used")->AsBoolean;

    if (Multiselection->Checked == false)
    {
        Clipboard()->AsText = ADOQuerySelect->FieldByName("Game_name")->AsString + " - " + ADOQuerySelect->FieldByName("Key_link")->AsString;
    }
    else
    {
        if (Only_keys->Checked == true)
        {
            if (Key_buffer->Lines->Text.IsEmpty())
            {
                Key_buffer->Lines->Add("!redeem " + ADOQuerySelect->FieldByName("Key_link")->AsString);
            }
            else
            {
                Key_buffer->Lines->Text = Key_buffer->Lines->Text.Trim() + "," + ADOQuerySelect->FieldByName("Key_link")->AsString;
            }
        }
        if (BGR->Checked == true)
        {
            Key_buffer->Lines->Add(ADOQuerySelect->FieldByName("Game_name")->AsString + "\t" + ADOQuerySelect->FieldByName("Key_link")->AsString);
        }
        if (Trade->Checked == true)
        {
            Key_buffer->Lines->Add(ADOQuerySelect->FieldByName("Game_name")->AsString + " - " + ADOQuerySelect->FieldByName("Key_link")->AsString);
        }
    }

    TextSelect = "SELECT * FROM Keys WHERE Game_name='";
    TextSelect += Game_name->Text;
    TextSelect += "' ";
    ADOQuerySelect->SQL->Clear();
    ADOQuerySelect->SQL->Add(TextSelect);
    ADOQuerySelect->Active = true;

    // Set selected item in games list corresponding to game name field
    int index = GamesListBox->Items->IndexOf(Game_name->Text);
    if (index >= 0)
    {
        GamesListBox->ItemIndex = index;
    }
    Number_keys->Text = ADOQuerySelect->RecordCount;
    Delete_key->Caption = "Delete key";
    Delete_key->Enabled = true;
    if (Multiselection->Checked == true)
    {
        Copy_buffer->Enabled = true;
    }
    Key_cache = Key_link->Text.Trim();
    if (Game_name->Text.IsEmpty())
    {
        Add_date->DateTime = TDateTime::CurrentDateTime();
        Key_link->Text = "Click on a key or link";
        Game_name->Text = "Click on a key or link";
    }
}

void __fastcall TMainForm::GamesListBoxClick(TObject *Sender)
{
    TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
    TextSQL += GamesListBox->Items->Strings[GamesListBox->ItemIndex];
    TextSQL += "' ORDER BY Add_date";
    ADOQueryDBGrid->SQL->Clear();
    ADOQueryDBGrid->SQL->Add(TextSQL);
    ADOQueryDBGrid->Active = true;
    Number_keys->Text = IntToStr(ADOQueryDBGrid->RecordCount);

    TextSelect = "SELECT * FROM Keys WHERE Game_name='";
    TextSelect += GamesListBox->Items->Strings[GamesListBox->ItemIndex];
    TextSelect += "' ORDER BY Add_date";
    ADOQuerySelect->SQL->Clear();
    ADOQuerySelect->SQL->Add(TextSelect);
    ADOQuerySelect->Active = true;
    ADOQuerySelect->FindFirst();
    Add_date->DateTime = ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
    Key_link->Text = ADOQuerySelect->FieldByName("Key_link")->AsString;
    Game_name->Text = ADOQuerySelect->FieldByName("Game_name")->AsString;
    Source->Text = ADOQuerySelect->FieldByName("Source")->AsString;
    Notes->Text = ADOQuerySelect->FieldByName("Notes")->AsString;

    Trading_cards->Checked = ADOQuerySelect->FieldByName("Trading_cards")->AsBoolean;
    DLC->Checked = ADOQuerySelect->FieldByName("DLC")->AsBoolean;
    Other->Checked = ADOQuerySelect->FieldByName("Other")->AsBoolean;
    Already_used->Checked = ADOQuerySelect->FieldByName("Already_used")->AsBoolean;

    Delete_key->Enabled = false;
    Key_cache = Key_link->Text.Trim();
}

void __fastcall TMainForm::Delete_keyClick(TObject *Sender)
{
    try
    {
        TextDelete = "DELETE FROM Keys WHERE Key_link = '";
        TextDelete += DBGridKeys_list->SelectedField->AsString;
        TextDelete += "'";
        ADOQueryDelete->Active = false;
        ADOQueryDelete->SQL->Clear();
        ADOQueryDelete->SQL->Add(TextDelete);
        ADOQueryDelete->ExecSQL();

        Timer->Enabled = true;
        KeySelect->Color = clLime;
        Delete_key->Caption = "DELETED";
        Delete_key->Enabled = false;

        TextSQL = "SELECT * FROM Keys";
        ADOQuerySelect->SQL->Clear();
        ADOQuerySelect->SQL->Add(TextSQL);
        ADOQuerySelect->Active = true;

        if (ADOQuerySelect->RecordCount != 0)
        {
            TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
            TextSQL += Game_name->Text;
            TextSQL += "' ORDER BY Add_date";
            ADOQueryDBGrid->SQL->Clear();
            ADOQueryDBGrid->SQL->Add(TextSQL);
            ADOQueryDBGrid->Active = true;
            Number_keys->Text = IntToStr(ADOQueryDBGrid->RecordCount);

            if (Number_keys->Text == "0")
            {
                ADOQueryDBGrid->Active = false;
                TextSQL = "SELECT * FROM Keys";
                ADOQuerySelect->SQL->Clear();
                ADOQuerySelect->SQL->Add(TextSQL);
                ADOQuerySelect->Active = true;
                ADOQuerySelect->FindLast();
                Add_date->DateTime = ADOQuerySelect->FieldByName("Add_date")->AsString;
                Key_link->Text = ADOQuerySelect->FieldByName("Key_link")->AsString;
                Game_name->Text = ADOQuerySelect->FieldByName("Game_name")->AsString;
                Source->Text = ADOQuerySelect->FieldByName("Source")->AsString;
                Notes->Text = ADOQuerySelect->FieldByName("Notes")->AsString;
                Number_keys->Text = ADOQuerySelect->RecordCount;

                Trading_cards->Checked = ADOQuerySelect->FieldByName("Trading_cards")->AsBoolean;
                DLC->Checked = ADOQuerySelect->FieldByName("DLC")->AsBoolean;
                Other->Checked = ADOQuerySelect->FieldByName("Other")->AsBoolean;
                Already_used->Checked = ADOQuerySelect->FieldByName("Already_used")->AsBoolean;

                TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
                TextSQL += Game_name->Text;
                TextSQL += "' ORDER BY Add_date";
                ADOQueryDBGrid->SQL->Clear();
                ADOQueryDBGrid->SQL->Add(TextSQL);
                ADOQueryDBGrid->Active = true;
                Number_keys->Text = ADOQueryDBGrid->RecordCount;

                TextSQL = "SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
                ADOQueryListBox->SQL->Clear();
                ADOQueryListBox->SQL->Add(TextSQL);
                ADOQueryListBox->Active = true;
                Source->Items->Clear();

                while (!ADOQueryListBox->Eof)
                {
                    Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
                    ADOQueryListBox->Next();
                }

                TextSQL = "SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
                ADOQueryListBox->SQL->Clear();
                ADOQueryListBox->SQL->Add(TextSQL);
                ADOQueryListBox->Active = true;
                GamesListBox->Items->Clear();
                Game_name->Items->Clear();

                while (!ADOQueryListBox->Eof)
                {
                    GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
                    Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
                    ADOQueryListBox->Next();
                }

                int index = GamesListBox->Items->IndexOf(Game_name->Text);
                if (index >= 0)
                {
                    GamesListBox->ItemIndex = index;
                }
            }
            else
            {
                int index = GamesListBox->Items->IndexOf(Game_name->Text);
                if (index >= 0)
                {
                    GamesListBox->ItemIndex = index;
                }

                TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
                TextSQL += GamesListBox->Items->Strings[GamesListBox->ItemIndex];
                TextSQL += "' ORDER BY Add_date";
                ADOQueryDBGrid->SQL->Clear();
                ADOQueryDBGrid->SQL->Add(TextSQL);
                ADOQueryDBGrid->Active = true;
                Number_keys->Text = ADOQueryDBGrid->RecordCount;

                TextSelect = "SELECT * FROM Keys WHERE Game_name='";
                TextSelect += GamesListBox->Items->Strings[GamesListBox->ItemIndex];
                TextSelect += " ' ORDER BY Add_date";
                ADOQuerySelect->SQL->Clear();
                ADOQuerySelect->SQL->Add(TextSelect);
                ADOQuerySelect->Active = true;
                ADOQuerySelect->FindFirst();
                Add_date->DateTime = ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
                Key_link->Text = ADOQuerySelect->FieldByName("Key_link")->AsString;
                Game_name->Text = ADOQuerySelect->FieldByName("Game_name")->AsString;
                Source->Text = ADOQuerySelect->FieldByName("Source")->AsString;
                Notes->Text = ADOQuerySelect->FieldByName("Notes")->AsString;

                Trading_cards->Checked = ADOQuerySelect->FieldByName("Trading_cards")->AsBoolean;
                DLC->Checked = ADOQuerySelect->FieldByName("DLC")->AsBoolean;
                Other->Checked = ADOQuerySelect->FieldByName("Other")->AsBoolean;
                Already_used->Checked = ADOQuerySelect->FieldByName("Already_used")->AsBoolean;

                Delete_key->Enabled = false;
                Key_cache = Key_link->Text.Trim();
            }
        }
        else
        {
            Number_keys->Text = "0";
            Add_date->DateTime.CurrentDateTime();
            Key_link->Text = "Enter a key or Indiegala link";
            Game_name->Text = "Enter a name of the game";
            Source->Text = "Enter source";
            Notes->Text = "You can write down some notes here. Delete this text for now :)";

            KeySelect->Visible = false;
            Game_select->Visible = false;
            SortingBox->Visible = false;
            Update_key->Enabled = false;
            MainForm->Height = 245;
            MainForm->Width = 663;
            GaOpener->TabVisible = false;
        }
    }
    catch (...)
    {
        KeySelect->Color = clRed;
        Delete_key->Caption = "ERROR";
    }
}

void __fastcall TMainForm::Add_Key_ButtonClick(TObject *Sender)
{
    // Check if any of the required fields are empty
    if (Key_link->Text.Trim().IsEmpty() || Game_name->Text.Trim().IsEmpty() || Source->Text.Trim().IsEmpty())
    {
        Add_Key_Button->Caption = "SOME FIELDS ARE EMPTY";
        Add_new_game->Color = clYellow;
        Timer->Enabled = true;
        return; // Exit the function if any required field is empty
    }

    if (Key_link->Text == "Click on a key or link")
    {
        Add_Key_Button->Caption = "Check your key or link ;)";
        Add_new_game->Color = clYellow;
        Key_link->Color = clRed;
        Add_date->Date.CurrentDate();
        Add_date->Time.CurrentTime();
        Timer->Enabled = true;
        return; // Exit the function if key link is not entered properly
	}
    else
    {
        ADOQuerySelect->SQL->Clear();
        AnsiString TextSQL;
        TextSQL = "INSERT INTO Keys(Add_date, Key_link, Game_name, Source, Trading_cards, DLC, Other, Already_used, Notes)";
        TextSQL += "values(:Add_date, :Key_link, :Game_name, :Source, :Trading_cards, :DLC, :Other, :Already_used, :Notes)";
        ADOQuerySelect->SQL->Add(TextSQL);
        ADOQuerySelect->Parameters->Items[0]->Value = Add_date->DateTime.CurrentDateTime();
        ADOQuerySelect->Parameters->Items[1]->Value = Key_link->Text.Trim();
        ADOQuerySelect->Parameters->Items[2]->Value = Game_name->Text.Trim();
        ADOQuerySelect->Parameters->Items[3]->Value = Source->Text.Trim();
        ADOQuerySelect->Parameters->Items[8]->Value = Notes->Text.Trim();

        ADOQuerySelect->Parameters->Items[4]->Value = Trading_cards->Checked ? 1 : 0;
        ADOQuerySelect->Parameters->Items[5]->Value = DLC->Checked ? 1 : 0;
        ADOQuerySelect->Parameters->Items[6]->Value = Other->Checked ? 1 : 0;
        ADOQuerySelect->Parameters->Items[7]->Value = Already_used->Checked ? 1 : 0;

        try
        {
            ADOQuerySelect->ExecSQL();
            ADOQuerySelect->SQL->Clear();

            Add_Key_Button->Caption = "KEY ADDED";
            Add_new_game->Color = clLime;
            Timer->Enabled = true;
            Key_link->Clear();
            Delete_key->Enabled = false;

            TextSQL = "SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
            ADOQueryListBox->SQL->Clear();
            ADOQueryListBox->SQL->Add(TextSQL);
            ADOQueryListBox->Active = true;
            Source->Items->Clear();

            while (!ADOQueryListBox->Eof)
            {
                Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
                ADOQueryListBox->Next();
            }

            TextSQL = "SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
            ADOQueryListBox->SQL->Clear();
            ADOQueryListBox->SQL->Add(TextSQL);
            ADOQueryListBox->Active = true;
            GamesListBox->Items->Clear();
            Game_name->Items->Clear();

            while (!ADOQueryListBox->Eof)
            {
                GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
                Game_name->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
                ADOQueryListBox->Next();
            }

            int index = GamesListBox->Items->IndexOf(Game_name->Text);
            if (index >= 0)
            {
                GamesListBox->ItemIndex = index;
            }

            KeySelect->Visible = true;
            Game_select->Visible = true;
            SortingBox->Visible = true;
            Update_key->Enabled = true;
            MainForm->Height = 555;
            MainForm->Width = 1123;
            GaOpener->TabVisible = true;
        }
        catch (...)
        {
            Add_Key_Button->Caption = "ERROR";
            Add_new_game->Color = clRed;
            Timer->Enabled = true;
        }
    }

    TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='";
    TextSQL += Game_name->Text;
    TextSQL += "' ORDER BY Add_date";
    ADOQueryDBGrid->SQL->Clear();
    ADOQueryDBGrid->SQL->Add(TextSQL);
    ADOQueryDBGrid->Active = true;
    Number_keys->Text = ADOQueryDBGrid->RecordCount;
    Key_cache = Key_link->Text.Trim();
}

void __fastcall TMainForm::TimerTimer(TObject *Sender)
{
    Add_Key_Button->Caption = "Add key";
    Copy_buffer->Caption = "Copy";
    Add_new_game->Color = clWhite;
    KeySelect->Color = clWhite;
    SortingBox->Color = clWhite;
    Key_link->Color = clWindow;
    Delete_key->Caption = "Delete key";
    Update_key->Caption = "Update info";
}

void __fastcall TMainForm::Update_keyClick(TObject *Sender)
{
    Delete_key->Enabled = false;

    if (Key_link->Text.IsEmpty() || Game_name->Text.IsEmpty() || Source->Text.IsEmpty())
    {
        Update_key->Caption = "SOME FIELDS ARE EMPTY";
        Add_new_game->Color = clYellow;
        Timer->Enabled = true;
    }
    else
    {
        String TextUpdate = "UPDATE Keys SET Add_date='" + Add_date->DateTime.DateTimeString() +
                            "', Key_link='" + Key_link->Text.Trim() +
                            "', Game_name='" + Game_name->Text.Trim() +
                            "', Source='" + Source->Text.Trim() +
                            "', Notes='" + Notes->Text.Trim() +
                            "', Trading_cards='" + (Trading_cards->Checked ? "1" : "0") +
                            "', DLC='" + (DLC->Checked ? "1" : "0") +
                            "', Other='" + (Other->Checked ? "1" : "0") +
                            "', Already_used='" + (Already_used->Checked ? "1" : "0") +
                            "' WHERE Key_link='" + Key_cache + "'";

        try
        {
            ADOQueryUpdate->Active = false;
            ADOQueryUpdate->SQL->Text = TextUpdate;
            ADOQueryUpdate->ExecSQL();

            Update_key->Caption = "INFO UPDATED";
            Add_new_game->Color = clLime;
            Timer->Enabled = true;

            // Refresh List Boxes and UI
            ADOQueryDBGrid->SQL->Text = "SELECT Add_date AS [Date added], Key_link AS [Key or link], Game_name AS [Game], Source FROM Keys WHERE Game_name='" + Game_name->Text + "' ORDER BY Add_date ASC";
            ADOQueryDBGrid->Active = true;
            Number_keys->Text = IntToStr(ADOQueryDBGrid->RecordCount);

            ADOQueryListBox->SQL->Text = "SELECT Source FROM Keys GROUP BY Source ORDER BY Source";
            ADOQueryListBox->Active = true;
            Source->Items->Clear();
            while (!ADOQueryListBox->Eof)
            {
                Source->Items->Add(ADOQueryListBox->FieldByName("Source")->AsString);
                ADOQueryListBox->Next();
            }

            ADOQueryListBox->SQL->Text = "SELECT Game_name FROM Keys GROUP BY Game_name ORDER BY Game_name";
            ADOQueryListBox->Active = true;
            Game_name->Items->Clear();
            GamesListBox->Items->Clear();
            while (!ADOQueryListBox->Eof)
            {
                String gameName = ADOQueryListBox->FieldByName("Game_name")->AsString;
                GamesListBox->Items->Add(gameName);
                Game_name->Items->Add(gameName);
                ADOQueryListBox->Next();
            }

            // Set selected item in games list corresponding to game name field
            int index = GamesListBox->Items->IndexOf(Game_name->Text);
            if (index != -1)
            {
                GamesListBox->ItemIndex = index;
            }
        }
        catch (...)
        {
            Update_key->Caption = "ERROR";
            Add_new_game->Color = clRed;
            Timer->Enabled = true;
        }
    }
    Key_cache = Key_link->Text.Trim();
}

void __fastcall TMainForm::SortBoxChange(TObject *Sender)
{
	String orderByClause, groupByClause, orderByListClause;

	switch (SortBox->ItemIndex)
	{
        case 0:
            orderByClause = "ORDER BY Add_date ASC, Game_name ASC";
            orderByListClause = "ORDER BY MIN(Add_date) ASC"; // Сортування за мінімальною датою додавання
            break;
        case 1:
            orderByClause = "ORDER BY Add_date DESC, Game_name DESC";
            orderByListClause = "ORDER BY MIN(Add_date) DESC"; // Сортування за максимальною датою додавання
			break;
		case 2:
			orderByClause = "ORDER BY Count(*) ASC";
			groupByClause = "GROUP BY Game_name";
			orderByListClause = "ORDER BY Count(*) ASC";
			break;
		case 3:
			orderByClause = "ORDER BY Count(*) DESC";
			groupByClause = "GROUP BY Game_name";
			orderByListClause = "ORDER BY Count(*) DESC";
			break;
		case 4:
			orderByClause = "ORDER BY Source ASC";
			groupByClause = "GROUP BY Game_name, Source";
			orderByListClause = "ORDER BY Source ASC";
			break;
		case 5:
			orderByClause = "ORDER BY Source DESC, Game_name DESC";
			groupByClause = "GROUP BY Game_name, Source";
			orderByListClause = "ORDER BY Source DESC, Game_name DESC";
			break;
		case 6:
			orderByClause = "ORDER BY Game_name ASC";
			groupByClause = "GROUP BY Game_name";
			orderByListClause = "ORDER BY Game_name ASC";
            break;
        case 7:
            orderByClause = "ORDER BY Game_name DESC";
			groupByClause = "GROUP BY Game_name";
            orderByListClause = "ORDER BY Game_name DESC";
            break;
        default:
			return; // Вихід з функції, якщо не вдалося знайти відповідний випадок
    }

	TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys " + orderByClause;
    // Оновлений запит для вибірки унікальних назв ігор з урахуванням дати
    TextSQLList = "SELECT Game_name, MIN(Add_date) AS FirstAddedDate FROM Keys GROUP BY Game_name " + orderByListClause;

    ADOQueryDBGrid->SQL->Clear();
    ADOQueryDBGrid->SQL->Add(TextSQL);
    ADOQueryDBGrid->Active = true;

    ADOQueryListBox->SQL->Clear();
    ADOQueryListBox->SQL->Add(TextSQLList);
    ADOQueryListBox->Active = true;

    GamesListBox->Items->Clear();

    while (!ADOQueryListBox->Eof)
    {
        GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
        ADOQueryListBox->Next();
    }

    // Встановлення вибраного елемента в списку ігор
    int index = GamesListBox->Items->IndexOf(Game_name->Text);
    if (index >= 0)
    {
        GamesListBox->ItemIndex = index;
    }

    Number_keys->Text = IntToStr(ADOQueryDBGrid->RecordCount);
    Delete_key->Enabled = false;
    Key_cache = Key_link->Text.Trim();
}

void __fastcall TMainForm::MultiselectionClick(TObject *Sender)
{
	// Determine the state based on the checkbox
	bool isChecked = Multiselection->Checked;

    // Toggle visibility based on the checkbox state
	Add_new_game->Visible = !isChecked;
    Key_buffer->Visible = isChecked;
    Only_keys->Visible = isChecked;
    BGR->Visible = isChecked;
    Trade->Visible = isChecked;

    // Set properties if checked
    if (isChecked)
    {
		Key_buffer->Lines->Clear();
		Key_buffer->Left=0;
		Key_buffer->Top=1;
		Key_buffer->Height=182;
		Key_buffer->Width=645;
		Multiselection->Top=36;
		Only_keys->Top=13;
		Only_keys->Left=548;
    }
    else
    {
        // Reset specific properties and states when not checked
        Copy_buffer->Enabled = false;
        Only_keys->Checked = false;
		BGR->Checked = false;
        Trade->Checked = false;
        Multiselection->Top = 23;
    }

    // Common setting regardless of the checkbox state
    Delete_key->Enabled = false;
}

void __fastcall TMainForm::Copy_bufferClick(TObject *Sender)
{
	if (Key_buffer->Text.IsEmpty())
	{
		Copy_buffer->Caption = "Empty";
	}
	else
	{
		SortingBox->Color = clLime;
		Copy_buffer->Caption = "DONE";
		Clipboard()->AsText = Key_buffer->Text.Trim();
	}
	Timer->Enabled = true;
}

void __fastcall TMainForm::Key_linkKeyPress(TObject *Sender, wchar_t &Key)
{
	if (Key == 13)
	{
		Add_Key_ButtonClick(Sender);
	}
}

void __fastcall TMainForm::Game_nameKeyPress(TObject *Sender, wchar_t &Key)
{
	if (Key == 13)
	{
		Add_Key_ButtonClick(Sender);
	}
}

void __fastcall TMainForm::Key_linkClick(TObject *Sender)
{
	if (Key_link->Text.IsEmpty())
	{
		Key_link->Text = Clipboard()->AsText;
	}
	else
	{
		Key_link->SelectAll();
		Key_link->SetFocus();
	}
}

void __fastcall TMainForm::SortGroupClick(TObject *Sender)
{
    String filterColumn;
    switch (SortGroup->ItemIndex)
    {
        case 0:
            filterColumn = "Trading_cards";
            break;
        case 1:
            filterColumn = "DLC";
            break;
        case 2:
            filterColumn = "Other";
            break;
        case 3:
            filterColumn = "Already_used";
            break;
        default:
            return;
    }

    String orderByClause = "ORDER BY Add_date ASC";
    TextSQL = "SELECT Add_date AS [Date added], Key_link AS [Key], Game_name AS [Game name], Source FROM Keys WHERE " + filterColumn + " = True " + orderByClause;
    TextSQLList = "SELECT DISTINCT Game_name FROM Keys WHERE " + filterColumn + " = True ORDER BY Game_name";
    TextSelect = "SELECT * FROM Keys WHERE " + filterColumn + " = True " + orderByClause;

    // Set SQL queries for ADOQuerySelect
    ADOQuerySelect->SQL->Clear();
    ADOQuerySelect->SQL->Add(TextSelect);
    ADOQuerySelect->Active = true;

    // Populate fields with the first record from ADOQuerySelect
    if (!ADOQuerySelect->Eof)
    {
        Add_date->DateTime = ADOQuerySelect->FieldByName("Add_date")->AsDateTime;
        Key_link->Text = ADOQuerySelect->FieldByName("Key_link")->AsString;
        Game_name->Text = ADOQuerySelect->FieldByName("Game_name")->AsString;
        Source->Text = ADOQuerySelect->FieldByName("Source")->AsString;
        Notes->Text = ADOQuerySelect->FieldByName("Notes")->AsString;
        Trading_cards->Checked = ADOQuerySelect->FieldByName("Trading_cards")->AsBoolean;
        DLC->Checked = ADOQuerySelect->FieldByName("DLC")->AsBoolean;
        Other->Checked = ADOQuerySelect->FieldByName("Other")->AsBoolean;
        Already_used->Checked = ADOQuerySelect->FieldByName("Already_used")->AsBoolean;
    }

    // Set SQL queries for ADOQueryDBGrid and ADOQueryListBox
    ADOQueryDBGrid->SQL->Clear();
    ADOQueryDBGrid->SQL->Add(TextSQL);
    ADOQueryDBGrid->Active = true;

    ADOQueryListBox->SQL->Clear();
    ADOQueryListBox->SQL->Add(TextSQLList);
    ADOQueryListBox->Active = true;

    // Populate GamesListBox with unique game names
    GamesListBox->Items->Clear();
    ADOQueryListBox->First(); // Move to the first record before the loop
    while (!ADOQueryListBox->Eof)
    {
        GamesListBox->Items->Add(ADOQueryListBox->FieldByName("Game_name")->AsString);
        ADOQueryListBox->Next();
    }

    // Set selected item in GamesListBox corresponding to game name field
    int index = GamesListBox->Items->IndexOf(Game_name->Text);
    if (index >= 0)
    {
        GamesListBox->ItemIndex = index;
    }

    Number_keys->Text = IntToStr(ADOQueryDBGrid->RecordCount);
    Delete_key->Enabled = false;
    Key_cache = Key_link->Text.Trim();
}

void __fastcall TMainForm::Copy_from_clipboardClick(TObject *Sender)
{
	Key_link->Text=Clipboard()->AsText;
	Add_Key_ButtonClick(Sender);
}

void __fastcall TMainForm::Activation_link_Change(TObject *Sender)
{
	AnsiString path = ExtractFilePath(Application->ExeName);
	switch (Device->ItemIndex)
	{
		case 0:
			path += "PC\\";
			break;
		case 1:
			path += "Notebook\\";
			break;
		default:
			break;
	}

    if (Activation_link->ItemIndex != -1)
    {
        AnsiString URL = "https://store.steampowered.com/account/registerkey?key=" + Key_link->Text.Trim();
        AnsiString BOT = path + Activation_link->Items->Strings[Activation_link->ItemIndex];
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

//--------------- GA OPENER ------------------------------

void __fastcall TMainForm::Add_linkClick(TObject *Sender)
{
	Links_list->Items->Add(Clipboard()->AsText);
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::Open_linkClick(TObject *Sender)
{
AnsiString path = ExtractFilePath(Application->ExeName);
	switch (Device->ItemIndex)
	{
		case 0:
			path += "PC\\";
			break;
		case 1:
			path += "Notebook\\";
			break;
		default:
			break;
	}

	if (Links_list->ItemIndex!=-1)
	{
		URL=Links_list->Items->Strings[Links_list->ItemIndex];
		BOT = path + Browser->Items->Strings[Browser->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Open_linksClick(TObject *Sender)
{
	AnsiString path = ExtractFilePath(Application->ExeName);
	switch (Device->ItemIndex)
	{
		case 0:
			path += "PC\\";
			break;
		case 1:
			path += "Notebook\\";
			break;
		default:
			break;
	}

	if (Links_list->ItemIndex!=-1)
	{
		BOT = path + Browser->Items->Strings[Browser->ItemIndex];
		for (Links_list->ItemIndex = 0; Links_list->ItemIndex < Links_list->Items->Count-1; Links_list->ItemIndex++)
		{
			URL=Links_list->Items->Strings[Links_list->ItemIndex];
			ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
		}
		URL=Links_list->Items->Strings[Links_list->ItemIndex];
		ShellExecute(Handle, "open", BOT.c_str(), URL.c_str(), NULL, SW_SHOW);
	}
}
//---------------------------------------------------------------------------

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
//---------------------------------------------------------------------------

void __fastcall TMainForm::Delete_linkClick(TObject *Sender)
{
	Links_list->Items->Delete(Links_list->ItemIndex);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Form_StayOnTopClick(TObject *Sender)
{
	MainForm->FormStyle=fsStayOnTop;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Form_NormalClick(TObject *Sender)
{
	MainForm->FormStyle=fsNormal;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TrayIconClick(TObject *Sender)
{
	TrayIcon->Visible=false;
	ShowWindow(MainForm->Handle, SW_SHOW);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormHide(TObject *Sender)
{
	TrayIcon->Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Form_hideClick(TObject *Sender)
{
	TrayIcon->Visible = true;
	ShowWindow(MainForm->Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

