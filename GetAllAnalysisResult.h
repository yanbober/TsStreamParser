/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : GetAllAnalysisResult.h
 * Description   : This file is used to declare related functions, cooperate
 * 				   with the GetAllAnalysisResult.c file.
 * Version	     : 1.0
 * History       : 2013-4-12  Created by yanbo
 *************************************************************************/
#ifndef GET_ALL_ANALYSIS_RESULT_H
#define GET_ALL_ANALYSIS_RESULT_H

typedef struct
{
	unsigned short service_id :16;
	unsigned char free_CA_mode :1;
	char service_name[256];
	unsigned int auiVideoPID[25];
	unsigned int auiAudioPID[25];
	unsigned int auiAvPID[50];
} PROGRAM_INFO;

int RevisedIllegalCharacter(PROGRAM_INFO *pstProgramInfo, char *pcNewName, int iIndex);
int GetAllAnalysisResult(PMT_INFO *pstPmtInfo, int iPmtInfoNum, SDT_INFO *pstSdtInfo, int iSdtInfoNum, PROGRAM_INFO *pstProgramInfo);

#endif
