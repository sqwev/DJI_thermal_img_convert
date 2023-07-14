/*
 * Custom library sample for DJI Thermal SDK.
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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <numeric>
#include <algorithm>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

#include "dirp_wrapper.h"

using namespace std;

#define CIRP_DEBUG_ENABLE   (0)

#define DIRP_ADJ_PTR_INPUT(ptr)             do{ \
                                                if (nullptr == ptr) \
                                                { \
                                                    return DIRP_ERROR_POINTER_NULL; \
                                                } \
                                            } while(0)

#if (CIRP_DEBUG_ENABLE)
// TODO: Create a list to manage multiple instances
static DIRPV_HANDLE __dirpv_handle = nullptr;
#endif

int32_t create_from_rjpeg(const uint8_t *data, int32_t size, DIRPV_HANDLE *ph)
{
    DIRP_ADJ_PTR_INPUT(data);
    DIRP_ADJ_PTR_INPUT(ph);

    //TODO: create handle

#if (CIRP_DEBUG_ENABLE)
    if (nullptr != __dirpv_handle)
    {
        *ph = __dirpv_handle;
        return DIRP_SUCCESS;
    }

    /* TODO: Parse R-JPEG APPx from EXIF metadata */
    /* TODO: Create a new DIRPV context instance */
    DIRPV_HANDLE __handle = malloc(sizeof(DIRPV_HANDLE));
    if (nullptr == __handle)
    {
        return DIRP_ERROR_MALLOC;
    }

    /* Output DIRPV handle */
    *ph = __handle;

    return DIRP_SUCCESS;
#else
    return DIRP_ERROR_UNSUPPORTED_FUNC;
#endif
}

int32_t destroy(DIRPV_HANDLE h)
{
    DIRP_ADJ_PTR_INPUT(h);

    //TODO: destroy handle

#if (CIRP_DEBUG_ENABLE)
    free(h);

    return DIRP_SUCCESS;
#else
    return DIRP_ERROR_UNSUPPORTED_FUNC;
#endif
}

int32_t get_api_version(dirp_api_version_t *version)
{
    DIRP_ADJ_PTR_INPUT(version);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_rjpeg_version(DIRPV_HANDLE h, dirp_rjpeg_version_t *version)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(version);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_rjpeg_resolution(DIRPV_HANDLE h, dirp_resolution_t *resolution)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(resolution);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_original_raw(DIRPV_HANDLE h, uint16_t *raw_image, int32_t size)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(raw_image);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t process(DIRPV_HANDLE h, uint8_t *color_image, const int32_t size)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(color_image);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t process_strech(DIRPV_HANDLE h, float *strech_image, int32_t size)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(strech_image);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t measure(DIRPV_HANDLE h, int16_t *temp_image, int32_t size)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(temp_image);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t measure_ex(DIRPV_HANDLE h, float *temp_image, int32_t size)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(temp_image);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t set_isotherm(DIRPV_HANDLE h, const dirp_isotherm_t *isotherm)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(isotherm);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_isotherm(DIRPV_HANDLE h, dirp_isotherm_t *isotherm)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(isotherm);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t set_color_bar(DIRPV_HANDLE h, const dirp_color_bar_t *color_bar)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(color_bar);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_color_bar(DIRPV_HANDLE h, dirp_color_bar_t *color_bar)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(color_bar);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_color_bar_adaptive_params(DIRPV_HANDLE h, dirp_color_bar_t *color_bar)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(color_bar);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t set_pseudo_color(DIRPV_HANDLE h, dirp_pseudo_color_e pseudo_color)
{
    DIRP_ADJ_PTR_INPUT(h);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_pseudo_color(DIRPV_HANDLE h, dirp_pseudo_color_e *pseudo_color)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(pseudo_color);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_pseudo_color_lut(dirp_isp_pseudo_color_lut_t *pseudo_color_lut)
{
    DIRP_ADJ_PTR_INPUT(pseudo_color_lut);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t set_enhancement_params(DIRPV_HANDLE h, const dirp_enhancement_params_t *enhancement_params)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(enhancement_params);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_enhancement_params(DIRPV_HANDLE h, dirp_enhancement_params_t *enhancement_params)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(enhancement_params);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t set_measurement_params(DIRPV_HANDLE h, const dirp_measurement_params_t *measurement_params)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(measurement_params);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

int32_t get_measurement_params(DIRPV_HANDLE h, dirp_measurement_params_t *measurement_params)
{
    DIRP_ADJ_PTR_INPUT(h);
    DIRP_ADJ_PTR_INPUT(measurement_params);

    //TODO: refer to the function definition in dirp_api.h

    return DIRP_ERROR_UNSUPPORTED_FUNC;
}

void set_verbose_level(dirp_verbose_level_e level)
{
    (void)level;

    //TODO: set log level
}

dllexport int32_t dirp_vendor_get_api(dirp_api_wrapper_t *api)
{
    DIRP_ADJ_PTR_INPUT(api);

    api->create_from_rjpeg              = create_from_rjpeg;
    api->destroy                        = destroy;
    api->get_rjpeg_version              = get_rjpeg_version;
    api->get_rjpeg_resolution           = get_rjpeg_resolution;
    api->get_original_raw               = get_original_raw;
    api->process                        = process;
    api->process_strech                 = process_strech;
    api->measure                        = measure;
    api->measure_ex                     = measure_ex;
    api->set_isotherm                   = set_isotherm;
    api->get_isotherm                   = get_isotherm;
    api->set_color_bar                  = set_color_bar;
    api->get_color_bar                  = get_color_bar;
    api->get_color_bar_adaptive_params  = get_color_bar_adaptive_params;
    api->set_pseudo_color               = set_pseudo_color;
    api->get_pseudo_color               = get_pseudo_color;
    api->set_enhancement_params         = set_enhancement_params;
    api->get_enhancement_params         = get_enhancement_params;
    api->set_measurement_params         = set_measurement_params;
    api->get_measurement_params         = get_measurement_params;
    api->get_pseudo_color_lut           = get_pseudo_color_lut;
    api->set_verbose_level              = set_verbose_level;

    return DIRP_SUCCESS;
}
