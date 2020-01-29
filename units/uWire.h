//---------------------------------------------------------------------------

#ifndef uWireH
#define uWireH

#include "uCommon.h"

void add_wire(int item[10][4]);
void delete_wire(int target);
bool pull_connected_wires(Component entity, int new_x, int new_y);
void autocorrect_wire_end(int X, int Y);
void decrease_wires_index(int index);
void correct_with_angle(int *x, int *y);

//---------------------------------------------------------------------------
#endif
