/**
 * @file lv_spinbox.h
 *
 */


#ifndef LV_SPINBOX_H
#define LV_SPINBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_SPINBOX != 0

#include "../lv_core/lv_obj.h"
#include "../lv_objx/lv_ta.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SPINBOX_MAX_DIGIT_COUNT	16

/**********************
 *      TYPEDEFS
 **********************/
/*Data of spinbox*/
typedef struct {
    lv_ta_ext_t ta; /*Ext. of ancestor*/
    /*New data for this type */
    int32_t value;
    int32_t rangeMax;
    int32_t rangeMin;
    int32_t step;
    uint8_t digitCount:4;
    uint8_t decPointPos:4;      /*if 0, there is no separator and the number is an integer*/
    uint8_t digitPaddingLeft:4;
    uint8_t digitPaddingRight:4;
    uint8_t digits[1+1+LV_SPINBOX_MAX_DIGIT_COUNT];     /*1 sign, 1 point, 16 num digits*/

} lv_spinbox_ext_t;


/*Styles*/
enum {
    LV_SPINBOX_STYLE_BG,
    LV_SPINBOX_STYLE_SB,
    LV_SPINBOX_STYLE_CURSOR,
};
typedef uint8_t lv_spinbox_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spinbox objects
 * @param par pointer to an object, it will be the parent of the new spinbox
 * @param copy pointer to a spinbox object, if not NULL then the new object will be copied from it
 * @return pointer to the created spinbox
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Select next lower digit for edition by dividing the step by 10
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * spinbox);

/**
 * Select next higher digit for edition by multiplying the step by 10
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_previous(lv_obj_t * spinbox);

/**
 * Increment spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_increment(lv_obj_t * spinbox);

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * spinbox);


/*======================
 * Add/remove functions
 *=====================*/


/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a spinbox.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_spinbox_set_style(lv_obj_t * spinbox, lv_spinbox_style_t type, lv_style_t *style);

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void lv_spinbox_set_value(const lv_obj_t * spinbox, int32_t i);

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not shown
 */
void lv_spinbox_set_digit_format(const lv_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position);

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param i rangeMin maximum value, inclusive
 * @param i rangeMax minimum value, inclusive
 */
void lv_spinbox_set_range(const lv_obj_t * spinbox, int32_t rangeMin, int32_t rangeMax);


/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a spinbox.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_spinbox_get_style(const lv_obj_t * spinbox, lv_spinbox_style_t type);

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(const lv_obj_t * spinbox);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_SPINBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_SPINBOX_H*/
