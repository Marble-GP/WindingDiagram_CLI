/**********************************************************************
 *
 * @file   main.cpp
 * @brief  main routine of Winding Diagram
 * 
 * @author S.Watanabe
 * @date   June 2023
 * 
 * @lisence MIT (only these codes. You must comply with the license set for each external library you reference.)
<<<<<<< Updated upstream
 * @version 0.1
=======
 * @version 0.3
>>>>>>> Stashed changes
 *  
 *********************************************************************/



#include "GL/glut.h"
#include "nlohmann/json.hpp"
#include "png.h"
#include "wd_config_decode.hpp"
#include "winding_diagram_render.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#if _WIN32
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng16.lib")
#endif

#define FILE_LINE_BUF_SIZE (0x0FFF)

json* p_code;
std::string file_name;

void display(void);
void OnKeyUp(unsigned char key, int x, int y);


void display(void)
{
	std::vector<GLdouble> c_background;
	color_name2RGB((*p_code)["background"].get<std::string>(), c_background);
	glClearColor(c_background[0], c_background[1], c_background[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	render_stator(*p_code);
	render_winding(*p_code);

	glFlush();
}

void OnKeyUp(unsigned char key, int x, int y)
{
	if (key == ' ')
	{
		std::string path, directory, original_filename, basename, ext;
		int sw, sh;

		png_image png;
		int stride;



		path_split(file_name, directory, original_filename);
		path_splitext(original_filename, basename, ext);

		display();

		glReadBuffer(GL_FRONT);

		sw = glutGet(GLUT_WINDOW_WIDTH) / 8 * 8;
		sh = glutGet(GLUT_WINDOW_HEIGHT) / 8 * 8;
		std::vector<GLubyte> buf(sw * sh * 3);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glReadPixels(0, 0, sw, sh, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(buf.data()));
		
		//libpng process to save rendering image
		memset(&png, 0, sizeof(png));
		png.version = PNG_IMAGE_VERSION;
		png.width = sw;
		png.height = sh;
		png.format = PNG_FORMAT_RGB;

		stride = PNG_IMAGE_ROW_STRIDE(png);

		png_image_write_to_file(&png, (directory + "\\" + basename + ".png").c_str(), 0, reinterpret_cast<const void*>(buf.data()), -stride, nullptr);
		std::cout << std::string("Image save as ") + "\"" + (directory + "\\" + basename + ".png") + "\"" << std::endl;
	}
}

int main(int argc, char* argv[])
{
	char file_line_buf[FILE_LINE_BUF_SIZE] = {};
	std::string file_basename, _;
	if (argc == 1)
	{
		std::cout << "Enter winding diagram configure json file:";
		std::cin >> file_name;
	}
	else if (argc == 2)
	{
		file_name = argv[1];
	}

	p_code = new json();
	std::ifstream json_f(file_name);

	path_splitext(file_name, file_basename, _);

	json_f >> *p_code;
	
	if (p_code->is_object()) { std::cout << "Load file successflly." << std::endl; }

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize((*p_code)["size"].get<vector<int>>()[0], (*p_code)["size"].get<vector<int>>()[1]);
	glutCreateWindow(file_basename.c_str());
	glutDisplayFunc(display);
	glutKeyboardUpFunc(OnKeyUp);
	glutShowWindow();
	glutMainLoop();


	return 0;
}