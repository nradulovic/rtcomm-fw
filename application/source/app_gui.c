/* ****************************************************************************
 *
 *
 *
 * web site:    
 * e-mail  :    
*//***********************************************************************//**
* @file
* @author      Predrag Nedeljkovic 
* @brief       
 *********************************************************************//** @{ */
/**
* @defgroup 
* @brief       

 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/
#include "app_gui.h"

/*=========================================================================================  LOCAL MACRO's  ==*/
#define DC1 			0x11 //command data to display
#define DC2 			0x12 //request for send buff content

/*======================================================================================  LOCAL DATA TYPES  ==*/
/*=============================================================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================================================  LOCAL VARIABLES  ==*/

uint8_t SendBuf[35]; //interni bafer za smestanje pakovanih podataka koji se salju prema tft-u
/*======================================================================================  GLOBAL VARIABLES  ==*/

/*============================================================================  LOCAL FUNCTION DEFINITIONS  ==*/

/***************************************************************************************************************
  * @name  BuildSmall(uint8_t* tft_ibuff, uint8_t tft_isize, uint8_t* i2c_obuff,uint8_t *i2c_osize)
  * @brief Kreira small packet prema small packet protokolu
  * @param tft_ibuff: Ulazni bafer
  * @param tft_isize: duzina ulaznog bafera (len - bez ceksume i dc1 karaktera)
  * @param i2c_obuff: izlazni bafer za slanje (formiran prema protokolu)
  * @param i2c_osize: broj podataka za slanje na i2c
  * @retval broj bajtova za slanje na I2C
  *************************************************************************************************************/

static  uint16_t BuildSmall(uint8_t* tft_ibuff, uint8_t tft_isize, uint8_t* i2c_obuff,uint16_t *i2c_osize){
 	                      /******** data ************** len *************** sbuff ******* sbuff size ***/
	uint8_t crc, i;
	uint16_t size;
	

  memset(i2c_obuff,0,tft_isize+3);

  /* kopiranje ulaznog bafera na i2c_obuff+2 mesto u izlaznom baferu */
  memcpy(i2c_obuff+2, tft_ibuff, tft_isize); //kopiram ulazni bafer u izlazni pocev od njegove i+2 pozicije  
  
  /** pridruzujem DC1, len zaglavlju **/	
  i2c_obuff[0]=DC1;
  i2c_obuff[1]=tft_isize;
	
	/* izracunavanje checksume preciznosti */
	
  *i2c_osize = tft_isize + 3;
  size = *i2c_osize;	
  crc = 0;	
  crc+=(DC1+tft_isize);   
	
  for(i=2;i<tft_isize+2; i++){	  
  crc+=i2c_obuff[i]; 	
  }
	
  /* nema veze s protokolom, crc se postavlja na 0x30 da bi 
	bilo i spisano na terminalu na odredjeno mesto u baferu	*/ 
	
  memcpy(i2c_obuff+2+tft_isize, &crc, 1); //kopiram crc u izlazni niz

	return size;
}


/*===========================================  GLOBAL FUNCTION DEFINITIONS  ==*/

/***********************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
  *********************************************************************************/

