/*
 * Common sample for DJI Thermal SDK.
 *
 * @Copyright (c) 2020-2023 DJI. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <string.h>
#include <sys/stat.h>

#include "dirp_api.h"
#include "argagg.hpp"

#ifdef _WIN32
#include <io.h>
#else
#include <sys/io.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

#define APP_VERSION "V1.4"

#define FSTREAM_OPEN_CHECK(fs, name, go) \
            { \
                if(!fs.is_open()) \
                { \
                    cout << "ERROR: open " << name << " file failed!" << endl; \
                    ret = -1; \
                    goto go; \
                } \
            }

typedef enum
{
    dirp_action_type_extract = 0,
    dirp_action_type_measure,
    dirp_action_type_process,
    dirp_action_type_num,
} dirp_action_type_e;

typedef enum
{
    dirp_measure_format_int16 = 0,
    dirp_measure_format_float32,
    dirp_measure_format_num,
} dirp_measure_format_e;

static argagg::parser_results args;
static argagg::parser argparser {{
    {
        "help", {"-h", "--help"},
        "Print help and exit", 0,
    },
    {
        "version", {"-V", "--version"},
        "Print version and exit", 0,
    },
    {
        "verbose", {"-v", "--verbose"},
        "verbose level" "\r\n"
        "        " "0: none      | 1: debug     | 2: detail" "\r\n"
        "        " "(default=\"none\")", 1,
    },
    {
        "logger", {"--logger"},
        "logger file" "\r\n"
        "        " "0: none      | 1: logger_file_name.txt" "\r\n"
        "        " "(default=\"none\")", 1,
    },
    {
        "source", {"-s", "--source"},
        "source file path", 1,
    },
    {
        "action", {"-a", "--action"},
        "action name " "\r\n"
        "        " "(possible values=\"extract\", \"measure\", \"process\")", 1,
    },
    {
        "output", {"-o", "--output"},
        "output file path", 1,
    },
    {
        "palette", {"-p", "--palette"},
        "(action[process] usage) pseudo color type" "\r\n"
        "        " "0: white_hot | 1: fulgurite | 2: iron_red" "\r\n"
        "        " "3: hot_iron  | 4: medical   | 5: arctic" "\r\n"
        "        " "6: rainbow1  | 7: rainbow2  | 8: tint" "\r\n"
        "        " "9: black_hot" "\r\n"
        "        " "(default=\"iron_red\")", 1,
    },
    {
        "isotherm", {"--isotherm"},
        "(action[process] usage) isotherm setting" "\r\n"
        "        " "argument format : [en],[high],[low]" "\r\n"
        "        " "[en]   string : \"off\" or \"on\"" "\r\n"
        "        " "[high] float  : high threshold temperature value" "\r\n"
        "        " "[low]  float  : low threshold temperature value"  "\r\n"
        "        " "(default=\"off,30.0,25.0\")", 1,
    },
    {
        "colorbar", {"--colorbar"},
        "(action[process] usage) color bar setting" "\r\n"
        "        " "argument format : [en],[high],[low]" "\r\n"
        "        " "[en]   string : \"off\" or \"on\"" "\r\n"
        "        " "[high] float  : high threshold temperature value" "\r\n"
        "        " "[low]  float  : low threshold temperature value"  "\r\n"
        "        " "(default=\"off,30.0,25.0\")", 1,
    },
    {
        "strech", {"--strech"},
        "(action[process] usage) process stresh only switch" "\r\n"
        "        " "0: off       | 1: on" "\r\n"
        "        " "(default=\"off\")", 1,
    },
    {
        "brightness", {"--brightness"},
        "(action[process] usage) brightness level" "\r\n"
        "        " "argument rage : [0,100]" "\r\n"
        "        " "(default=\"50\")", 1,
    },
    {
        "measurefmt", {"--measurefmt"},
        "(action[measure] usage) output format for temperature measurement" "\r\n"
        "        " "0: int16     | 1: float32" "\r\n"
        "        " "(default=\"int16\")", 1,
    },
    {
        "distance", {"--distance"},
        "(action[measure] usage) distance to the target" "\r\n"
        "        " "argument rage : [1.0,25.0]" "\r\n"
        "        " "(default=\"5.0\")", 1,
    },
    {
        "humidity", {"--humidity"},
        "(action[measure] usage) relative himidity of the environment" "\r\n"
        "        " "argument rage : [20.0,100.0]" "\r\n"
        "        " "(default=\"70.0\")", 1,
    },
    {
        "emissivity", {"--emissivity"},
        "(action[measure] usage) emissivity of the target" "\r\n"
        "        " "argument rage : [0.10,1.00]" "\r\n"
        "        " "(default=\"1.0\")", 1,
    },
    {
        "reflection", {"--reflection"},
        "(action[measure] usage) reflection of the target" "\r\n"
        "        " "argument rage : [-40.0,500.0]" "\r\n"
        "        " "(default=\"23.0\")", 1,
    },
}};

static dirp_isotherm_t s_isotherm =  {false, 30.0f, 25.0f};
static dirp_color_bar_t s_color_bar = {false, 30.0f, 25.0f};

int argparse_init(int argc, char *argv[])
{
    ostringstream usage;
    usage
        << argv[0] << " " << APP_VERSION << "\n"
        << '\n'
        << "Usage: " << argv[0] << " [OPTIONS]... [FILES]...\n"
        << '\n';

    try {
        args = argparser.parse(argc, argv);
    } catch (const std::exception& e) {
        argagg::fmt_ostream fmt(cerr);
        fmt << usage.str() << argparser << '\n'
            << "Encountered exception while parsing arguments: " << e.what()
            << '\n';
        return -1;
    }

    return 0;
}

string argparse_get_source_path(void)
{
    if (args["source"])
    {
        return args["source"].as<string>();
    }

    return "input.raw";
}

string argparse_get_output_path(void)
{
    if (args["output"])
    {
        return args["output"].as<string>();
    }

    return "output.raw";
}

string argparse_get_logger_file(void)
{
    if (args["logger"])
    {
        return args["logger"].as<string>();
    }

    return string("none");
}

int32_t argparse_get_measurement_params(dirp_measurement_params_t *measurement_params, bool *modified)
{
    int32_t ret = DIRP_SUCCESS;
    *modified = false;

    if (args["distance"])
    {
        *modified = true;
        float original_distance = measurement_params->distance;
        measurement_params->distance = args["distance"].as<float>();
        cout << "Change distance from " << original_distance << " to " << measurement_params->distance << endl;
    }
    if (args["humidity"])
    {
        *modified = true;
        float original_humidity = measurement_params->humidity;
        measurement_params->humidity = args["humidity"].as<float>();
        cout << "Change humidity from " << original_humidity << " to " << measurement_params->humidity << endl;
    }
    if (args["emissivity"])
    {
        *modified = true;
        float original_emissivity = measurement_params->emissivity;
        measurement_params->emissivity = args["emissivity"].as<float>();
        cout << "Change emissivity from " << original_emissivity << " to " << measurement_params->emissivity << endl;
    }
    if (args["reflection"])
    {
        *modified = true;
        float original_reflection = measurement_params->reflection;
        measurement_params->reflection = args["reflection"].as<float>();
        cout << "Change reflection from " << original_reflection << " to " << measurement_params->reflection << endl;
    }

    return ret;
}

int32_t argparse_get_enhancement_params(dirp_enhancement_params_t *enhancement_params, bool *modified)
{
    int32_t ret = DIRP_SUCCESS;
    *modified = false;

    if (args["brightness"])
    {
        *modified = true;
        int32_t original_brightness = enhancement_params->brightness;
        enhancement_params->brightness = args["brightness"].as<int32_t>();
        cout << "Change brightness from " << original_brightness << " to " << enhancement_params->brightness << endl;
    }

    return ret;
}

int32_t argparse_get_isotherm_params(dirp_isotherm_t *isotherm)
{
    int32_t ret = DIRP_SUCCESS;

    string arg_str;

    if (args["isotherm"])
    {
        arg_str = args["isotherm"].as<string>();
    }
    else
    {
        arg_str = "off,30.0,25.0";
    }

    vector<string> arg_vec;

    stringstream ss(arg_str);
    string str;

    while (getline(ss, str, ','))
    {
        arg_vec.push_back(str);
        // cout << "isotherm params : " << str << endl;
    }

    if (3 != arg_vec.size())
    {
        cout << "ERROR: isotherm arguments number is not [3]" << endl;
        return -1;
    }

    string str_enable   = arg_vec.at(0);
    string str_high     = arg_vec.at(1);
    string str_low      = arg_vec.at(2);

    try
    {
        isotherm->high = stof(str_high);
        isotherm->low  = stof(str_low);
    }
    catch(exception const & e)
    {
        (void)e;
        cout << "ERROR: isotherm threshold format is not float" << endl;
        return -1;
    }
    isotherm->enable = ("on" == str_enable)? true: false;

    return ret;
}

int32_t argparse_get_color_bar_params(dirp_color_bar_t *color_bar)
{
    int32_t ret = DIRP_SUCCESS;

    string arg_str;

    if (args["colorbar"])
    {
        arg_str = args["colorbar"].as<string>();
    }
    else
    {
        arg_str = "off,30.0,25.0";
    }

    vector<string> arg_vec;

    stringstream ss(arg_str);
    string str;

    while (getline(ss, str, ','))
    {
        arg_vec.push_back(str);
        // cout << "color bar params : " << str << endl;
    }

    if (3 != arg_vec.size())
    {
        cout << "ERROR: color bar arguments number is not [3]" << endl;
        return -1;
    }

    string str_enable   = arg_vec.at(0);
    string str_high     = arg_vec.at(1);
    string str_low      = arg_vec.at(2);

    try
    {
        color_bar->high = stof(str_high);
        color_bar->low  = stof(str_low);
    }
    catch(exception const & e)
    {
        (void)e;
        cout << "ERROR: color bar threshold format is not float" << endl;
        return -1;
    }
    color_bar->manual_enable = ("on" == str_enable)? true: false;

    return ret;
}

dirp_verbose_level_e argparse_get_verbose_level(void)
{
    string verbose_name;

    if (args["verbose"])
    {
        verbose_name = args["verbose"].as<string>();
    }
    else
    {
        verbose_name = "none";
    }

    if      ("none" == verbose_name)    return DIRP_VERBOSE_LEVEL_NONE;
    else if ("debug" == verbose_name)   return DIRP_VERBOSE_LEVEL_DEBUG;
    else if ("detail" == verbose_name)  return DIRP_VERBOSE_LEVEL_DETAIL;
    else                                return DIRP_VERBOSE_LEVEL_NONE;
}

dirp_action_type_e argparse_get_action_type(void)
{
    string action_name;

    if (args["action"])
    {
        action_name = args["action"].as<string>();
    }
    else
    {
        action_name = "process";
    }

    if      ("extract" == action_name)  return dirp_action_type_extract;
    else if ("measure" == action_name)  return dirp_action_type_measure;
    else if ("process" == action_name)  return dirp_action_type_process;
    else                                return dirp_action_type_process;
}

dirp_measure_format_e argparse_get_measure_format(void)
{
    string measure_format;

    if (args["measurefmt"])
    {
        measure_format = args["measurefmt"].as<string>();
    }
    else
    {
        measure_format = "int16";
    }

    if      ("int16" == measure_format)     return dirp_measure_format_int16;
    else if ("float32" == measure_format)   return dirp_measure_format_float32;
    else                                    return dirp_measure_format_int16;
}

dirp_pseudo_color_e argparse_get_pseudo_color(dirp_pseudo_color_e pseudo_color_old, bool *modified)
{
    dirp_pseudo_color_e pseudo_color_new = pseudo_color_old;

    *modified = false;
    if (args["palette"])
    {
        string pseudo_color_name = args["palette"].as<string>();
        *modified = true;
        if      ("white_hot" == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_WHITEHOT;
        else if ("fulgurite" == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_FULGURITE;
        else if ("iron_red"  == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_IRONRED;
        else if ("hot_iron"  == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_HOTIRON;
        else if ("medical"   == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_MEDICAL;
        else if ("arctic"    == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_ARCTIC;
        else if ("rainbow1"  == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_RAINBOW1;
        else if ("rainbow2"  == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_RAINBOW2;
        else if ("tint"      == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_TINT;
        else if ("black_hot" == pseudo_color_name)  pseudo_color_new = DIRP_PSEUDO_COLOR_BLACKHOT;
        else                                        pseudo_color_new = DIRP_PSEUDO_COLOR_IRONRED;
        cout << "Change pseudo color from " << pseudo_color_old << " to " << pseudo_color_new << endl;
    }

    return pseudo_color_new;
}

bool argparse_is_strech_only(void)
{
    string strech_only;

    if (args["strech"])
    {
        strech_only = args["strech"].as<string>();
    }
    else
    {
        strech_only = "off";
    }

    if      ("on" == strech_only)   return true;
    else                            return false;
}

int32_t prv_rjpeg_info_print(DIRP_HANDLE dirp_handle)
{
    int32_t ret = DIRP_SUCCESS;
    dirp_rjpeg_version_t rjpeg_version = {0};
    dirp_resolution_t rjpeg_resolution = {0};

    ret = dirp_get_rjpeg_version(dirp_handle, &rjpeg_version);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_rjpeg_version failed" << endl;
        goto ERR_RJPEG_INFO_PRINT_RET;
    }
    cout << "R-JPEG version information" << endl;
    cout << "    R-JPEG version : 0x" << hex << rjpeg_version.rjpeg  << dec << endl;
    cout << "    header version : 0x" << hex << rjpeg_version.header << dec << endl;
    cout << " curve LUT version : 0x" << hex << rjpeg_version.curve  << dec << endl;

    ret = dirp_get_rjpeg_resolution(dirp_handle, &rjpeg_resolution);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_rjpeg_version failed" << endl;
        goto ERR_RJPEG_INFO_PRINT_RET;
    }
    cout << "R-JPEG resolution size" << endl;
    cout << "      image  width : " << rjpeg_resolution.width  << endl;
    cout << "      image height : " << rjpeg_resolution.height << endl;

ERR_RJPEG_INFO_PRINT_RET:
    return ret;
}

int32_t prv_isp_config(DIRP_HANDLE dirp_handle)
{
    int32_t ret = DIRP_SUCCESS;
    bool modified = false;
    dirp_enhancement_params_t enhancement_params = {0};
    dirp_pseudo_color_e pseudo_color_old = DIRP_PSEUDO_COLOR_IRONRED;
    dirp_pseudo_color_e pseudo_color_new = DIRP_PSEUDO_COLOR_IRONRED;

    /* Set pseudo color type */
    ret = dirp_get_pseudo_color(dirp_handle, &pseudo_color_old);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_pseudo_color failed" << endl;
        goto ERR_ISP_CONFIG_RET;
    }

    /* Refresh custom pseudo color type */
    modified = false;
    pseudo_color_new = argparse_get_pseudo_color(pseudo_color_old, &modified);
    if (modified)
    {
        ret = dirp_set_pseudo_color(dirp_handle, pseudo_color_new);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call dirp_set_pseudo_color failed" << endl;
            goto ERR_ISP_CONFIG_RET;
        }
    }

    /* Set isotherm parameters */
    ret = argparse_get_isotherm_params(&s_isotherm);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call argparse_get_isotherm_params failed" << endl;
        s_isotherm.enable = false;
    }
    if (s_isotherm.enable)
    {
        ret = dirp_set_isotherm(dirp_handle, &s_isotherm);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call dirp_set_isotherm failed" << endl;
            goto ERR_ISP_CONFIG_RET;
        }
    }

    /* Set color bar parameters */
    ret = argparse_get_color_bar_params(&s_color_bar);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call argparse_get_color_bar_params failed" << endl;
        s_color_bar.manual_enable = false;
    }
    if (s_color_bar.manual_enable)
    {
        ret = dirp_set_color_bar(dirp_handle, &s_color_bar);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call dirp_set_color_bar failed" << endl;
            goto ERR_ISP_CONFIG_RET;
        }
    }

    /* Get original enhancement parameters */
    ret = dirp_get_enhancement_params(dirp_handle, &enhancement_params);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_enhancement_params failed" << endl;
        goto ERR_ISP_CONFIG_RET;
    }

    /* Refresh custom enhancement parameters */
    modified = false;
    ret = argparse_get_enhancement_params(&enhancement_params, &modified);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call argparse_get_enhancement_params failed" << endl;
        goto ERR_ISP_CONFIG_RET;
    }
    if (modified)
    {
        /* Set custom enhancement parameters */
        ret = dirp_set_enhancement_params(dirp_handle, &enhancement_params);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call dirp_set_enhancement_params failed" << endl;
            goto ERR_ISP_CONFIG_RET;
        }
    }

    /* TODO: Set another ISP processing parameters */

