/*
 * DJI Thermal SDK wrapper header
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

#pragma once

#ifndef _DIRP_WRAPPER_H_
#define _DIRP_WRAPPER_H_

#include "dirp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *DIRPV_HANDLE;

typedef struct
{
    int32_t (*create_from_rjpeg)                (const uint8_t *, int32_t, DIRPV_HANDLE *);
    int32_t (*destroy)                          (DIRPV_HANDLE);
    int32_t (*get_rjpeg_version)                (DIRPV_HANDLE, dirp_rjpeg_version_t *);
    int32_t (*get_rjpeg_resolution)             (DIRPV_HANDLE, dirp_resolution_t *);
    int32_t (*get_original_raw)                 (DIRPV_HANDLE, uint16_t *, int32_t);
    int32_t (*process)                          (DIRPV_HANDLE, uint8_t *, int32_t );
    int32_t (*process_strech)                   (DIRPV_HANDLE, float *, int32_t);
    int32_t (*measure)                          (DIRPV_HANDLE, int16_t *, int32_t);
    int32_t (*measure_ex)                       (DIRPV_HANDLE, float *, int32_t);
    int32_t (*set_isotherm)                     (DIRPV_HANDLE, const dirp_isotherm_t *);
    int32_t (*get_isotherm)                     (DIRPV_HANDLE, dirp_isotherm_t *);
    int32_t (*set_color_bar)                    (DIRPV_HANDLE, const dirp_color_bar_t *);
    int32_t (*get_color_bar)                    (DIRPV_HANDLE, dirp_color_bar_t *);
    int32_t (*get_color_bar_adaptive_params)    (DIRPV_HANDLE, dirp_color_bar_t *);
    int32_t (*set_pseudo_color)                 (DIRPV_HANDLE, dirp_pseudo_color_e);
    int32_t (*get_pseudo_color)                 (DIRPV_HANDLE, dirp_pseudo_color_e *);
    int32_t (*set_enhancement_params)           (DIRPV_HANDLE, const dirp_enhancement_params_t *);
    int32_t (*get_enhancement_params)           (DIRPV_HANDLE, dirp_enhancement_params_t *);
    int32_t (*set_measurement_params)           (DIRPV_HANDLE, const dirp_measurement_params_t *);
    int32_t (*get_measurement_params)           (DIRPV_HANDLE, dirp_measurement_params_t *);
    int32_t (*get_measurement_params_range)     (DIRPV_HANDLE, dirp_measurement_params_range_t*);
    int32_t (*get_pseudo_color_lut)             (dirp_isp_pseudo_color_lut_t *);
    void    (*set_verbose_level)                (dirp_verbose_level_e);
} dirp_api_wrapper_t;

typedef int32_t (*func_dirp_vendor_get_api)(dirp_api_wrapper_t *);

dllexport int32_t dirp_vendor_get_api(dirp_api_wrapper_t *api);

#ifdef __cplusplus
}
#endif

#endif /* _DIRP_WRAPPER_H_ */
