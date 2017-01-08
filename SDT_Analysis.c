/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : SDT_Analysis.c
 * Description   : analysis the SDT in the TS file,this is function module.
 * Version	     : 1.0
 * History       : 2013-4-11  Created by yanbo
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include "SDT_Analysis.h"
#include "GetFullTableFromPID.h"

#define CRC_32_SIZE	4
#define SDT_TABLE_ID 	0x42
#define SDT_PID 		0x11
/*************************************************************************
 * Function Name : GetSdtSectionHeader
 * Description   : get the sdt section header program information.
 * Parameters    : pstSdtSectionHeader --  struct point.
 *                 pucSectionBuffer -- one section buffer.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetSdtSectionHeader(SDT_SECTION *pstSdtSectionHeader, unsigned char *pucSectionBuf)
{
	if ((NULL == pstSdtSectionHeader) || (NULL == pucSectionBuf))
	{
		printf("Incoming Parameters Error (GetSdtSectionHeader)!\n");
		return -1;
	}

	pstSdtSectionHeader->table_id = pucSectionBuf[0];
	pstSdtSectionHeader->section_syntax_indicator = (pucSectionBuf[1] & 0x80) >> 7;
	pstSdtSectionHeader->transport_stream_id = (pucSectionBuf[3] << 8) | pucSectionBuf[4];
	pstSdtSectionHeader->section_length = ((pucSectionBuf[1] & 0x0f) << 8) | pucSectionBuf[2];
	pstSdtSectionHeader->version_number = (pucSectionBuf[5] & 0x3E) >> 1;
	pstSdtSectionHeader->current_next_indicator = pucSectionBuf[5] & 0x01;
	pstSdtSectionHeader->section_number = pucSectionBuf[6];
	pstSdtSectionHeader->last_section_number = pucSectionBuf[7];
	pstSdtSectionHeader->original_network_id = pucSectionBuf[8] << 8 | pucSectionBuf[9];

	return 0;
}
/*************************************************************************
 * Function Name : GetSdtLoopPartInfo
 * Description   : get the sdt section of one loop part information.
 * Parameters    : pstSdtInfo --  the SDT loop info.
 *                 pucSectionBuf -- one section buffer.
 *                 piSdtInfoNum -- the index of SDT info.
 *                 iPosition -- the byte index.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetSdtLoopPartInfo(SDT_INFO *pstSdtInfo, unsigned char *pucSectionBuf, int *piSdtInfoNum, int iPosition)
{
	if ((NULL == pstSdtInfo) || (NULL == pucSectionBuf) || (NULL == piSdtInfoNum))
	{
		printf("Incoming Parameters Error (GetSdtLoopPartInfo)!\n");
		return -1;
	}

	pstSdtInfo[*piSdtInfoNum].service_id = (pucSectionBuf[iPosition] << 8) | pucSectionBuf[iPosition + 1];
	pstSdtInfo[*piSdtInfoNum].EIT_schedule_flag = (pucSectionBuf[iPosition + 2] & 0x02) >> 1;
	pstSdtInfo[*piSdtInfoNum].EIT_present_following_flag = pucSectionBuf[iPosition + 2] & 0x01;
	pstSdtInfo[*piSdtInfoNum].running_status = (pucSectionBuf[iPosition + 3] & 0xE0) >> 5;
	pstSdtInfo[*piSdtInfoNum].free_CA_mode = (pucSectionBuf[iPosition + 3] & 0x10) >> 4;
	pstSdtInfo[*piSdtInfoNum].descriptors_loop_length = ((pucSectionBuf[iPosition + 3] & 0x0f) << 8) | pucSectionBuf[iPosition + 4];

	return 0;
}
/*************************************************************************
 * Function Name : ServiceDescriptor
 * Description   : get the sdt service descriptor information.
 * Parameters    : pstSdtInfo --  struct point of an array.
 *                 pucSectionBuf -- one section buffer.
 *				   piSdtInfoNum -- the program number counter.
 *				   iPosition -- the byte index.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int ServiceDescriptor(SDT_INFO *pstSdtInfo, unsigned char *pucSectionBuf, int *piSdtInfoNum, int iPosition)
{
	if ((NULL == pstSdtInfo) || (NULL == pucSectionBuf) || (NULL == piSdtInfoNum))
	{
		printf("Incoming Parameters Error (ServiceDescriptor)!\n");
		return -1;
	}

	pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.descriptor_tag = pucSectionBuf[iPosition + 5];
	pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.descriptor_length = pucSectionBuf[iPosition + 6];
	pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.service_type = pucSectionBuf[iPosition + 7];
	pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.service_provider_name_length = pucSectionBuf[iPosition + 8];
	memcpy(pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.service_provider_name, &pucSectionBuf[iPosition + 9], pucSectionBuf[iPosition + 8]);
	pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.service_name_length = pucSectionBuf[(iPosition + 9) + pucSectionBuf[iPosition + 8]];
	memcpy(pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.service_name, &pucSectionBuf[1 + (iPosition + 9) + pucSectionBuf[iPosition + 8]], pstSdtInfo[*piSdtInfoNum].stServiceDescriptor.service_name_length);

	return 0;
}
/*************************************************************************
 * Function Name : GetSdtInfo
 * Description   : get the sdt program information.
 * Parameters    : pstSdtInfo --  struct point of an array.
 *                 pucSectionBuf -- one section buffer.
 *				   piSdtInfoNum -- the program number counter.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
static int GetSdtInfo(SDT_INFO *pstSdtInfo, unsigned char *pucSectionBuf, int *piSdtInfoNum)
{
	int iPosition = 0;
	int iIndex = 0;
	SDT_SECTION stSdtSectionHeader = {0};
	int iDescriptorTag = 0;
	int iDescriptorLength = 0;

	if ((NULL == pstSdtInfo) || (NULL == pucSectionBuf))
	{
		printf("Incoming Parameters Error (GetSdtInfo)!\n");
		return -1;
	}

	if (-1 == GetSdtSectionHeader(&stSdtSectionHeader, pucSectionBuf))
	{
		printf("GetSdtSectionHeader Error (GetSdtInfo)!\n");
		return -1;
	}

	for (iPosition = 11; iPosition < ((int)stSdtSectionHeader.section_length + 3 - CRC_32_SIZE); )
	{
		if (-1 == GetSdtLoopPartInfo(pstSdtInfo, pucSectionBuf, piSdtInfoNum, iPosition))
		{
			printf("GetSdtLoopPartInfo Error (GetSdtInfo)!\n");
			return -1;
		}
		
		for (iIndex = 0; iIndex < (int)pstSdtInfo[*piSdtInfoNum].descriptors_loop_length; )
		{
			iDescriptorTag = pucSectionBuf[iPosition + 5 + iIndex];
			iDescriptorLength = pucSectionBuf[iPosition + 6 + iIndex];
			/*-----------------------------------------------------------
				The descriptor_tag table in page 30.
			------------------------------------------------------------*/
			switch (iDescriptorTag)
			{
				case 0x48:
					ServiceDescriptor(pstSdtInfo, pucSectionBuf, piSdtInfoNum, iPosition);
					break;
				default:
					/*reserve for other description*/
					break;
			}
			iIndex += iDescriptorLength + 2;
		}
		/*----------------------------------------------------------------
		 The 5 stand for (service_id + reserved_future + EIT_schedule_flag
		 + EIT_present_following_flag + running_status + free_CA_mode +
		 descriptors_loop_length)*Byte length.
		----------------------------------------------------------------*/
		iPosition += pstSdtInfo[*piSdtInfoNum].descriptors_loop_length + 5;
		(*piSdtInfoNum)++;
	}
	return 0;
}
/*************************************************************************
 * Function Name : ParseTheSDT
 * Description   : get the sdt program information.
 * Parameters    : pstSdtInfo --  struct point of an array.
 *				   pfFile -- the source file point.
 *                 iStartPosition -- the first 0x47 start position.
 *				   iPacketLength -- the length of one packet.
 *				   piSdtInfoNum -- the SDT info index.
 * Returns       : -1 - Fail, 0 -  OK.
 **************************************************************************/