ERR_ISP_CONFIG_RET:
    return ret;
}

int32_t prv_measurement_config(DIRP_HANDLE dirp_handle)
{
    int32_t ret = DIRP_SUCCESS;
    bool modified = false;
    dirp_measurement_params_t measurement_params = {0};
    dirp_measurement_params_range_t params_range = {0};

    /* Get original measurement parameters */
    ret = dirp_get_measurement_params(dirp_handle, &measurement_params);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_measurement_params failed" << endl;
        goto ERR_MEASUREMENT_CONFIG_RET;
    }

    /* Refresh custom measurement parameters */
    ret = argparse_get_measurement_params(&measurement_params, &modified);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call argparse_get_measurement_params failed" << endl;
        goto ERR_MEASUREMENT_CONFIG_RET;
    }

    ret = dirp_get_measurement_params_range(dirp_handle, &params_range);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_measurement_params_range failed" << endl;
        goto ERR_MEASUREMENT_CONFIG_RET;
    }
    cout << "Measurement: get params range:" << endl;
    cout << "distance: [" << params_range.distance.min << "," << params_range.distance.max <<"]"<< endl;
    cout << "humidity: [" << params_range.humidity.min << "," << params_range.humidity.max <<"]"<< endl;
    cout << "emissivity: [" << params_range.emissivity.min << "," << params_range.emissivity.max <<"]"<< endl;
    cout << "reflection: [" << params_range.reflection.min << "," << params_range.reflection.max <<"]"<< endl;

    if (modified)
    {
        /* Set custom measurement parameters */
        ret = dirp_set_measurement_params(dirp_handle, &measurement_params);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call dirp_set_measurement_params failed" << endl;
            goto ERR_MEASUREMENT_CONFIG_RET;
        }
    }

