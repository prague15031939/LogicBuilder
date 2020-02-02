//---------------------------------------------------------------------------

#pragma hdrstop
#pragma package(smart_init)

#include "uValidation.h"
#include "uComponent.h"
#include "uWire.h"
#include "uCommon.h"

extern Component component_array[100];
extern int component_array_pos;
extern std::string current_component;

extern Wire wire_array[300];
extern int wire_array_pos;
extern int current_wire[10][4];
extern int current_wire_pos;

extern int comp_width, comp_height, wire_length, grid_width;
extern int entry_coords[4][4];
extern int picked_line_num;

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
				return true;
			}
		}
	}
	return false;
}

int valid_branch_wire_start(int *x0, int *y0){

	picked_line_num = -1;
	for (int i = 0; i < wire_array_pos; i++) {
		int lines[10][4];
		wire_array[i].get_lines(lines);
		for (int j = 0; j < wire_array[i].get_lines_amount(); j++) {
			if ((abs(*x0 - lines[j][0]) < grid_width && abs(*x0 - lines[j][2]) < grid_width &&
				(*y0 >= lines[j][1] && *y0 <= lines[j][3] || *y0 <= lines[j][1] && *y0 >= lines[j][3])) ||
				(abs(*y0 - lines[j][1]) < grid_width && abs(*y0 - lines[j][3]) < grid_width &&
				(*x0 >= lines[j][0] && *x0 <= lines[j][2] || *x0 <= lines[j][0] && *x0 >= lines[j][2]))) {

				picked_line_num = j;
				if (lines[j][2] - lines[j][0] != 0)
					*y0 = lines[j][1];
				else if (lines[j][3] - lines[j][1] != 0)
					*x0 = lines[j][2];
				return i;
			}
		}
	}
	return -1;
}

bool valid_line_is_alone(int target_wire, int target_line_num) {
	if (wire_array[target_wire].get_connected_wires_amount() != 0) {
		int connected_wires[5];
		wire_array[target_wire].get_connected_wires(connected_wires);
		for (int i = 0; i < wire_array[target_wire].get_connected_wires_amount(); i++)
			if (wire_array[connected_wires[i]].get_parent_line_num() == target_line_num)
				return false;
	}
	return true;
}

bool valid_wire_start_can_move(int target_wire, int dr) {

	int lines[10][4], parent_line_num;
	int parent = wire_array[target_wire].get_parent_wire();
	int x1, y1, x2, y2, temp1, temp2;
	if (parent != -1) {
		wire_array[target_wire].get_first_line(&x1, &y1, &x2, &y2, &temp1, &temp2);
		wire_array[parent].get_lines(lines);
		parent_line_num = wire_array[target_wire].get_parent_line_num();
		if (x1 == x2 && y1 != y2) {
			if (!(x1 + dr >= lines[parent_line_num][0] && x1 + dr <= lines[parent_line_num][2] ||
				x1 + dr <= lines[parent_line_num][0] && x1 + dr >= lines[parent_line_num][2]))
				return false;
		}
		if (y1 == y2 && x1 != x2) {
			if (!(y1 + dr >= lines[parent_line_num][1] && y1 + dr <= lines[parent_line_num][3] ||
				y1 + dr <= lines[parent_line_num][1] && y1 + dr >= lines[parent_line_num][3]))
				return false;
		}
	}
	return true;
}

bool valid_line_can_move(int target_wire, int i, int lines[10][4], int dr) {

	int connected_wires[5];
	int x1, y1, x2, y2, temp1, temp2;
	wire_array[target_wire].get_connected_wires(connected_wires);

	if (lines[i][0] == lines[i][2] && lines[i][1] != lines[i][3]) {

		for (int j = 0; j < wire_array[target_wire].get_connected_wires_amount(); j++) {
			if (i != 0 && wire_array[connected_wires[j]].get_parent_line_num() == i - 1) {
				wire_array[connected_wires[j]].get_first_line(&x1, &y1, &x2, &y2, &temp1, &temp2);
				if (!(x1 >= lines[i - 1][0] && x1 <= lines[i - 1][2] + dr ||
					x1 <= lines[i - 1][0] && x1 >= lines[i - 1][2] + dr))
					return false;
			}
			if (wire_array[connected_wires[j]].get_parent_line_num() == i + 1) {
				wire_array[connected_wires[j]].get_first_line(&x1, &y1, &x2, &y2, &temp1, &temp2);
				if (!(x1 >= lines[i + 1][0] + dr && x1 <= lines[i + 1][2] ||
					x1 <= lines[i + 1][0] + dr && x1 >= lines[i + 1][2]))
					return false;
			}
		}
	}
	else if (lines[i][1] == lines[i][3] && lines[i][0] != lines[i][2]) {

		for (int j = 0; j < wire_array[target_wire].get_connected_wires_amount(); j++) {
			if (i != 0 && wire_array[connected_wires[j]].get_parent_line_num() == i - 1) {
				wire_array[connected_wires[j]].get_first_line(&x1, &y1, &x2, &y2, &temp1, &temp2);
				if (!(y1 >= lines[i - 1][1] && y1 <= lines[i - 1][3] + dr ||
					y1 <= lines[i - 1][1] && y1 >= lines[i - 1][3] + dr))
					return false;
			}
			if (wire_array[connected_wires[j]].get_parent_line_num() == i + 1) {
				wire_array[connected_wires[j]].get_first_line(&x1, &y1, &x2, &y2, &temp1, &temp2);
				if (!(y1 >= lines[i + 1][1] + dr && y1 <= lines[i + 1][3] ||
					y1 <= lines[i + 1][1] + dr && y1 >= lines[i + 1][3]))
					return false;
			}
		}
	}

	return true;
}

bool valid_component_can_move(Component entity, int dx, int dy) {

	int in_wires[4];
	int lines[10][4];
	int out_wire = entity.get_out_wire();
	entity.get_in_wires(in_wires);

	for (int i = 0; i < entity.get_entry_amount(); i++) {
		if (in_wires[i] != -1) {
			wire_array[in_wires[i]].get_lines(lines);
			int num = wire_array[in_wires[i]].get_lines_amount();
			if (lines[num - 1][1] == lines[num - 1][3] && lines[num - 1][0] != lines[num - 1][2]) {
				if (!valid_line_can_move(in_wires[i], num - 1, lines, dy))
					return false;
			}
			else if (lines[num - 1][0] == lines[num - 1][2] && lines[num - 1][1] != lines[num - 1][3]) {
				if (!valid_line_can_move(in_wires[i], num - 1, lines, dx))
					return false;
			}
		}
	}

	if (out_wire != -1) {
		wire_array[out_wire].get_lines(lines);
		if (lines[0][1] == lines[0][3] && lines[0][0] != lines[0][2]) {
			if (!valid_line_can_move(out_wire, 0, lines, dy))
				return false;
		}
		else if (lines[0][0] == lines[0][2] && lines[0][1] != lines[0][3]) {
			if (!valid_line_can_move(out_wire, 0, lines, dx))
				return false;
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
