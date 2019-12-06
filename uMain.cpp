//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
Component component_array[100];
static int component_array_pos = 0;
int comp_width = 20, comp_height = 50, wire_length = 10, grid_width = 10;
std::string current_component = "";
bool cursor = true;
int selected_comp = -1;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}

bool valid_place(int x0, int y0, int except = -1){
	bool res = true;
	for (int i = 0; i < component_array_pos; i++) {
		if (i == except) {
			continue;
		}
		int x, y;
		x = component_array[i].get_x();
		y = component_array[i].get_y();
		if ((x0 >= x - wire_length && x0 <= x + comp_width + 2 * wire_length && y0 >= y && y0 <= y + comp_height) ||
			(x0 + comp_width + wire_length >= x - wire_length && x0 + comp_width + wire_length <= x + comp_width + 2 * wire_length && y0 >= y && y0 <= y + comp_height) ||
			(x0 >= x - wire_length && x0 <= x + comp_width + 2 * wire_length && y0 + comp_height >= y && y0 + comp_height <= y + comp_height) ||
			(x0 + comp_width + wire_length>= x - wire_length && x0 + comp_width + wire_length <= x + comp_width + 2 * wire_length && y0 + comp_height >= y && y0 + comp_height <= y + comp_height)){
			res = false;
		}
	}
	return res;
}

void draw_grid(TPaintBox *pb){
	pb -> Canvas -> Pen -> Color = clBtnFace;
	int x_line = 0, y_line = 0;
	for (int i = 0; i < (int)pb->Width / grid_width; i++) {
		pb -> Canvas -> MoveTo(x_line, 0);
		pb -> Canvas -> LineTo(x_line, pb->Height);
		x_line += grid_width;
	}
	for (int i = 0; i < (int)pb->Height / grid_width; i++) {
		pb -> Canvas -> MoveTo(0, y_line);
		pb -> Canvas -> LineTo(pb -> Width, y_line);
		y_line += grid_width;
	}
	pb -> Canvas -> Pen -> Color = clBlack;
}

void round_coords(int* X, int* Y){
	while (!(*X % grid_width == 0))
		(*X)--;
	while (!(*Y % grid_width == 0))
		(*Y)--;
	if (!valid_place(*X, *Y)) {
		(*X) += grid_width;
	}
	else
		return;
	if (!valid_place(*X, *Y)) {
		(*X) += grid_width;
	}
	else
		return;
	if (!valid_place(*X, *Y)) {
		(*Y) += 2 * grid_width;
	}
	else
		return;
}

std::string fetch_component_name(std::string src){
	std::string res = "";
	int i = 0;
	while (!(src[i] >= '0' && src[i] <= '9'))
		res.push_back(src[i++]);
	return res;
}

void add_component(int X, int Y){
	Component entity;
	if (!(current_component == "not")) {
		entity.set_type(fetch_component_name(current_component));
		entity.set_entry_amount(current_component[current_component.length() - 1] - '0');
	}
	else{
		entity.set_type("not");
		entity.set_entry_amount(1);
	}
	entity.set_coords(X, Y);
	component_array[component_array_pos++] = entity;
}

void draw_highlight(TPaintBox *pb, int i){

	pb -> Canvas -> Pen -> Color = clHighlight;
	pb -> Canvas -> Pen -> Style = psDash;

	int x, y;
	x = component_array[i].get_x();
	y = component_array[i].get_y();
	pb -> Canvas -> MoveTo(x - wire_length - 1, y - 1);
	pb -> Canvas -> LineTo(x + comp_width + wire_length + 1, y - 1);
	pb -> Canvas -> LineTo(x + comp_width + wire_length + 1, y + comp_height + 1);
	pb -> Canvas -> LineTo(x - wire_length - 1, y + comp_height + 1);
	pb -> Canvas -> LineTo(x - wire_length - 1, y - 1);

	pb -> Canvas -> Pen -> Color = clBlack;
	pb -> Canvas -> Pen -> Style = psSolid;
}

