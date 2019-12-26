//---------------------------------------------------------------------------

#pragma hdrstop

#include "uComponent.h"
#include "uWire.h"
#include "uValidation.h"
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

void add_component(int X, int Y){
	if (component_array_pos == 100)
		return;

	Component entity;
	if (!(current_component == "not" || current_component == "src" || current_component == "probe")) {
		entity.set_type(fetch_component_name(current_component));
		entity.set_entry_amount(current_component[current_component.length() - 1] - '0');
	}
	else{
		entity.set_type(current_component);
		if (current_component != "src")
			entity.set_entry_amount(1);
		else
			entity.set_entry_amount(0);
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

std::string fetch_component_name(std::string src){
	std::string res = "";
	int i = 0;
	while (!(src[i] >= '0' && src[i] <= '9'))
		res.push_back(src[i++]);
	return res;
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

void round_coords(int* X, int* Y){
	while (!(*X % grid_width == 0))
		(*X)--;
	while (!(*Y % grid_width == 0))
		(*Y)--;
	if (!valid_place(*X, *Y, -1)) {
		(*X) += grid_width;
	}
	else
		return;
	if (!valid_place(*X, *Y, -1)) {
		(*X) += grid_width;
	}
	else
		return;
	if (!valid_place(*X, *Y, -1)) {
		(*Y) += 2 * grid_width;
	}
	else
		return;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
