/*****************************************************************//**
 * @file   gl_util.h
 * @brief  Basic geometric drawing function set based on freeGLUT.
 *
 * 
 * @author S.Watanabe
 * @date   June 2023
 * 
 * @license MIT (only these codes. You must comply with the license set for each external library you reference.)
 * @version 0.1
 * 
 *********************************************************************/

#ifndef GL_UTIL_H
#define GL_UTIL_H

#include "GL/glut.h"
#include "GL/freeglut.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>


/**
 * @brief drawing rectangle on openGL 2D buffer.
 * @param[in] p1  point at upper-left edge of rectangle.
 * @param[in] p2  point at bottom-right edge of rectangle.
 * @param[in] fill  Set true to fill with color. Default is true.
 */
void draw_rect(GLdouble* p1, GLdouble* p2, bool fill = true);


/**
 * @brief drawing arrow on openGL 2D buffer. this function considers freeGLUT window aspect ratio.
 * @param[in] pos_start  arrow line start point.
 * @param[in] pos_end  arrow line end point.
 * @param[in] r_pos_arrow  Location of arrowhead (0. ~ 1.), 
 *			When set to 0, the arrowhead is drawn at the start point;
 *			when set to 0.5, the arrowhead is drawn at the mid point; 
 *			when set to 1, the arrowhead is drawn at the end point.
 * 
 * @param[in] arrow_size  arrowhead size
 * @param[in] line_type  select line type from "solid", "dash", "dot", "dash-dot", Default is "solid".
 * 
 */
void draw_arrow(std::vector<GLdouble>& pos_start, std::vector<GLdouble>& pos_end, GLdouble r_pos_arrow, GLdouble arrow_size, std::string line_type="solid");


/**
 * @brief drawing polyline on openGL 2D buffer. Endpoints are not connected by a line.
 * @param[in] p_list  vector of point coordinates. Number of elements must be at least 2.  example: {{x1, y1}, {x2, y2}, {x3, y3}}
 * @param[in] line_type  select line type from "solid", "dash", "dot", "dash-dot", Default is "solid".
 *
 */
void draw_lines(std::vector<std::vector<GLdouble>>& p_list, std::string line_type = "solid");

/**
 * @brief drawing a string on openGL 2D buffer. This function calls freeGLUT functions.
 * @param[in] x  x-axis of drawing point. 
 * @param[in] y  y-axis of drawing point. 
 * @param[in] font  select font style from font set provided by freeGLUT. Default is GLUT_BITMAP_HELVETICA_12.
 *
 */
void draw_string(GLdouble x, GLdouble y, std::string text, void* font = GLUT_BITMAP_HELVETICA_12);

#endif