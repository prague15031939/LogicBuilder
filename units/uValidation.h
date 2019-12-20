//---------------------------------------------------------------------------

#ifndef uValidationH
#define uValidationH
//---------------------------------------------------------------------------

bool valid_wire_start(int *x0, int *y0);
bool valid_wire_middle(int *x0, int *y0);
bool valid_wire_end(int *x0, int *y0);
int valid_branch_wire_start(int *x0, int *y0);
bool valid_local_line_is_alone(int target_wire, int mode);
bool valid_place(int x0, int y0, int except);

#endif
