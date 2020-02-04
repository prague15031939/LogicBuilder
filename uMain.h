//---------------------------------------------------------------------------

#pragma once

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
	TMenuItem *Tools1;
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
	TMenuItem *btnCursor;
	TMenuItem *btnWire;
	TMenuItem *btnBranchWire;
	TMenuItem *btnEndWire;
	TMenuItem *actDeleteComponent1;
	TMenuItem *N1;
	TMenuItem *N2;
	TAction *actDrawGrid;
	TMenuItem *actDrawGrid1;
	TAction *actSetModelMode;
	TMenuItem *actSetModelMode1;
	TMenuItem *DeleteObjectTree1;
	TAction *actDeleteObjectTree;
	TAction *actUndo;
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
	void __fastcall actDrawGridExecute(TObject *Sender);
	void __fastcall actSetModelModeExecute(TObject *Sender);
	void __fastcall pbMainMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall actDeleteObjectTreeExecute(TObject *Sender);
	void __fastcall actUndoExecute(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------

#endif
