#include "gl_util.h"

void draw_rect(GLdouble* p1, GLdouble* p2, bool fill)
{
	if (fill)
	{
		glBegin(GL_QUADS);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}

	glVertex2d(p1[0], p1[1]);
	glVertex2d(p1[0], p2[1]);
	glVertex2d(p2[0], p2[1]);
	glVertex2d(p2[0], p1[1]);

	glEnd();
}


void draw_lines(std::vector<std::vector<GLdouble>>& p_list, std::string line_type)
{
	if (line_type == "solid")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xFFFF);
	}
	else if (line_type == "dash")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xF0F0);
	}
	else if (line_type == "dot")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xCCCC);
	}
	else if (line_type == "dash-dot")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xF018);
	}

	if (p_list.size() > 1)
	{
		glBegin(GL_LINE_STRIP);
		for (auto& p : p_list)
		{
			glVertex2dv(p.data());
		}

		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
}

void draw_arrow(std::vector<GLdouble>& pos_start, std::vector<GLdouble>& pos_end, GLdouble r_pos_arrow, GLdouble arrow_size, std::string line_type)
{
	if (line_type == "solid")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xFFFF);
	}
	else if (line_type == "dash")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xF0F0);
	}
	else if (line_type == "dot")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xCCCC);
	}
	else if (line_type == "dash-dot")
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0xF018);
	}

	glBegin(GL_LINES);
	glVertex2dv(pos_start.data());
	glVertex2dv(pos_end.data());
	glEnd();

	glDisable(GL_LINE_STIPPLE);

	const GLdouble x_arrow = pos_end[0] * r_pos_arrow + pos_start[0] * (1 - r_pos_arrow);
	const GLdouble y_arrow = pos_end[1] * r_pos_arrow + pos_start[1] * (1 - r_pos_arrow);

	const GLdouble w = glutGet(GLUT_WINDOW_WIDTH);
	const GLdouble h = glutGet(GLUT_WINDOW_HEIGHT);
	const GLdouble r = sqrt(w * w + h * h + 1);
	const GLdouble theta = atan2((pos_end[1] - pos_start[1]) / (h / r), (pos_end[0] - pos_start[0]) / (w / r));
	const GLdouble theta1 = M_PI/6.0;

	glPushMatrix();

	glTranslated(x_arrow, y_arrow, 0.0);
	glScaled(1 / (w/r), 1 / (h/r), 1.);
	glRotated(theta / M_PI * 180.0, 0., 0., 1.);


	glBegin(GL_TRIANGLES);
	glVertex2d(arrow_size / (w/r), 0.);
	glVertex2d(arrow_size * (cos(M_PI - theta1)), -arrow_size * (sin(M_PI - theta1)));
	glVertex2d(arrow_size * (cos(M_PI + theta1)), -arrow_size * (sin(M_PI + theta1)));
	glEnd();

	glPopMatrix();
}

void draw_string(GLdouble x, GLdouble y, std::string text, void* font)
{
	glRasterPos2d(x, y);
	for (char c : text)
	{
		glutBitmapCharacter(font, c);
	}
}