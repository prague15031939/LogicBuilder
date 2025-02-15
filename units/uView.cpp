//---------------------------------------------------------------------------

#pragma hdrstop

#include "uView.h"
#include "uCommon.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

extern Component component_array[100];
extern int component_array_pos;
extern ModelComponent model_component_array[100];
extern std::string current_component;

extern Wire wire_array[300];
extern int wire_array_pos;
extern int current_wire[10][4];
extern int current_wire_pos;

extern int comp_width, comp_height, wire_length, grid_width;
extern int entry_coords[4][4];
extern int move_line_buffer[10][4];
extern int move_line_buffer_pos;

extern int guides[20][4];
extern int guides_pos;


void draw_component(TPaintBox *pb, Component entity){

	int X, Y;
	X = entity.get_x();
	Y = entity.get_y();
	std::string comp_type = entity.get_type();
	pb -> Canvas -> Font -> Size = 8;
	pb -> Canvas -> Brush -> Color = clWhite;

	if (!(comp_type == "src" || comp_type == "probe")) {
		pb -> Canvas -> Rectangle(X, Y, X + comp_width, Y + comp_height);

		draw_entries(pb, entity);
		pb -> Canvas -> MoveTo(X + comp_width, entity.get_out_y());
		pb -> Canvas -> LineTo(entity.get_out_x(), entity.get_out_y());

		if (comp_type == "and" || comp_type == "nand") {
			pb -> Canvas -> TextOut(X + 6, Y + 5, "&");
		}
		else if (comp_type == "or" || comp_type == "nor") {
			pb -> Canvas -> TextOut(X + 7, Y + 5, "1");
			}
			else if (comp_type == "xor" || comp_type == "nxor") {
				pb -> Canvas -> TextOut(X + 2, Y + 5, "=1");
				}
		if (comp_type[0] == 'n') {
			pb -> Canvas -> Ellipse(X + comp_width - 3, (int)Y + comp_height / 2 - 3, X + comp_width + 3, (int)Y + comp_height / 2 + 3);
		}
	}

	else if (comp_type == "src") {
		pb -> Canvas -> Rectangle(X, Y + (int) comp_height * 0.3, X + comp_width, Y + (int) comp_height * 0.75);
		pb -> Canvas -> FillRect(Rect(X + 1, Y + (int) comp_height * 0.3 + 1, X + comp_width - 1, Y + (int) comp_height * 0.75 - 1));
		pb -> Canvas -> MoveTo(X + comp_width, entity.get_out_y());
		pb -> Canvas -> LineTo(entity.get_out_x(), entity.get_out_y());
	}
	else{
		pb -> Canvas -> Ellipse(X, Y, X + comp_width, Y + comp_width);
		pb -> Canvas -> MoveTo(X + (int) comp_width / 2, Y + comp_width);
		pb -> Canvas -> LineTo(X + (int) comp_width / 2, Y + (int) comp_height / 2);
		pb -> Canvas -> MoveTo(entity.get_in_x(), entity.get_out_y());
		pb -> Canvas -> LineTo(entity.get_out_x(), entity.get_out_y());
	}
}

