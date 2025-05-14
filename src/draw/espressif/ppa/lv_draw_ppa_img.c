/**
 * @file lv_draw_ppa_img.c
 *
 */

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA
#include <math.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_ppa_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_draw_ppa_unit_t u = CONTAINER_OF(t, lv_draw_ppa_unit_t, task_act);
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    int width = lv_area_get_width(coords);
    int height = lv_area_get_height(coords);

    ppa_blend_oper_config_t cfg = {
        .src = {
            .buffer = (void *)dsc->src,
            .pitch_bytes = width * sizeof(lv_color_t),
            .rect = {
                .x = 0,
                .y = 0,
                .width = width,
                .height = height,
            },
            .color_format = lv_color_format_to_ppa(dsc->base.layer->color_format),
        },
        .fg_alpha = dsc->opa,
        .dst = {
            .buffer = draw_buf->data,
            .pitch_bytes = draw_buf->header.stride * sizeof(lv_color_t),
            .rect = {
                .x = coords->x1,
                .y = coords->y1,
                .width = width,
                .height = height,
            },
            .color_format = img_fmt,
        },
        .blend_mode = PPA_BLEND_MODE_NORMAL,
        .clipping_en = true,
        .clip_window = {
            .x = t->clip_area.x1,
            .y = t->clip_area.y1,
            .width = lv_area_get_width(&t->clip_area),
            .height = lv_area_get_height(&t->clip_area),
        },
    };

    return ppa_do_blend(u->blend_client, &cfg, PPA_TRANS_MODE_BLOCKING, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_PPA*/
