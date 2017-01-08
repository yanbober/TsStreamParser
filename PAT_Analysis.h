/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : PAT_Analysis.h
 * Description   : This file is used to declare related functions, cooperate
 * 				   with the PAT_Analysis.c file.
 * Version	     : 1.0
 * History       : 2013-4-8  Created by yanbo
 *************************************************************************/
#ifndef PAT_ANALYSIS_H
#define PAT_ANALYSIS_H

/*------------------------------------------------------------------------
    This section struct defined in ISO-IEC-13818,Page of 43 to 44 have
    descript it.
-------------------------------------------------------------------------*/
typedef struct
{
	unsigned short program_number :16;
	unsigned short program_map_PID :13;
	unsigned short network_PID :13;
} PAT_PROGRAM;

typedef struct
{
	unsigned char table_id :8;
	unsigned char section_syntax_indicator :1;
	unsigned short section_length :12;
	unsigned short transport_stream_id :16;
	unsigned char version_number :5;
	unsigned char current_next_indicator :1;
	unsigned char section_number :8;
	unsigned char last_section_number :8;
} PAT_SECTION;

int ParseThePAT(FILE *pfFile, int iPacketLength, int iStartPosition, PAT_PROGRAM *pstPatInfo, int *piPatInfoNum);

#endif