void draw_component(TPaintBox *pb, int X, int Y, std::string comp_type){

	pb -> Canvas -> Rectangle(X, Y, X + comp_width, Y + comp_height);
	pb -> Font -> Size = 8;

	if (comp_type == "not") {
		pb -> Canvas -> MoveTo(X, Y + (int)comp_height / 2);
		pb -> Canvas -> LineTo(X - wire_length, Y + (int)comp_height / 2);
	}
	else{
		//memoDbg -> Lines -> Add(component_array[i].get_type().c_str());
		pb -> Canvas -> MoveTo(X, Y + (int)comp_height / 3);
		pb -> Canvas -> LineTo(X - wire_length, Y + (int)comp_height / 3);
		pb -> Canvas -> MoveTo(X, Y + (int)2 * comp_height / 3);
		pb -> Canvas -> LineTo(X - wire_length, Y + (int)2 * comp_height / 3);
	}
	pb -> Canvas -> MoveTo(X + comp_width, Y + (int)comp_height / 2);
	pb -> Canvas -> LineTo(X + comp_width + wire_length, Y + (int)comp_height / 2);

	if (comp_type == "and" || comp_type == "nand") {
		pb -> Canvas -> TextOut(X + 6, Y + 5, "&");
	}
	else if (comp_type == "or" || comp_type == "nor") {
		pb -> Canvas -> TextOut(X + 7, Y + 5, "1");
		}
		else if (comp_type == "xor" || comp_type == "nxor") {
			pb -> Canvas -> TextOut(X + 2, Y + 5, "=1");
			}
	if (comp_type[0] == 'n') {
		pb -> Canvas -> Ellipse(X + comp_width - 3, (int)Y + comp_height / 2 - 3, X + comp_width + 3, (int)Y + comp_height / 2 + 3);
	}
}

void __fastcall TfrmMain::pbMainMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	if (!(current_component == "")) {
		round_coords(&X, &Y);
		if (valid_place(X, Y)) {
			add_component(X, Y);
			//draw_component(pbMain, X, Y, fetch_component_name(current_component));
			selected_comp = component_array_pos - 1;
			pbMain -> Invalidate();
			//draw_highlight(pbMain, selected_comp);
		}
	}
	else if (cursor) {
		if (selected_comp != -1){
			pbMain -> Invalidate();
			selected_comp = -1;
		}
		for (int i = 0; i < component_array_pos; i++) {
			int X0, Y0;
			X0 = component_array[i].get_x();
			Y0 = component_array[i].get_y();
			if (X >= X0 && X <= X0 + comp_width && Y >= Y0 && Y <= Y0 + comp_height) {
				selected_comp = i;
				break;
			}
		}
		if (selected_comp != -1) {
			draw_highlight(pbMain, selected_comp);
		}
		else
			pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnDbgClick(TObject *Sender)
{
	memoDbg->Lines->Clear();
	for (int i = 0; i < component_array_pos; i++) {
		memoDbg->Lines->Add(component_array[i].get_type().c_str());
		memoDbg->Lines->Add(component_array[i].get_entry_amount());
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pbMainPaint(TObject *Sender)
{
	draw_grid(pbMain);
	for (int i = 0; i < component_array_pos; i++) {
		int X, Y;
		X = component_array[i].get_x();
		Y = component_array[i].get_y();
		std::string comp_type = component_array[i].get_type();
		draw_component(pbMain, X, Y, comp_type);
	}
	if (selected_comp != -1) {
		draw_highlight(pbMain, selected_comp);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::lboxComponentsDblClick(TObject *Sender)
{
	current_component = AnsiString(lboxComponents -> Items -> Strings[lboxComponents -> ItemIndex]).c_str();
	cursor = false;
	selected_comp = -1;
	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::actTakeCursorExecute(TObject *Sender)
{
	cursor = true;
	current_component = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveUpExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x, y - grid_width, selected_comp)) {
			component_array[selected_comp].set_coords(x, y - grid_width);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveDownExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x, y + grid_width, selected_comp)) {
			component_array[selected_comp].set_coords(x, y + grid_width);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveLeftExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x - grid_width, y, selected_comp)) {
			component_array[selected_comp].set_coords(x - grid_width, y);
		}
		pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveRightExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x + grid_width, y, selected_comp)) {
			component_array[selected_comp].set_coords(x + grid_width, y);
		}
		pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------

