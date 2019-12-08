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
int comp_width = 20, comp_height = 50, wire_length = 10, grid_width = 5;
std::string current_component = "";
bool cursor = true;
int selected_comp = -1;
int move_step;
std::string file_dir = "";
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

	frmMain -> Saveas1 -> Enabled = true;
}

void delete_component(int target){
	for (int i = target; i < component_array_pos - 1; i++) {
		component_array[i] = component_array[i + 1];
	}
	component_array_pos--;

    frmMain -> Saveas1 -> Enabled = true;
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

void draw_entries(TPaintBox *pb, int X, int Y, int entry_amount){
	int entry_coords[4][4] = {{25, 0, 0, 0}, {15, 35, 0, 0}, {10, 25, 40, 0}, {10, 20, 30, 40}};
	for (int i = 0; i < entry_amount; i++) {
		pb -> Canvas -> MoveTo(X, Y + entry_coords[entry_amount - 1][i]);
		pb -> Canvas -> LineTo(X - wire_length, Y + entry_coords[entry_amount - 1][i]);
	}
}

void draw_component(TPaintBox *pb, int X, int Y, std::string comp_type, int entry_amount = 2){

	pb -> Canvas -> Rectangle(X, Y, X + comp_width, Y + comp_height);
	pb -> Font -> Size = 8;

	draw_entries(pb, X, Y, entry_amount);
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

void save_to_file(std::string dest){
	ofstream file_obj;
	file_obj.open(dest.c_str(), ios::out | ios::trunc | ios::binary);
	for (int i = 0; i < component_array_pos; i++) {
		file_obj.write((char*)&component_array[i], sizeof(component_array[i]));
	}
	file_obj.close();
}

void open_file(std::string src){
   ifstream file_obj;
   file_obj.open(src.c_str(), ios::in | ios::binary);
   component_array_pos = 0;
   while (!file_obj.eof()){
		file_obj.read((char*)&component_array[component_array_pos], sizeof(component_array[component_array_pos]));
		component_array_pos++;
   }
   component_array_pos--;
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
		memoDbg->Lines->Add(component_array[i].get_x());
		memoDbg->Lines->Add(component_array[i].get_y());
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
		int entry_amount = component_array[i].get_entry_amount();
		draw_component(pbMain, X, Y, comp_type, entry_amount);
	}
	if (selected_comp != -1) {
		draw_highlight(pbMain, selected_comp);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	Save1 -> Enabled = false;
	Saveas1 -> Enabled = false;
	pbMain -> Invalidate();
	move_step = 2 * grid_width;
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

void pick_component(TPaintBox *pb, std::string src){
	current_component = src;
	cursor = false;
	selected_comp = -1;
	pb -> Invalidate();
}

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
		if (valid_place(x, y - 2*grid_width, selected_comp)) {
			component_array[selected_comp].set_coords(x, y - move_step);
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
		if (valid_place(x, y + 2*grid_width, selected_comp)) {
			component_array[selected_comp].set_coords(x, y + move_step);
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
		if (valid_place(x - 2*grid_width, y, selected_comp)) {
			component_array[selected_comp].set_coords(x - move_step, y);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveRightExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x + move_step, y, selected_comp)) {
			component_array[selected_comp].set_coords(x + move_step, y);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actDeleteComponentExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		delete_component(selected_comp);
		selected_comp = -1;
		cursor = true;
		current_component = "";
		pbMain -> Invalidate();
	}
}

void __fastcall TfrmMain::actCheangeMoveStepExecute(TObject *Sender)
{
	int coef = ((int)move_step / grid_width + 1) % 5;
	if (coef == 0)
		coef = 1;
	move_step = grid_width * coef;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSaveFileExecute(TObject *Sender)
{
	if (file_dir != "") {
		save_to_file(file_dir);
        Save1 -> Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actOpenFileExecute(TObject *Sender)
{
	if (OpenDialog -> Execute()) {
		file_dir = AnsiString(OpenDialog -> FileName).c_str();
		open_file(file_dir);
		frmMain -> Caption = ("LogicBuilder - " + file_dir).c_str();
		Save1 -> Enabled = true;
		selected_comp = -1;
		current_component = "";
		cursor = true;
		pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSaveFileAsExecute(TObject *Sender)
{
	if (SaveDialog -> Execute()) {
		file_dir = AnsiString(SaveDialog -> FileName).c_str();
		save_to_file(file_dir);
        Save1 -> Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actExitExecute(TObject *Sender)
{
    frmMain -> Close();
}
//---------------------------------------------------------------------------

