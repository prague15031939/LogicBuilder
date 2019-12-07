//---------------------------------------------------------------------------

#ifndef uMainH
#define uMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <string>
#include <cstdlib>
//---------------------------------------------------------------------------
using namespace std;

class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TPaintBox *pbMain;
	TButton *btnDbg;
	TMemo *memoDbg;
	TListBox *lboxComponents;
	TMainMenu *menuMain;
	TMenuItem *File1;
	TMenuItem *Settings1;
	TMenuItem *Open1;
	TMenuItem *Save1;
	TMenuItem *Saveas1;
	TMenuItem *Exit1;
	TMenuItem *Help1;
	TToolBar *tbMain;
	TToolButton *ToolButton1;
	TToolButton *ToolButton4;
	TActionList *actlistMain;
	TAction *actTakeCursor;
	TAction *actMoveUp;
	TAction *actMoveDown;
	TAction *actMoveLeft;
	TAction *actMoveRight;
	TAction *actDeleteComponent;
	TAction *actCheangeMoveStep;
	void __fastcall btnDbgClick(TObject *Sender);
	void __fastcall pbMainPaint(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall lboxComponentsDblClick(TObject *Sender);
	void __fastcall actTakeCursorExecute(TObject *Sender);
	void __fastcall pbMainMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall actMoveUpExecute(TObject *Sender);
	void __fastcall actMoveDownExecute(TObject *Sender);
	void __fastcall actMoveLeftExecute(TObject *Sender);
	void __fastcall actMoveRightExecute(TObject *Sender);
	void __fastcall actDeleteComponentExecute(TObject *Sender);
	void __fastcall actCheangeMoveStepExecute(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
class Component {
public:
	void set_type(std::string name){
		this->type = name;
	}
	void set_entry_amount(int num){
		this->entry_amount = num;
	}
	void set_coords(int x, int y){
		this->x_coord = x;
		this->y_coord = y;
	}
	std::string get_type(){
		return this->type;
	}
	int get_entry_amount(){
		return this->entry_amount;
	}
	int get_x(){
		return this->x_coord;
	}
	int get_y(){
		return this->y_coord;
	}
private:
	std::string type;
	int entry_amount;
	int x_coord, y_coord;
};
#endif
