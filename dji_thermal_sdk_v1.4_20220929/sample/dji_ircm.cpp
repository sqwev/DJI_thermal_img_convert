/*
 * Color mapping sample for DJI Thermal SDK.
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
        "output", {"-o", "--output"},
        "output file path", 1,
    },
    {
        "ref", {"-r", "--ref"},
        "reference pseudo color R-JPEG file path", 1,
    },
    {
        "width", {"--width"},
        "streching image width" "\r\n"
        "        " "(range=[64,65536] default=\"640\")", 1,
    },
    {
        "height", {"--height"},
        "streching image height" "\r\n"
        "        " "(range=[64,65536] default=\"512\")", 1,
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
        "lutout", {"-l", "--lutout"},
        "palette pseudo color LUT file save path", 1,
    },
}};

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

string argparse_get_ref_path(void)
{
    if (args["ref"])
    {
        return args["ref"].as<string>();
    }

    return "REF_R.JPG";
}

string argparse_get_logger_file(void)
{
    if (args["logger"])
    {
        return args["logger"].as<string>();
    }

    return string("none");
}

void argparse_get_strech_image_size(int32_t *width, int32_t *height)
{
    if (args["width"])
    {
        *width = args["width"].as<int>();
    }
    else
    {
        *width = 640;
    }

    if (args["height"])
    {
        *height = args["height"].as<int>();
    }
    else
    {
        *height = 480;
    }
}

string argparse_get_lut_out_path(void)
{
    if (args["lutout"])
    {
        return args["lutout"].as<string>();
    }

    return string("");
}

dirp_pseudo_color_e argparse_get_pseudo_color(void)
{
    string pseudo_color_name;

    if (args["palette"])
    {
        pseudo_color_name = args["palette"].as<string>();
    }
    else
    {
        pseudo_color_name = "int16";
    }

    if      ("white_hot" == pseudo_color_name)  return DIRP_PSEUDO_COLOR_WHITEHOT;
    else if ("fulgurite" == pseudo_color_name)  return DIRP_PSEUDO_COLOR_FULGURITE;
    else if ("iron_red"  == pseudo_color_name)  return DIRP_PSEUDO_COLOR_IRONRED;
    else if ("hot_iron"  == pseudo_color_name)  return DIRP_PSEUDO_COLOR_HOTIRON;
    else if ("medical"   == pseudo_color_name)  return DIRP_PSEUDO_COLOR_MEDICAL;
    else if ("arctic"    == pseudo_color_name)  return DIRP_PSEUDO_COLOR_ARCTIC;
    else if ("rainbow1"  == pseudo_color_name)  return DIRP_PSEUDO_COLOR_RAINBOW1;
    else if ("rainbow2"  == pseudo_color_name)  return DIRP_PSEUDO_COLOR_RAINBOW2;
    else if ("tint"      == pseudo_color_name)  return DIRP_PSEUDO_COLOR_TINT;
    else if ("black_hot" == pseudo_color_name)  return DIRP_PSEUDO_COLOR_BLACKHOT;
    else                                        return DIRP_PSEUDO_COLOR_IRONRED;
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

int32_t prv_process_color_mapping(dirp_isp_pseudo_color_lut_t *pseudo_color_lut, float *src_data, int32_t src_size)
{
#if !defined(MIN)
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif

    int32_t ret = DIRP_SUCCESS;

    int32_t width = 0;
    int32_t height = 0;

    argparse_get_strech_image_size(&width, &height);
    cout << "Streching image size : " << width << " * " << height << endl;

    if ((width < 0) || (height < 0))
    {
        cout << "ERROR: width and height must larger than zero" << endl;
        return -1;
    }
    if (src_size < (width * height * (int32_t)sizeof(float)))
    {
        cout << "ERROR: source data size " <<src_size << " is smaller than resolution size " << width * height * sizeof(float) << endl;
        return -1;
    }

    dirp_pseudo_color_e pseudo_color = argparse_get_pseudo_color();
    cout << "Color mapping pseudo type : " << pseudo_color << endl;

    uint8_t  black_rgb[3] = {0,0,0};
    uint8_t *lut_r = &pseudo_color_lut->red  [pseudo_color][0];
    uint8_t *lut_g = &pseudo_color_lut->green[pseudo_color][0];
    uint8_t *lut_b = &pseudo_color_lut->blue [pseudo_color][0];

    string output_file_path = argparse_get_output_path();
    ofstream ofstream(output_file_path.c_str(), ios::binary);
    if (!ofstream.is_open())
    {
        cout << "ERROR: create ofstream failed" << endl;
        ret = -1;
        goto ERR_PCM_RET;
    }

    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            if ((src_data[i * width + j] >= 0.0f) && (src_data[i * width + j] <= 256.0f))
            {
                uint8_t index = (uint8_t)MIN(floor(src_data[i * width + j]), DIRP_PSEUDO_COLOR_LUT_DEPTH - 1);

                ofstream.write((const char *)&lut_r[index], sizeof(uint8_t));
                ofstream.write((const char *)&lut_g[index], sizeof(uint8_t));
                ofstream.write((const char *)&lut_b[index], sizeof(uint8_t));
            }
            else
            {
                ofstream.write((const char *)&black_rgb[0], 3 * sizeof(uint8_t));
            }
        }
    }

ERR_PCM_RET:
    if (ofstream.is_open())
        ofstream.close();

    return ret;
}

int32_t prv_save_pseudo_color_lut(dirp_isp_pseudo_color_lut_t *pseudo_color_lut)
{
    int32_t ret = DIRP_SUCCESS;

    string palette_out_path = argparse_get_lut_out_path();
    if ("" == palette_out_path)
    {
        return ret;
    }

    for (int32_t i=0; i<DIRP_PSEUDO_COLOR_NUM; i++)
    {
        string file_name = palette_out_path + to_string(i) + string(".raw");
        ofstream ofstream;
        ofstream.open(file_name.c_str(), ios::binary);
        if (!ofstream.is_open())
        {
            cout << "ERROR: create ofstream failed" << endl;
            goto ERR_PCL_RET;
        }

        for (int32_t j=DIRP_PSEUDO_COLOR_LUT_DEPTH-1; j>=0; j--)
        {
            uint8_t pixel_red   =  pseudo_color_lut->red  [i][j];
            uint8_t pixel_green =  pseudo_color_lut->green[i][j];
            uint8_t pixel_blue  =  pseudo_color_lut->blue [i][j];

            // Save pseudo color LUT as a 25 * 256 RGB image
            for (int32_t k=0; k<25; k++)
            {
                ofstream.write((const char *)&pixel_red,    sizeof(uint8_t));
                ofstream.write((const char *)&pixel_green,  sizeof(uint8_t));
                ofstream.write((const char *)&pixel_blue,   sizeof(uint8_t));
            }
        }

        ofstream.close();
        cout << "Save pseudo color LUT image as " << file_name << endl;
    }

ERR_PCL_RET:

    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    ifstream fs_i_rjpeg;
    int32_t  rjpeg_size;
    uint8_t *rjpeg_data = nullptr;
    DIRP_HANDLE dirp_handle = nullptr;
    dirp_api_version_t api_version = {0};
    dirp_isp_pseudo_color_lut_t pseudo_color_lut = {0};

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
    string source_file_path = argparse_get_source_path();
#ifdef _WIN32
    ret = _access(source_file_path.c_str(), 0);
#else
    ret = access(source_file_path.c_str(), 0);
#endif
    if (0 != ret)
    {
        cout << "ERROR: source file " << source_file_path.c_str() << " not exist" << endl;
        return ret;
    }

    /* Get reference R-JPEG file information */
    string rjpeg_file_path = argparse_get_ref_path();
