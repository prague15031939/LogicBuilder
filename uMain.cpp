//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "uMain.h"
#include "uComponent.h"
#include "uWire.h"
#include "uValidation.h"
#include "uView.h"
#include "uModelMode.h"
#include "uCommon.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

extern Component component_array[100];
extern ModelComponent model_component_array[100];
extern int component_array_pos;
extern std::string current_component;

extern Wire wire_array[300];
extern int wire_array_pos;
extern int current_wire[10][4];
extern int current_wire_pos;
typedef enum {wsBegin, wsMiddle, wsEnd} TWireStage;
extern TWireStage wire_stage;

extern int x_dot_highlight;
extern int y_dot_highlight;

extern int comp_width, comp_height, wire_length, grid_width;
extern int entry_coords[4][4];
extern int move_step;
extern int x_start_move, y_start_move;
extern bool cursor_mode;
extern bool model_mode;
extern bool wire_mode;
extern bool branch_wire_mode;
extern int parent_wire;
extern int move_line_buffer[10][4];
extern int move_line_buffer_pos;
extern int selected_comp;
extern int selected_wire;

extern std::string file_dir;
extern bool to_draw_grid;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}


void open_file(std::string src){
   ifstream file_obj;
   file_obj.open(src.c_str(), ios::in | ios::binary);
   component_array_pos = 0;
   wire_array_pos = 0;

   file_obj.read((char*)&component_array_pos, sizeof(int));
   for (int i = 0; i < component_array_pos; i++) {
		file_obj.read((char*)&component_array[i], sizeof(component_array[i]));
   }

   file_obj.read((char*)&wire_array_pos, sizeof(int));
   for (int i = 0; i < wire_array_pos; i++) {
		file_obj.read((char*)&wire_array[i], sizeof(wire_array[i]));
   }
   file_obj.close();

}

void save_to_file(std::string dest){
	ofstream file_obj;
	file_obj.open(dest.c_str(), ios::out | ios::trunc | ios::binary);

	file_obj.write((char*)&component_array_pos, sizeof(int));
	for (int i = 0; i < component_array_pos; i++) {
		file_obj.write((char*)&component_array[i], sizeof(component_array[i]));
	}

	file_obj.write((char*)&wire_array_pos, sizeof(int));
	for (int i = 0; i < wire_array_pos; i++) {
		file_obj.write((char*)&wire_array[i], sizeof(wire_array[i]));
	}
	file_obj.close();
}

