/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : GetAllAnalysisResult.c
 * Description   : get the PAT PMT SDT information and save struct.
 * Version	     : 1.0
 * History       : 2013-4-12  Created by yanbo
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include "PAT_Analysis.h"
#include "PMT_Analysis.h"
#include "SDT_Analysis.h"
#include "GetAllAnalysisResult.h"
/*************************************************************************
 * Function Name : CombineVideoAndAudio
 * Description   : combine the vedio and audio to a array.
 * Parameters    : pstProgramInfo --  program struct point.
 *                 iLoop -- the current index.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int CombineVideoAndAudio(PROGRAM_INFO *pstProgramInfo, int iLoop)
{
	int iVideoIndex = 0;
	int iAudioIndex = 0;

	if (NULL == pstProgramInfo)
	{
		printf("Incoming Parameters Error (CombineVideoAndAudio)!\n");
		return -1;
	}

	while (0 != pstProgramInfo[iLoop].auiVideoPID[iVideoIndex])
	{
		pstProgramInfo[iLoop].auiAvPID[iVideoIndex] = pstProgramInfo[iLoop].auiVideoPID[iVideoIndex];
		iVideoIndex++;
	}

	while (0 != pstProgramInfo[iLoop].auiAudioPID[iAudioIndex])
	{
		pstProgramInfo[iLoop].auiAvPID[iVideoIndex] = pstProgramInfo[iLoop].auiAudioPID[iAudioIndex];
		iAudioIndex++;
		iVideoIndex++;
	}

	return 0;
}
/*************************************************************************
 * Function Name : RevisedIllegalCharacter
 * Description   : revised the program name for sprintf function.Because
 * 				   windows OS think : \ / * and some characters is illegal.
 * Parameters    : pstProgramInfo --  program struct point.
 *                 iIndex -- the current index.
 *				   pcNewName -- the revised file name.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
int RevisedIllegalCharacter(PROGRAM_INFO *pstProgramInfo, char *pcNewName, int iIndex)
{
	int iCount = 0;
	int iNum = 0;
	char acCharacter[10] = {':', '\\', '/', '*', '?', '"', '|', '<', '>'};

	strcpy(pcNewName, pstProgramInfo[iIndex].service_name);
	while ('\0' != pcNewName[iNum++])
	{
		/*---------------------------------------------------------------
		  	  Number 32 and 122 stand for ASCII value.
		 --------------------------------------------------------------*/
		if ((32 > pcNewName[iNum - 1]) || (122 < pcNewName[iNum - 1]))
		{
			pcNewName[iNum - 1] = '-';
			continue;
		}
		
		for (iCount = 0; iCount < 10; iCount++)
		{
			if (acCharacter[iCount] == pcNewName[iNum - 1])
			{
				pcNewName[iNum - 1] = '-';
				break;
			}
		}
	}

	return 0;
}
/*************************************************************************
 * Function Name : GetAllAnalysisResult
 * Description   : get the PAT PMT SDT result,Aalysis it and save.
 * Parameters    : pstPmtInfo --  PMT info.
 *                 pstSdtInfo -- SDT info.
 *				   iPmtInfoNum -- the PMT count.
 *				   iSdtInfoNum -- the SDT count.
 *				   pstProgramInfo -- the program info point.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
int GetAllAnalysisResult(PMT_INFO *pstPmtInfo, int iPmtInfoNum, SDT_INFO *pstSdtInfo, int iSdtInfoNum, PROGRAM_INFO *pstProgramInfo)
{
	int iIndex = 0;
	int iCounter = 0;
	int iLoop = 0;

	if ((NULL == pstPmtInfo) || (NULL == pstSdtInfo) || (NULL == pstProgramInfo))
	{
		printf("Incoming Parameters Error (GetAllAnalysisResult)!\n");
		return -1;
	}

	for (iIndex = 0; iIndex < iSdtInfoNum; iIndex++)
	{
		for (iCounter = 0; iCounter < iPmtInfoNum; iCounter++)
		{
			if (pstPmtInfo[iCounter].program_number == pstSdtInfo[iIndex].service_id)
			{
				pstProgramInfo[iLoop].service_id = pstPmtInfo[iCounter].program_number;
				pstProgramInfo[iLoop].free_CA_mode = pstSdtInfo[iIndex].free_CA_mode;
				memcpy(pstProgramInfo[iLoop].service_name, pstSdtInfo[iIndex].stServiceDescriptor.service_name, 256);
				memcpy(pstProgramInfo[iLoop].auiVideoPID, pstPmtInfo[iCounter].auiVideoPID, sizeof(unsigned int) * 25);
				memcpy(pstProgramInfo[iLoop].auiAudioPID, pstPmtInfo[iCounter].auiAudioPID, sizeof(unsigned int) * 25);
				CombineVideoAndAudio(pstProgramInfo, iLoop);
				iLoop++;
				break;
			}
		}
	}

	return 0;
}
