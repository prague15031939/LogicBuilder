//---------------------------------------------------------------------------

#pragma hdrstop
#include "uCommon.h"

Component component_array[100];
ModelComponent model_component_array[100];
int component_array_pos = 0;
std::string current_component = "";

Wire wire_array[300];
int wire_array_pos = 0;
int current_wire[10][4];
int current_wire_pos = 0;

int comp_width = 20, comp_height = 50, wire_length = 10, grid_width = 5;
int entry_coords[4][4] = {{25, 0, 0, 0}, {15, 35, 0, 0}, {10, 25, 40, 0}, {10, 20, 30, 40}};

typedef enum {wsBegin, wsMiddle, wsEnd} TWireStage;
TWireStage wire_stage = wsBegin;

int x_dot_highlight = -1;
int y_dot_highlight = -1;

int move_step;
bool cursor_mode = true;
bool model_mode = false;
bool wire_mode = false;
bool branch_wire_mode = false;
int parent_wire = -1;
int move_line_buffer[10][4];
int move_line_buffer_pos = 0;
int selected_comp = -1;
int selected_wire = -1;

std::string file_dir = "";
bool to_draw_grid = true;

//---------------------------------------------------------------------------
#pragma package(smart_init)
