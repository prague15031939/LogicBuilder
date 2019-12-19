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
bool branch_wire_mode = false;
int parent_wire = -1;
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
				if (current_wire_pos == 0 && (*x0 != current_wire[0][0] && *y0 != current_wire[0][1] || *y0 != current_wire[0][1] && *x0 != current_wire[0][0]))
					return false;
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

int valid_branch_wire_start(int *x0, int *y0){

	for (int i = 0; i < wire_array_pos; i++) {
		int lines[10][4];
		wire_array[i].get_lines(lines);
		for (int j = 0; j < wire_array[i].get_lines_amount(); j++) {
			if ((abs(*x0 - lines[j][0]) < grid_width && abs(*x0 - lines[j][2]) < grid_width &&
				(*y0 >= lines[j][1] && *y0 <= lines[j][3] || *y0 <= lines[j][1] && *y0 >= lines[j][3])) ||
				(abs(*y0 - lines[j][1]) < grid_width && abs(*y0 - lines[j][3]) < grid_width &&
				(*x0 >= lines[j][0] && *x0 <= lines[j][2] || *x0 <= lines[j][0] && *x0 >= lines[j][2]))) {

				if (lines[j][2] - lines[j][0] != 0){
					*y0 = lines[j][1];
				}
				else if (lines[j][3] - lines[j][1] != 0) {
					 *x0 = lines[j][2];
					 }
				return i;
			}
		}
	}
	return -1;
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

void delete_in_component_in_all_connected_wires(int out_wire){

	int connected_wires[5];
	wire_array[out_wire].get_connected_wires(connected_wires);
	for (int i = 0; i < wire_array[out_wire].get_connected_wires_amount(); i++) {
		wire_array[connected_wires[i]].set_in_out_component(-1, wire_array[connected_wires[i]].get_out_component());
		delete_in_component_in_all_connected_wires(connected_wires[i]);
	}

}

void decrease_components_index(int index){

	for (int i = 0; i < wire_array_pos; i++) {
		int in_comp, out_comp;
		in_comp = wire_array[i].get_in_component();
		out_comp = wire_array[i].get_out_component();
		if (in_comp != -1 && in_comp >= index)
			in_comp--;
		if (out_comp != -1 && out_comp >= index)
			out_comp--;
		if (out_comp == -1)
			wire_array[i].set_out_component_entry(-1);
		wire_array[i].set_in_out_component(in_comp, out_comp);
	}
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
		delete_in_component_in_all_connected_wires(out_wire);
	}

	for (int i = target; i < component_array_pos - 1; i++) {
		component_array[i] = component_array[i + 1];
	}
	component_array_pos--;
	decrease_components_index(target);

	frmMain -> Saveas1 -> Enabled = true;
}

void add_wire(int item[10][4]){
	int start;
	int end[2] = {-1, -1};
	if (!branch_wire_mode)
		start = find_start_component(item[0][0], item[0][1]);
	else {
		start = wire_array[parent_wire].get_in_component();
		int temp = wire_array[parent_wire].get_connected_wires_amount();
		wire_array[parent_wire].set_connected_wire(wire_array_pos, temp++);
		wire_array[parent_wire].set_connected_wires_amount(temp);
	}
	find_end_component(end, item[current_wire_pos][2], item[current_wire_pos][3]);

	if (!branch_wire_mode) {
		component_array[start].set_out_wire(wire_array_pos);
		wire_array[wire_array_pos].set_parent_wire(-1);
	}
	else
        wire_array[wire_array_pos].set_parent_wire(parent_wire);
	component_array[end[0]].set_in_wire(wire_array_pos, end[1]);

	for (int i = 0; i < 5; i++) {
		wire_array[wire_array_pos].set_connected_wire(-1, i);
	}
	wire_array[wire_array_pos].set_connected_wires_amount(0);

	wire_array[wire_array_pos].set_in_out_component(start, end[0]);
	wire_array[wire_array_pos].set_out_component_entry(end[1]);
	wire_array[wire_array_pos].set_lines(item);
	wire_array[wire_array_pos++].set_lines_amount(++current_wire_pos);
}

