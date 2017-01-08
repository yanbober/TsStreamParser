/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : PMT_Analysis.c
 * Description   : analysis the PMT in the TS file,this is a declare.
 * Version	     : 1.0
 * History       : 2013-4-10  Created by yanbo
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include "PAT_Analysis.h"
#include "PMT_Analysis.h"
#include "GetFullTableFromPID.h"

#define CRC_32_SIZE	4
#define PMT_TABLE_ID 	0x02
/*************************************************************************
 * Function Name : GetPmtSectionHeader
 * Description   : get the pmt section header information.
 * Parameters    : pstPmtSectionHeader --  struct point.
 *                 pucSectionBuffer -- one section buffer.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetPmtSectionHeader(PMT_SECTION *pstPmtSectionHeader, unsigned char *pucSectionBuffer)
{
	if ((NULL == pstPmtSectionHeader) || (NULL == pucSectionBuffer))
	{
		printf("Incoming Parameters Error (GetPmtSectionHeader)!\n");
		return -1;
	}

	pstPmtSectionHeader->table_id = pucSectionBuffer[0];
	pstPmtSectionHeader->section_syntax_indicator = (pucSectionBuffer[1] & 0x80) >> 7;
	pstPmtSectionHeader->section_length = ((pucSectionBuffer[1] & 0x0f) << 8) | pucSectionBuffer[2];
	pstPmtSectionHeader->program_number = (pucSectionBuffer[3] << 8) | pucSectionBuffer[4];
	pstPmtSectionHeader->version_number = (pucSectionBuffer[5] & 0x3E) >> 1;
	pstPmtSectionHeader->current_next_indicator = pucSectionBuffer[5] & 0x01;
	pstPmtSectionHeader->section_number = pucSectionBuffer[6];
	pstPmtSectionHeader->last_section_number = pucSectionBuffer[7];
	pstPmtSectionHeader->PCR_PID = ((pucSectionBuffer[9] & 0xf8) >> 3) | (pucSectionBuffer[8] << 6);
	pstPmtSectionHeader->program_info_length = (pucSectionBuffer[10] & 0x0F) << 8 | pucSectionBuffer[11];

	return 0;
}
/*************************************************************************
 * Function Name : GetPmtLoopPartInfo
 * Description   : get the pmt section of one loop part information.
 * Parameters    : pstPmtLoopInfo --  struct point.
 *                 pucSectionBuffer -- one section buffer.
 *                 iPosition -- the bit index.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetPmtLoopPartInfo(PMT_LOOP_INFO *pstPmtLoopInfo, unsigned char *pucSectionBuffer, int iPosition)
{
	if ((NULL == pstPmtLoopInfo) || (NULL == pucSectionBuffer))
	{
		printf("Incoming Parameters Error (GetPmtLoopPartInfo)!\n");
		return -1;
	}

	pstPmtLoopInfo->stream_type = pucSectionBuffer[iPosition];
	pstPmtLoopInfo->ES_info_length = (pucSectionBuffer[iPosition + 3] & 0x0F) << 8 | pucSectionBuffer[iPosition + 4];
	pstPmtLoopInfo->elementary_PID = ((pucSectionBuffer[iPosition + 1] << 8) | pucSectionBuffer[iPosition + 2]) & 0x1FFF;

	return 0;
}
/*************************************************************************
 * Function Name : GetVideoAndAudeoPID
 * Description   : save video and audio pid information.
 * Parameters    : pstPmtInfo --  struct point of an array.
 *				   piPmtInfoNum -- the program number counter.
 *				   piVideoCount -- the video counter point.
 *				   stPmtLoopInfo -- the descriptor loop info.
 *				   piAudioCount -- the audio counter point.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetVideoAndAudeoPID(PMT_INFO *pstPmtInfo, PMT_LOOP_INFO stPmtLoopInfo, int *piPmtInfoNum, int *piVideoCount, int *piAudioCount)
{
	if ((NULL == pstPmtInfo) || (NULL == piPmtInfoNum))
	{
		printf("Incoming Parameters Error (GetVideoAndVideo)!\n");
		return -1;
	}
	/*------------------------------------------------------------------------
	    Stream type assignments table in the ISO-IEC13818 datasheet page 47.
	-------------------------------------------------------------------------*/
	switch (stPmtLoopInfo.stream_type)
	{
		case 0x01:
		case 0x02:
		case 0x10:
		case 0x1b:
			pstPmtInfo[*piPmtInfoNum].auiVideoPID[(*piVideoCount)++] = stPmtLoopInfo.elementary_PID;
			break;
		case 0x03:
		case 0x04:
		case 0x06:
		case 0x0f:
		case 0x11:
			pstPmtInfo[*piPmtInfoNum].auiAudioPID[(*piAudioCount)++] = stPmtLoopInfo.elementary_PID;
			break;
	}

	return 0;
}
/*************************************************************************
 * Function Name : GetPmtInfo
 * Description   : get the pmt program information.
 * Parameters    : pstPmtInfo --  struct point of an array.
 *                 pucSectionBuffer -- one section buffer.
 *				   piPmtInfoNum -- the program number counter.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetPmtInfo(PMT_INFO *pstPmtInfo, unsigned char *pucSectionBuffer, int *piPmtInfoNum)
{
	int iPosition = 12;
	int iVideoCount = 0;
	int iAudioCount = 0;
	PMT_SECTION stPmtSectionHeader = {0};
	PMT_LOOP_INFO stPmtLoopInfo = {0};

	if ((NULL == pstPmtInfo) || (NULL == pucSectionBuffer))
	{
		printf("Incoming Parameters Error (GetPmtInfo)!\n");
		return -1;
	}

	if (-1 == GetPmtSectionHeader(&stPmtSectionHeader, pucSectionBuffer))
	{
		printf("GetPmtSectionHeader Error (GetPmtInfo)!\n");
		return -1;
	}

	iPosition += stPmtSectionHeader.program_info_length;

	pstPmtInfo[*piPmtInfoNum].program_number = stPmtSectionHeader.program_number;

	for (iPosition = iPosition; iPosition < (int)stPmtSectionHeader.section_length + 3 - CRC_32_SIZE; iPosition += 5)
	{
		if (-1 == GetPmtLoopPartInfo(&stPmtLoopInfo, pucSectionBuffer, iPosition))
		{
			printf("function GetPmtLoopPartInfo Error (GetPmtInfo)!\n");
			return -1;
		}

		if (-1 == GetVideoAndAudeoPID(pstPmtInfo, stPmtLoopInfo, piPmtInfoNum, &iVideoCount, &iAudioCount))
		{
			printf("function GetVideoAndAudeoPID Error (GetPmtInfo)!\n");
			return -1;
		}

		iPosition += stPmtLoopInfo.ES_info_length;
	}

	return 0;
}
/*************************************************************************
 * Function Name : ParseOneProgram
 * Description   : get one program information.
 * Parameters    : pfFile -- the source file point.
 *				   iPacketLength -- the ts file packet length.
 *                 iStartPosition -- the first 0x47 start position.
 *				   pstPmtInfo -- the pmt data of info.
 *				   uiPID -- the program pid.
 *				   piPmtInfoNum -- the program number counter.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int ParseOneProgram(FILE *pfFile, int iPacketLength, int iStartPosition, PMT_INFO *pstPmtInfo, int *piPmtInfoNum, unsigned uiPID)
{
	int iReturnValue = 0;
	unsigned char aucSectionBuffer[1024] = "\0";
	int iPreSectionNum[257] = {0};
	int iJudgeValue = 0;
	
	memset(iPreSectionNum, -1, sizeof(iPreSectionNum));

	if ((NULL == pfFile) || (NULL == pstPmtInfo) || (NULL == piPmtInfoNum))
	{
		printf("Incoming Parameters Error (ParseOneProgram)!\n");
		return -1;
	}

	if (-1 == fseek(pfFile, iStartPosition - 1, SEEK_SET))
	{
		printf("Open File Error (ParseOneProgram)!\n");
		return -1;
	}

	while (!feof(pfFile))
	{
		iJudgeValue = SaveOneSection(pfFile, iPacketLength, PMT_TABLE_ID, aucSectionBuffer, iPreSectionNum, uiPID);
		if (-1 == iJudgeValue)
		{
			printf("SaveOneSection Error (ParseOneProgram)!\n");
			iReturnValue = -1;
			break;
		}
		
		if (2 == iJudgeValue)
		{
			memset(pstPmtInfo, 0, (*piPmtInfoNum) * sizeof(PMT_INFO));
			(*piPmtInfoNum) = 0;
			continue;
		}

		if (-1 == GetPmtInfo(pstPmtInfo, aucSectionBuffer, piPmtInfoNum))
		{
			printf("GetPmtInfo Error (ParseOneProgram)!\n");
			iReturnValue = -1;
			break;
		}
		(*piPmtInfoNum)++;

		if (3 == iJudgeValue)
		{
			break;
		}
	}

	return iReturnValue;
}
/*************************************************************************
 * Function Name : ParseThePMT
 * Description   : get the pmt program information.
 * Parameters    : pstPmtInfo --  struct point of an array.
 *				   pfFile -- the source file point.
 *				   iPacketLength -- the ts file packet length.
 *                 iStartPosition -- the first 0x47 start position.
 *				   pstPatInfo -- the pat data of pid.
 *				   iPatInfoNum -- the pat data counter.
 *				   piPmtInfoNum -- the program number counter.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
int ParseThePMT(FILE *pfFile, int iPacketLength, int iStartPosition, PMT_INFO *pstPmtInfo, int *piPmtInfoNum, PAT_PROGRAM *pstPatInfo, int iPatInfoNum)
{
	int iIndex = 0;

	if ((NULL == pfFile) || (NULL == pstPmtInfo) || (NULL == pstPatInfo) || (NULL == piPmtInfoNum))
	{
		printf("Incoming Parameters Error (ParseThePMT)!\n");
		return -1;
	}

	for (iIndex = 0; iIndex < iPatInfoNum; iIndex++)
	{
		if (0 == pstPatInfo[iIndex].program_map_PID)
		{
			continue;
		}

		if (-1 == ParseOneProgram(pfFile, iPacketLength, iStartPosition, pstPmtInfo, piPmtInfoNum, pstPatInfo[iIndex].program_map_PID))
		{
			printf("ParseOneProgram Error (ParseThePMT)!\n");
			return -1;
		}
	}

	return 0;
}