ERR_MEASUREMENT_CONFIG_RET:
    return ret;
}

int32_t prv_get_rjpeg_output_size(const dirp_action_type_e action_type, const dirp_resolution_t *resolution)
{
    int32_t image_width     = resolution->width;
    int32_t image_height    = resolution->height;
    int32_t image_size      = 0;

    dirp_measure_format_e   measure_format  = argparse_get_measure_format();
    bool                    strech_only     = argparse_is_strech_only();

    switch (action_type)
    {
        case dirp_action_type_extract:
            image_size = image_width * image_height * sizeof(uint16_t);
            break;
        case dirp_action_type_measure:
            if (dirp_measure_format_float32 == measure_format)
            {
                image_size = image_width * image_height * sizeof(float);
            }
            else
            {
                image_size = image_width * image_height * sizeof(int16_t);
            }
            break;
        case dirp_action_type_process:
            if (strech_only)
            {
                image_size = image_width * image_height * sizeof(float);
            }
            else
            {
                image_size = image_width * image_height * 3 * sizeof(uint8_t);
            }
            break;
    }

    return image_size;
}

int32_t prv_action_run(DIRP_HANDLE dirp_handle)
{
    int32_t ret = DIRP_SUCCESS;
    int32_t out_size = 0;
    dirp_resolution_t rjpeg_resolution = {0};
    void *raw_out = nullptr;
    dirp_measure_format_e measure_format = argparse_get_measure_format();
    bool strech_only = argparse_is_strech_only();
    dirp_action_type_e action_type = argparse_get_action_type();
    string output_file_path = argparse_get_output_path();

    cout << "Run action " << (int)action_type << endl;

    ofstream ofstream;
    ofstream.open(output_file_path.c_str(), ios::binary);
    if (!ofstream.is_open())
    {
        cout << "ERROR: create ofstream failed" << endl;
        ret = -1;
        goto ERR_ACT_RET;
    }

    ret = dirp_get_rjpeg_resolution(dirp_handle, &rjpeg_resolution);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_rjpeg_version failed" << endl;
        goto ERR_ACT_RET;
    }

    out_size = prv_get_rjpeg_output_size(action_type, &rjpeg_resolution);
    if (0 == out_size)
    {
        cout << "ERROR: get zero raw size" << endl;
        ret = -1;
        goto ERR_ACT_RET;
    }

    raw_out = (void *)malloc(out_size);
    if (nullptr == raw_out)
    {
        cout << "ERROR: malloc memory failed" << endl;
        ret = -1;
        goto ERR_ACT_RET;
    }

    switch(action_type)
    {
        case dirp_action_type_extract:
            ret = dirp_get_original_raw(dirp_handle, (uint16_t *)raw_out, out_size);
            break;
        case dirp_action_type_measure:
            if (dirp_measure_format_float32 == measure_format)
            {
                ret = dirp_measure_ex(dirp_handle, (float *)raw_out, out_size);
            }
            else
            {
                ret = dirp_measure(dirp_handle, (int16_t *)raw_out, out_size);
            }
            break;
        case dirp_action_type_process:
            if (strech_only)
            {
                ret = dirp_process_strech(dirp_handle, (float *)raw_out, out_size);
            }
            else
            {
                ret = dirp_process(dirp_handle, (uint8_t *)raw_out, out_size);
            }
            break;
    }
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_[original_raw/measure/proess] failed" << endl;
        goto ERR_ACT_RET;
    }
    ofstream.write((const char *)raw_out, out_size);

    cout << "Save image file as : " << output_file_path.c_str() << endl;

    if ((dirp_action_type_process == action_type) && (false == s_color_bar.manual_enable))
    {
        dirp_color_bar_t color_bar_adaptive = {0};
        ret = dirp_get_color_bar_adaptive_params(dirp_handle, &color_bar_adaptive);
        if (DIRP_SUCCESS == ret)
        {
            cout << "Corlor bar adaptive range is [" << color_bar_adaptive.low << "," << color_bar_adaptive.high << "]" << endl;
        }
    }