void decrease_wires_index(int index){

	for (int i = 0; i < component_array_pos; i++) {
		int out_wire;
		out_wire = component_array[i].get_out_wire();
		if (out_wire != -1 && out_wire >= index)
			component_array[i].set_out_wire(out_wire - 1);
		int in_wires[4];
		component_array[i].get_in_wires(in_wires);
		for (int j = 0; j < component_array[i].get_entry_amount(); j++)
			if (in_wires[j] != -1 && in_wires[j] >= index)
				component_array[i].set_in_wire(in_wires[j] - 1, j);
	}

	for (int i = 0; i < wire_array_pos; i++) {
		int parent_wire = wire_array[i].get_parent_wire();
		if (parent_wire != -1 && parent_wire >= index)
			wire_array[i].set_parent_wire(parent_wire - 1);
		int connected_wires[5];
		wire_array[i].get_connected_wires(connected_wires);
		for (int j = 0; j < wire_array[i].get_connected_wires_amount(); j++)
			if (connected_wires[j] != -1 && connected_wires[j] >= index)
				connected_wires[j]--;

		int k = 0;
		for (int j = 0; j < 5; j++)
			if (connected_wires[j] != -1)
				wire_array[i].set_connected_wire(connected_wires[j], k++);
		wire_array[i].set_connected_wires_amount(k);
		for (k; k < 5; k++)
			wire_array[i].set_connected_wire(-1, k);
	}
}

