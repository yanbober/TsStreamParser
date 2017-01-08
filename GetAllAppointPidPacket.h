/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : GetAllAppointPidPacket.h
 * Description   : This file is used to declare related functions, cooperate
 * 				   with the GetAllAppointPidPacket.c file.
 * Version	     : 1.0
 * History       : 2013-3-22  Created by yanbo
 *************************************************************************/
#ifndef GET_ALL_APPOINT_PID_PACKET
#define GET_ALL_APPOINT_PID_PACKET
/*-----------------------------------------------------------------
 This struct definition depends on ISO/IEC-138181.pdf page on 32.
 The Variable named as the official,not fit Goosat standard.
 -------------------------------------------------------------------*/
typedef struct
{
	unsigned char sync_byte :8;
	unsigned char transport_error_indicator :1;
	unsigned char payload_unit_start_indicator :1;
	unsigned char transport_priority :1;
	unsigned short PID :13;
	unsigned char transport_scrambling_control :2;
	unsigned char adaptation_field_control :2;
	unsigned char continuity_counter :4;
} TS_PACKET_HEADER;

void AdjustTsPacketHeader(TS_PACKET_HEADER * pstTsHeader, unsigned char * pucPacketBuffer);
int JudgePidAndSave(FILE * pfFilePoint, FILE * pfNewFile, int iPacketLength, int iStartPosition, unsigned int *puiInputPID);

#endif
