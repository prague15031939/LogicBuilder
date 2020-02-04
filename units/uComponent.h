//---------------------------------------------------------------------------

#pragma once

#ifndef uComponentH
#define uComponentH

#include "uCommon.h"
#include <string>
#include <cmath>
using namespace std;

void add_component(int X, int Y);
void delete_component(int target, char mode);
void delete_in_component_in_all_connected_wires(int out_wire);
void decrease_components_index(int index);
std::string fetch_component_name(std::string src);
int find_start_component(int x0, int y0);
void find_end_component(int end[2], int x0, int y0);
void round_coords(int* X, int* Y);
int modify_component_position(int selected_comp, int new_x, int new_y);

//---------------------------------------------------------------------------
#endif
