/**
 * @file lv_ppa_utils.h
 *
 */

#ifndef LV_PPA_UTILS_H
#define LV_PPA_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_PPA
#include "../../../misc/lv_color.h"
#include "../../lv_draw_private.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_area_private.h"

/* The ppa driver depends heavily on the esp-idf headers*/
#include "sdkconfig.h"
#ifndef CONFIG_SOC_PPA_SUPPORTED
#error "This SoC does not support PPA"
#endif

#include <stddef.h>
#include "driver/ppa.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "hal/color_hal.h"
#include "esp_cache.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
 typedef struct lv_draw_ppa_unit_t {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    ppa_client_handle_t srm_client;
    ppa_client_handle_t fill_client;
    ppa_client_handle_t blend_client;
} lv_draw_ppa_unit_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool ppa_src_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline bool ppa_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline ppa_color_format_t lv_color_format_to_ppa(lv_color_format_t lv_fmt)
{
    switch (lv_fmt) {
        case LV_COLOR_FORMAT_RGB565:
            return PPA_COLOR_FORMAT_RGB565;

        case LV_COLOR_FORMAT_RGB888:
            return PPA_COLOR_FORMAT_RGB888;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return PPA_COLOR_FORMAT_ARGB8888;

        case LV_COLOR_FORMAT_L8:
            return PPA_COLOR_FORMAT_A8;

        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_I8:
            return PPA_COLOR_FORMAT_LUT;

        case LV_COLOR_FORMAT_A8:
            return PPA_COLOR_FORMAT_A8;

        default:
            return PPA_COLOR_FORMAT_RGB565;
    }
}

#define CONTAINER_OF(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

#endif /*LV_USE_PPA*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PPA_UTILS_H*/