ERR_ACT_RET:
    if (ofstream.is_open())
        ofstream.close();

    if (raw_out)
        free(raw_out);

    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    DIRP_HANDLE dirp_handle = nullptr;
    dirp_api_version_t api_version = {0};

    /* Parse CLI arguments */
    ret = argparse_init(argc, argv);
    if (ret)
    {
        cout << "ERROR: Command line arguement parse failed" << endl;
        return ret;
    }

    /* APP help */
    if (args["help"])
    {
        argagg::fmt_ostream fmt(cerr);
        fmt << argparser;
        return 0;
    }
    if (argc < 2)
    {
        argagg::fmt_ostream fmt(cerr);
        fmt << argparser;
        return 0;
    }

    if (args["version"])
    {
        cerr << "APP version : " << APP_VERSION << "\n";
        return 0;
    }

    /* Get source file information */
    string rjpeg_file_path = argparse_get_source_path();
#ifdef _WIN32
    ret = _access(rjpeg_file_path.c_str(), 0);
#else
    ret = access(rjpeg_file_path.c_str(), 0);
#endif
    if (0 != ret)
    {
        cout << "ERROR: source file " << rjpeg_file_path.c_str() << " not exist" << endl;
        return ret;
    }

    /* Adjust logger method */
    string logger_file = argparse_get_logger_file();
    if ("none" != logger_file)
    {
        dirp_set_logger_file(logger_file.c_str());
    }

    /* Adjust verbose level */
    dirp_verbose_level_e verbose_level = argparse_get_verbose_level();
    dirp_set_verbose_level(verbose_level);

    /* Get DIRP API version number */
    ret = dirp_get_api_version(&api_version);
    {
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: get dirp api verion failed" << endl;
            return -1;
        }
    }
    cout << "DIRP API version number : 0x"  << hex << api_version.api << dec << endl;
    cout << "DIRP API magic version  : "    << api_version.magic << endl;

    /* Get action type */
    dirp_action_type_e action_type = argparse_get_action_type();

    ifstream fs_i_rjpeg;
    cout << "R-JPEG file path : " << rjpeg_file_path.c_str() << endl;

    /* Load R-JPEG data to buffer */
