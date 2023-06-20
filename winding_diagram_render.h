/*****************************************************************//**
 * @file   winding_diagram_render.h
 * @brief  Winding Diagram rendering procedure.
 * 
 * @author S.Watanabe
 * @date   June 2023
 * 
 * @license MIT (only these codes. You must comply with the license set for each external library you reference.)
 * @version 0.2.1
 * 
 *********************************************************************/


#ifndef WINDING_DIAGRAM_RENDER_H
#define WINDING_DIAGRAM_RENDER_H

#include "GL/glut.h"
#include "GL/freeglut.h"
#include "nlohmann/json.hpp"

#include "gl_util.h"
#include "wd_config_decode.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>

#include <iostream>

using namespace nlohmann;
using namespace std;

int _sign(double x);

void render_stator(json& code);

void render_winding(json& code);


#endif