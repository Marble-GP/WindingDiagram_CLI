/*****************************************************************//**
 * @file   wd_config_decode.hpp
 * @brief  This includes json interpretation and path decomposition functions.
 * 
 * @author S.Watanabe
 * @date   June 2023
 * 
 * @license MIT (only these codes. You must comply with the license set for each external library you reference.)
 * @version 0.1
 *********************************************************************/


#ifndef WD_CONFIG_DECODE_H
#define WD_CONFIG_DECODE_H

#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <regex>

using namespace std;
using namespace nlohmann;

extern const json COLOR_TABLE;
extern const json GLUT_FONT_TABLE;


/**
 * @brief  Converts color name or color code to 8bit RGB vector.
 * @param[in] color_name  Set color name or 8bit*3 hex RGB color code. The list of convertible color names is Python-matplotlib compliant.
 * @param[out] dst  returns 8bit RGB color vector. (output value range: 0~255)
 * 
 */
void color_name2RGB(string color_name, vector<int>& dst);

/**
 * @brief  Converts color name or color code to 8bit RGB vector.
 * @param[in] color_name  Set color name or 8bit hex RGB color code. The list of convertible color names is Python-matplotlib compliant.
 * @param[out] dst  returns color vector. (output value range: 0~1)
 *
 */
void color_name2RGB(string color_name, vector<double>& dst);

/**
 * @brief  Get object items from json dictionary.
 * @param[in] src  source json dictionary.
 * @param[out] dst  destination json dictionary.
 *
 */
void get_wirering_command(json& src, json& dst);


/**
 * @brief  Split path strings by directory and file name.
 * @param[in] src_path  source path string to split.
 * @param[out] dst_dir  The directory portion is assigned to this.
 * @param[out] dst_file  The file name portion is assigned to this.
 * @param[in] delimiter  Set path delimiter character. Default is backslash(\).
 * @return split point of source string.
 *
 */
int path_split(std::string src_path, std::string& dst_dir, std::string& dst_file, char delimiter='\\');

/**
 * @brief  Split path or file name strings by file base name and file extension.
 * @param[in] src_path  source path or file name string to split.
 * @param[out] dst_basename  The base name portion is assigned to this.
 * @param[out] dst_ext  The file extension portion is assigned to this.
 * @param[in] delimite  Set path delimiter character. Default is backslash(\).
 * @return split point of source string.
 *
 */
int path_splitext(std::string src_path, std::string& dst_basename, std::string& dst_ext, char delimiter='\\');



/**
* @brief dummy overload.
*/
template<typename T> void jsonlist2vector(json& ndlist, T& dst)
{
	if (ndlist.is_number())
	{
		dst = ndlist.get<T>();
	}
}

/**
 * @brief  convert data type from json array to vector. nD-array representation by vectors is possible.
 * @param[in] ndlist  Set json array
 * @param[out] dst  Set vector. The number of vector nest must match the order of the json array. (ex: "[[1,2][3,4]]"_json -> vector<vector<int>>)
 *
 */
template<typename T> void jsonlist2vector(json& ndlist, vector<T>& dst)
{
	dst.clear();
	dst = vector<T>(ndlist.size());
	for (int i = 0; i < ndlist.size(); ++i)
	{
		if (ndlist.at(i).is_array())
		{
			jsonlist2vector(static_cast<json&>(ndlist.at(i)), static_cast<T&>(dst[i]));
		}
		else
		{
			dst[i] = ndlist.at(i).get<T>();
		}
	}
}

#endif 