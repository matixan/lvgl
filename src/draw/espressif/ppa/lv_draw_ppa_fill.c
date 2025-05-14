/**
 * @file lv_draw_ppa_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

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

void lv_draw_ppa_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_draw_ppa_unit_t u = CONTAINER_OF(t, lv_draw_ppa_unit_t, task_act);
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    int width = lv_area_get_width(coords);
    int height = lv_area_get_height(coords);

    if (width <= 0 || height <= 0) {
        LV_LOG_WARN("Invalid draw area for filling!");  
        return;
    } 

    ppa_fill_oper_config_t cfg = {
        .color = dsc->color,
        .rect = {
            .x = coords->x1,
            .y = coords->y1,
            .width = width,
            .height = height
        },
        .dst = {
            .buffer = draw_buf->data,
            .pitch_bytes = draw_buf->header.stride * sizeof(lv_color_t),
            .color_format = lv_color_format_to_ppa(dsc->base.layer->color_format);
        },
        .fg_alpha = dsc->opa,
        .clipping_en = true,
        .clip_window = {
            .x = t->clip_area.x1,
            .y = t->clip_area.y1,
            .width = lv_area_get_width(&t->clip_area),
            .height = lv_area_get_height(&t->clip_area),
        },
    };

    esp_err_t ret = ppa_do_fill(u->fill_client, &cfg, PPA_TRANS_MODE_BLOCKING, NULL);
    if (ret != ESP_OK) {
        LV_LOG_ERR("PPA fill failed: %d", ret);
    }
}

#endif /*LV_USE_PPA*/
