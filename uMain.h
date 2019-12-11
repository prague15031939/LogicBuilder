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
#include <Vcl.Dialogs.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <string>
#include <cstdlib>
#include <cmath>
#include <fstream.h>
//---------------------------------------------------------------------------
using namespace std;

class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TPaintBox *pbMain;
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
	TActionList *actlistMain;
	TAction *actTakeCursor;
	TAction *actMoveUp;
	TAction *actMoveDown;
	TAction *actMoveLeft;
	TAction *actMoveRight;
	TAction *actDeleteComponent;
	TAction *actCheangeMoveStep;
	TSaveDialog *SaveDialog;
	TOpenDialog *OpenDialog;
	TAction *actOpenFile;
	TAction *actSaveFile;
	TAction *actSaveFileAs;
	TImageList *ImageList;
	TAction *actExit;
	TAction *actSetWireMode;
	TAction *actEndWire;
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
	void __fastcall actSaveFileExecute(TObject *Sender);
	void __fastcall actOpenFileExecute(TObject *Sender);
	void __fastcall actSaveFileAsExecute(TObject *Sender);
	void __fastcall actExitExecute(TObject *Sender);
	void __fastcall actSetWireModeExecute(TObject *Sender);
	void __fastcall actEndWireExecute(TObject *Sender);
	void __fastcall pbMainMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);

private:	// User declarations
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
typedef enum {wsBegin, wsMiddle, wsEnd, wsUnknown} TWireStage;

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
	void set_out_x(int x){
		this->out_x = x;
	}
	void set_out_y(int y){
		this->out_y = y;
	}
	void set_in_x(int x){
		this->in_x = x;
	}
	void set_in_y(int arr[4]){
		for (int i = 0; i < 4; i++) {
			this->in_y[i] = arr[i];
		}
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
	int get_out_x(){
		return this->out_x;
	}
	int get_out_y(){
		return this->out_y;
	}
	int get_in_x(){
		return this->in_x;
	}
	void get_in_y(int arr[4]){
		for (int i = 0; i < 4; i++) {
			arr[i] = this->in_y[i];
		}
	}
private:
	std::string type;
	int entry_amount;
	int x_coord, y_coord;
	int out_x, out_y;
	int in_x;
	int in_y[4];
};

class Wire {
public:
	void set_lines(arr[10][4]){
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				this->lines[i][j] = arr[i][j];
			}
		}
	}
	void get_lines(arr[10][4]){
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				arr[i][j] = this->lines[i][j];
			}
		}
	}
	void set_lines_amount(int value){
		this->lines_amount = value;
	}
	int get_lines_amount(){
        return this->lines_amount;
	}
private:
	int lines[10][4];
	int lines_amount;
};
#endif
