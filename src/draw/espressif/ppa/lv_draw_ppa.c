/**
 * @file lv_draw_ppa.c
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

#define DRAW_UNIT_ID_PPA            80
#define DRAW_UNIT_PPA_PREF_SCORE    70

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t ppa_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t ppa_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t ppa_delete(lv_draw_unit_t * draw_unit);
static void ppa_execute_drawing(lv_draw_ppa_unit_t * u);

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

void lv_draw_ppa_init(void)
{
    esp_err_t res;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };

    lv_draw_buf_ppa_init_handlers();
    lv_draw_ppa_unit_t * draw_ppa_unit = lv_draw_create_unit(sizeof(lv_draw_ppa_unit_t));
    draw_ppa_unit->base_unit.evaluate_cb = ppa_evaluate;
    draw_ppa_unit->base_unit.dispatch_cb = ppa_dispatch;
    draw_ppa_unit->base_unit.delete_cb = ppa_delete;
    draw_ppa_unit->base_unit.name = "ESP_PPA";

    res = ppa_register_client(&ppa_client_config, &draw_ppa_unit->srm_client);
    LV_ASSERT(res == ESP_ERR_OK);
    ppa_client_config.oper_type = PPA_OPERATION_FILL;
    ppa_register_client(&ppa_client_config, &draw_ppa_unit->fill_client);
    LV_ASSERT(res == ESP_ERR_OK);
    ppa_client_config.oper_type = PPA_OPERATION_BLEND;
    ppa_register_client(&ppa_client_config, &draw_ppa_unit->blend_client);
    LV_ASSERT(res == ESP_ERR_OK);

}

void lv_draw_ppa_deinit(void)
{
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static int32_t _ppa_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);

    const lv_draw_dsc_base_t * draw_dsc_base = (lv_draw_dsc_base_t *) t->draw_dsc;

    if(!ppa_dest_cf_supported(draw_dsc_base->layer->color_format))
        return 0;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * draw_dsc = (lv_draw_fill_dsc_t *) t->draw_dsc;

                if((draw_dsc->radius != 0) || (draw_dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))
                    return 0;

                if(t->preference_score > DRAW_UNIT_PPA_PREF_SCORE) {
                    t->preference_score = DRAW_UNIT_PPA_PREF_SCORE;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PPA;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                const lv_image_dsc_t * img_dsc = draw_dsc->src;

                if(img_dsc->header.cf >= LV_COLOR_FORMAT_PROPRIETARY_START)
                    return 0;

                if(draw_dsc->tile)
                    return 0;

                if(!ppa_src_cf_supported(img_dsc->header.cf))
                    return 0;

                if(t->preference_score > DRAW_UNIT_PPA_PREF_SCORE) {
                    t->preference_score = DRAW_UNIT_PPA_PREF_SCORE;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PPA;
                }
                return 1;
            }
        default:
            return 0;
    }

    return 0;
}

static int32_t ppa_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_ppa_unit_t * draw_ppa_unit = (lv_draw_ppa_unit_t *) draw_unit;

    if(draw_ppa_unit->task_act)
        return 0;

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_PPA);

    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_PPA)
        return LV_DRAW_UNIT_IDLE;

    if(lv_draw_layer_alloc_buf(layer) == NULL)
        return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_ppa_unit->task_act = t;

    ppa_execute_drawing(draw_ppa_unit);

    draw_ppa_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_ppa_unit->task_act = NULL;

    lv_draw_dispatch_request();

    return 1;
}

static int32_t ppa_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *) draw_unit;

    ppa_unregister_client(u->srm_client);
    ppa_unregister_client(u->fill_client);
    return 0;
}

static void ppa_execute_drawing(lv_draw_ppa_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_draw_unit_t * draw_unit = (lv_draw_unit_t *)u;
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &t->area, &t->clip_area))
        return;

    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_draw_buf_invalidate_cache(draw_buf, &draw_area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_ppa_fill(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_ppa_img(t, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}
#endif /*LV_USE_PPA*/
