/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	   : GetFullTableFromPID.c
 * Description : Get the full section for table.
 * Version	   : 1.0
 * History     : yanbo 2013-04-03  Create.
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include "GetAllAppointPidPacket.h"
#include "GetFullTableFromPID.h"
/*************************************************************************
 * Function Name : GetSectionHeader
 * Description   : Adjust each section's header struct.
 * Parameters    : pstSectionHeader -- SECTION_HEADER struct point.
 *                 pucPayLoadBuffer -- Payload real start position.
 * Returns       : -1 - Failure code, 0 - Judge not OK.
 **************************************************************************/
static int GetSectionHeader(SECTION_HEADER * pstSectionHeader, unsigned char * pucPayLoadBuffer)
{
	if ((NULL == pstSectionHeader) || (NULL == pucPayLoadBuffer))
	{
		printf("Incoming Parameters Error (GetSectionHeader)!\n");
		return -1;
	}

	pstSectionHeader->table_id = pucPayLoadBuffer[0];
	pstSectionHeader->section_syntax_indicator = (pucPayLoadBuffer[1] & 0x80) >> 7;
	pstSectionHeader->section_length = ((pucPayLoadBuffer[1] & 0x0f) << 8) | pucPayLoadBuffer[2] + 3;
	pstSectionHeader->table_id_extension = (pucPayLoadBuffer[3] << 8) | pucPayLoadBuffer[4];
	pstSectionHeader->version_number = (pucPayLoadBuffer[5] & 0x3E) >> 1;
	pstSectionHeader->current_next_indicator = pucPayLoadBuffer[5] & 0x01;
	pstSectionHeader->section_number = pucPayLoadBuffer[6];
	pstSectionHeader->last_section_number = pucPayLoadBuffer[7];

	return 0;
}
/*************************************************************************
 * Function Name : GetPacketPayLoadPosition
 * Description   : Get the section payload start position.
 * Parameters    : pstTsHeader -- TS_PACKET_HEADER struct point.
 *                 pucPacketBuffer -- a packet buffer.
 * Returns       : -1 - Failure code,0 - Judge OK.
 **************************************************************************/
static int GetPacketPayLoadPosition(TS_PACKET_HEADER * pstTsHeader, unsigned char * pucPacketBuffer)
{
	int iReturnValue = 0;

	if ((NULL == pstTsHeader) || (NULL == pucPacketBuffer))
	{
		printf("Incoming Parameters Error (GetPacketPayLoadPosition)!\n");
		return -1;
	}
	/*-------------------------------------------------------------------
	 ISO-IEC-13818 page 20 to 22 descript this info.
	 -------------------------------------------------------------------*/
	switch (pstTsHeader->adaptation_field_control)
	{
		case 0:
		case 2:
			break;
		case 1:
			iReturnValue = 4;
			break;
		case 3:
			iReturnValue = 5 + pucPacketBuffer[4];
			break;
	}

	if ((0 != iReturnValue) && (1 == pstTsHeader->payload_unit_start_indicator))
	{
		iReturnValue = iReturnValue + 1 + pucPacketBuffer[iReturnValue];
	}

	return iReturnValue;
}
/*************************************************************************
 * Function Name : PayLoadCopy
 * Description   : copy the payload as define size and condition.
 * Parameters    : iRealPacketSize -- one packet payload size..
 *				   pucSectionBuffer -- a section buffer.
 *				   pstSectionHeader -- section head info.
 *				   pucPayLoad -- copy start position.
 *				   piOffect -- cycle position regist here.
 * Returns       : 0 - copy end, 1 - copy continue.
 **************************************************************************/
static int PayLoadCopy(int iRealPacketSize, SECTION_HEADER *pstSectionHeader, unsigned char * pucSectionBuffer, unsigned char *pucPayLoad, int * piOffect)
{
	int iReturnValue = 0;
	int iLoadSectionSize = 0;

	if ((NULL == pucSectionBuffer) || (NULL == pucPayLoad) || (NULL == piOffect))
	{
		printf("Incoming Parameters Error (PayLoadCopy)!\n");
		return -1;
	}

	iLoadSectionSize = pstSectionHeader->section_length - *piOffect;

	if (iLoadSectionSize > iRealPacketSize)
	{
		memcpy(pucSectionBuffer + *piOffect, pucPayLoad, iRealPacketSize);
		*piOffect += iRealPacketSize;
		iReturnValue = 1;
	}
	else
	{
		memcpy(pucSectionBuffer + *piOffect, pucPayLoad, iLoadSectionSize);
		iReturnValue = 0;
	}

	return iReturnValue;
}
/*************************************************************************
 * Function Name : CompareCurrentPreviousSection
 * Description   : Judge each section header to decide save or throw.
 * Parameters    : pstSectionHeader -- struct point.
 *                 piPreSectionNum -- the old data save point.
 *				   iSaveNumber -- How many section saved in different.
 * Returns       : 1 - continue a section, 2 - version changed. 
 **************************************************************************/