void delete_wire(int target){

	delete_in_component_in_all_connected_wires(target);

	int connected_wires[5];
	wire_array[target].get_connected_wires(connected_wires);
	for (int i = 0; i < wire_array[target].get_connected_wires_amount(); i++)
		wire_array[connected_wires[i]].set_parent_wire(-1);

	int parent = wire_array[target].get_parent_wire();
	if (parent != -1) {
		int connected_wires[5];
		wire_array[parent].get_connected_wires(connected_wires);
		for (int i = 0; i < wire_array[parent].get_connected_wires_amount(); i++) {
			if (connected_wires[i] == target) {
				connected_wires[i] = -1;
				int k = 0;
				for (int j = 0; j < 5; j++)
					if (connected_wires[j] != -1)
						wire_array[parent].set_connected_wire(connected_wires[j], k++);
				for (k; k < 5; k++)
					wire_array[parent].set_connected_wire(-1, k);
				wire_array[parent].set_connected_wires_amount(wire_array[parent].get_connected_wires_amount() - 1);
				break;
			}
		}
	}

	if (parent == -1) {
		int in_comp = wire_array[target].get_in_component();
		if (in_comp != -1)
			component_array[in_comp].set_out_wire(-1);
	}

	int out_comp = wire_array[target].get_out_component();
	int in_wires[4];
	component_array[out_comp].get_in_wires(in_wires);
	for (int i = 0; i < component_array[out_comp].get_entry_amount(); i++) {
		if (in_wires[i] == target) {
			component_array[out_comp].set_in_wire(-1, i);
			break;
		}
	}

	for (int i = target; i < wire_array_pos - 1; i++) {
		wire_array[i] = wire_array[i + 1];
	}
	wire_array_pos--;
	decrease_wires_index(target);

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
				if (branch_wire_mode)
					parent_wire = valid_branch_wire_start(&X, &Y);
				if (valid_wire_start(&X, &Y) && !branch_wire_mode || branch_wire_mode && parent_wire != -1) {
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
   file_obj.close();

}
//---------------------------------------------------------------------------

void to_svg(std::string dest){
	ofstream file_obj;
	file_obj.open(dest.c_str(), ios::out | ios::trunc);

	file_obj << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" <<
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" <<
		"<svg version=\"1.1\" baseProfile=\"full\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:ev=\"http://www.w3.org/2001/xml-events\" width=\"100%\" height=\"100%\">\n";

	char temp_str[300];
	for (int i = 0; i < component_array_pos; i++) {
		Component entity = component_array[i];
		sprintf(temp_str, "<rect fill=\"none\" style=\"stroke:black;stroke-width:1\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n", entity.get_x(), entity.get_y(), comp_width, comp_height);
		file_obj << temp_str;
		int in_y[4] = {0, 0, 0, 0};
		entity.get_in_y(in_y);
		for (int i = 0; i < entity.get_entry_amount(); i++) {
			sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", entity.get_x(), in_y[i], entity.get_in_x(), in_y[i]);
			file_obj << temp_str;
		}
		sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", entity.get_x() + comp_width, entity.get_out_y(), entity.get_out_x(), entity.get_out_y());
		file_obj << temp_str;

		std::string comp_type = entity.get_type();
		if (comp_type[0] == 'n') {
			sprintf(temp_str, "<circle cx=\"%dpx\" cy=\"%dpx\" r=\"%dpx\" fill=\"white\" stroke=\"black\" />", entity.get_x() + comp_width, entity.get_out_y(), 3);
			file_obj << temp_str;
		}

		if (comp_type == "and" || comp_type == "nand") {
			sprintf(temp_str, "<text x=\"%d\" y=\"%d\" font-size=\"14\" font-family=\"Arial\">%s</text>\n", entity.get_x() + 6, entity.get_y() + 20, "&amp;");
		}
		else if (comp_type == "or" || comp_type == "nor") {
			sprintf(temp_str, "<text x=\"%d\" y=\"%d\" font-size=\"14\" font-family=\"Arial\">%s</text>\n", entity.get_x() + 6, entity.get_y() + 20, "1");
			}
			else if (comp_type == "xor" || comp_type == "nxor") {
				sprintf(temp_str, "<text x=\"%d\" y=\"%d\" font-size=\"13\" font-family=\"Arial\">%s</text>\n", entity.get_x() + 3, entity.get_y() + 20, "=1");
				}
		if (comp_type != "not") {
			file_obj << temp_str;
		}

	}

	for (int i = 0; i < wire_array_pos; i++) {
		int lines[10][4];
		wire_array[i].get_lines(lines);
		for (int j = 0; j < wire_array[i].get_lines_amount(); j++) {
			sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", lines[j][0], lines[j][1], lines[j][2], lines[j][3]);
			file_obj << temp_str;
		}
	}

	file_obj << "</svg>";
	file_obj.close();
}

void logger(){
	ofstream file_obj;
	file_obj.open("log.txt", ios::out | ios::trunc);

	for (int i = 0; i < component_array_pos; i++) {
		file_obj << "component #" << i << ":\n";
		file_obj << component_array[i].get_type() << " " << component_array[i].get_entry_amount() << "\n";
		int in_wires[4];
		component_array[i].get_in_wires(in_wires);
		file_obj << "in wires: " << in_wires[0] << " " << in_wires[1] << " "  << in_wires[2] << " "  << in_wires[3] << "\n";
		file_obj << "out wire: " << component_array[i].get_out_wire() << "\n\n";
	}

	for (int i = 0; i < wire_array_pos; i++) {
		file_obj << "wire #" << i << ":\n";
		file_obj << "in_component: " << wire_array[i].get_in_component() << "\n";
		file_obj << "out_component: " << wire_array[i].get_out_component() << ", entry: " << wire_array[i].get_out_component_entry() << "\n";
		int connected_wires[5];
		wire_array[i].get_connected_wires(connected_wires);
		file_obj << "connected wires: " << connected_wires[0] << " " << connected_wires[1] << " "  << connected_wires[2] << " "  << connected_wires[3] << " " << connected_wires[4] << "\n";
		file_obj << "parent wire: " << wire_array[i].get_parent_wire() << "\n\n";
	}
	file_obj.close();
}

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
	OpenDialog -> Filter = "LogicBuilder filse(.lb)|*.lb|";
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
	branch_wire_mode = false;
	current_component = "";
	logger();
}
//---------------------------------------------------------------------------

