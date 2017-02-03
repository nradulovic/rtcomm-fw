/* ************************************************************************//**
 *
 * web site:  
 * e-mail  :    predrag_nedeljkovic@sentronis.rs
 *//***********************************************************************//**
 * @file      app_gui.h
 * @date      12.05.2016
 * @author    Predrag Nedeljkovic 
 * @brief     Header for app_gui. It contains graphical object definitions and
							function prototypes.
 *********************************************************************//** @{ */
#ifndef APP_GUI_H_
#define APP_GUI_H_

/*=========================================================  INCLUDE FILES  ==*/
#include <string.h>
#include <stdint.h>

/*============================================================  DATA TYPES  ==*/



struct text_object{
	uint8_t font;
	uint8_t foreground_c;
	uint8_t background_c;	
	uint16_t xx1_coord;
	uint16_t yy1_coord;
  char* text;	
};

struct box_object{
	uint8_t t_n1;
	uint8_t t_n2;
	uint8_t c1;
	uint8_t c2;
	uint8_t c3;
	uint16_t xx1_coord;
	uint16_t yy1_coord;
	uint16_t xx2_coord;
	uint16_t yy2_coord;
};

struct area_object
{
  uint16_t xx1_coord;
	uint16_t yy1_coord;
	uint16_t xx2_coord;
	uint16_t yy2_coord;
	uint8_t color;
};

struct display_object
{
	uint8_t fg_color;
	uint8_t bg_color;	
};

/*==================================== external objects ========================================*/



/*=====================================  GLOBAL FUNCTION PROTOTYPES  ===========================*/

/************************** text object handling routines ************************************/
uint16_t Set_font(const struct text_object *font, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t FontProperties(const struct text_object *text, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t SendText(const struct text_object *text, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t StringWrite(const struct text_object *txt, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t send_character(const struct text_object *txt, uint8_t *i2c_buff, uint16_t *ibuff_size);

/****************************** box object handling routines ************************************/
uint16_t BorderBox (struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t BorderType(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t BorderColor(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t BorderBox_TypeAndColor(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t Print_box(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size);

/************************************** other routines ******************************************/
uint16_t BackgroundColor(const struct display_object *display, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t CallMakro(uint8_t makro_no, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t ClearDispTFT(uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t TerminalOff(uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t FillAreaColor(const struct area_object *area, uint8_t *i2c_buff, uint16_t *ibuff_size);
uint16_t FillDisplay(const struct display_object *rear, uint8_t *i2c_buff, uint16_t* ibuff_size);
uint16_t FontColor(const struct text_object *text, uint8_t *i2c_buff, uint16_t *ibuff_size);


void insert_character_to_string(char *input_string, char character, uint8_t position);


/*******************************************************************************
 * END of   .h
 *
********************************************************************************/
#endif /* DD_DD_H_ */