int ParseTheSDT(FILE *pfFile, int iPacketLength, int iStartPosition, SDT_INFO *pstSdtInfo, int *piSdtInfoNum)
{
	unsigned char aucSectionBuffer[1024] = "\0";
	int iPreSectionNum[257] = {	0};
	int iJudgeValue = 0;
	int iReturnValue = 0;
	
	memset(iPreSectionNum, -1, sizeof(iPreSectionNum));

	if ((NULL == pfFile) || (NULL == pstSdtInfo) || (NULL == piSdtInfoNum))
	{
		printf("Incoming Parameters Error (ParseTheSDT)!\n");
		return -1;
	}

	if (-1==fseek(pfFile, iStartPosition - 1, SEEK_SET))
	{
		printf("Open File Error (ParseTheSDT)!\n");
		return -1;
	}

	while (!feof(pfFile))
	{
		iJudgeValue = SaveOneSection(pfFile, iPacketLength, SDT_TABLE_ID, aucSectionBuffer, iPreSectionNum, SDT_PID);
		if (-1 == iJudgeValue)
		{
			iReturnValue = -1;
			printf("SaveOneSection Error (ParseTheSDT)!\n");
			break;
		}
		
		if (2 == iJudgeValue)
		{
			memset(pstSdtInfo, 0, (*piSdtInfoNum)*sizeof(SDT_INFO));
			*piSdtInfoNum = 0;
			continue;
		}

		if (-1 == GetSdtInfo(pstSdtInfo, aucSectionBuffer, piSdtInfoNum))
		{
			iReturnValue = -1;
			printf("GetSdtInfo Error (ParseTheSDT)!\n");
			break;
		}

		if (3==iJudgeValue)
		{
			break;
		}
	}

	return iReturnValue;
}
