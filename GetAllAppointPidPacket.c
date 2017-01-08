/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : GetAllAppointPidPacket.c
 * Description   : Get all appoint PID packet in the TS file and store it
 * 				   into a new file.
 * Version	     : 1.0
 * History       : 2013-3-25  Created by yanbo
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GetAllAppointPidPacket.h"
/*************************************************************************
 * Function Name : AdjustTsPacketHeader
 * Description   : Reassign the memory value as define bit.
 * Parameters    : pstTsHeader -- an TS_PACKET_HEADER struct point.
 *                 pucPacketBuffer -- A packet buffer point.
 * Returns       :  void.
 **************************************************************************/
void AdjustTsPacketHeader(TS_PACKET_HEADER * pstTsHeader, unsigned char * pucPacketBuffer)
{
	pstTsHeader->sync_byte = pucPacketBuffer[0];
	pstTsHeader->transport_error_indicator = pucPacketBuffer[1] >> 7;
	pstTsHeader->payload_unit_start_indicator = (pucPacketBuffer[1] >> 6) & 0x01;
	pstTsHeader->transport_priority = (pucPacketBuffer[1] >> 5) & 0x01;
	pstTsHeader->PID = ((pucPacketBuffer[1] & 0x1F) << 8) | pucPacketBuffer[2];
	pstTsHeader->transport_scrambling_control = pucPacketBuffer[3] >> 6;
	pstTsHeader->adaptation_field_control = (pucPacketBuffer[3] >> 4) & 0x03;
	pstTsHeader->continuity_counter = pucPacketBuffer[3] & 0x0F;
}
/*************************************************************************
 * Function Name : JudgePidAndSave
 * Description   : Judge and save the same PID packet.
 * Parameters    : pfFilePoint -- The TS file point.
 *				  pfNewFile -- new file point.
 *                iPacketLength -- the Ts packet length.
 *                puiInputPID -- the user input PID value.
 *				  iStartPosition -- the first 0x47 position.
 * Returns       : 0 - Success, -1 - Error.
 **************************************************************************/
int JudgePidAndSave(FILE * pfFilePoint, FILE * pfNewFile, int iPacketLength, int iStartPosition, unsigned int * puiInputPID)
{
	TS_PACKET_HEADER stTsPacketHeader = {0};
	unsigned char aucPacketBuffer[204] = "\0";
	int iCounter = 0;
	memset(aucPacketBuffer, 0, 204);

	if (0 != fseek(pfFilePoint, iStartPosition - 1, SEEK_SET))
	{
		printf("Fseek SEEK_SET Error (GetFileSizeAndSetStartPosition)!\n");
		return -1;
	}

	while (iPacketLength == (int)fread(aucPacketBuffer, 1, iPacketLength, pfFilePoint))
	{
		AdjustTsPacketHeader(&stTsPacketHeader, aucPacketBuffer);

		if (0x47 != stTsPacketHeader.sync_byte)
		{
			continue;
		}
		
		for (iCounter=0; puiInputPID[iCounter]; iCounter++)
		{
			if (puiInputPID[iCounter] != stTsPacketHeader.PID)
			{
				continue;
			}
			
			if (iPacketLength != (int)fwrite(aucPacketBuffer, 1, iPacketLength, pfNewFile))
			{
				printf("Write NewFile Error (JudgePidAndSave)!\n");
				return -1;
			}
		}
	}

	return 0;
}