void to_svg(std::string dest){

	ofstream file_obj;
	file_obj.open(dest.c_str(), ios::out | ios::trunc);

	file_obj << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" <<
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" <<
		"<svg version=\"1.1\" baseProfile=\"full\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:ev=\"http://www.w3.org/2001/xml-events\" width=\"100%\" height=\"100%\">\n";

	char temp_str[300];
	for (int i = 0; i < component_array_pos; i++) {
		Component entity = component_array[i];
		if (entity.get_type() == "src"){
			int temp1 = entity.get_y() + (int) comp_height * 0.3;
            int temp2 = (int) comp_height * 0.45;
			sprintf(temp_str, "<rect fill=\"none\" style=\"stroke:black;stroke-width:1\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n", entity.get_x(), temp1, comp_width, temp2);
		}
		else if (entity.get_type() == "probe"){
			sprintf(temp_str, "<circle cx=\"%dpx\" cy=\"%dpx\" r=\"%dpx\" fill=\"white\" stroke=\"black\" />", entity.get_x() + (int) comp_width / 2, entity.get_y() + (int) comp_width / 2, (int) comp_width / 2);
			file_obj << temp_str;
			sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", entity.get_x() + (int) comp_width / 2, entity.get_y() + comp_width, entity.get_x() + (int) comp_width / 2, entity.get_y() + (int) comp_height / 2);
			file_obj << temp_str;
			sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", entity.get_in_x() + wire_length, entity.get_out_y(), entity.get_out_x() - wire_length, entity.get_out_y());
		}
		else
			sprintf(temp_str, "<rect fill=\"none\" style=\"stroke:black;stroke-width:1\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n", entity.get_x(), entity.get_y(), comp_width, comp_height);
		file_obj << temp_str;

		int in_y[4] = {0, 0, 0, 0};
		entity.get_in_y(in_y);
		for (int i = 0; i < entity.get_entry_amount(); i++) {
			sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", entity.get_x(), in_y[i], entity.get_in_x(), in_y[i]);
			file_obj << temp_str;
		}
		sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", entity.get_x() + comp_width, entity.get_out_y(), entity.get_out_x(), entity.get_out_y());
		file_obj << temp_str;

		std::string comp_type = entity.get_type();
		if (comp_type[0] == 'n') {
			sprintf(temp_str, "<circle cx=\"%dpx\" cy=\"%dpx\" r=\"%dpx\" fill=\"white\" stroke=\"black\" />", entity.get_x() + comp_width, entity.get_out_y(), 3);
			file_obj << temp_str;
		}

		if (comp_type == "and" || comp_type == "nand") {
			sprintf(temp_str, "<text x=\"%d\" y=\"%d\" font-size=\"14\" font-family=\"Arial\">%s</text>\n", entity.get_x() + 6, entity.get_y() + 20, "&amp;");
		}
		else if (comp_type == "or" || comp_type == "nor") {
			sprintf(temp_str, "<text x=\"%d\" y=\"%d\" font-size=\"14\" font-family=\"Arial\">%s</text>\n", entity.get_x() + 6, entity.get_y() + 20, "1");
		}
		else if (comp_type == "xor" || comp_type == "nxor") {
			sprintf(temp_str, "<text x=\"%d\" y=\"%d\" font-size=\"13\" font-family=\"Arial\">%s</text>\n", entity.get_x() + 3, entity.get_y() + 20, "=1");
		}
		if (comp_type != "not") {
			file_obj << temp_str;
		}

	}

	for (int i = 0; i < wire_array_pos; i++) {
		int lines[10][4];
		wire_array[i].get_lines(lines);
		for (int j = 0; j < wire_array[i].get_lines_amount(); j++) {
			sprintf(temp_str, "<path fill=\"none\" stroke=\"black\" d=\"M %d %d L %d %d\" />\n", lines[j][0], lines[j][1], lines[j][2], lines[j][3]);
			file_obj << temp_str;
		}
	}

	file_obj << "</svg>";
	file_obj.close();
}

void logger(){
	ofstream file_obj;
	file_obj.open("log.txt", ios::out | ios::trunc);

	for (int i = 0; i < component_array_pos; i++) {
		file_obj << "component #" << i << ":\n";
		file_obj << component_array[i].get_type() << " " << component_array[i].get_entry_amount() << "\n";
        file_obj << "x_coord: " << component_array[i].get_x() << ", y_coord: " << component_array[i].get_y() << "\n";
		int in_wires[4];
		component_array[i].get_in_wires(in_wires);
		file_obj << "in wires: " << in_wires[0] << " " << in_wires[1] << " "  << in_wires[2] << " "  << in_wires[3] << "\n";
		file_obj << "out wire: " << component_array[i].get_out_wire() << "\n\n";
	}

	for (int i = 0; i < wire_array_pos; i++) {
		file_obj << "wire #" << i << ":\n";
		file_obj << "in_component: " << wire_array[i].get_in_component() << "\n";
		file_obj << "out_component: " << wire_array[i].get_out_component() << ", entry: " << wire_array[i].get_out_component_entry() << "\n";
		int connected_wires[5];
		wire_array[i].get_connected_wires(connected_wires);
		file_obj << "connected wires: " << connected_wires[0] << " " << connected_wires[1] << " "  << connected_wires[2] << " "  << connected_wires[3] << " " << connected_wires[4] << "\n";
		file_obj << "parent wire: " << wire_array[i].get_parent_wire() << "\n\n";
	}
	file_obj.close();
}

