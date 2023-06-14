#include "winding_diagram_render.h"
#include <math.h>

int _sign(double x)
{
	return x == 0. ? 0 : (x > 0 ? 1 : -1);
}

void render_stator(json& code)
{
	const vector<int> slot_range = code["slot_range"].get<vector<int>>();
	const int direction =  _sign(slot_range[1] - slot_range[0]);
	const int slots = abs(slot_range[1] - slot_range[0]) + 1;
	const int period = code["period"].get<int>();
	const int s0 = (max(abs(slot_range[0]), abs(slot_range[1])) / period + 1) * period;

	vector<GLdouble> num_offset = {0., 0.};
	const vector<GLdouble> x_range = {(-1. + static_cast<double>(code["side_margin"].get<double>())/2.0), (1. - static_cast<double>(code["side_margin"].get<double>()) / 2.0) };
	const double W = abs(x_range[1] - x_range[0]);
	const double w_teeth = W / static_cast<double>(slots) * static_cast<double>(code["teeth_propotion"].get<double>());
	const double w_slot = W / static_cast<double>(slots) * (1. - code["teeth_propotion"].get<double>());

	void* font = GLUT_BITMAP_HELVETICA_12;
	int font_addr_num;

	vector<GLdouble> pos_start, pos_end;
	vector<GLdouble> c_teeth = {};
	vector<GLdouble> c_frame = {};
	GLdouble pitch, rect_size_y, rect_dx, x, y;

	if (code.contains("slot_annotation_offset")){ jsonlist2vector(code["slot_annotation_offset"], num_offset); }
	if (code.contains("font")){ font_addr_num = GLUT_FONT_TABLE[static_cast<string>(code["font"].get<string>())]; font = reinterpret_cast<void*>(font_addr_num); }


	for (int j = 0; j < static_cast<int>(code["divs"].get<int>()); ++j)
	{
		pos_start = { (-1. + static_cast<double>(code["side_margin"].get<double>()) / 2.0), -1. + static_cast<double>(code["div_margin"].get<double>()) / static_cast<double>(code["divs"].get<double>()) / 2. * (2 * j + 1) + (2. - static_cast<double>(code["div_margin"].get<double>())) / static_cast<double>(code["divs"].get<double>()) * j };
		pos_end = { (1. - static_cast<double>(code["side_margin"].get<double>()) / 2.0), -1 + static_cast<double>(code["div_margin"].get<double>()) / static_cast<double>(code["divs"].get<double>()) / 2. * (2 * j + 1) + (2. - static_cast<double>(code["div_margin"].get<double>())) / static_cast<double>(code["divs"].get<double>()) * (j + 1) };
		pitch = (pos_end[0] - pos_start[0]) / slots;
		rect_size_y = (pos_end[1] - pos_start[1]);
		rect_dx = pitch * code["teeth_propotion"].get<double>();

		x = pos_start[0];
		y = pos_start[1];

		for (int i = slot_range[0]; slot_range[1] - i * direction + direction != 0; i += direction)
		{
			color_name2RGB(code["teeth_color"].get<string>(), c_teeth);

			glColor3d(0.0, 0.0, 0.0);
			glLineWidth(1);

			draw_string(x - pitch / 2 - direction * w_teeth / 2.0 + num_offset[0], (pos_start[1] + pos_end[1]) / 2.0 + num_offset[1], to_string((i - 1 + s0) % period + 1), font = font);
			glColor3dv(c_teeth.data());
			draw_rect(vector<GLdouble>({ x - direction * w_teeth / 2.0, y }).data(), vector<GLdouble>({x + rect_dx - direction * w_teeth / 2.0, y + rect_size_y}).data());
			
			if (code.contains("teeth_frame_color"))
			{
				color_name2RGB(code["teeth_frame_color"].get<string>(), c_frame);
				glColor3dv(c_frame.data());
				if (code.contains("teeth_frame_width"))
				{
					glLineWidth(code["teeth_frame_width"].get<double>());
				}
				else
				{
					glLineWidth(1.);
				}

				draw_rect(vector<GLdouble>({ x - direction * w_teeth / 2.0, y }).data(), vector<GLdouble>({ x + rect_dx - direction * w_teeth / 2.0, y + rect_size_y }).data(), false);
			}

			x += pitch;
		}

	}
}


