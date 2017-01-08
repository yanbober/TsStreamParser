/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	   : GetTsPacketLength.c
 * Description : This file is used to resolve the Ts stream packet length
 * 				 operation (Auto test 188Byte or 204Byte).
 * Version	   : 1.0
 * History     : yanbo 2013-03-18  Create
 *************************************************************************/
#include <stdio.h>

#define SYNC_0x47  0x47
/*************************************************************************
 * Function Name : JumpAndJudge
 * Description   : Cycle test 10 consecutive jump position and test the 
 * 				   value whether is 0x47.
 * Parameters    : pfFilePoint -- File point variable.
 *                 iJumpValue -- Jump back length.
 *				   iStartPosition -- where to start.
 * Returns       : -1 - Failure code, -2 - Judge not OK, other - Judge OK.
 **************************************************************************/
int static JumpAndJudge(FILE * pfFilePoint, int iJumpValue, int iStartPosition)
{
	int iReturnValue = 0;
	int iIndex = 0;

	if (NULL == pfFilePoint)
	{
		printf("Incoming Parameters Error (JumpAndJudge)!\n");
		return -1;
	}

	for (iIndex = 0; iIndex < 10; ++iIndex)
	{
		if (0 != fseek(pfFilePoint, iJumpValue - 1, SEEK_CUR))
		{
			printf("Fseek return Error (JumpAndJudge)!\n");
			return -1;
		}

		iReturnValue = fgetc(pfFilePoint);
		if (feof(pfFilePoint))
		{
			break;
		}

		if (SYNC_0x47 != iReturnValue)
		{
			iReturnValue = 0;
			break;
		}
	}
	/*-----------------------------------------------------------------
	 When each cycle jump iJumpValue position and the 10-times testing 
	 results is all right,the return value is iJumpValue.Such as 188 or 204.
	 -------------------------------------------------------------------*/
	if (0 != iReturnValue)
	{
		iReturnValue = iJumpValue;
	}

	if (0 != fseek(pfFilePoint, iStartPosition, SEEK_SET))
	{
		iReturnValue = -1;
		printf("Incoming Parameters Error (JumpAndJudge)!\n");
	}

	return iReturnValue;
}
/*************************************************************************
 * Function Name : GetTsPacketLength
 * Description   : Unpack the TS file packet length (188/204byte).
 * Parameters    : pfFile -- The TS file path and name.
 *                iStartPosition -- From point address get start position value.
 * Returns       : >0 - Success Length Value, -1 - Failure code.
 **************************************************************************/
int GetTsPacketLength(FILE * pfFile, int * iStartPosition)
{
	int iReturnData = -1;

	if (NULL == pfFile)
	{
		printf("Incoming Parameters Error (GetTsPacketLength)!\n");
		return -1;
	}

	while (!feof(pfFile))
	{
		if (SYNC_0x47 == (fgetc(pfFile)))
		{
			*iStartPosition = ftell(pfFile);
			iReturnData = JumpAndJudge(pfFile, 204, *iStartPosition);
			if ((204 == iReturnData) || (-1 == iReturnData))
			{
				break;
			}

			iReturnData = JumpAndJudge(pfFile, 188, *iStartPosition);
			if ((188 == iReturnData) || (-1 == iReturnData))
			{
				break;
			}
		}
	}

	return iReturnData;
}
