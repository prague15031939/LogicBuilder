//---------------------------------------------------------------------------

#pragma hdrstop
#pragma package(smart_init)

#include "uValidation.h"
#include "uComponent.h"
#include "uWire.h"
#include "uCommon.h"

extern Component component_array[100];
extern ModelComponent model_component_array[100];
extern int component_array_pos;
extern std::string current_component;

extern Wire wire_array[300];
extern int wire_array_pos;
extern int current_wire[10][4];
extern int current_wire_pos;
typedef enum {wsBegin, wsMiddle, wsEnd} TWireStage;
extern TWireStage wire_stage;

extern int x_dot_highlight;
extern int y_dot_highlight;

extern int comp_width, comp_height, wire_length, grid_width;
extern int entry_coords[4][4];
extern int move_step;
extern bool cursor_mode;
extern bool model_mode;
extern bool wire_mode;
extern bool branch_wire_mode;
extern int parent_wire;
extern int move_line_buffer[10][4];
extern int move_line_buffer_pos;
extern int selected_comp;
extern int selected_wire;

extern std::string file_dir;


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

bool valid_place(int x0, int y0, int except){
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
