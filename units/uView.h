//---------------------------------------------------------------------------

#ifndef uViewH
#define uViewH
//---------------------------------------------------------------------------
#include "uMain.h"
#include "uCommon.h"

void draw_component(TPaintBox *pb, Component entity);
void draw_model_component(TPaintBox *pb, int index);
void draw_entries(TPaintBox *pb, Component entity);
void draw_wire(TPaintBox *pb, Wire entity, char mode);
void draw_temp_lines(TPaintBox *pb);
void draw_guides(TPaintBox *pb);
void draw_highlight(TPaintBox *pb, int i);
void draw_wire_highlight(TPaintBox *pb, int i);
void draw_dot_highlight(TPaintBox *pb, int x, int y);
void draw_grid(TPaintBox *pb);
void detect_closest_guides(TPaintBox *pb, int target_comp);
void attract_to_guides(int *x_comp, int *y_comp);

#endif