bool valid_local_line_is_alone(int target_wire, int mode){
	if (wire_array[target_wire].get_connected_wires_amount() != 0) {
		int connected_wires[5];
		wire_array[target_wire].get_connected_wires(connected_wires);
		int x01, y01, x02, y02;
		if (mode == 1)
			wire_array[target_wire].get_first_line(&x01, &y01, &x02, &y02);
		else
			wire_array[target_wire].get_last_line(&x01, &y01, &x02, &y02);
		for (int j = 0; j < wire_array[target_wire].get_connected_wires_amount(); j++) {
			int x1, y1, x2, y2;
			wire_array[connected_wires[j]].get_first_line(&x1, &y1, &x2, &y2);
			if ((y1 >= y01 && y1 <= y02 || y1 <= y01 && y1 >= y01) &&
				(x1 >= x01 && x1 <= x02 || x1 <= x01 && x1 >= x01)) {
				return false;
			}
		}
	}
	return true;
}

Component modify_component_position(Component entity, int new_x, int new_y){

	int out_wire = entity.get_out_wire();
	if (out_wire != -1)
		if (!valid_local_line_is_alone(out_wire, 1))
			return entity;

	int in_wires[4] = {-1, -1, -1, -1};
	entity.get_in_wires(in_wires);
	int num = entity.get_entry_amount();
	for (int i = 0; i < num; i++)
		if (in_wires[i] != -1)
			if (!valid_local_line_is_alone(in_wires[i], 0))
				return entity;

	for (int i = 0; i < num; i++)
		if (in_wires[i] != -1)
			wire_array[in_wires[i]].set_last_coords(new_x - wire_length, new_y + entry_coords[num - 1][i]);
	if (out_wire != -1)
		wire_array[out_wire].set_first_coords(new_x + comp_width + wire_length, new_y + (int)comp_height / 2);

	entity.set_coords(new_x, new_y);
	entity.set_out_x(new_x + comp_width + wire_length);
	entity.set_out_y(new_y + (int)comp_height / 2);
	entity.set_in_x(new_x - wire_length);

	int in_y[4] = {0, 0, 0, 0};
	for (int i = 0; i < num; i++)
		in_y[i] = new_y + entry_coords[num - 1][i];
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
		save_to_file(file_dir + ".lb");
        frmMain -> Caption = ("LogicBuilder - " + file_dir + ".lb").c_str();
		Save1 -> Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSVGExportExecute(TObject *Sender)
{
	if (SaveDialog -> Execute()) {
		file_dir = AnsiString(SaveDialog -> FileName).c_str();
        to_svg(file_dir);
	}
}

void __fastcall TfrmMain::actExitExecute(TObject *Sender)
{
	frmMain -> Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSetWireModeExecute(TObject *Sender)
{
	wire_mode = true;
	branch_wire_mode = false;
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
		if (!branch_wire_mode) {
			if (valid_wire_start(&X, &Y)) {
				x_dot_highlight = X;
				y_dot_highlight = Y;
				draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
			}
		}
		else{
			if (valid_branch_wire_start(&X, &Y) != -1) {
				x_dot_highlight = X;
				y_dot_highlight = Y;
				draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
			}
        }
	}

	if (wire_mode && wire_stage == wsEnd) {
		if (valid_wire_end(&X, &Y)) {
			x_dot_highlight = X;
			y_dot_highlight = Y;
			draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
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

void __fastcall TfrmMain::actBranchWireExecute(TObject *Sender)
{
	wire_mode = true;
    branch_wire_mode = true;
	wire_stage = wsBegin;
	cursor_mode = false;
	current_component = "";
}
//---------------------------------------------------------------------------

