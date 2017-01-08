/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	   : GetFullTableFromPID.h
 * Description : This file is used to declare related functions, cooperate
 * 				 with the GetFullTableFromPID.c file.
 * Version	   : 1.0
 * History     : yanbo 2013-04-02  Create
 *************************************************************************/
#ifndef GET_FULL_TABLE_FROM_PID_H
#define GET_FULL_TABLE_FROM_PID_H
/*-----------------------------------------------------------------
 This struct definition depends on ISO/IEC-138181.pdf page on 43.
 The Variable named as the official,not fit Goosat standard.
 -------------------------------------------------------------------*/
typedef struct
{
	unsigned char table_id :8;
	unsigned char section_syntax_indicator :1;
	unsigned short section_length :12;
	unsigned short table_id_extension :16;
	unsigned char version_number :5;
	unsigned char current_next_indicator :1;
	unsigned char section_number :8;
	unsigned char last_section_number :8;
} SECTION_HEADER;

int SaveOneSection(FILE * pfFile, int iPacketLength, unsigned int uiTableID, unsigned char * pucSectionBuffer, int *piPreSectionNum, int iPID);

#endif
