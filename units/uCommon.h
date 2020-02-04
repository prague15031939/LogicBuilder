//---------------------------------------------------------------------------

#pragma once

#ifndef uCommonH
#define uCommonH

#include <string>
using namespace std;

class Component {
public:
	void set_type(std::string name){
		this->type = name;
	}
	void set_entry_amount(int num){
		this->entry_amount = num;
	}
	void set_coords(int x, int y){
		this->x_coord = x;
		this->y_coord = y;
	}
	void set_out_x(int x){
		this->out_x = x;
	}
	void set_out_y(int y){
		this->out_y = y;
	}
	void set_in_x(int x){
		this->in_x = x;
	}
	void set_in_y(int arr[4]){
		for (int i = 0; i < 4; i++) {
			this->in_y[i] = arr[i];
		}
	}
	void set_in_wire(int value, int index){
		this->in_wires[index] = value;
	}
	void set_out_wire(int index){
		this->out_wire = index;
	}

	std::string get_type(){
		return this->type;
	}
	int get_entry_amount(){
		return this->entry_amount;
	}
	int get_x(){
		return this->x_coord;
	}
	int get_y(){
		return this->y_coord;
	}
	int get_out_x(){
		return this->out_x;
	}
	int get_out_y(){
		return this->out_y;
	}
	int get_in_x(){
		return this->in_x;
	}
	void get_in_y(int arr[4]){
		for (int i = 0; i < 4; i++) {
			arr[i] = this->in_y[i];
		}
	}
	void get_in_wires(int arr[4]){
		for (int i = 0; i < 4; i++) {
			arr[i] = this->in_wires[i];
		}
	}
	int get_out_wire(){
		return this->out_wire;
	}

private:
	std::string type;
	int entry_amount;
	int x_coord, y_coord;
	int out_x, out_y;
	int in_x;
	int in_y[4];
	int in_wires[4];
	int out_wire;
};

class ModelComponent : public Component {
public:
	void set_in_charge(int value, int index){
		this->in_charge[index] = value;
	}
	void get_in_charges(int arr[4]){
		for (int i = 0; i < 4; i++) {
			arr[i] = this->in_charge[i];
		}
	}
	void set_out_charge(int value){
		this->out_charge = value;
	}
	int get_out_charge(){
		return this->out_charge;
	}

private:
	int in_charge[4];
	int out_charge;
};

class Wire {
public:
	void set_lines(arr[10][4]){
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				this->lines[i][j] = arr[i][j];
			}
		}
	}
	void set_first_line(int x1, int y1, int x2, int y2){
		this->lines[0][0] = x1;
		this->lines[0][1] = y1;
		this->lines[0][2] = x2;
		this->lines[0][3] = y2;
		this->lines[1][0] = x2;
		this->lines[1][1] = y2;
	}
	void set_last_line(int x1, int y1, int x2, int y2){
		this->lines[this->get_lines_amount() - 2][2] = x1;
		this->lines[this->get_lines_amount() - 2][3] = y1;
		this->lines[this->get_lines_amount() - 1][0] = x1;
		this->lines[this->get_lines_amount() - 1][1] = y1;
		this->lines[this->get_lines_amount() - 1][2] = x2;
		this->lines[this->get_lines_amount() - 1][3] = y2;
	}
	void get_first_line(int *x1, int *y1, int *x2, int *y2, int *x0, int *y0){
		*x1 = this->lines[0][0];
		*y1 = this->lines[0][1];
		*x2 = this->lines[0][2];
		*y2 = this->lines[0][3];
		if (this->get_lines_amount() > 1) {
			*x0 = this->lines[1][2];
			*y0 = this->lines[1][3];
		}
	}
	void get_last_line(int *x0, int *y0, int *x1, int *y1, int *x2, int *y2){
		int last = this->get_lines_amount();
		if (last > 1) {
			*x0 = this->lines[last - 2][0];
			*y0 = this->lines[last - 2][1];
		}
		*x1 = this->lines[last - 1][0];
		*y1 = this->lines[last - 1][1];
		*x2 = this->lines[last - 1][2];
		*y2 = this->lines[last - 1][3];
	}
	void set_connected_wire(int value, int index){
		this->connected_wires[index] = value;
	}
	void get_connected_wires(int arr[5]){
		for (int i = 0; i < 5; i++) {
			arr[i] = this->connected_wires[i];
		}
	}
	void set_connected_wires_amount(int value){
		this->connected_wires_amount = value;
	}
	int get_connected_wires_amount(){
		return this->connected_wires_amount;
	}
	void set_parent_wire(int value){
		this->parent_wire = value;
	}
	int get_parent_wire(){
		return this->parent_wire;
	}
	void set_parent_line_num(int value) {
		this->parent_line_num = value;
	}
	int get_parent_line_num() {
        return this->parent_line_num;
    }

	void get_lines(arr[10][4]){
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				arr[i][j] = this->lines[i][j];
			}
		}
	}
	void set_lines_amount(int value){
		this->lines_amount = value;
	}
	int get_lines_amount(){
		return this->lines_amount;
	}
	void set_in_out_component(int in, int out){
		this->in_component = in;
		this->out_component = out;
	}
	void set_out_component_entry(int out_entry){
		this->out_component_entry = out_entry;
	}
	int get_in_component(){
		return this->in_component;
	}
	int get_out_component(){
		return this->out_component;
	}
	int get_out_component_entry(){
		return this->out_component_entry;
	}

private:
	int lines[10][4];
	int lines_amount;
	int in_component, out_component, out_component_entry;
	int connected_wires[5];
	int connected_wires_amount;
	int parent_wire;
	int parent_line_num;
};

class Echelon {
public:
	Echelon *ptrNext;
	int c_array_pos;
	Component c_array[100];
	int w_array_pos;
	Wire w_array[300];
};

class Stack {
public:
	void init_stack() {
		this->top = NULL;
	}
	void push(int component_array_pos, Component *component_array, int wire_array_pos, Wire *wire_array) {
		Echelon *entity = new Echelon;
		entity->c_array_pos = component_array_pos;
		for (int i = 0; i < entity->c_array_pos; i++)
			entity->c_array[i] = component_array[i];
		entity->w_array_pos = wire_array_pos;
		for (int i = 0; i < entity->w_array_pos; i++)
			entity->w_array[i] = wire_array[i];
		entity->ptrNext = top;
		this->top = entity;
		this->control_bottom();
	}
	void pop(int *component_array_pos, Component *component_array, int *wire_array_pos, Wire *wire_array) {
		if (this->top != NULL && this->top->ptrNext != NULL) {
			Echelon *entity = this->top->ptrNext;
			*component_array_pos = entity->c_array_pos;
			for (int i = 0; i < *component_array_pos; i++)
				component_array[i] = entity->c_array[i];
			*wire_array_pos = entity->w_array_pos;
			for (int i = 0; i < *wire_array_pos; i++)
				wire_array[i] = entity->w_array[i];
			delete this->top;
			this->top = entity;
		}
	}
	void clear() {
		while (this->top != NULL) {
			Echelon *entity = this->top->ptrNext;
			delete this->top;
			this->top = entity;
		}
	}

private:
	Echelon *top;

	void control_bottom() {
		int count = 0;
		Echelon *ptr = this->top;
		while (ptr->ptrNext != NULL && count <= 10) {
			if (count == 10) {
				delete ptr->ptrNext;
				ptr->ptrNext = NULL;
				break;
			}
			ptr = ptr->ptrNext;
			count++;
		}
	}
};

//---------------------------------------------------------------------------
#endif
