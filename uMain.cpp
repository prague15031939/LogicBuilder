//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

Component component_array[100];
int component_array_pos = 0;
std::string current_component = "";

Wire wire_array[300];
int wire_array_pos = 0;
int current_wire[10][4];
int current_wire_pos = 0;
TWireStage wire_stage = wsBegin;

int x_dot_highlight = -1;
int y_dot_highlight = -1;

int comp_width = 20, comp_height = 50, wire_length = 10, grid_width = 5;
int entry_coords[4][4] = {{25, 0, 0, 0}, {15, 35, 0, 0}, {10, 25, 40, 0}, {10, 20, 30, 40}};
int move_step;
bool cursor_mode = true;
bool wire_mode = false;
int selected_comp = -1;

std::string file_dir = "";
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}

bool valid_wire_middle(int *x0, int *y0){
	for (int i = 0; i < component_array_pos; i++) {
		int x_in, x_out;
		x_in = component_array[i].get_in_x();
		x_out = component_array[i].get_out_x();
		if (*x0 >= x_in && *x0 <= x_out && *y0 >= component_array[i].get_y() && *y0 <= component_array[i].get_y() + comp_height) {
			return false;
		}
	}
	return true;
}

bool valid_wire_end(int *x0, int *y0){
	for (int i = 0; i < component_array_pos; i++) {
		int x, y[4];
		x = component_array[i].get_in_x();
		component_array[i].get_in_y(y);
		for (int j = 0; j < component_array[i].get_entry_amount(); j++) {
			if (abs(*x0 - x) < grid_width && abs(*y0 - y[j]) < grid_width) {
				*x0 = x;
				*y0 = y[j];
				return true;
			}
		}
	}
	return false;
}