#ifdef _WIN32
    ret = _access(rjpeg_file_path.c_str(), 0);
#else
    ret = access(rjpeg_file_path.c_str(), 0);
#endif
    if (0 != ret)
    {
        cout << "ERROR: reference R-JPEG file " << rjpeg_file_path.c_str() << " not exist" << endl;
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

    /* Load streching image data to buffer */
#ifdef _WIN32
    struct _stat source_file_info;
    _stat(source_file_path.c_str(), &source_file_info);
#else
    struct stat source_file_info;
    stat(source_file_path.c_str(), &source_file_info);
#endif
    int32_t source_file_size = (uint32_t)source_file_info.st_size;
    float *source_file_data = (float *)malloc(source_file_size);
    if (nullptr == source_file_data)
    {
        cout << "ERROR: malloc failed" << endl;
        return -1;
    }

    cout << "Streching file path : " << source_file_path.c_str() << endl;
    ifstream fs_i_src(source_file_path.c_str(), ios::binary);

    FSTREAM_OPEN_CHECK(fs_i_src , "ir.raw", ERR_FILE_OPEN);
    fs_i_src.read((char *)source_file_data, source_file_size);

    cout << "R-JPEG file path : " << rjpeg_file_path.c_str() << endl;

    /* Load R-JPEG data to buffer */
#ifdef _WIN32
    struct _stat rjpeg_file_info;
    _stat(rjpeg_file_path.c_str(), &rjpeg_file_info);
#else
    struct stat rjpeg_file_info;
    stat(rjpeg_file_path.c_str(), &rjpeg_file_info);
#endif
    rjpeg_size = (uint32_t)rjpeg_file_info.st_size;
    rjpeg_data = (uint8_t *)malloc(rjpeg_size);
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

    /* Get DIRP API version number */
    ret = dirp_get_api_version(&api_version);
    {
        if (DIRP_SUCCESS != ret)
        {
            cout << "ERROR: get dirp api verion failed" << endl;
            goto ERR_DIRP_RET;
        }
    }
    cout << "DIRP API version number : 0x"  << hex << api_version.api << dec << endl;
    cout << "DIRP API magic version  : "    << api_version.magic << endl;

    /* Get pseudo color LUT */
    ret = dirp_get_pseudo_color_lut(dirp_handle, &pseudo_color_lut);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call dirp_get_pseudo_color_lut failed" << endl;
        goto ERR_DIRP_RET;
    }

    /* Process color mapping from streching image */
    ret = prv_process_color_mapping(&pseudo_color_lut, source_file_data, source_file_size);
    if (DIRP_SUCCESS != ret)
    {
        cout << "ERROR: call prv_process_color_mapping failed" << endl;
        goto ERR_DIRP_RET;
    }

    /* Save pseudo color LUT */
    ret = prv_save_pseudo_color_lut(&pseudo_color_lut);
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

ERR_FILE_OPEN:
    if (fs_i_rjpeg.is_open())
        fs_i_rjpeg.close();

    if (rjpeg_data)
        free(rjpeg_data);

    if (fs_i_src.is_open())
        fs_i_src.close();

    if (source_file_data)
        free(source_file_data);

    //system("pause");
    return ret;
}
