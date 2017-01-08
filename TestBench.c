/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	   : TestBench.c
 * Description : This project runing at VC 6.0,if you want to run in linux
 * 				 ,some function need revise, such as windows.h.
 * Version	   : 1.1
 * History     : yanbo 2013-03-22  Create
 *************************************************************************/
#include <stdio.h>
#include "GetTsPacketLength.h"
#include "GetAllAppointPidPacket.h"
#include "GetFullTableFromPID.h"
#include "PAT_Analysis.h"
#include "PMT_Analysis.h"
#include "SDT_Analysis.h"
#include "GetAllAnalysisResult.h"
#include "PrintTerminal.h"

#define SOURCE_TS_FILE "C:\\TsStream\\188.ts"
#define RESULT_TS_FILE "C:\\Documents and Settings\\Administrator\\桌面\\%s.ts"

static int FunctionUnion(FILE *pfFile, int *piPacketLength, int *piStartPosition, PROGRAM_INFO *pstProgramInfo);
static int SaveFunction(FILE *pfFile, PROGRAM_INFO *pstProgramInfo, int iEnjoyPID, int iPacketLength, int iStartPosition);
/*************************************************************************
 * Function Name : Main
 * Description   : allocate and transport the function in the analysis Ts.
 * Parameters    :argc -- The C program standard income parameter of number.
 *                argv -- The C program standard income parameter.
 * Returns       : >0 - Success, -1 - Failure Process.
 **************************************************************************/
int main(int argc, char **argv)
{
	FILE * pfFile = NULL;
	int iPacketLength = 0;
	int iStartPosition = 0;	
	int iEnjoyPID = 1;
	int iReturnValue = 0;
	PROGRAM_INFO astProgramInfo[256] = {0};

	pfFile = fopen(SOURCE_TS_FILE, "rb");
	if (NULL == pfFile)
	{
		printf("Open File Error (main)!\n");
		return -1;
	}

	if (-1 == FunctionUnion(pfFile, &iPacketLength, &iStartPosition, astProgramInfo))
	{
		printf("FunctionUnion Error (main)!\n");
		iReturnValue = -1;
	}

	while ((0 != iEnjoyPID) && (0 == iReturnValue))
	{
		if (-1 == PrintTerminal(astProgramInfo, &iEnjoyPID))
		{
			printf("PrintTerminal Error (main)!\n");
			iReturnValue = -1;
			break;
		}

		if (0 == iEnjoyPID)
		{
			break;
		}

		if (-1 == SaveFunction(pfFile, astProgramInfo, iEnjoyPID, iPacketLength, iStartPosition))
		{
			printf("SaveFunction Error (main)!\n");
			iReturnValue = -1;
			break;
		}
	}

	fclose(pfFile);

	return iReturnValue;
}
/*************************************************************************
 * Function Name : FunctionUnion
 * Description   : Get the analysis result.
 * Parameters    :pfFile -- The source file.
 *                piPacketLength -- packet length.
 *				  piStartPosition -- first 0x47 start position.
 *				  pstProgramInfo -- the program info.
 * Returns       : >0 - Success Length Value, -1 - Failure code.
 **************************************************************************/
static int FunctionUnion(FILE *pfFile, int *piPacketLength, int *piStartPosition, PROGRAM_INFO *pstProgramInfo)
{
	PAT_PROGRAM astPatInfo[256] = {0};
	int iPatNum = 0;
	PMT_INFO astPmtInfo[256] = {0};
	int iPmtNum = 0;
	SDT_INFO astSdtInfo[256] = {0};
	int iSdtNum = 0;

	if ((NULL == pfFile) || (NULL == pstProgramInfo))
	{
		printf("Incoming Parameters Error (FunctionUnion)!\n");
		return -1;
	}

	*piPacketLength = GetTsPacketLength(pfFile, piStartPosition);
	if (-1 == *piPacketLength)
	{
		printf("GetTsPacketLength Error (FunctionUnion)!\n");
		return -1;
	}

	if (-1 == ParseThePAT(pfFile, *piPacketLength, *piStartPosition, astPatInfo, &iPatNum))
	{
		printf("ParseThePAT Error (FunctionUnion)!\n");
		return -1;
	}

	if (-1 == ParseThePMT(pfFile, *piPacketLength, *piStartPosition, astPmtInfo, &iPmtNum, astPatInfo, iPatNum))
	{
		printf("ParseThePMT Error (FunctionUnion)!\n");
		return -1;
	}

	if (-1 == ParseTheSDT(pfFile, *piPacketLength, *piStartPosition, astSdtInfo, &iSdtNum))
	{
		printf("ParseTheSDT Error (FunctionUnion)!\n");
		return -1;
	}

	if (-1 == GetAllAnalysisResult(astPmtInfo, iPmtNum, astSdtInfo, iSdtNum, pstProgramInfo))
	{
		printf("GetAllAnalysisResult Error (FunctionUnion)!\n");
		return -1;
	}

	return 0;
}
/*************************************************************************
 * Function Name : SaveFunction
 * Description   : save as file.
 * Parameters    :pfFile -- The source file.
 *                piPacketLength -- packet length.
 *				  piStartPosition -- first 0x47 start position.
 *				  pstProgramInfo -- the program info.
 *				  iEnjoyPID -- the enjoy pid.
 * Returns       : >0 - Success Length Value, -1 - Failure code.
 **************************************************************************/
static int SaveFunction(FILE *pfFile, PROGRAM_INFO *pstProgramInfo, int iEnjoyPID, int iPacketLength, int iStartPosition)
{
	FILE * pfNewFile = NULL;
	int iIndex = 0;
	char acFileName[256] = "0";
	char cNewName[256] = "\0";
	int iReturnValue = 0;

	for (iIndex = 0; (0 != pstProgramInfo[iIndex].service_id); iIndex++)
	{
		if (iEnjoyPID == (int)pstProgramInfo[iIndex].service_id)
		{
			RevisedIllegalCharacter(pstProgramInfo, cNewName, iIndex);
			sprintf(acFileName, RESULT_TS_FILE, cNewName);
			pfNewFile = fopen(acFileName, "wb");
			if (NULL == pfNewFile)
			{
				printf("Open NewFile Error (SaveFunction)!\n");
				return -1;
			}

			if (-1 == JudgePidAndSave(pfFile, pfNewFile, iPacketLength, iStartPosition, pstProgramInfo[iIndex].auiAvPID))
			{
				printf("JudgePidAndSave Error (SaveFunction)!\n");
				iReturnValue =  -1;
			}

			fclose(pfNewFile);
			break;
		}
	}
	PrintTerminalEnd(pstProgramInfo[iIndex].service_name);

	return iReturnValue;
}