bool valid_wire_start(int *x0, int *y0){
	for (int i = 0; i < component_array_pos; i++) {
		int x, y;
		x = component_array[i].get_out_x();
		y = component_array[i].get_out_y();
		if (abs(*x0 - x) < grid_width && abs(*y0 - y) < grid_width) {
			*x0 = x;
			*y0 = y;
			return true;
		}
	}
	return false;
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

	int num = entity.get_entry_amount();
	int in_y[4] = {0, 0, 0, 0};
	for (int i = 0; i < num; i++) {
		in_y[i] = Y + entry_coords[num - 1][i];
	}
	entity.set_in_y(in_y);
	entity.set_in_x(X - wire_length);
	entity.set_out_x(X + comp_width + wire_length);
	entity.set_out_y(Y + (int)comp_height / 2);
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

void add_wire(int item[10][4]){
	wire_array[wire_array_pos].set_lines(item);
	wire_array[wire_array_pos++].set_lines_amount(current_wire_pos);
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

void draw_dot_highlight(TPaintBox *pb, int x, int y){
	pb -> Canvas -> Brush -> Color = clRed;
	pb -> Canvas -> Pen -> Color = clRed;
	pb -> Canvas -> Pie(x - 3, y - 3, x + 3, y + 3, 0, 0, 0, 0);
	pb -> Canvas -> Brush -> Color = clBlack;
    pb -> Canvas -> Pen -> Color = clBlack;
}

void draw_entries(TPaintBox *pb, Component entity){
	int in_y[4] = {0, 0, 0, 0};
	entity.get_in_y(in_y);
	for (int i = 0; i < entity.get_entry_amount(); i++) {
		pb -> Canvas -> MoveTo(entity.get_x(), in_y[i]);
		pb -> Canvas -> LineTo(entity.get_in_x(), in_y[i]);
	}
}

void draw_component(TPaintBox *pb, Component entity){

	int X, Y;
	X = entity.get_x();
	Y = entity.get_y();
	std::string comp_type = entity.get_type();

	pb -> Canvas -> Rectangle(X, Y, X + comp_width, Y + comp_height);
	pb -> Font -> Size = 8;

	draw_entries(pb, entity);
	pb -> Canvas -> MoveTo(X + comp_width, entity.get_out_y());
	pb -> Canvas -> LineTo(entity.get_out_x(), entity.get_out_y());

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

void draw_wire(TPaintBox *pb, Wire entity){

	int lines_amount;
	lines_amount = entity.get_lines_amount();
	int lines[10][4];
	entity.get_lines(lines);

	for (int i = 0; i < lines_amount; i++) {
		pb -> Canvas -> MoveTo(lines[i][0], lines[i][1]);
		pb -> Canvas -> LineTo(lines[i][2], lines[i][3]);
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
	else if (cursor_mode) {
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
	else if (wire_mode) {

		switch (wire_stage) {
			case wsBegin:
				if (valid_wire_start(&X, &Y)) {
					current_wire[current_wire_pos][0] = X;
					current_wire[current_wire_pos][1] = Y;
					wire_stage = wsMiddle;
				}
				break;

			case wsMiddle:
				if (valid_wire_middle(&X, &Y)) {
					current_wire[current_wire_pos][2] = X;
					current_wire[current_wire_pos][3] = Y;
					current_wire_pos++;
					current_wire[current_wire_pos][0] = X;
					current_wire[current_wire_pos][1] = Y;
				}
				break;

			case wsEnd:
				if (valid_wire_end(&X, &Y)) {
					current_wire[current_wire_pos][2] = X;
					current_wire[current_wire_pos][3] = Y;
					current_wire_pos++;

					add_wire(current_wire);
                    current_wire_pos = 0;
					wire_stage = wsBegin;
					pbMain -> Invalidate();
				}
				break;
		}
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
		draw_component(pbMain, component_array[i]);
	}
	if (selected_comp != -1) {
		draw_highlight(pbMain, selected_comp);
	}
	if (x_dot_highlight != -1 && y_dot_highlight != -1) {
		draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
	}
	for (int i = 0; i < wire_array_pos; i++) {
		draw_wire(pbMain, wire_array[i]);
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
	cursor_mode = false;
	selected_comp = -1;
	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actTakeCursorExecute(TObject *Sender)
{
	cursor_mode = true;
	wire_mode = false;
	current_component = "";
}
//---------------------------------------------------------------------------

Component modify_component_position(Component entity, int new_x, int new_y){

	entity.set_coords(new_x, new_y);
	entity.set_in_x(new_x - wire_length);
	entity.set_out_x(new_x + comp_width + wire_length);
	entity.set_out_y(new_y + (int)comp_height / 2);
	int in_y[4] = {0, 0, 0, 0};
	int num = entity.get_entry_amount();
	for (int i = 0; i < num; i++) {
		in_y[i] = new_y + entry_coords[num - 1][i];
	}
	entity.set_in_y(in_y);

	return entity;
}

void __fastcall TfrmMain::actMoveUpExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x, y - move_step, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x, y - move_step);
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
		if (valid_place(x, y + move_step, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x, y + move_step);
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
		if (valid_place(x - move_step, y, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x - move_step, y);
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
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x + move_step, y);
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
		cursor_mode = true;
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
		cursor_mode = true;
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

void __fastcall TfrmMain::actSetWireModeExecute(TObject *Sender)
{
	wire_mode = true;
    wire_stage = wsBegin;
	cursor_mode = false;
	current_component = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actEndWireExecute(TObject *Sender)
{
	wire_stage = wsEnd;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pbMainMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y)
{
	if (x_dot_highlight != -1 && y_dot_highlight != -1) {
		if (abs(x_dot_highlight - X) > grid_width || abs(y_dot_highlight - Y) > grid_width){
            x_dot_highlight = -1;
			y_dot_highlight = -1;
            pbMain -> Invalidate();
		}
	}
	if (wire_mode && wire_stage == wsBegin) {
		for (int i = 0; i < component_array_pos; i++) {
			int out_x, out_y;
			out_x = component_array[i].get_out_x();
			out_y = component_array[i].get_out_y();
			if (abs(out_x - X) < grid_width && abs(out_y - Y) < grid_width) {
				x_dot_highlight = out_x;
				y_dot_highlight = out_y;
				draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
				break;
			}
		}
	}
	if (wire_mode && wire_stage == wsEnd) {
		for (int i = 0; i < component_array_pos; i++) {
			bool exit = false;
			int in_x, in_y[4];
			in_x = component_array[i].get_in_x();
			component_array[i].get_in_y(in_y);
			for (int j = 0; j < component_array[i].get_entry_amount(); j++) {
				if (abs(in_x - X) < grid_width && abs(in_y[j] - Y) < grid_width) {
					x_dot_highlight = in_x;
					y_dot_highlight = in_y[j];
					draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
                    exit = true;
					break;
				}
				if (exit)
					break;
			}
		}
	}
}
//---------------------------------------------------------------------------