void render_winding(json& code)
{
	json com = json::object();
	get_wirering_command(code, com);
	const vector<int> slot_range = code["slot_range"].get<vector<int>>();
	const int direction = _sign(slot_range[1] - slot_range[0]);
	int x_index = 1 - (direction + 1) / 2;
	const int slots = abs(slot_range[1] - slot_range[0]) + 1;
	const int slot0 = min(slot_range[0], slot_range[1]) + direction;

	for (auto& coil_prop_set : com.items())
	{
		string label = coil_prop_set.key();
		json coil_prop = coil_prop_set.value();

		GLdouble side_margin = code["side_margin"].get<double>(), div_margin = code["div_margin"].get<double>();
		int stator_divs = code["divs"].get<int>(), coil_divs = coil_prop["divs"].get<int>();

		vector<GLdouble> x_range = { (-1. + side_margin / 2.), (1. - side_margin / 2.) };
		vector<GLdouble> y_range = { -1. + div_margin / stator_divs / 2. * (2 * coil_divs - 1) + (2. - div_margin) / stator_divs * (coil_divs - 1),
				   -1. + div_margin / stator_divs / 2. * (2 * coil_divs - 1) + (2. - div_margin) / stator_divs * coil_divs };

		const GLdouble W = abs(x_range[1] - x_range[0]);
		const GLdouble H = abs(y_range[1] - y_range[0]);
		const GLdouble w_teeth = W / static_cast<GLdouble>(slots) * static_cast<GLdouble>(code["teeth_propotion"].get<double>());
		const GLdouble w_slot = W / static_cast<GLdouble>(slots) * (1. - static_cast<GLdouble>(code["teeth_propotion"].get<double>()));

		// vector<vector<int>> path = static_cast<vector<vector<int>>>(coil_prop["path"]);
		vector<vector<int>> path;
		jsonlist2vector(coil_prop["path"], path);

		int insert_dir = 1;
		vector<GLdouble> x_margin = { 0.8 * w_slot };
		vector<GLdouble> y_margin = { static_cast<GLdouble>(code["div_margin"].get<double>()) / static_cast<GLdouble>(code["divs"].get<double>()) * 3 / 10. };
		vector<GLdouble> y_jumper_margin = { static_cast<GLdouble>(code["div_margin"].get<double>()) / static_cast<GLdouble>(code["divs"].get<double>()) * 2 / 10, static_cast<GLdouble>(code["div_margin"].get<double>()) / static_cast<GLdouble>(code["divs"].get<double>()) * 4 / 10 };
		GLdouble y_terminal_margin = static_cast<GLdouble>(code["div_margin"].get<double>()) / static_cast<GLdouble>(code["divs"].get<double>()) / 2.0 * 0.2;
		bool en_jumper = true;
		bool coil_arrow = true;
		bool jumper_arrow = false;
		bool terminal = true;
		GLdouble pos_coil_arrow = 0.5;
		GLdouble pos_jumper_arrow = 0.8;
		vector<GLdouble> coil_offset = { 0., 0. };
		vector<GLdouble> label_offset = { 0.01, 0. };
		int font_addr_num;
		void* font = GLUT_BITMAP_HELVETICA_12;

		vector<GLdouble> c_coil;

		//Optional Variables
		if (coil_prop.contains("insert_direction")) { insert_dir = _sign(coil_prop["insert_direction"].get<int>()); }
		if (coil_prop.contains("coil_x_margin")) { jsonlist2vector(coil_prop["coil_x_margin"], x_margin);/*x_margin = static_cast<vector<GLdouble>>(coil_prop["coil_x_margin"]);*/ }
		if (coil_prop.contains("coil_y_margin")) { jsonlist2vector(coil_prop["coil_y_margin"], y_margin);/*y_margin = static_cast<vector<GLdouble>>(coil_prop["coil_y_margin"]);*/ }
		if (coil_prop.contains("jumper_y_margin")) { jsonlist2vector(coil_prop["jumper_y_margin"], y_jumper_margin);/*y_jumper_margin = static_cast<vector<GLdouble>>(coil_prop["jumper_y_margin"]);*/ }
		if (coil_prop.contains("terminal_y_margin")) { y_terminal_margin = static_cast<GLdouble>(coil_prop["terminal_y_margin"].get<double>()); }
		if (coil_prop.contains("jumper")) { en_jumper = static_cast<bool>(coil_prop["jumper"].get<bool>()); }
		if (coil_prop.contains("coil_arrow")) { coil_arrow = static_cast<bool>(coil_prop["coil_arrow"].get<bool>()); }
		if (coil_prop.contains("jumper_arrow")) { jumper_arrow = static_cast<bool>(coil_prop["jumper_arrow"].get<bool>()); }
		if (coil_prop.contains("terminal")) { terminal = static_cast<bool>(coil_prop["terminal"].get<bool>()); }
		if (coil_prop.contains("coil_arrow_position")) { pos_coil_arrow = static_cast<GLdouble>(coil_prop["coil_arrow_position"].get<double>()); }
		if (coil_prop.contains("jumper_arrow_position")) { pos_jumper_arrow = static_cast<GLdouble>(coil_prop["jumper_arrow_position"].get<double>()); }
		if (coil_prop.contains("coil_offset")) { jsonlist2vector(coil_prop["coil_offset"], coil_offset);/*coil_offset = static_cast<vector<GLdouble>>(coil_prop["coil_offset"]);*/ }
		if (coil_prop.contains("label_offset")) { jsonlist2vector(coil_prop["label_offset"], label_offset);/*label_offset = static_cast<vector<GLdouble>>(coil_prop["label_offset"]);*/ }
		if (coil_prop.contains("font")) { font_addr_num = GLUT_FONT_TABLE[static_cast<string>(coil_prop["font"].get<string>())]; font = reinterpret_cast<void*>(font_addr_num); }

		color_name2RGB(static_cast<string>(coil_prop["color"].get<string>()), c_coil);
		glColor3dv(c_coil.data());

		if (insert_dir < 0)// When the insert direction is reversed
		{
			auto tmp = y_range[0];
			y_range[0] = y_range[1];
			y_range[1] = tmp;
		}

		//for margin counter
		int i = 0, j = 0;

		for (auto& p_set : path)
		{
			auto p = p_set[0], p_next = p_set[1];
			GLdouble x0, x1, x2, x3, x4, x5, x6, x7, y1, y2, y3, y4, y5, y6, y7;
			vector<vector<GLdouble>> v_coil_path;

			//jumper node
			x0 = x_range[x_index] + W * (p + p_next - 2 * slot0) / slots / 2.0 * direction + w_slot * direction / 2.0 + w_teeth / 2.0 + coil_offset[0];
			x1 = x0;
			y1 = y_range[0] - y_margin[j] * insert_dir + coil_offset[1];
			x2 = x_range[x_index] + W * (p - slot0) / slots * direction + x_margin[i] * direction + w_teeth / 2.0 + coil_offset[0];
			y2 = y_range[0] + coil_offset[1];
			x3 = x2;
			y3 = y_range[1] + coil_offset[1];
			x4 = x0;
			y4 = y_range[1] + y_margin[j] * insert_dir + coil_offset[1];
			x5 = x_range[x_index] + W * (p_next - slot0) / slots * direction + w_slot * direction - x_margin[i] * direction + w_teeth / 2.0 + coil_offset[0];
			y5 = y_range[1] + coil_offset[1];
			x6 = x5;
			y6 = y_range[0] + coil_offset[1];
			x7 = x0;
			y7 = y1;

			v_coil_path.emplace_back(vector<GLdouble>({ x1, y1 }));
			v_coil_path.emplace_back(vector<GLdouble>({ x2, y2 }));
			v_coil_path.emplace_back(vector<GLdouble>({ x3, y3 }));
			v_coil_path.emplace_back(vector<GLdouble>({ x4, y4 }));
			v_coil_path.emplace_back(vector<GLdouble>({ x5, y5 }));
			v_coil_path.emplace_back(vector<GLdouble>({ x6, y6 }));
			v_coil_path.emplace_back(vector<GLdouble>({ x7, y7 }));


			if (coil_prop.contains("coil_width")) { glLineWidth(static_cast<GLdouble>(coil_prop["coil_width"].get<double>())); }
			else { glLineWidth(1.); }

			if (coil_prop.contains("coil_line_style")) 
			{ 
				draw_lines(v_coil_path, coil_prop["coil_line_style"].get<string>());
				if (coil_arrow)
				{
					vector<GLdouble> v_arrow_path_st = {x2, y2}, v_arrow_path_ed = {x3, y3};
					draw_arrow(v_arrow_path_st, v_arrow_path_ed, pos_coil_arrow, 0.01, coil_prop["coil_line_style"].get<string>());
					v_arrow_path_st[0] = x5; v_arrow_path_st[1] = y5; v_arrow_path_ed[0] = x6; v_arrow_path_ed[1] = y6;
					draw_arrow(v_arrow_path_st, v_arrow_path_ed, pos_coil_arrow, 0.01, coil_prop["coil_line_style"].get<string>());
				}
			}
			else
			{
				draw_lines(v_coil_path);
				if (coil_arrow)
				{
					vector<GLdouble> v_arrow_path_st = { x2, y2 }, v_arrow_path_ed = { x3, y3 };
					draw_arrow(v_arrow_path_st, v_arrow_path_ed, pos_coil_arrow, 0.01);
					v_arrow_path_st[0] = x5; v_arrow_path_st[1] = y5; v_arrow_path_ed[0] = x6; v_arrow_path_ed[1] = y6;
					draw_arrow(v_arrow_path_st, v_arrow_path_ed, pos_coil_arrow, 0.01);
				}
			}

			i = (i + 1) % x_margin.size();
			j = (j + 1) % y_margin.size();
		}

		//draw start&end point
		if (terminal)
		{
			GLdouble x_start, x_end, y_L_start, y_L_end, y_H_start, y_H_end;
			vector<GLdouble> v_arrow_path_1 = { 0., 0. }, v_arrow_path_2 = { 0., 0. };

			x_start = x_range[x_index] + W * (path[0][0] - slot0) / slots * direction + x_margin[0] * direction + w_teeth / 2.0 + coil_offset[0];
			x_end = x_range[x_index] + W * (path.back()[1] - slot0) / slots * direction + w_slot * direction - x_margin[(path.size() - 1)%x_margin.size()] * direction + w_teeth / 2.0 + coil_offset[0];
			y_L_start = y_range[0] - static_cast<GLdouble>(code["div_margin"].get<double>()) / static_cast<GLdouble>(code["divs"].get<double>()) / 2.0 * insert_dir + y_terminal_margin * insert_dir + coil_offset[1];
			y_H_start = y_range[0] + coil_offset[1];
			y_L_end = y_range[0] - static_cast<GLdouble>(code["div_margin"].get<double>()) / static_cast<GLdouble>(code["divs"].get<double>()) / 2.0 * insert_dir + y_terminal_margin * insert_dir + coil_offset[1];
			y_H_end = y_range[0] + coil_offset[1];

			v_arrow_path_1[0] = x_start; v_arrow_path_1[1] = y_L_start; v_arrow_path_2[0] = x_start; v_arrow_path_2[1] = y_H_start;

			draw_arrow(v_arrow_path_1, v_arrow_path_2, 1.0, 0.01);
			v_arrow_path_1[0] = x_end; v_arrow_path_1[1] = y_H_end; v_arrow_path_2[0] = x_end; v_arrow_path_2[1] = y_L_end;
			draw_arrow(v_arrow_path_1, v_arrow_path_2, 1.0, 0.01);

			draw_string(x_start + label_offset[0], y_L_start + label_offset[1], label, font);
			draw_string(x_end + label_offset[0], y_L_start + label_offset[1], label + "\'", font);
		}


		//draw jumper
		if (en_jumper)
		{
			vector<vector<int>> jumper_path;
			for (int i = 0; i < path.size() - 1; ++i)
			{
				jumper_path.emplace_back(vector<int>({ path[i][1], path[i + 1][0] }));
			}

			//margin counter
			int i = 0, j = 0;

			//vars
			vector<vector<GLdouble>> jumper_coord;
			vector<GLdouble> v_arrow_path_1 = { 0., 0. }, v_arrow_path_2 = { 0., 0. };
			GLdouble x_jumper_start, x_jumper_end, y_jumper_L, y_jumper_H;
			for (auto& p_set : jumper_path)
			{
				jumper_coord.clear();

				x_jumper_start = x_range[x_index] + W * (p_set[0] - slot0) / slots * direction + w_slot * direction - x_margin[i] * direction + w_teeth / 2.0 + coil_offset[0];
				x_jumper_end = x_range[x_index] + W * (p_set[1] - slot0) / slots * direction + x_margin[(i + 1) % x_margin.size()] * direction + w_teeth / 2.0 + coil_offset[0];
				y_jumper_L = y_range[0] - y_jumper_margin[j] * insert_dir + coil_offset[1];
				y_jumper_H = y_range[0] + coil_offset[1];

				jumper_coord.emplace_back(vector<GLdouble>({ x_jumper_start, y_jumper_H }));
				jumper_coord.emplace_back(vector<GLdouble>({ x_jumper_start, y_jumper_L }));
				jumper_coord.emplace_back(vector<GLdouble>({ x_jumper_end, y_jumper_L }));
				jumper_coord.emplace_back(vector<GLdouble>({ x_jumper_end, y_jumper_H }));

				v_arrow_path_1[0] = x_jumper_start;
				v_arrow_path_1[1] = y_jumper_L;
				v_arrow_path_2[0] = x_jumper_end;
				v_arrow_path_2[1] = y_jumper_L;

				if (coil_prop.contains("jumper_width")) { glLineWidth(coil_prop["jumper_width"].get<double>()); }
				else { glLineWidth(1.); }

				if(coil_prop.contains("jumper_line_style"))
				{
					draw_lines(jumper_coord, coil_prop["jumper_line_style"].get<string>());
					if (jumper_arrow)
					{
						draw_arrow(v_arrow_path_1, v_arrow_path_2, pos_jumper_arrow, 0.01, coil_prop["jumper_line_style"].get<string>());
					}
				}
				else
				{
					draw_lines(jumper_coord);
					if (jumper_arrow)
					{
						draw_arrow(v_arrow_path_1, v_arrow_path_2, pos_jumper_arrow, 0.01);
					}
				}

				i = (i + 1) % x_margin.size();
				j = (j + 1) % y_jumper_margin.size();

			}
		}

	}
}