void __fastcall TfrmMain::pbMainMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
	if (current_component != "") {
		round_coords(&X, &Y);
		if (valid_place(X, Y, -1)) {
			add_component(X, Y);
			selected_comp = component_array_pos - 1;
			frmMain -> Saveas1 -> Enabled = true;
			pbMain -> Invalidate();
		}
	}

	else if (model_mode) {
		for (int i = 0; i < component_array_pos; i++) {
			if (model_component_array[i].get_type() == "src"){
				int X0, Y0;
				X0 = model_component_array[i].get_x();
				Y0 = model_component_array[i].get_y();
				if (X >= X0 && X <= X0 + comp_width && Y >= Y0 + (int) comp_height * 0.3 && Y <= Y0 + (int) comp_height * 0.75){
					int temp = model_component_array[i].get_out_charge();
					temp = temp ? 0 : 1;
					model_component_array[i].set_out_charge(temp);
					break;
				}
			}
		}
		reset_charges();
		if (model_scheme() == 1){
			cursor_mode = true;
			model_mode = false;
			Application -> MessageBox(L"Undefined behaviour", L"LogicBuilder", MB_OK | MB_ICONERROR);
		}
		pbMain -> Invalidate();
	}

	else if (cursor_mode) {
		if (selected_comp != -1){
			pbMain -> Invalidate();
			selected_comp = -1;
		}
		if (selected_wire != -1){
			pbMain -> Invalidate();
			selected_wire = -1;
		}
		for (int i = 0; i < component_array_pos; i++) {
			int X0, Y0;
			X0 = component_array[i].get_x();
			Y0 = component_array[i].get_y();
			if (X >= X0 && X <= X0 + comp_width && Y >= Y0 && Y <= Y0 + comp_height) {
				selected_comp = i;
				if (Shift.Contains(ssLeft)) {
					x_start_move = X;
					y_start_move = Y;
				}
				break;
			}
		}
		for (int i = 0; i < wire_array_pos; i++) {
			int lines[10][4];
			wire_array[i].get_lines(lines);
			for (int j = 0; j < wire_array[i].get_lines_amount(); j++) {
				if ((abs(X - lines[j][0]) < 4 && abs(X - lines[j][2]) < 4 &&
					(Y >= lines[j][1] && Y <= lines[j][3] || Y <= lines[j][1] && Y >= lines[j][3])) ||
					(abs(Y - lines[j][1]) < 4 && abs(Y - lines[j][3]) < 4 &&
					(X >= lines[j][0] && X <= lines[j][2] || X <= lines[j][0] && X >= lines[j][2]))) {
					selected_wire = i;
					break;
				}
			}
		}
		pbMain -> Invalidate();
	}

	else if (wire_mode) {

		switch (wire_stage) {
			case wsBegin:
				if (branch_wire_mode)
					parent_wire = valid_branch_wire_start(&X, &Y);
				if (valid_wire_start(&X, &Y) && !branch_wire_mode || branch_wire_mode && parent_wire != -1) {
					current_wire[current_wire_pos][0] = X;
					current_wire[current_wire_pos][1] = Y;
					wire_stage = wsMiddle;

					move_line_buffer[move_line_buffer_pos][0] = X;
					move_line_buffer[move_line_buffer_pos][1] = Y;
				}
				break;

			case wsMiddle:
				correct_with_angle(&X, &Y);
				if (current_wire_pos == 9){
					wire_stage = wsEnd;
					break;
				}
				if (valid_wire_middle(&X, &Y)) {
					current_wire[current_wire_pos][2] = X;
					current_wire[current_wire_pos][3] = Y;
					current_wire_pos++;
					current_wire[current_wire_pos][0] = X;
					current_wire[current_wire_pos][1] = Y;

					move_line_buffer[move_line_buffer_pos][2] = X;
					move_line_buffer[move_line_buffer_pos][3] = Y;
					move_line_buffer_pos++;
					move_line_buffer[move_line_buffer_pos][0] = X;
					move_line_buffer[move_line_buffer_pos][1] = Y;
				}
				break;

			case wsEnd:
				if (valid_wire_end(&X, &Y)) {

					autocorrect_wire_end(X, Y);
					current_wire[current_wire_pos][2] = X;
					current_wire[current_wire_pos][3] = Y;

					add_wire(current_wire);
					current_wire_pos = 0;
					wire_stage = wsBegin;
					pbMain -> Invalidate();
					move_line_buffer_pos = 0;
				}
				break;
		}
	}
}