void draw_model_component(TPaintBox *pb, int index){
	draw_component(pb, component_array[index]);
	ModelComponent entity = model_component_array[index];

	int X, Y;
	X = entity.get_x();
	Y = entity.get_y();
	if (entity.get_type() == "src") {
		pb -> Canvas -> Font -> Size = 10;
		if (entity.get_out_charge() == 0)
			pb -> Canvas -> TextOut(X + 5, Y + 15, "0");
		else if (entity.get_out_charge() == 1)
				pb -> Canvas -> TextOut(X + 5, Y + 15, "1");
		pb -> Canvas -> Font -> Size = 8;
	}
	else if (entity.get_type() == "probe") {
			int in_charges[4];
			entity.get_in_charges(in_charges);
			if (in_charges[0] == 1){
				pb -> Canvas -> Brush -> Color = clRed;
				pb -> Canvas -> FloodFill(X + 10, Y + 10, clBlack, fsBorder);
			}
			else if (in_charges[0] == 0)
				pb -> Canvas -> FloodFill(X + 10, Y + 10, clBlack, fsBorder);
			pb -> Canvas -> Brush -> Color = clWhite;
	}

	else{
		int out_charge, in_charges[4], in_x, in_y[4], out_x, out_y;
		entity.get_in_charges(in_charges);
		out_charge = entity.get_out_charge();
		entity.get_in_y(in_y);
		in_x = entity.get_in_x();
		out_x = entity.get_out_x();
        out_y = entity.get_out_y();

		for (int i = 0; i < entity.get_entry_amount(); i++) {
			if (in_charges[i] != -1) {
				if (in_charges[i] == 1)
					pb -> Canvas -> Brush -> Color = clRed;
				pb -> Canvas -> Rectangle(in_x + 4, in_y[i] - 7, in_x + 9, in_y[i] - 2);
				pb -> Canvas -> Brush -> Color = clWhite;
			}
		}

		if (out_charge != -1) {
			if (out_charge == 1)
				pb -> Canvas -> Brush -> Color = clRed;
			pb -> Canvas -> Rectangle(out_x - 8, out_y - 7, out_x - 3, out_y - 2);
			pb -> Canvas -> Brush -> Color = clWhite;
		}
	}
}

void draw_entries(TPaintBox *pb, Component entity){
	int in_y[4] = {0, 0, 0, 0};
	entity.get_in_y(in_y);
	for (int i = 0; i < entity.get_entry_amount(); i++) {
		pb -> Canvas -> MoveTo(entity.get_x(), in_y[i]);
		pb -> Canvas -> LineTo(entity.get_in_x(), in_y[i]);
	}
}

void draw_wire(TPaintBox *pb, Wire entity, char mode){

	int lines_amount;
	lines_amount = entity.get_lines_amount();
	int lines[10][4];
	entity.get_lines(lines);

	for (int i = 0; i < lines_amount; i++) {
		if (mode == 'd') {
            pb -> Canvas -> MoveTo(lines[i][0], lines[i][1]);
			pb -> Canvas -> LineTo(lines[i][2], lines[i][3]);
		}
		else if (mode == 'h') {
			pb -> Canvas -> Rectangle(lines[i][0] - 3, lines[i][1] - 3, lines[i][0] + 3, lines[i][1] + 3);
            pb -> Canvas -> Rectangle(lines[i][2] - 3, lines[i][3] - 3, lines[i][2] + 3, lines[i][3] + 3);
		}
	}
}

void draw_temp_lines(TPaintBox *pb){
	for (int i = 0; i <= move_line_buffer_pos; i++) {
		pb -> Canvas -> MoveTo(move_line_buffer[i][0], move_line_buffer[i][1]);
		pb -> Canvas -> LineTo(move_line_buffer[i][2], move_line_buffer[i][3]);
	}
}

void draw_guides(TPaintBox *pb) {

	pb -> Canvas -> Pen -> Color = clMedGray;
	pb -> Canvas -> Pen -> Style = psDash;

	for (int i = 0; i < guides_pos; i++) {
		pb -> Canvas -> MoveTo(guides[i][0], guides[i][1]);
		pb -> Canvas -> LineTo(guides[i][2], guides[i][3]);

		if (guides[i][0] == guides[i][2]) {
			pb -> Canvas -> MoveTo(guides[i][0] + comp_width, guides[i][1]);
			pb -> Canvas -> LineTo(guides[i][2] + comp_width, guides[i][3]);
		}
		if (guides[i][1] == guides[i][3]) {
			pb -> Canvas -> MoveTo(guides[i][0], guides[i][1] + comp_height);
			pb -> Canvas -> LineTo(guides[i][2], guides[i][3] + comp_height);
		}
	}

	pb -> Canvas -> Pen -> Color = clBlack;
	pb -> Canvas -> Pen -> Style = psSolid;

}

