#ifndef __MCU_SAL_TERMINAL_H__
#define __MCU_SAL_TERMINAL_H__

#include "mcu_sal_def.h"

#ifdef MCU_TERMINAL_MODULE_ENABLED

typedef struct
{
	uint8_t 	 			argc;	 					//�����������
	char 	 			    argv[CLI_STR_SECTION_LEN][CLI_STR_SECTION_LEN];
}PARAM_INFO_T;

typedef  uint8_t (*REG_FUN)(PARAM_INFO_T *param);

typedef struct
{
	const char      *name;
	REG_FUN    		func;
}REGISTER_CMD_T;

typedef struct
{
	REGISTER_CMD_T command[CLI_CMD_NUM];
	uint8_t 	   num;
}REGISTER_CMD_STRUCT_T;

typedef struct
{
	uint8_t  write;
	uint8_t  read;
	char     buff[CLI_HISTORY_CMD_NUM][TERMINAL_BUFF_LEN];
	uint8_t	 full : 1;
}HISTORY_CMD_STRUCT_T;

typedef struct
{
	PARAM_INFO_T			param;
	HISTORY_CMD_STRUCT_T	history;
	REGISTER_CMD_STRUCT_T	cmd;
}CLI_STRUCT_T;

typedef struct
{
	 char 			 	 buff[TERMINAL_BUFF_LEN + 1];
	 char     			*read;     //read pointer
	 char     			*prompt;   //��ʾ�ַ���
	 volatile char  	*write;	   //write pointer, because busy is used by UART0 interrupt handle function and main loop,declare variable by "volatile" keywords
	 volatile uint8_t    lock : 1;	   //дָ���buff���µı�����
	 uint8_t  	 		 skip : 1;	   //���ڹ��˵���SKIP_CHAR�궨���ַ���ͷ���ַ���������ע��
	 uint16_t	     	 cursor;   //���λ��
	 uint16_t      	     len;	   //echo char len
	 CLI_STRUCT_T 	     cli;
}TERMINAL_STRUCT_T;

typedef enum
{
	TYPE_MULTI_PARAMS_STR 		  = 0,   //�����ʾ����������ַ���������[opr|opw]
	TYPE_VARIABLE_STR	  		  = 1,   //�����ʾ�������ַ���������opr &baseAddr &devAddr
	TYPE_CONSTANT_PARAM_STR		  = 2,   //�����ʾ�������ַ���������#[0|1]
	TYPE_NONE			  		  = 3,
}CMD_PARAM_TYPE_T;

extern void 		  terminal_init(TERMINAL_STRUCT_T *terminal);
extern void 		  terminal_recv_chars(char *buff, uint32_t len);
extern void 		  terminal_event_handle(void);
extern void  	 	  terminal_register_cmd(const char *name,REG_FUN func);

#endif

#endif

