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
	TMainMenu *menuMain;
	TMenuItem *File1;
	TMenuItem *Settings1;
	TMenuItem *Open1;
	TMenuItem *Save1;
	TMenuItem *Saveas1;
	TMenuItem *Exit1;
	TMenuItem *Help1;
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
	TListBox *lboxComponents;
	TAction *actNewFile;
	TMenuItem *New1;
	TAction *actSVGExport;
	TMenuItem *ExporttoSVG1;
	TAction *actBranchWire;
	TMemo *memo;
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
	void __fastcall actNewFileExecute(TObject *Sender);
	void __fastcall actSVGExportExecute(TObject *Sender);
	void __fastcall actBranchWireExecute(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
typedef enum {wsBegin, wsMiddle, wsEnd} TWireStage;

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
	void set_in_wire(int value, int index){
		this->in_wires[index] = value;
	}
	void set_out_wire(int index){
		this->out_wire = index;
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
	void get_in_wires(int arr[4]){
		for (int i = 0; i < 4; i++) {
			arr[i] = this->in_wires[i];
		}
	}
	int get_out_wire(){
		return this->out_wire;
	}

private:
	std::string type;
	int entry_amount;
	int x_coord, y_coord;
	int out_x, out_y;
	int in_x;
	int in_y[4];
	int in_wires[4];
	int out_wire;
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
	void set_first_coords(int x, int y){
		this->lines[0][0] = x;
		this->lines[0][1] = y;
	}
	void set_last_coords(int x, int y){
		this->lines[this->get_lines_amount() - 1][2] = x;
		this->lines[this->get_lines_amount() - 1][3] = y;
	}
	void get_first_line(int *x1, int *y1, int *x2, int *y2){
		*x1 = this->lines[0][0];
		*y1 = this->lines[0][1];
		*x2 = this->lines[0][2];
		*y2 = this->lines[0][3];
	}
	void get_last_line(int *x1, int *y1, int *x2, int *y2){
		int last = this->get_lines_amount();
		*x1 = this->lines[last - 1][0];
		*y1 = this->lines[last - 1][1];
		*x2 = this->lines[last - 1][2];
		*y2 = this->lines[last - 1][3];
	}
	void set_connected_wire(int value, int index){
		this->connected_wires[index] = value;
	}
	void get_connected_wires(int arr[5]){
		for (int i = 0; i < 5; i++) {
			arr[i] = this->connected_wires[i];
		}
	}
	void set_connected_wires_amount(int value){
		this->connected_wires_amount = value;
	}
	int get_connected_wires_amount(){
		return this->connected_wires_amount;
	}
	void set_parent_wire(int value){
		this->parent_wire = value;
	}
	int get_parent_wire(){
		return this->parent_wire;
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
	void set_in_out_component(int in, int out){
		this->in_component = in;
		this->out_component = out;
	}
	void set_out_component_entry(int out_entry){
		this->out_component_entry = out_entry;
	}
	int get_in_component(){
		return this->in_component;
	}
	int get_out_component(){
		return this->out_component;
	}
	int get_out_component_entry(){
		return this->out_component_entry;
	}

private:
	int lines[10][4];
	int lines_amount;
	int in_component, out_component, out_component_entry;
	int connected_wires[5];
	int connected_wires_amount;
    int parent_wire;
};
#endif