static int CompareCurrentPreviousSection(SECTION_HEADER * pstSectionHeader, int * piPreSectionNum)
{
	int iReturnValue = 0;
	int iCounter = 0;
	int iFlag = 1;

	if ((NULL == pstSectionHeader) || (NULL == piPreSectionNum))
	{
		printf("Incoming Parameters Error (JudgeSectionHeader)!\n");
		return -1;
	}
	/*--------------------------------------------------------------------
	  if piPreSectionNum[i] value equal -1 stand for not be handle.
	  if piPreSectionNum[i] value equal 1 stand for have been handled.
	 --------------------------------------------------------------------*/
	if ((piPreSectionNum[0] != (int)pstSectionHeader->version_number) && (-1 != piPreSectionNum[0]))
	{
		return 2;
	}
	
	for (iCounter = 1; iCounter < (pstSectionHeader->last_section_number + 1); iCounter++)
	{
		if (-1 == piPreSectionNum[iCounter]) 
		{
			iFlag = 0;
			break;
		}
	}
	
	if (1 == iFlag)
	{
		return 3;
	}

	if (-1 == piPreSectionNum[pstSectionHeader->section_number + 1])
	{
		piPreSectionNum[pstSectionHeader->section_number + 1] = 1;
	}
	else 
	{
		iReturnValue = 1;
	}

	return iReturnValue;
}
/*************************************************************************
 * Function Name : CheckOnePacketAndPreparation
 * Description   : Judge 0x47 and same PID, get header of section.
 * Parameters    : pstTsHeader -- packet header point.
 *                 pucPacketBuffer -- packet buffer.
 *                 pstSectionHeader -- section head info.
 *				   iPID -- the PID value.
 *				   piOffset -- get the offset value.
 * Returns       : -1 - Error,1 - Continue,0 - normally.
 **************************************************************************/
static int CheckOnePacketAndPreparation(TS_PACKET_HEADER * pstTsHeader, unsigned char * pucPacketBuffer, SECTION_HEADER * pstSectionHeader, int iPID, int * piOffset)
{
	if ((NULL == pstTsHeader) || (NULL == pucPacketBuffer) || (NULL == pstSectionHeader))
	{
		printf("Incoming Parameters Error (CheckOnePacketAndPrepare)!\n");
		return -1;
	}

	AdjustTsPacketHeader(pstTsHeader, pucPacketBuffer);
	if ((iPID != (int)pstTsHeader->PID) || (0x47 != (int)pstTsHeader->sync_byte))
	{
		return 1;
	}

	*piOffset = GetPacketPayLoadPosition(pstTsHeader, pucPacketBuffer);
	if (0 == *piOffset)
	{
		return 1;
	}

	if (1 == pstTsHeader->payload_unit_start_indicator)
	{
		GetSectionHeader(pstSectionHeader, pucPacketBuffer + *piOffset);
	}

	return 0;
}
/*************************************************************************
 * Function Name : SaveOneSection
 * Description   : section to table.
 * Parameters    : pfFile --  file point.
 *                 iPacketLength -- packet length.
 *				   uiTableID -- enter a table_id.
 *				   pucSectionBuffer -- save one section data.
 *				   piPreSectionNum -- save the parsed section info.
 *				   iPID -- enter a PID.
 * Returns       : 0 - One section OK,3 - All section OK
 *				   2 - last_section_num!=section count and version changed.
 **************************************************************************/
int SaveOneSection(FILE *pfFile, int iPacketLength, unsigned int uiTableID, unsigned char *pucSectionBuffer, int *piPreSectionNum, int iPID)
{
	SECTION_HEADER stSectionHeader = {0};
	TS_PACKET_HEADER stTsHeader = {0};
	unsigned char aucPacketBuffer[204] = "\0";
	int iSectionContinueFlag = 2;
	int iJudgeValue = 0;
	int iOffset = 0;
	int iSectionCopySize = 0;
	int iReturnValue = 0;

	if ((NULL == pfFile) || (NULL == pucSectionBuffer) || (NULL == piPreSectionNum))
	{
		printf("Incoming Parameters Error (SaveOneSection)!\n");
		return -1;
	}

	while (iPacketLength == (int)fread(aucPacketBuffer, 1, iPacketLength, pfFile))
	{
		if (1 == CheckOnePacketAndPreparation(&stTsHeader, aucPacketBuffer, &stSectionHeader, iPID, &iOffset))
		{
			continue;
		}

		if ((stSectionHeader.table_id == uiTableID) && (1 == stTsHeader.payload_unit_start_indicator))
		{
			iJudgeValue = CompareCurrentPreviousSection(&stSectionHeader, piPreSectionNum);
			if (1 == iJudgeValue)
			{
				continue;
			}
			else if (2 == iJudgeValue)
			{
				memset(piPreSectionNum, -1, 256*sizeof(int));
				iReturnValue = 2;
				break;
			}

			iSectionContinueFlag = PayLoadCopy((188 - iOffset), &stSectionHeader, pucSectionBuffer, (aucPacketBuffer+iOffset), &iSectionCopySize);
		}
		else if (1 == iSectionContinueFlag)
		{
			iSectionContinueFlag = PayLoadCopy((188 - iOffset), &stSectionHeader, pucSectionBuffer, (aucPacketBuffer+iOffset), &iSectionCopySize);
		}
		/*----------------------------------------------------------------
		 One section save ok and save some info for next time compare.
		 -----------------------------------------------------------------*/
		if (0 == iSectionContinueFlag)
		{
			piPreSectionNum[0] = stSectionHeader.version_number;
			iReturnValue = iJudgeValue;
			break;
		}
	}

	return iReturnValue;
}