void draw_highlight(TPaintBox *pb, int i){

	pb -> Canvas -> Pen -> Color = clHighlight;
	pb -> Canvas -> Pen -> Style = psDash;

	int x, y;
	x = component_array[i].get_x();
	y = component_array[i].get_y();
	pb -> Canvas -> MoveTo(x - wire_length - 1, y - 1);
	pb -> Canvas -> LineTo(x + comp_width + wire_length + 1, y - 1);
	pb -> Canvas -> LineTo(x + comp_width + wire_length + 1, y + comp_height + 1);
	pb -> Canvas -> LineTo(x - wire_length - 1, y + comp_height + 1);
	pb -> Canvas -> LineTo(x - wire_length - 1, y - 1);

	pb -> Canvas -> Pen -> Color = clBlack;
	pb -> Canvas -> Pen -> Style = psSolid;
}

void draw_wire_highlight(TPaintBox *pb, int i){

	pb -> Canvas -> Pen -> Color = clRed;
	pb -> Canvas -> Brush -> Color = clRed;

	draw_wire(pb, wire_array[i], 'h');

	pb -> Canvas -> Pen -> Color = clBlack;
	pb -> Canvas -> Brush -> Color = clWhite;
}

void draw_dot_highlight(TPaintBox *pb, int x, int y){
	pb -> Canvas -> Brush -> Color = clRed;
	pb -> Canvas -> Pen -> Color = clRed;
	pb -> Canvas -> Pie(x - 3, y - 3, x + 3, y + 3, 0, 0, 0, 0);
	pb -> Canvas -> Brush -> Color = clBlack;
	pb -> Canvas -> Pen -> Color = clBlack;
}

void draw_grid(TPaintBox *pb){
	pb -> Canvas -> Pen -> Color = clBtnFace;
	int x_line = 0, y_line = 0;
	for (int i = 0; i < (int)pb->Width / grid_width; i++) {
		pb -> Canvas -> MoveTo(x_line, 0);
		pb -> Canvas -> LineTo(x_line, pb->Height);
		x_line += grid_width;
	}
	for (int i = 0; i < (int)pb->Height / grid_width; i++) {
		pb -> Canvas -> MoveTo(0, y_line);
		pb -> Canvas -> LineTo(pb -> Width, y_line);
		y_line += grid_width;
	}
	pb -> Canvas -> Pen -> Color = clBlack;
}

void detect_closest_guides(TPaintBox *pb, int target_comp) {
	guides_pos = 0;
	int x = component_array[target_comp].get_x();
	int y = component_array[target_comp].get_y();

	for (int i = 0; i < component_array_pos; i++) {
		int temp_x = component_array[i].get_x();
		int temp_y = component_array[i].get_y();
		if (abs(x - temp_x) <= comp_width && guides_pos < 20 && i != target_comp) {
			guides[guides_pos][0] = guides[guides_pos][2] = temp_x;
			guides[guides_pos][1] = 0;
			guides[guides_pos++][3] = pb -> Height;
		}
		if (abs(y - temp_y) <= comp_height && guides_pos < 20 && i != target_comp) {
			guides[guides_pos][1] = guides[guides_pos][3] = temp_y;
			guides[guides_pos][0] = 0;
			guides[guides_pos++][2] = pb -> Width;
		}
	}
}

void attract_to_guides(int *x_comp, int *y_comp) {
	bool x_corrected = false;
	bool y_corrected = false;
	for (int i = 0 ; i < guides_pos; i++) {
		if (guides[i][0] == guides[i][2]) {
			if (abs(*x_comp - guides[i][0]) <= 2 * grid_width && !x_corrected) {
				*x_comp = guides[i][0];
				x_corrected = true;
			}
		}
		if (guides[i][1] == guides[i][3]) {
			if (abs(*y_comp - guides[i][1]) <= 2 * grid_width && !y_corrected) {
				*y_comp = guides[i][1];
				y_corrected = true;
			}
		}
	}
}






