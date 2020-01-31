//---------------------------------------------------------------------------

#ifndef uValidationH
#define uValidationH

#include "uCommon.h"

bool valid_wire_start(int *x0, int *y0);
bool valid_wire_middle(int *x0, int *y0);
bool valid_wire_end(int *x0, int *y0);
int valid_branch_wire_start(int *x0, int *y0);
bool valid_line_is_alone(int target_wire, int target_line_num);
bool valid_wire_start_can_move(int target_wire, int dr);
bool valid_line_can_move(int target_wire, int i, int lines[10][4], int dr);
bool valid_two_lines_are_same(int target_wire, int i);
bool valid_component_can_move(Component entity, int dx, int dy);
bool valid_place(int x0, int y0, int except);

#endif
