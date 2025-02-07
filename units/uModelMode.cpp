//---------------------------------------------------------------------------

#pragma hdrstop
#pragma package(smart_init)

#include "uModelMode.h"
#include "uCommon.h"
#include <string>

using namespace std;
//---------------------------------------------------------------------------

extern Component component_array[100];
extern ModelComponent model_component_array[100];
extern int component_array_pos;

extern Wire wire_array[300];
extern int wire_array_pos;
extern int current_wire[10][4];
extern int current_wire_pos;
extern int selected_wire;

int check_array[100];

void init_model_array(void){

	for (int i = 0; i < component_array_pos; i++) {
		Component basis = component_array[i];
		ModelComponent super;

		super.set_type(basis.get_type());
		super.set_entry_amount(basis.get_entry_amount());
		super.set_coords(basis.get_x(), basis.get_y());
		super.set_out_x(basis.get_out_x());
		super.set_out_y(basis.get_out_y());
		super.set_in_x(basis.get_in_x());
		super.set_out_wire(basis.get_out_wire());

		int in_y[4];
		basis.get_in_y(in_y);
		super.set_in_y(in_y);
		int in_wires[4];
		basis.get_in_wires(in_wires);
		for (int j = 0; j < 4; j++)
			super.set_in_wire(in_wires[j], j);

		if (super.get_type() == "src")
			super.set_out_charge(1);
		else
			super.set_out_charge(-1);
		for (int j = 0; j < 4; j++)
  			super.set_in_charge(-1, j);

        model_component_array[i] = super;
	}
}

void spread_charge_by_wire(int base_wire, int out_charge){
	int connected_wires[5];
	wire_array[base_wire].get_connected_wires(connected_wires);
	for (int i = 0; i < wire_array[base_wire].get_connected_wires_amount(); i++) {
		int out_comp = wire_array[connected_wires[i]].get_out_component();
		int out_comp_entry = wire_array[connected_wires[i]].get_out_component_entry();
		model_component_array[out_comp].set_in_charge(out_charge, out_comp_entry);
		spread_charge_by_wire(connected_wires[i], out_charge);
	}
}

bool is_ready_to_generate(ModelComponent entity){

	int in_wires[4], in_charges[4];
	entity.get_in_wires(in_wires);
	entity.get_in_charges(in_charges);

	for (int i = 0; i < 4; i++)
		if (in_wires[i] != -1 && in_charges[i] == -1)
			return false;

	return true;
}

int generate_out_charge(int in_charges[4], int (*bool_func)(int, int)){

	int res = -1;
	for (int i  = 0 ; i < 4; i++){
		if (res == -1 && in_charges[i] != -1){
			res = in_charges[i];
			continue;
		}
		if (in_charges[i] != -1)
			res = (*bool_func)(res, in_charges[i]);
	}

	return res;
}

int and_function(int op1, int op2){
	return op1 && op2;
}

int or_function(int op1, int op2){
	return op1 || op2;
}

int xor_function(int op1, int op2){
	if (op1 == 1 && op2 == 1)
		return 0;
	else
		return op1 || op2;
}

bool is_trash_component(ModelComponent entity){
	int in_wires[4];
	entity.get_in_wires(in_wires);

	for (int i = 0; i < 4; i++)
		if (in_wires[i] != -1)
			if (wire_array[in_wires[i]].get_in_component() != -1)
				return false;

	return true;
}

void break_in_wires_in_connected_components(int base_wire){

	int connected_wires[5];
	wire_array[base_wire].get_connected_wires(connected_wires);
	for (int i = 0; i < wire_array[base_wire].get_connected_wires_amount(); i++) {
		int out_comp = wire_array[connected_wires[i]].get_out_component();
		int out_comp_entry = wire_array[connected_wires[i]].get_out_component_entry();
		if (out_comp != -1)
			model_component_array[out_comp].set_in_wire(-1, out_comp_entry);
		break_in_wires_in_connected_components(connected_wires[i]);
	}
}

int is_cycle_exist(int out_wire){

	if (out_wire == -1)
		return -1;

	check_array[wire_array[out_wire].get_in_component()] = true;

	int connected_wires[5];
	wire_array[out_wire].get_connected_wires(connected_wires);
	for (int i = 0; i < wire_array[out_wire].get_connected_wires_amount(); i++)
		return is_cycle_exist(connected_wires[i]);

	int out_component = wire_array[out_wire].get_out_component();
	if (out_component != -1) {
		if (check_array[out_component])
			return out_wire;
		else
			return is_cycle_exist(component_array[out_component].get_out_wire());
	}
	else
		return -1;

}

int model_scheme(void){

	bool components_checked[100];
	for (int i = 0; i < component_array_pos; i++)
		components_checked[i] = false;

	while (true){

		for (int i = 0; i < component_array_pos; i++) {
			if (!components_checked[i]) {

				for (int j = 0 ; j < component_array_pos; j++)
					check_array[j] = false;
				int cycle_wire = is_cycle_exist(model_component_array[i].get_out_wire());
				if (cycle_wire != -1){
					selected_wire = cycle_wire;
					return 1;
				}

				if (is_trash_component(model_component_array[i]) && model_component_array[i].get_type() != "src") {
					components_checked[i] = true;
					int out_wire = model_component_array[i].get_out_wire();
					if (out_wire != -1) {
						int out_comp = wire_array[out_wire].get_out_component();
						int out_comp_entry = wire_array[out_wire].get_out_component_entry();
						if (out_comp != -1)
							model_component_array[out_comp].set_in_wire(-1, out_comp_entry);
						break_in_wires_in_connected_components(out_wire);
					}
				}

				int out_charge = model_component_array[i].get_out_charge();
				if (out_charge != -1){
					int out_wire = model_component_array[i].get_out_wire();
					int out_comp = -1, out_comp_entry = -1;
					if (out_wire != -1) {
						out_comp = wire_array[out_wire].get_out_component();
						out_comp_entry = wire_array[out_wire].get_out_component_entry();
					}

					if (out_comp != -1) {
						model_component_array[out_comp].set_in_charge(out_charge, out_comp_entry);
						spread_charge_by_wire(out_wire, out_charge);
					}
					components_checked[i] = true;
				}
			}
		}

		int not_checked;
		not_checked = 0;
		for (int i = 0; i < component_array_pos; i++) {
			if (!components_checked[i]){
				not_checked++;

				if (is_ready_to_generate(model_component_array[i])) {
					int out_charge, in_charges[4];
					model_component_array[i].get_in_charges(in_charges);
					out_charge = in_charges[0];
					std::string comp_type = model_component_array[i].get_type();

					if (comp_type == "and" || comp_type == "nand")
						out_charge = generate_out_charge(in_charges, and_function);
					else if (comp_type == "or" || comp_type == "nor")
						out_charge = generate_out_charge(in_charges, or_function);
					else if (comp_type == "xor" || comp_type == "nxor")
						out_charge = generate_out_charge(in_charges, xor_function);
					else if (comp_type == "probe")
						out_charge = in_charges[0];

					if (comp_type[0] == 'n' && out_charge != -1)
						out_charge = out_charge ? 0 : 1;

					if (out_charge != -1)
						model_component_array[i].set_out_charge(out_charge);
				}
			}
		}

		if (not_checked == 0)
			return 0;

	}

}

void reset_charges(void){

	for (int i = 0; i < component_array_pos; i++) {
		if (model_component_array[i].get_type() != "src")
			model_component_array[i].set_out_charge(-1);
		for (int j = 0; j < 4; j++)
			model_component_array[i].set_in_charge(-1, j);
	}
}
