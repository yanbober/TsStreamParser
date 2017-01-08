/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : PrintTerminal.c
 * Description   : print info at the screen.
 * Version	     : 1.0
 * History       : 2013-4-17  Created by yanbo
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "PAT_Analysis.h"
#include "PMT_Analysis.h"
#include "SDT_Analysis.h"
#include "GetAllAnalysisResult.h"
/*************************************************************************
 * Function Name : PrintTerminal
 * Description   : The display logic in the terminal.
 * Parameters    : puiLovePID -- the input service id.
 *                 pstProgramInfo -- the program info.
 * Returns       : >0 - Success Length Value, -1 - Failure code.
 **************************************************************************/
int PrintTerminal(PROGRAM_INFO *pstProgramInfo, unsigned int *puiLovePID)
{
	int iIndex = 0;

	system("color 1F");
	system("cls");
	printf("*******************当前TS流节目信息***************************\n");
	printf(" 节目ID\t节目类型\tTV/Radio\t节目名称\n");
	for (iIndex = 0; (0 != pstProgramInfo[iIndex].service_id); iIndex++)
	{
		if (0 == pstProgramInfo[iIndex].auiAvPID[0])
		{
			continue;
		}

		printf(" %4.4d\t", pstProgramInfo[iIndex].service_id);
		if (pstProgramInfo[iIndex].free_CA_mode)
		{
			printf("  加密\t\t");
		}
		else
		{
			printf("  免费\t\t");
		}

		if ((0 == pstProgramInfo[iIndex].auiVideoPID[0]) && (0 != pstProgramInfo[iIndex].auiAudioPID[0]))
		{
			printf(" Radio\t\t");
		}
		else if ((0 != pstProgramInfo[iIndex].auiVideoPID[0]) && (0 != pstProgramInfo[iIndex].auiAudioPID[0]))
		{
			printf(" TV\t\t");
		}

		printf("%s\n", pstProgramInfo[iIndex].service_name);
	}
	printf("**************************************************************\n");

	printf("请按节目单信息输入您喜好的节目ID(0:退出):");
	scanf("%d", puiLovePID);

	return 0;
}
/*************************************************************************
 * Function Name : PrintTerminalEnd
 * Description   : The display logic in the terminal.
 * Parameters    : pcName -- the selected file name.
 * Returns       : void.
 **************************************************************************/
void PrintTerminalEnd(char *pcName)
{
	if (0 == strcmp(pcName,"\0"))
	{
		printf("您输入的ID不在当前列表！请重新输入ID！\n\n");
	}
	else
	{
		printf("桌面节目：%s 已保存文件成功！请查看！\n", pcName);
	}
	Sleep(1500);
}