uint16_t Set_font(const struct text_object *font, uint8_t *i2c_buff, uint16_t *ibuff_size)
{ 
  uint16_t sbuff_size, size;
	uint8_t setbuff[4];
				
	SendBuf[0]=0x1b;
	SendBuf[1]= 'Z';
	SendBuf[2]= 'F';
	SendBuf[3]= font->font;
	sbuff_size=4;	
	
	size = BuildSmall(setbuff, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/**************************************************************************************************************
  * @name    FontColor()
  * @brief   Kreira bafer za slanje boje fonta
  * @param   fg: Foreground color
  * @param   bg: Background color
  * @retval  Bez povratnih vredosti
  ************************************************************************************************************/

uint16_t FontColor(const struct text_object *text, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size;
	
	SendBuf[0]=0x1B;
	SendBuf[1]='F';
	SendBuf[2]='Z';
	SendBuf[3]=text->foreground_c;
	SendBuf[4]=text->background_c;
	sbuff_size=5;
  
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/**************************************************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
  ************************************************************************************************************/
uint16_t FontProperties(const struct text_object *text, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	
	uint16_t sbuff_size, size;
		
	sbuff_size = 0;			
	SendBuf[0]= 0x1b;
	SendBuf[1]= 'Z';
	SendBuf[2]= 'F';
	SendBuf[3]= text->font;
	sbuff_size += 4;	
	
	SendBuf[0]=0x1B;
	SendBuf[1]='F';
	SendBuf[2]='Z';
	SendBuf[3]=text->foreground_c;
	SendBuf[4]=text->background_c;
	sbuff_size += 5;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	
	return size;
}



/****************************************************************************************************
 * @name
 * @brief  
 * @param  
 * @retval 
 ****************************************************************************************************/
uint16_t SendText(const struct text_object *text, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
uint16_t txt_len, i, sbuff_size;	
uint16_t size;

		/*********set font**********/	
      sbuff_size = 0;	
	    SendBuf[0] = 0x1B;	
			SendBuf[1] = 'Z';
			SendBuf[2] = 'F';
			SendBuf[3] = text->font;
	    sbuff_size += 4;

		/** font color **/
			SendBuf[4]=0x1B;
			SendBuf[5]='F';
			SendBuf[6]='Z';
			SendBuf[7]= text->foreground_c;
			SendBuf[8]= text->background_c;
	    sbuff_size+=5;
																			
			txt_len=strlen(text->text);	//from string.h
			
			SendBuf[9] = 0x1B;
			SendBuf[10] = 'Z';
			SendBuf[11] = 'L';
			SendBuf[12] = text->xx1_coord;
			SendBuf[13] = text->xx1_coord>>8;
			SendBuf[14] = text->yy1_coord;
			SendBuf[15] = text->yy1_coord>>8;
			sbuff_size +=7;

			for (i=16; i<= txt_len + sbuff_size; i++)
			{
				SendBuf[i] = text->text[i-sbuff_size];		
			}

			SendBuf[sbuff_size+txt_len+1]= '\0'; 
			sbuff_size += (txt_len+1);
			
			size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
			return size;										
}

/**************************************************************************************************************
  * @name    StringWrite ()
  * @brief   Kreira bafer za slanje teksta na i2c
  * @param   xx1, yy1: x, y koordinata displeja
  * @param   text - string koji treba da bude ispisan
  * @param   i2c_buff: bafer za slanje na i2c
  * @param   ibuff_size: obim bafera za slanje na i2c
  * @retval  bez povratnih vrednosti
  *************************************************************************************************************/

uint16_t StringWrite (const struct text_object *txt, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, txt_len, i, size;
	
	txt_len=strlen(txt->text);	//from string.h
	SendBuf[0] = 0x1B;
	SendBuf[1] = 'Z';
	SendBuf[2] = 'L';
	SendBuf[3] = txt->xx1_coord;
	SendBuf[4] = txt->xx1_coord>>8;
	SendBuf[5] = txt->yy1_coord;
	SendBuf[6] = txt->yy1_coord>>8;

	for (i=7; i<= txt_len+7; i++)
	{
		SendBuf[i] = txt->text[i-7];		
	}

	SendBuf[8+txt_len]= '\0'; 
	sbuff_size = 8 + txt_len;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);

	return size;
}

/**************************************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
  *************************************************************************************************/
uint16_t send_character(const struct text_object *txt, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size;	
  uint16_t size;
	
	sbuff_size = 0;

	
	SendBuf[0] = 0x1B;	
	SendBuf[1] = 'Z';
	SendBuf[2] = 'F';
	SendBuf[3] = txt->font;
	sbuff_size += 4;

	/** font color **/
	SendBuf[4]=0x1B;
	SendBuf[5]='F';
	SendBuf[6]='Z';
	SendBuf[7]= txt->foreground_c;
	SendBuf[8]= txt->background_c;
	
	sbuff_size+=5;  
	
	SendBuf[0] = 0x1B;
	SendBuf[1]='Z';
	SendBuf[2]='L';
	SendBuf[3]=txt->xx1_coord;
	SendBuf[4]=txt->xx1_coord>>8;
	SendBuf[5]=txt->yy1_coord;
	SendBuf[6]=txt->yy1_coord>>8;
	SendBuf[7]=txt->text[0];
	SendBuf[8]=0x00;
	
	sbuff_size += 9;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/**********************************************************************************************************************
  * @name		FillAreaColor()
	* @brief boji region pravougaonika izabranom bojom
	* @param xx1: prva x koordinata
	* @param xx2: druga x koordinata 
	* @param yy1: prva y koordinata
	* @param yy2: prva y koordinata
	* @param boja: boja kojom se region boji
	* @param i2c_obuff: izlazni bafer za slanje (formiran prema protokolu)
	* @param i2c_osize: broj podataka za slanje na i2c 
  * @retval 
  *********************************************************************************************************************/
uint16_t FillAreaColor(const struct area_object *area, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size;
	
	SendBuf[0] = 0x1B;
	SendBuf[1] = 'R';
	SendBuf[2] = 'F';
	SendBuf[3] = area->xx1_coord;
	SendBuf[4] = area->xx1_coord>>8;
	SendBuf[5] = area->yy1_coord;
	SendBuf[6] = area->yy1_coord>>8;
	SendBuf[7] = area->xx2_coord;
	SendBuf[8] = area->xx2_coord>>8;
	SendBuf[9] = area->yy2_coord;
	SendBuf[10] = area->yy2_coord>>8;
	SendBuf[11] = area->color;
	
	sbuff_size = 12;
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/******************************************************************************************************************
  * @name  			ClearDispTFT()
  * @brief      Svicuje displej iz terminal moda u graficki mod
  * @param      bez ul. parametara
  * @retval     bez povr. vredosti
  *****************************************************************************************************************/
uint16_t ClearDispTFT(uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size /*, SendBuff[3]*/;
	
	SendBuf[0]=0x1B;
	SendBuf[1]='D';       
	SendBuf[2]='L';     
  sbuff_size = 3;	  
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/******************************************************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
	*****************************************************************************************************************/
uint16_t TerminalOff(uint8_t *i2c_buff, uint16_t *ibuff_size)
{ 
	uint16_t sbuff_size, size; /*,SendBuff[3] */;
	SendBuf[0]=0x1B;
	SendBuf[1]='T';
	SendBuf[2]='A';
	sbuff_size = 3;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/*********************************************************************************************************
  * @name    BackgroundColor(uint8_t fg, uint8_t bg)
  * @brief   Set display background colour defined by fg, bg
  * @param   fg - foreground color
  * @param   fg - background color
  * @retval 
  ********************************************************************************************************/

uint16_t BackgroundColor(const struct display_object *display, uint8_t *i2c_buff, uint16_t *ibuff_size){
	
uint16_t sbuff_size, size;
	
	SendBuf[0] = 0x1B;
	SendBuf[1] = 'F';
	SendBuf[2] = 'D';
	SendBuf[3] = display->fg_color;
	SendBuf[4] = display->bg_color;
	sbuff_size = 5;
	
	size = BuildSmall(SendBuf, sbuff_size ,i2c_buff, ibuff_size);
	return size;
} 

/*******************************************************************************************
  * @name  FillDisp()
  * @brief Kreira frejm za slanje na i2c. Frejm sadrzi boju displeja
  * @param *i2c_buff
  * @retval 
  *****************************************************************************************/

uint16_t FillDisplay(const struct display_object *rear, uint8_t *i2c_buff, uint16_t* ibuff_size){

uint16_t sbuff_size, size;
	
	SendBuf[0] = 0x1B;
	SendBuf[1] = 'D';
	SendBuf[2] = 'F';
	SendBuf[3] = rear->bg_color;
	sbuff_size = 4;
		
	size = BuildSmall(SendBuf, sbuff_size ,i2c_buff, ibuff_size);
return size;	
}

/*********************************************************************************
  * @name    CallMakro()
  * @brief   Kreira bafer za poziv makroa
  * @param   makro_no: redni broj makroa
  * @retval  Bez povratnih vredosti
  ********************************************************************************/

uint16_t CallMakro(uint8_t makro_no, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size;
	
	SendBuf[0]=0x1B;
	SendBuf[1]='M';
	SendBuf[2]='N';
	SendBuf[3]= makro_no;
	sbuff_size=4;
  
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/***************************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
  *************************************************************************************/

uint16_t BorderColor(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size;
	
	SendBuf[0]=0x1B;
	SendBuf[1]='F';
	SendBuf[2]='R';
	SendBuf[3]=box->c1;
	SendBuf[4]=box->c2;
	SendBuf[5]=box->c3;
	sbuff_size = 6;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}

/***********************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
  **********************************************************************************/
uint16_t BorderType(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size;
	
	SendBuf[0]=0x1B;
	SendBuf[1]='R';
	SendBuf[2]='E';
	SendBuf[3]=box->t_n1;
	SendBuf[4]=box->t_n2;
	sbuff_size = 5;

  size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;	
}

/***********************************************************************************
  * @name    BorderBox()
  * @brief   Kreira bafer za poziv makroa
  * @param   xx1: leva x koordinata 
  * @param   xx2: desna x koordinata
  * @param	 yy1: gornja y koordinata
	* @param	 yy2: donja y koordinata
  * @retval  Bez povratnih vredosti
  *********************************************************************************/
uint16_t BorderBox (struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
	uint16_t sbuff_size, size;
	
    SendBuf[0] = 0x1B;
	SendBuf[1] = 'R';
	SendBuf[2] = 'R';
	SendBuf[3] = box->xx1_coord;
	SendBuf[4] = box->xx1_coord>>8;
	SendBuf[5] = box->yy1_coord;
	SendBuf[6] = box->yy1_coord>>8;
	SendBuf[7] = box->xx2_coord;
	SendBuf[8] = box->xx2_coord>>8;
	SendBuf[9] = box->yy2_coord;
	SendBuf[10] = box->yy2_coord>>8;
	sbuff_size = 11;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
  return size;	
}

/***********************************************************************************************
  * @name  BorderBox_TypeAndColor()
  * @brief Kreira bafer za slanje tipa i boje granica pravougaonika na i2c
	* @param n1: tip ivice
	* @param n2: tip ugla
	* @param c1: spoljasnji frejm
	* @param c2: unutrasnji frejm
	* @param c3: boja ispune
	* @param  ibuff_size: obim i2c bafera
  * @retval Bez pravih povratnih parametara
  **********************************************************************************************/

 uint16_t BorderBox_TypeAndColor(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size){
																		 
uint8_t sbuff_size;
uint16_t size;

	/*** Border type ***/
	sbuff_size = 0;
																		 
	SendBuf[0]=0x1B;
	SendBuf[1]='R';
	SendBuf[2]='E';
	SendBuf[3]= box->t_n1;
	SendBuf[4]= box->t_n2;
	sbuff_size+=5;
	
	/*** border color ***/
	SendBuf[0]=0x1B;
	SendBuf[1]='F';
	SendBuf[2]='R';
	SendBuf[3]= box->c1;
	SendBuf[4]= box->c2;
	SendBuf[5]= box->c3;
	sbuff_size+=6;																 
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}


																	 
/**********************************************************************************
  * @name
  * @brief  
  * @param  
  * @retval 
  ********************************************************************************/

uint16_t Print_box(const struct box_object *box, uint8_t *i2c_buff, uint16_t *ibuff_size)
{
uint8_t sbuff_size = 0;
uint16_t size;

	/*** Border type ***/

    /*
	SendBuf[0]=0x1B;
	SendBuf[1]='R';
	SendBuf[2]='E';
	SendBuf[3]= box->t_n1;
	SendBuf[4]= box->t_n2;
	sbuff_size+=5; 
	*/
	
	//** border color
	SendBuf[0]=0x1B;
	SendBuf[1]='F';
	SendBuf[2]='R';
	SendBuf[3]= box->c1;
	SendBuf[4]= box->c2;
	SendBuf[5]= box->c3;
	sbuff_size+=6; 

    SendBuf[0] = 0x1B;
	SendBuf[1] = 'R';
	SendBuf[2] = 'R';
	SendBuf[3] = box->xx1_coord;
	SendBuf[4] = box->xx1_coord>>8;
	SendBuf[5] = box->yy1_coord;
	SendBuf[6] = box->yy1_coord>>8;
	SendBuf[7] = box->xx2_coord;
	SendBuf[8] = box->xx2_coord>>8;
	SendBuf[9] = box->yy2_coord;
	SendBuf[10] = box->yy2_coord>>8;
	sbuff_size += 11;
	
	size = BuildSmall(SendBuf, sbuff_size, i2c_buff, ibuff_size);
	return size;
}


void insert_character_to_string(char *input_string,  char character, uint8_t position){
	
	input_string[position] = character;	
	
}


/** @endcond *//** @} *//** @} *//**********************************************
 * END of source_template.c
********************************************************************************/
