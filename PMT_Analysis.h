/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : PMT_Analysis.h
 * Description   : This file is used to declare related functions, cooperate
 * 				   with the PMT_Analysis.c file.
 * Version	     : 1.0
 * History       : 2013-4-10  Created by yanbo
 *************************************************************************/
#ifndef PMT_ANALYSIS_H
#define PMT_ANALYSIS_H

/*------------------------------------------------------------------------
    This section struct defined in ISO-IEC-13818,Page of 46 to 48 have
    descript it. The stream_type table in page 47.
-------------------------------------------------------------------------*/
typedef struct
{
	unsigned short program_number :16;
	unsigned auiVideoPID[25];
	unsigned auiAudioPID[25];
} PMT_INFO;

typedef struct
{
	unsigned short ES_info_length :12;
	unsigned char stream_type :8;
	unsigned short elementary_PID :13;
} PMT_LOOP_INFO;

typedef struct
{
	unsigned char table_id :8;
	unsigned char section_syntax_indicator :1;
	unsigned short section_length :12;
	unsigned short program_number :16;
	unsigned char version_number :5;
	unsigned char current_next_indicator :1;
	unsigned char section_number :8;
	unsigned char last_section_number :8;
	unsigned short PCR_PID :13;
	unsigned short program_info_length :12;
} PMT_SECTION;

int ParseThePMT(FILE *pfFile, int iPacketLength, int iStartPosition, PMT_INFO *pstPmtInfo, int *piPmtInfoNum, PAT_PROGRAM *pstPatInfo, int iPatInfoNum);

#endif
