//---------------------------------------------------------------------------

#ifndef uWireH
#define uWireH

#include "uCommon.h"

void add_wire(int item[10][4]);
void delete_wire(int target, char mode);
bool pull_connected_wires(Component entity, int new_x, int new_y);
void autocorrect_wire_end(int X, int Y);
bool reduce_doubling_lines(int i);
void decrease_wires_index(int index);
void correct_with_angle(int *x, int *y);

//---------------------------------------------------------------------------
#endif
