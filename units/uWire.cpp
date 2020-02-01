//---------------------------------------------------------------------------

#pragma hdrstop
#pragma package(smart_init)

#include "uWire.h"
#include "uComponent.h"
#include "uValidation.h"
#include "uCommon.h"

extern Component component_array[100];
extern int component_array_pos;
extern std::string current_component;

extern Wire wire_array[300];
extern int wire_array_pos;
extern int current_wire[10][4];
extern int current_wire_pos;
extern int move_line_buffer[10][4];
extern int move_line_buffer_pos;

extern int comp_width, comp_height, wire_length, grid_width;
extern int entry_coords[4][4];
extern bool branch_wire_mode;
extern int parent_wire;
extern int picked_line_num;

void add_wire(int item[10][4]){
	if (wire_array_pos == 300)
		return;

	int start;
	int end[2] = {-1, -1};
	if (!branch_wire_mode)
		start = find_start_component(item[0][0], item[0][1]);
	else {
		start = wire_array[parent_wire].get_in_component();
		int temp = wire_array[parent_wire].get_connected_wires_amount();
		if (temp == 5)
			return;
		wire_array[parent_wire].set_connected_wire(wire_array_pos, temp++);
		wire_array[parent_wire].set_connected_wires_amount(temp);
	}
	find_end_component(end, item[current_wire_pos][2], item[current_wire_pos][3]);

	if (!branch_wire_mode) {
		component_array[start].set_out_wire(wire_array_pos);
		wire_array[wire_array_pos].set_parent_wire(-1);
		wire_array[wire_array_pos].set_parent_line_num(-1);
	}
	else {
		wire_array[wire_array_pos].set_parent_wire(parent_wire);
		wire_array[wire_array_pos].set_parent_line_num(picked_line_num);
	}
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

void delete_wire(int target, char mode){

	delete_in_component_in_all_connected_wires(target);

	int connected_wires[5];
	wire_array[target].get_connected_wires(connected_wires);
	if (mode == 't') {
		while (connected_wires[0] != -1) {
			delete_wire(connected_wires[0], 't');
			wire_array[target].get_connected_wires(connected_wires);
		}
	}
	else if (mode == 'd') {
		for (int i = 0; i < wire_array[target].get_connected_wires_amount(); i++)
			wire_array[connected_wires[i]].set_parent_wire(-1);
	}

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
	component_array[out_comp].set_in_wire(-1, wire_array[target].get_out_component_entry());

	for (int i = target; i < wire_array_pos - 1; i++) {
		wire_array[i] = wire_array[i + 1];
	}
	wire_array_pos--;
	decrease_wires_index(target);

}

bool pull_connected_wires(Component entity, int new_x, int new_y){

	int in_wires[4], out_wire;
	entity.get_in_wires(in_wires);
	out_wire = entity.get_out_wire();

	int num = entity.get_entry_amount();
	for (int i = 0; i < num; i++)
		if (in_wires[i] != -1)
			if (valid_two_lines_are_same(in_wires[i], wire_array[in_wires[i]].get_lines_amount() - 2))
				return false;
	if (out_wire != -1)
		if (valid_two_lines_are_same(out_wire, 0))
			return false;

	int x0, y0, x1, y1, x2, y2;
	for (int i = 0; i < num; i++) {
		if (in_wires[i] != -1) {
			wire_array[in_wires[i]].get_last_line(&x0, &y0, &x1, &y1, &x2, &y2);

			if (x1 == x2 && y1 == y2 && x2 == x0){
				new_x += grid_width;
				x2 += grid_width;
			}
			else if (x1 == x2 && y1 == y2 && y2 == y0) {
				new_y += grid_width;
				y2 += grid_width;
			}

			if (x1 == x2 && y1 != y2){
				if (x2 != new_x - wire_length)
					x1 += new_x - wire_length - x2;
			}
			else if (y1 == y2 && x1 != x2){
				if (y2 != new_y + entry_coords[num - 1][i])
					y1 += new_y + entry_coords[num - 1][i] - y2;
			}
			wire_array[in_wires[i]].set_last_line(x1, y1, new_x - wire_length, new_y + entry_coords[num - 1][i]);
		}
	}

	if (out_wire != -1) {
		wire_array[out_wire].get_first_line(&x1, &y1, &x2, &y2, &x0, &y0);

		if (x1 == x2 && y1 == y2 && x1 == x0) {
			new_x += grid_width;
			x1 += grid_width;
		}
		else if (x1 == x2 && y1 == y2 && y1 == y0) {
			new_y += grid_width;
			y1 += grid_width;
		}

		if (x1 == x2 && y1 != y2){
			if (x1 != new_x + comp_width + wire_length)
				x2 += new_x + comp_width + wire_length - x1;
		}
		else if (y1 == y2 && x1 != x2){
			if (y1 != new_y + (int)comp_height / 2)
				y2 += new_y + (int)comp_height / 2 - y1;
		}
		wire_array[out_wire].set_first_line(new_x + comp_width + wire_length, new_y + (int)comp_height / 2, x2, y2);
	}

	return true;
}

void autocorrect_wire_end(int X, int Y) {

	if (move_line_buffer[move_line_buffer_pos][1] == move_line_buffer[move_line_buffer_pos][3]) {
		if (move_line_buffer_pos != 0 && move_line_buffer[move_line_buffer_pos][1] != move_line_buffer[move_line_buffer_pos - 1][1]) {
			current_wire[current_wire_pos - 1][3] = Y;
			current_wire[current_wire_pos][1] = Y;
		}
		else {
			current_wire[current_wire_pos + 1][0] = current_wire[current_wire_pos][2] = current_wire[current_wire_pos][0];
			current_wire[current_wire_pos + 1][1] = current_wire[current_wire_pos][3] = Y;
			current_wire_pos++;
		}
	}
	if (move_line_buffer[move_line_buffer_pos][0] == move_line_buffer[move_line_buffer_pos][2]) {
		if (move_line_buffer_pos != 0 && move_line_buffer[move_line_buffer_pos][0] != move_line_buffer[move_line_buffer_pos - 1][0]) {
			current_wire[current_wire_pos - 1][2] = X;
			current_wire[current_wire_pos][0] = X;
		}
		else {
			current_wire[current_wire_pos + 1][1] = current_wire[current_wire_pos][3] = current_wire[current_wire_pos][1];
			current_wire[current_wire_pos + 1][0] = current_wire[current_wire_pos][2] = X;
			current_wire_pos++;
		}
	}

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
