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
int move_line_buffer[10][4];
int move_line_buffer_pos = 0;
int selected_comp = -1;
int selected_wire = -1;

std::string file_dir = "";
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}
void draw_wire(TPaintBox *pb, Wire entity);

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
		int x, y[4], in_wires[4];
		x = component_array[i].get_in_x();
		component_array[i].get_in_y(y);
		component_array[i].get_in_wires(in_wires);
		for (int j = 0; j < component_array[i].get_entry_amount(); j++) {
			if (abs(*x0 - x) < grid_width && abs(*y0 - y[j]) < grid_width && in_wires[j] == -1) {
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
		int out_wire = component_array[i].get_out_wire();
		if (abs(*x0 - x) < grid_width && abs(*y0 - y) < grid_width && out_wire == -1) {
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

void correct_with_angle(int *x, int *y){
	int x0, y0;
	x0 = current_wire[current_wire_pos][0];
	y0 = current_wire[current_wire_pos][1];
	if (*x - x0 != 0) {
		if (atan((float)abs(*y - y0)/abs(*x - x0)) * 180.0 / 3.1415926 >= 45)
			*x = x0;
		else
			*y = y0;
	}
}

int find_start_component(int x0, int y0){
	for (int i = 0; i < component_array_pos; i++) {
		if (x0 == component_array[i].get_out_x() && y0 == component_array[i].get_out_y()) {
			return i;
		}
	}
	return -1;
}

void find_end_component(int end[2], int x0, int y0){
	for (int i = 0; i < component_array_pos; i++) {
		int in_y[4];
		component_array[i].get_in_y(in_y);
		for (int j = 0; j < component_array[i].get_entry_amount(); j++) {
			if (x0 == component_array[i].get_in_x() && y0 == in_y[j]) {
				end[0] = i;
				end[1] = j;
				return;
			}
		}
	}
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

	for (int i = 0; i < 4; i++) {
		entity.set_in_wire(-1, i);
	}
	entity.set_out_wire(-1);
	component_array[component_array_pos++] = entity;

	frmMain -> Saveas1 -> Enabled = true;
}

void delete_component(int target){
	int in_wires[4];
	component_array[target].get_in_wires(in_wires);
	for (int i = 0; i < 4; i++) {
		if (in_wires[i] != -1) {
			wire_array[in_wires[i]].set_in_out_component(wire_array[in_wires[i]].get_in_component(), -1);
			wire_array[in_wires[i]].set_out_component_entry(-1);
		}
	}
	int out_wire = component_array[target].get_out_wire();
	if (out_wire != -1) {
		wire_array[out_wire].set_in_out_component(-1, wire_array[out_wire].get_out_component());
	}

	for (int i = target; i < component_array_pos - 1; i++) {
		component_array[i] = component_array[i + 1];
	}
	component_array_pos--;

	frmMain -> Saveas1 -> Enabled = true;
}

void add_wire(int item[10][4]){
	int start;
	int end[2] = {-1, -1};
	start = find_start_component(item[0][0], item[0][1]);
	find_end_component(end, item[current_wire_pos][2], item[current_wire_pos][3]);

	component_array[start].set_out_wire(wire_array_pos);
	component_array[end[0]].set_in_wire(wire_array_pos, end[1]);

	wire_array[wire_array_pos].set_in_out_component(start, end[0]);
	wire_array[wire_array_pos].set_out_component_entry(end[1]);
	wire_array[wire_array_pos].set_lines(item);
	wire_array[wire_array_pos++].set_lines_amount(++current_wire_pos);
}

void delete_wire(int target){
	int in_comp = wire_array[target].get_in_component();
	component_array[in_comp].set_out_wire(-1);

	int out_comp = wire_array[target].get_out_component();
	component_array[out_comp].set_in_wire(-1, wire_array[target].get_out_component_entry());

	for (int i = target; i < wire_array_pos - 1; i++) {
		wire_array[i] = wire_array[i + 1];
	}
	wire_array_pos--;

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

void draw_wire_highlight(TPaintBox *pb, int i){

	pb -> Canvas -> Pen -> Color = clRed;
	pb -> Canvas -> Pen -> Width += 1;

	draw_wire(pb, wire_array[i]);

	pb -> Canvas -> Pen -> Color = clBlack;
	pb -> Canvas -> Pen -> Width -= 1;
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

void draw_temp_lines(TPaintBox *pb){
	for (int i = 0; i <= move_line_buffer_pos; i++) {
		pb -> Canvas -> MoveTo(move_line_buffer[i][0], move_line_buffer[i][1]);
		pb -> Canvas -> LineTo(move_line_buffer[i][2], move_line_buffer[i][3]);
	}
}

void __fastcall TfrmMain::pbMainMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	if (!(current_component == "")) {
		round_coords(&X, &Y);
		if (valid_place(X, Y)) {
			add_component(X, Y);
			selected_comp = component_array_pos - 1;
			pbMain -> Invalidate();
		}
	}
	else if (cursor_mode) {
		if (selected_comp != -1){
			pbMain -> Invalidate();
			selected_comp = -1;
		}
		if (selected_wire != -1){
			pbMain -> Invalidate();
			selected_wire = -1;
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
		for (int i = 0; i < wire_array_pos; i++) {
			int lines[10][4];
			wire_array[i].get_lines(lines);
			for (int j = 0; j < wire_array[i].get_lines_amount(); j++) {
				if ((abs(X - lines[j][0]) < 4 && abs(X - lines[j][2]) < 4 &&
					(Y >= lines[j][1] && Y <= lines[j][3] || Y <= lines[j][1] && Y >= lines[j][3])) ||
					(abs(Y - lines[j][1]) < 4 && abs(Y - lines[j][3]) < 4 &&
					(X >= lines[j][0] && X <= lines[j][2] || X <= lines[j][0] && X >= lines[j][2]))) {
					selected_wire = i;
					break;
				}
			}
		}
		pbMain -> Invalidate();
	}
	else if (wire_mode) {

		switch (wire_stage) {
			case wsBegin:
				if (valid_wire_start(&X, &Y)) {
					current_wire[current_wire_pos][0] = X;
					current_wire[current_wire_pos][1] = Y;
					wire_stage = wsMiddle;

					move_line_buffer[move_line_buffer_pos][0] = X;
					move_line_buffer[move_line_buffer_pos][1] = Y;
				}
				break;

			case wsMiddle:
				correct_with_angle(&X, &Y);
				if (valid_wire_middle(&X, &Y)) {
					current_wire[current_wire_pos][2] = X;
					current_wire[current_wire_pos][3] = Y;
					current_wire_pos++;
					current_wire[current_wire_pos][0] = X;
					current_wire[current_wire_pos][1] = Y;

					move_line_buffer[move_line_buffer_pos][2] = X;
					move_line_buffer[move_line_buffer_pos][3] = Y;
					move_line_buffer_pos++;
					move_line_buffer[move_line_buffer_pos][0] = X;
					move_line_buffer[move_line_buffer_pos][1] = Y;
				}
				break;

			case wsEnd:
				if (valid_wire_end(&X, &Y)) {
					if (move_line_buffer[move_line_buffer_pos][1] == move_line_buffer[move_line_buffer_pos][3]) {
						current_wire[current_wire_pos - 1][3] = Y;
						current_wire[current_wire_pos][1] = Y;
					}
					if (move_line_buffer[move_line_buffer_pos][0] == move_line_buffer[move_line_buffer_pos][2]) {
						current_wire[current_wire_pos - 1][2] = X;
						current_wire[current_wire_pos][0] = X;
					}
					current_wire[current_wire_pos][2] = X;
					current_wire[current_wire_pos][3] = Y;

					move_line_buffer[move_line_buffer_pos][2] = X;
					move_line_buffer[move_line_buffer_pos][3] = Y;

					add_wire(current_wire);
					current_wire_pos = 0;
					wire_stage = wsBegin;
					pbMain -> Invalidate();
					move_line_buffer_pos = 0;
				}
				break;
		}
	}
}

void save_to_file(std::string dest){
	ofstream file_obj;
	file_obj.open(dest.c_str(), ios::out | ios::trunc | ios::binary);

	file_obj.write((char*)&component_array_pos, sizeof(int));
	for (int i = 0; i < component_array_pos; i++) {
		file_obj.write((char*)&component_array[i], sizeof(component_array[i]));
	}

	file_obj.write((char*)&wire_array_pos, sizeof(int));
	for (int i = 0; i < wire_array_pos; i++) {
		file_obj.write((char*)&wire_array[i], sizeof(wire_array[i]));
	}
	file_obj.close();
}

void open_file(std::string src){
   ifstream file_obj;
   file_obj.open(src.c_str(), ios::in | ios::binary);
   component_array_pos = 0;
   wire_array_pos = 0;

   file_obj.read((char*)&component_array_pos, sizeof(int));
   for (int i = 0; i < component_array_pos; i++) {
		file_obj.read((char*)&component_array[i], sizeof(component_array[i]));
   }

   file_obj.read((char*)&wire_array_pos, sizeof(int));
   for (int i = 0; i < wire_array_pos; i++) {
		file_obj.read((char*)&wire_array[i], sizeof(wire_array[i]));
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
	if (selected_wire != -1) {
		draw_wire_highlight(pbMain, selected_wire);
	}
	if (wire_mode && wire_stage != wsBegin) {
		draw_temp_lines(pbMain);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	Save1 -> Enabled = false;
	Saveas1 -> Enabled = false;
	move_step = 2 * grid_width;
	frmMain -> DoubleBuffered = true;
	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::lboxComponentsDblClick(TObject *Sender)
{
	current_component = AnsiString(lboxComponents -> Items -> Strings[lboxComponents -> ItemIndex]).c_str();
	cursor_mode = false;
	selected_comp = -1;
    selected_wire = -1;
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

	int out_wire = entity.get_out_wire();
	if (out_wire != -1) {
		wire_array[out_wire].set_first_coords(new_x + comp_width + wire_length, new_y + (int)comp_height / 2);
	}
	int in_wires[4] = {-1, -1, -1, -1};
	entity.get_in_wires(in_wires);
	for (int i = 0; i < num; i++) {
		if (in_wires[i] != -1) {
			wire_array[in_wires[i]].set_last_coords(new_x - wire_length, new_y + entry_coords[num - 1][i]);
		}
	}

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
	}
	if (selected_wire != -1) {
		delete_wire(selected_wire);
		selected_wire = -1;
	}
	cursor_mode = true;
	current_component = "";
	pbMain -> Invalidate();
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
		selected_wire = -1;
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
        frmMain -> Caption = ("LogicBuilder - " + file_dir).c_str();
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
			int out_wire = component_array[i].get_out_wire();
			if (abs(out_x - X) < grid_width && abs(out_y - Y) < grid_width && out_wire == -1) {
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
			int in_x, in_y[4], in_wires[4];
			in_x = component_array[i].get_in_x();
			component_array[i].get_in_y(in_y);
			component_array[i].get_in_wires(in_wires);
			for (int j = 0; j < component_array[i].get_entry_amount(); j++) {
				if (abs(in_x - X) < grid_width && abs(in_y[j] - Y) < grid_width && in_wires[j] == -1) {
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

	if (wire_mode) {
		correct_with_angle(&X, &Y);
		move_line_buffer[move_line_buffer_pos][2] = X;
		move_line_buffer[move_line_buffer_pos][3] = Y;
		pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::actNewFileExecute(TObject *Sender)
{
	Save1 -> Enabled = false;
	Saveas1 -> Enabled = false;

	component_array_pos = 0;
	current_component = "";

	wire_array_pos = 0;
	current_wire_pos = 0;
	wire_stage = wsBegin;

	x_dot_highlight = -1;
	y_dot_highlight = -1;

	cursor_mode = true;
	wire_mode = false;
	move_line_buffer_pos = 0;
	selected_comp = -1;
	selected_wire = -1;
	file_dir = "";

	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