void __fastcall TfrmMain::pbMainPaint(TObject *Sender)
{
	if (to_draw_grid)
		draw_grid(pbMain);
	if (!model_mode)
		for (int i = 0; i < component_array_pos; i++)
			draw_component(pbMain, component_array[i]);
	else
		for (int i = 0; i < component_array_pos; i++)
			draw_model_component(pbMain, i);
	if (selected_comp != -1) {
		draw_highlight(pbMain, selected_comp);
	}
	if (x_dot_highlight != -1 && y_dot_highlight != -1) {
		draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
	}
	for (int i = 0; i < wire_array_pos; i++) {
		draw_wire(pbMain, wire_array[i]);
	}
	if (selected_wire != -1) {
		draw_wire_highlight(pbMain, selected_wire);
	}
	if (wire_mode && wire_stage != wsBegin) {
		draw_temp_lines(pbMain);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	Save1 -> Enabled = false;
	Saveas1 -> Enabled = false;
	OpenDialog -> Filter = "LogicBuilder files(.lb)|*.lb|";
	move_step = grid_width;
	frmMain -> DoubleBuffered = true;
	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::lboxComponentsDblClick(TObject *Sender)
{
	if (!model_mode) {
        current_component = AnsiString(lboxComponents -> Items -> Strings[lboxComponents -> ItemIndex]).c_str();
		cursor_mode = false;
		selected_comp = -1;
		selected_wire = -1;
		pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actTakeCursorExecute(TObject *Sender)
{
	cursor_mode = true;
	wire_mode = false;
	move_line_buffer_pos = 0;
	branch_wire_mode = false;
	current_component = "";
	logger();
    pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveUpExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x, y - move_step, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x, y - move_step);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveDownExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x, y + move_step, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x, y + move_step);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveLeftExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x - move_step, y, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x - move_step, y);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actMoveRightExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		if (valid_place(x + move_step, y, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp],
				x + move_step, y);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actDeleteComponentExecute(TObject *Sender)
{
	if (selected_comp != -1) {
		delete_component(selected_comp);
		frmMain -> Saveas1 -> Enabled = true;
		selected_comp = -1;
	}
	if (selected_wire != -1) {
		delete_wire(selected_wire);
        frmMain -> Saveas1 -> Enabled = true;
		selected_wire = -1;
	}
	cursor_mode = true;
	current_component = "";
	pbMain -> Invalidate();
}

void __fastcall TfrmMain::actSaveFileExecute(TObject *Sender)
{
	if (file_dir != "") {
		save_to_file(file_dir);
		Save1 -> Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actOpenFileExecute(TObject *Sender)
{
	if (OpenDialog -> Execute()) {
		file_dir = AnsiString(OpenDialog -> FileName).c_str();
		open_file(file_dir);
		frmMain -> Caption = ("LogicBuilder - " + file_dir).c_str();
		Save1 -> Enabled = true;
		selected_comp = -1;
		selected_wire = -1;
		current_component = "";
		cursor_mode = true;
		pbMain -> Invalidate();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSaveFileAsExecute(TObject *Sender)
{
	if (SaveDialog -> Execute()) {
		file_dir = AnsiString(SaveDialog -> FileName).c_str();
		save_to_file(file_dir + ".lb");
		frmMain -> Caption = ("LogicBuilder - " + file_dir + ".lb").c_str();
		Save1 -> Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSVGExportExecute(TObject *Sender)
{
	if (SaveDialog -> Execute()) {
		file_dir = AnsiString(SaveDialog -> FileName).c_str();
		to_svg(file_dir);
	}
}

void __fastcall TfrmMain::actExitExecute(TObject *Sender)
{
	frmMain -> Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSetWireModeExecute(TObject *Sender)
{
	wire_mode = true;
	branch_wire_mode = false;
	move_line_buffer_pos = 0;
	wire_stage = wsBegin;
	cursor_mode = false;
	current_component = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actEndWireExecute(TObject *Sender)
{
	if (wire_mode)
		wire_stage = wsEnd;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pbMainMouseMove(TObject *Sender, TShiftState Shift, int X,
		  int Y)
{
	if (x_dot_highlight != -1 && y_dot_highlight != -1) {
		if (abs(x_dot_highlight - X) > grid_width || abs(y_dot_highlight - Y) > grid_width){
			x_dot_highlight = -1;
			y_dot_highlight = -1;
			pbMain -> Invalidate();
		}
	}

	if (wire_mode && wire_stage == wsBegin) {
		if (!branch_wire_mode) {
			if (valid_wire_start(&X, &Y)) {
				x_dot_highlight = X;
				y_dot_highlight = Y;
				draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
			}
		}
		else{
			if (valid_branch_wire_start(&X, &Y) != -1) {
				x_dot_highlight = X;
				y_dot_highlight = Y;
				draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
			}
		}
	}

	if (wire_mode && wire_stage == wsEnd) {
		if (valid_wire_end(&X, &Y)) {
			x_dot_highlight = X;
			y_dot_highlight = Y;
			draw_dot_highlight(pbMain, x_dot_highlight, y_dot_highlight);
		}
	}

	if (wire_mode) {
		correct_with_angle(&X, &Y);
		move_line_buffer[move_line_buffer_pos][2] = X;
		move_line_buffer[move_line_buffer_pos][3] = Y;
		pbMain -> Invalidate();
	}

	if (selected_comp != -1 && cursor_mode && Shift.Contains(ssLeft)) {
		int x, y;
		x = component_array[selected_comp].get_x() + X - x_start_move;
		y = component_array[selected_comp].get_y() + Y - y_start_move;
		if (valid_place(x, y, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp], x, y);
			x_start_move = X;
			y_start_move = Y;
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::actNewFileExecute(TObject *Sender)
{
	Save1 -> Enabled = false;
	Saveas1 -> Enabled = false;
    frmMain -> Caption = "LogicBuilder";

	component_array_pos = 0;
	current_component = "";

	wire_array_pos = 0;
	current_wire_pos = 0;
	wire_stage = wsBegin;

	x_dot_highlight = -1;
	y_dot_highlight = -1;

	cursor_mode = true;
	wire_mode = false;
	move_line_buffer_pos = 0;
	selected_comp = -1;
	selected_wire = -1;
	file_dir = "";

	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actBranchWireExecute(TObject *Sender)
{
	wire_mode = true;
	branch_wire_mode = true;
	wire_stage = wsBegin;
	move_line_buffer_pos = 0;
	cursor_mode = false;
	current_component = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actDrawGridExecute(TObject *Sender)
{
	to_draw_grid = to_draw_grid ? false : true;
	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actSetModelModeExecute(TObject *Sender)
{
	model_mode = model_mode ? false : true;
	cursor_mode = model_mode ? false : true;
	wire_mode = false;
	branch_wire_mode = false;
	current_component = "";
	move_line_buffer_pos = 0;

	if (model_mode){
		init_model_array();
		if (model_scheme() == 1){
			cursor_mode = true;
			model_mode = false;
			Application -> MessageBox(L"Undefined behaviour", L"LogicBuilder", MB_OK | MB_ICONERROR);
		}

	}

	pbMain -> Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Help1Click(TObject *Sender)
{
	frmHelp -> Show();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::pbMainMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{

	if (selected_comp != -1 && cursor_mode) {
		int x, y;
		x = component_array[selected_comp].get_x();
		y = component_array[selected_comp].get_y();
		round_coords(&x, &y);
		if (valid_place(x, y, selected_comp)) {
			component_array[selected_comp] = modify_component_position(component_array[selected_comp], x, y);
			pbMain -> Invalidate();
		}
	}
}
//---------------------------------------------------------------------------