#ifdef _WIN32
    struct _stat rjpeg_file_info;
    _stat(rjpeg_file_path.c_str(), &rjpeg_file_info);
#else
    struct stat rjpeg_file_info;
    stat(rjpeg_file_path.c_str(), &rjpeg_file_info);
#endif
    int32_t  rjpeg_size = (uint32_t)rjpeg_file_info.st_size;
    uint8_t *rjpeg_data = (uint8_t *)malloc(rjpeg_size);
    if (nullptr == rjpeg_data)
    {
        cout << "ERROR: malloc failed" << endl;
        goto ERR_DIRP_RET;
    }

    fs_i_rjpeg.open(rjpeg_file_path.c_str(), ios::binary);
    FSTREAM_OPEN_CHECK(fs_i_rjpeg , "rjpeg.jpg", ERR_FILE_OPEN);
    fs_i_rjpeg.read((char *)rjpeg_data, rjpeg_size);

    /* Create a new DIRP handle */
    ret = dirp_create_from_rjpeg(rjpeg_data, rjpeg_size, &dirp_handle);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: create R-JPEG dirp handle failed" << endl;
        goto ERR_DIRP_RET;
    }

    /* Print R-JPEG information */
    ret = prv_rjpeg_info_print(dirp_handle);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call prv_rjpeg_info_print failed" << endl;
        goto ERR_DIRP_RET;
    }

    /* Configure ISP parameters */
    if (dirp_action_type_process == action_type)
    {
        ret = prv_isp_config(dirp_handle);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call prv_isp_config failed" << endl;
            goto ERR_DIRP_RET;
        }
    }

    /* Configure measurement parameters */
    if ((dirp_action_type_measure == action_type) || (dirp_action_type_process == action_type))
    {
        ret = prv_measurement_config(dirp_handle);
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: call prv_isp_config failed" << endl;
            goto ERR_DIRP_RET;
        }
    }

    /* Run actions */
    ret = prv_action_run(dirp_handle);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call prv_action_run failed" << endl;
        goto ERR_DIRP_RET;
    }

ERR_DIRP_RET:
    /* Destroy DIRP handle */
    if (dirp_handle)
    {
        int status = dirp_destroy(dirp_handle);
        if (DIRP_SUCCESS != status)
        {
            cout << "ERROR: destroy dirp handle failed" << endl;
        }
    }

    cout << "Test done with return code " << ret << endl;

ERR_FILE_OPEN:
    if (fs_i_rjpeg.is_open())
        fs_i_rjpeg.close();

    if (rjpeg_data)
        free(rjpeg_data);

    //system("pause");
    return ret;
}
