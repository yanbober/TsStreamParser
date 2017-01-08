/*************************************************************************
 *                      All Rights yanbo Reserved                        *
 *************************************************************************
 *************************************************************************
 * Filename	     : SDT_Analysis.h
 * Description   : This file is used to declare related functions, cooperate
 * 				   with the SDT_Analysis.c file.
 * Version	     : 1.0
 * History       : 2013-4-11  Created by yanbo
 *************************************************************************/
#ifndef SDT_ANALYSIS_H
#define SDT_ANALYSIS_H

/*------------------------------------------------------------------------
   This descriptor struct defined in EN-300468,Page of 68 have descript
   it. The descriptor_tag table in page 30.
-------------------------------------------------------------------------*/
typedef struct
{
	unsigned char descriptor_tag :8;
	unsigned char descriptor_length :8;
	unsigned char service_type :8;
	unsigned char service_provider_name_length :8;
	char service_provider_name[256];
	unsigned char service_name_length :8;
	char service_name[256];
} SERVICE_DESCRIPTOR;
/*------------------------------------------------------------------------
  This two struct defined in EN-300468,Page of 24 to 25 have descript it.
-------------------------------------------------------------------------*/
typedef struct
{
	unsigned short service_id :16;
	unsigned char EIT_schedule_flag :1;
	unsigned char EIT_present_following_flag :1;
	unsigned char running_status :3;
	unsigned char free_CA_mode :1;
	unsigned short descriptors_loop_length :12;
	SERVICE_DESCRIPTOR stServiceDescriptor;
} SDT_INFO;

typedef struct
{
	unsigned char table_id :8;
	unsigned char section_syntax_indicator :1;
	unsigned short section_length :12;
	unsigned short transport_stream_id :16;
	unsigned char version_number :5;
	unsigned char current_next_indicator :1;
	unsigned char section_number :8;
	unsigned char last_section_number :8;
	unsigned short original_network_id :16;
} SDT_SECTION;

int ParseTheSDT(FILE *pfFile, int iPacketLength, int iStartPosition, SDT_INFO *pstSDT_Info, int *piSdtInfoNum);

#endif
