/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : PAT_Analysis.c
 * Description   : analysis the PAT in the TS file,this is the function.
 * Version	     : 1.0
 * History       : 2013-4-8  Created by yanbo
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include "GetFullTableFromPID.h"
#include "PAT_Analysis.h"

#define CRC_32_SIZE	4
#define PAT_TABLE_ID 	0x00
#define PAT_PID 		0x00
/*************************************************************************
 * Function Name : GetPatSectionHeader
 * Description   : get the pat section header info.
 * Parameters    : pstPatSectionHeader --  struct point.
 *                 pucSectionBuffer -- one section buffer.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetPatSectionHeader(PAT_SECTION *pstPatSectionHeader, unsigned char *pucSectionBuffer)
{
	if ((NULL == pstPatSectionHeader) || (NULL == pucSectionBuffer))
	{
		printf("Incoming Parameters Error (GetPatSectionHeader)!\n");
		return -1;
	}

	pstPatSectionHeader->table_id = pucSectionBuffer[0];
	pstPatSectionHeader->section_syntax_indicator = (pucSectionBuffer[1] & 0x80) >> 7;
	pstPatSectionHeader->transport_stream_id = (pucSectionBuffer[3] << 8) | pucSectionBuffer[4];
	pstPatSectionHeader->section_length = ((pucSectionBuffer[1] & 0x0f) << 8) | pucSectionBuffer[2];
	pstPatSectionHeader->version_number = (pucSectionBuffer[5] & 0x3E) >> 1;
	pstPatSectionHeader->current_next_indicator = pucSectionBuffer[5] & 0x01;
	pstPatSectionHeader->section_number = pucSectionBuffer[6];
	pstPatSectionHeader->last_section_number = pucSectionBuffer[7];

	return 0;
}
/*************************************************************************
 * Function Name : GetPatInfo
 * Description   : get the pat program information.
 * Parameters    : pstPatInfo --  struct point of an array.
 *                 pucSectionBuffer -- one section buffer.
 *				   piPatInfoNum -- the program number counter.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetPatInfo(PAT_PROGRAM * pstPatInfo, unsigned char * pucSectionBuffer, int * piPatInfoNum)
{
	PAT_SECTION stPatSectionHeader = {0};
	int iIndex = 0;

	if ((NULL == pstPatInfo) || (NULL == pucSectionBuffer))
	{
		printf("Incoming Parameters Error (GetPatInfo)!\n");
		return -1;
	}

	if (-1 == GetPatSectionHeader(&stPatSectionHeader, pucSectionBuffer))
	{
		printf("GetPatSectionHeader Error (GetPatInfo)!\n");
		return -1;
	}

	for (iIndex = 8; iIndex < (int)(stPatSectionHeader.section_length + 3 - CRC_32_SIZE); iIndex += 4)
	{
		pstPatInfo[*piPatInfoNum].program_number = pucSectionBuffer[iIndex] << 8 | pucSectionBuffer[1 + iIndex];

		if (0x00 != pstPatInfo[*piPatInfoNum].program_number)
		{
			pstPatInfo[*piPatInfoNum].program_map_PID = (pucSectionBuffer[2 + iIndex] << 3) << 5 | pucSectionBuffer[3 + iIndex];
		}
		else
		{
			pstPatInfo[*piPatInfoNum].network_PID = (pucSectionBuffer[2 + iIndex] << 3) << 5 | pucSectionBuffer[3 + iIndex];
		}

		(*piPatInfoNum)++;
	}

	return 0;
}
/*************************************************************************
 * Function Name : ParseThePAT
 * Description   : parse the PAT table.
 * Parameters    : pfFile --  file point.
 *                 iPacketLength -- packet length.
 *				   iStartPosition -- the first 0x47 start position.
 *				   pstPatInfo -- PAT table data.
 *				   piPatInfoNum -- the PID counter.
 * Returns       : -1 - fail, 0 - OK.
 **************************************************************************/
int ParseThePAT(FILE *pfFile, int iPacketLength, int iStartPosition, PAT_PROGRAM *pstPatInfo, int *piPatInfoNum)
{
	unsigned char aucSectionBuffer[1024] = "\0";
	int iPreSectionNum[257] = {0};
	int iJudgeValue = 0;
	int iReturnValue = 0;

	memset(iPreSectionNum, -1, sizeof(iPreSectionNum));
	if ((NULL == pfFile) || (NULL == pstPatInfo) || (NULL == piPatInfoNum))
	{
		printf("Incoming Parameters Error (ParseThePAT)!\n");
		return -1;
	}

	if (-1 == fseek(pfFile, iStartPosition - 1, SEEK_SET))
	{
		printf("Open File Error (ParseThePAT)!\n");
		return -1;
	}

	while (!feof(pfFile))
	{
		iJudgeValue = SaveOneSection(pfFile, iPacketLength, PAT_TABLE_ID, aucSectionBuffer, iPreSectionNum, PAT_PID);
		if (-1 == iJudgeValue)
		{
			printf("SaveOneSection Error (ParseThePAT)!\n");
			iReturnValue = -1;
			break;
		}

		if (2 == iJudgeValue)
		{
			memset(pstPatInfo, 0, (*piPatInfoNum) * sizeof(PAT_PROGRAM));
			(*piPatInfoNum) = 0;
			continue;
		}

		if (-1 == GetPatInfo(pstPatInfo, aucSectionBuffer, piPatInfoNum))
		{
			printf("SaveOneSection Error (ParseThePAT)!\n");
			iReturnValue = -1;
			break;
		}

		if (3 == iJudgeValue)
		{
			break;
		}
	}

	return iReturnValue;
}
