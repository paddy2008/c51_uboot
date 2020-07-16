/*
 * mcu_sal_terminal����Ҫ����:
 * (1)�ն˿����Ӧ���ڴ���ͨ�ţ�USBͨ�ţ�����ͨ�ŵ�
 * (2)��Ҫ�����ն����ݣ��������ݵ���ʾ������ƶ��������ַ���
 * (3)�ṩע�������API
 * (4)������ʷ����
 * */
#include "mcu_sal.h"

#ifdef MCU_TERMINAL_MODULE_ENABLED

static  TERMINAL_STRUCT_T *_terminal_struct = NULL;

static void _terminal_del_string(void);
static void _terminal_run_cmd(char *str);
static void _terminal_read_history_cmd(void);

static uint8_t _terminal_show_cmd_help(PARAM_INFO_T *param);

void terminal_init(TERMINAL_STRUCT_T *terminal)
{
	ASSERT(terminal);

	_terminal_struct = terminal;

	memset((void *)_terminal_struct, 0, sizeof(TERMINAL_STRUCT_T));
	_terminal_struct->lock   = 1;
	_terminal_struct->read 	= _terminal_struct->buff;
	_terminal_struct->write	= _terminal_struct->buff;
	_terminal_struct->prompt =  PROMPT;

	terminal_register_cmd("help" ,_terminal_show_cmd_help);
}

static void _terminal_deinit(void)
{
	ASSERT(_terminal_struct);
	memset((void *)_terminal_struct->buff, 0, TERMINAL_BUFF_LEN + 1);
	_terminal_struct->read 	= _terminal_struct->buff;
	_terminal_struct->write	= _terminal_struct->buff;
	_terminal_struct->cursor = 0;
	_terminal_struct->len	= 0;
	_terminal_struct->skip	= false;
	memset((void *)&_terminal_struct->cli.param, 0, sizeof(PARAM_INFO_T));
}

static void _terminal_write_char(const char ch)
{
	ASSERT(_terminal_struct);
	*(_terminal_struct->write) = ch;
	_terminal_struct->write ++;
	task_fast_handle(TASK_ID_TERMINAL_HANDLE);
}

/*
 * 1.TERMINAL_BUFF�Ĵ�С��100 + 1���ַ������һ���ռ����ڱ����ַ���������'\0'
 * 2.Ϊ�˱��������ַ�����100���ַ������������������⴦��ֻ����ǰ97���ɻ����ַ���ʣ��3
 * ���ַ���������ַ�����'\b','\r','\n'�ͷ��������Ϊ���������ϼ�������3���ַ�,����ʵ�ʿɻ��Ե��ַ���97��
 * ������97���ַ��Ժ󣬿����ƶ���꣬�����ܼ��������µ��ַ�����Ȼɾ��һ���ַ��Ժ��ֿ�������һ���µ��ַ�
 */
static void _terminal_recv_char(const char recv)
{
	static bool key_flag = false;

	ASSERT(_terminal_struct);
	if(_terminal_struct->write < _terminal_struct->buff + TERMINAL_BUFF_LEN - 3)  //ֻ����ǰ97���ַ��������3���ַ�ֻ���ǿ����ַ���'\b','\r','\n'�ͷ��������Ϊ���������ϼ�������3���ַ�
	{
		if((*_terminal_struct->write == '\0') && _terminal_struct->lock) //if current position is empty, �����ǰ���Ǵ򿪵ģ����ո��ַ�������ֱ�Ӷ���
		{
			_terminal_write_char(recv);
		}
	}
	else  //������3���ַ�
	{
		if(*_terminal_struct->write == '\0')
		{
			if(((recv == '\b') || (recv == '\r') || (recv == '\n')) &&  _terminal_struct->lock)
			{
				_terminal_write_char(recv);
			}
			else if(recv == 0x1b)
			{
				key_flag = true;
			}

			if(key_flag && _terminal_struct->lock)
			{
				_terminal_write_char(recv);
			}

			if(_terminal_struct->write == _terminal_struct->buff + TERMINAL_BUFF_LEN) //���buff�Ѿ�д��
			{
				key_flag = false;
			}
		}
	}
}

void terminal_recv_chars(char *buff, uint32_t len)
{
	uint32_t i = 0;

	for(i = 0; i < len; i++)
	{
		_terminal_recv_char(buff[i]);
	}
}

/*
 * ���ַ�������������һ����ʱ��������Ϊ�������һ����ϼ���
 * �������ַ���϶��ɣ�������һ���ַ��Ժ���ʱ�򲢲�������
 * ��ȡ�ڶ����ַ�����Ϊmcu�ڲ���Ƶ����ԶԶ����uart��ͨ�����ʵ�
 */
static char _terminal_read_char(uint16_t time)
{
	char ch = 0;

	RE_ASSERT(_terminal_struct);

	if(_terminal_struct->write == _terminal_struct->buff)  //indicate there are no datas
	{
		return ch;
	}

	while(time-- != 0)
	{
		if(_terminal_struct->read < _terminal_struct->write) //��Ϊ�Ƿ�ѭ��buff,����readָ��һ����С�ڵ���writeָ��
		{
			if(*_terminal_struct->read != '\0')  //if not equal NUL(���ַ�)
			{
				ch = *_terminal_struct->read;
				_terminal_struct->read ++;
				return ch;
			}
			else
			{
				ch = 0;
			}
		}
	}
	return ch;
}

static void _terminal_putchar(const char ch)
{
	lib_putchar(ch);
}

/*
 * ��Ϊ�ڳ�ʼ���Ĺ����в���ȫ���ж�δ��֮ǰ���п�����Ҫ��ӡ��Ϣ
 * ������SDA��SCL�ĵ�ƽ״̬�ȵȣ����������������Ϣ֮ǰ���Ƚ�
 * ȫ���жϴ�
 */
static void _terminal_put_string(const char *str)
{
	lib_put_string(str);
}

/*
 * ��ʽ���ַ�����Ŀ��:
 * ɾ����Щ�����ַ���ֻ����������ϵĿɻ����ַ�
 */
static void _terminal_format_buff(void)
{
	uint8_t i = 0, j = 0;
	uint16_t len = 0;
	char buff[TERMINAL_BUFF_LEN] = {'\0'};

	ASSERT(_terminal_struct);

	len = strlen(_terminal_struct->buff);

	for(i = 0; _terminal_struct->buff[i] != '\0'; i++)
	{
		if(_terminal_struct->buff[i] == 0x1b && _terminal_struct->buff[i + 1] == 0x5b)  //indicate direction key pressed
		{
			i += 3; //skip 3 chars

		}

		if(CHECK_CHAR_VALID(_terminal_struct->buff[i]))
		{
			buff[j++] = _terminal_struct->buff[i];
		}
	}

	if(len > 0)
	{
		COM_LOCK_SEM(_terminal_struct->lock);
		memcpy(_terminal_struct->buff, buff, TERMINAL_BUFF_LEN);
		_terminal_struct->write  = _terminal_struct->buff + j;  //update write pointer
		COM_UNLOCK_SEM(_terminal_struct->lock);
		_terminal_struct->read   = _terminal_struct->buff + j;  //update read pointer
	}
}

/*
 *ע��һЩ�����ַ��Ĵ�����Щ�����ַ�������Ҫ��������
 */
static void _terminal_handle_direction_key(uint8_t ch)
{
	switch(ch)
	{
		case 'A': //up direction key, ������ַ�'ESC +�� + A',ASCIIΪ0x1b + 0x5b + 0x41
		{
			_terminal_read_history_cmd();
			break;
		}
		case 'B': //down direction key��������ַ�'ESC +�� + B',ASCIIΪ0x1b + 0x5b + 0x42
		{
			_terminal_read_history_cmd();
			break;
		}
		case 'C': //right direciton key��������ַ�'ESC +�� + C',ASCIIΪ0x1b + 0x5b + 0x43
		{
			_terminal_format_buff();
			if(_terminal_struct->cursor < _terminal_struct->len)
			{
				_terminal_putchar(0x1b);
				_terminal_putchar(0x5b);
				_terminal_putchar('C');   //����������þ��ǹ������
				_terminal_struct->cursor ++;
			}
			break;
		}
		case 'D': //left direciton key��������ַ�'ESC +�� + D',ASCIIΪ0x1b + 0x5b + 0x44
		{
			_terminal_format_buff();
			if(_terminal_struct->cursor > 0)
			{
				_terminal_putchar(0x1b);
				_terminal_putchar(0x5b);
				_terminal_putchar('D'); //����������þ��ǹ������
				_terminal_struct->cursor--;
			}
			break;
		}
		default :
			break;
	}
}

static void _terminal_cursor_left_move(uint32_t len)
{
	uint32_t i = 0;

	ASSERT(_terminal_struct);

	for(i = len; i > 0 && (_terminal_struct->cursor > 0); i--)
	{
		_terminal_putchar('\b');
		_terminal_struct->cursor --;
	}
}

/*
 * �����ַ��Ĵ�������:
 * 1.�����ж������ַ�����β�����м�
 * 2.������ַ�����β����ô�����ף�ֱ������ͺ�
 * 3.������ַ����м䣬�����������£����赱ǰ�ַ�����12345,Ҫ��23ֱ�Ӳ����ַ�a����ô�����23֮��
 * 4.��ô��ʾ��Ϊ12a345��ʵ��buff�е�����Ϊ12345a����Ϊ����a��ʱ�����Ƚ��ղ���ŵ���buff��
 * 5.�ȿ���������������ַ�������345��ŵ�buff
 * 6.Ȼ���²�����ַ����뵽��굱ǰλ��
 * 7.�ٽ�buff�����������λ�ã���ɶ�ԭbuf�ĸ��£�������������ʾ�Ĵ����ˣ�ע�������ַ������ַ����Ժ󣬹��λ�øı���Ҫ��ʱ����_uart_struct.cursor��ֵ
 */
static void _terminal_insert_char(char ch, uint8_t pos)
{
	char buff[TERMINAL_BUFF_LEN] = {'\0'};
	char *cur_pos = NULL;

	ASSERT(_terminal_struct);

	cur_pos = _terminal_struct->buff + pos;

	if(ch == SKIP_CHAR && pos == 0x0)
	{
		_terminal_struct->skip = true;
	}

	if(!CHECK_CHAR_VALID(ch) || _terminal_struct->skip)
	{
		return;
	}

	if(pos < _terminal_struct->len)  //indicate the cursor at middle
	{
		_terminal_format_buff();

		/*����ԭbuff*/
		strncpy(buff, cur_pos, _terminal_struct->len - pos);  	//�����������ַ���
		*cur_pos = ch;
		cur_pos ++;    																//��1��Ŀ����Ϊ�˸��²����ַ�Ԥ���ռ�
		memcpy(cur_pos, buff, strlen(buff) + 1);  			//����ԭbuff������strlen(buff) + 1���ַ���Ŀ����Ϊ�������ַ���ĩβ��'\0'�ַ�,ʣ�೤��ΪUART0_BUFF_LEN - pos - 1

		/*��ʾ����*/
		_terminal_putchar(ch);
		_terminal_put_string(buff);
		_terminal_struct->cursor = strlen(_terminal_struct->buff);    //���¹��λ��
		_terminal_cursor_left_move(strlen(buff)); 				//�ָ���굽�����ַ���λ��
	}
	else  //indicate the cursor at end
	{
		_terminal_putchar(ch);
		_terminal_struct->cursor ++;
	}

	_terminal_struct->len ++;
}


/*
 * ɾ���ַ��Ĵ�������
 * ������ַ��Ĵ���˼·һ������ɾ��ָ���ַ���Ȼ���ַ���ƴ��
 * 123456789,���赱ǰ���λ��56֮��
 * 1234 6789,ִ�����_terminal_put_string("\b \b");���Ч�������ҵ�ǰ�����4���棬������5����
 * 123467899,ִ�����_terminal_put_string(str);���Ч��,���ҵ�ǰ���������9֮��
 * 12346789, ִ�����_terminal_put_string(" \b");���Ч��,��ǰ�����9����
 */
static void _terminal_delete_char(uint16_t pos)
{
	char  buff[TERMINAL_BUFF_LEN] = {'\0'};
	char  *cur_pos = NULL;

	ASSERT(_terminal_struct);

	cur_pos =  _terminal_struct->buff + pos;

	if(pos > 0)
	{
		_terminal_format_buff();
		/*����ԭbuff*/
		strcpy(buff, cur_pos);  	//�����������ַ���
		cur_pos --;  				//��1��Ŀ����Ϊ�˸��Ǳ�ɾ�����Ǹ��ַ�
		COM_LOCK_SEM(_terminal_struct->lock);
		memcpy(cur_pos, buff, strlen(buff) + 1);  	//����ԭbuff������strlen(buff) + 1���ַ���Ŀ����Ϊ�������ַ���ĩβ��'\0'�ַ�,ʣ�೤��ΪUART0_BUFF_LEN - pos + 1
		_terminal_struct->write --;	//��Ϊɾ����һ���ַ�������writeָ����Ҫ��1
		COM_UNLOCK_SEM(_terminal_struct->lock);

		_terminal_struct->read -- ;  //��Ϊɾ����һ���ַ�������readָ����Ҫ��1
		_terminal_struct->len = strlen(_terminal_struct->buff);

		/*��ʾ����*/
		_terminal_put_string("\b \b");   //��ӡ'\b',��ʾ���ǰ��һ���ַ���Ȼ���ӡ�ո�ɾ��������ַ����ٴ�ӡ'\b', ����ٴ�ǰ��һ���ַ����ۺ�Ч������ɾ���ַ��󣬹��λ�ò���
		_terminal_put_string(buff);      //��������ַ�ͬ��ǰ�ƣ�����Ѿ��Ƶ����ַ����ĵ����ڶ�λ
		_terminal_put_string(" \b");	  //������ַ����������
		_terminal_struct->cursor = _terminal_struct->len;
		_terminal_cursor_left_move(strlen(buff));  //�����ָ���ɾ���ַ���λ��
	}
}

static void _terminal_del_string(void)
{
	uint16_t i = 0;

	_terminal_format_buff();

	ASSERT(_terminal_struct);

	//������ƶ����ַ����������
	for(i = 0; _terminal_struct->cursor < _terminal_struct->len; i ++)
	{
		_terminal_putchar(0x1b);
		_terminal_putchar(0x5b);
		_terminal_putchar('C');   //����������þ��ǹ������
		_terminal_struct->cursor ++;
	}

	for(i = 0; i < _terminal_struct->len; i++)
	{
		_terminal_put_string("\b \b");   //��ӡ'\b',��ʾ���ǰ��һ���ַ���Ȼ���ӡ�ո�ɾ��������ַ����ٴ�ӡ'\b', ����ٴ�ǰ��һ���ַ����ۺ�Ч������ɾ���ַ��󣬹��λ�ò���
	}

	_terminal_deinit();
}

/*
 * UART�Ĵ�������
 * 1.UARTֻ����������ݣ��������ݱ��浽UART_BUFF,���������������ȫ˫������ߴ��ڵ���Ӧ�ٶȣ��������һ���ַ���ʾһ���ַ��������ٶȻ����ܶ�
 * 2.CMD_BUFF�������Ǵ�UART_BUFF�ж�ȡ�ģ���ʵ�ǿ��Թ��õģ���������Ͳ�ȡ���õĲ��ԣ���ԼRAM
 * 3.���ÿ����һ���ַ�����������UART_BUFF��ȡ��CMD_BUFF,��ôɾ���ַ���ʱ���������ݶ�Ҫɾ��������Ӧ�õ���������Ժ󣬲Ž����ݸ��µ�CMD_BUFF
 * 4.д�ַ��Ͷ��ַ�����ͬ����Ŀ�ľ���Ϊ�˻���
 * 5.����һ����ѭ��UART_BUFF,��д����ʱ�򣬽�дָ������ָ��UART_BUFF����ʼλ�ã���Ϊ�Ƿ�ѭ��buff������readָ����ԶС�ڵ���writeָ��
 * */
void terminal_event_handle(void)
{
	char ch = 0;

	ASSERT(_terminal_struct);

	if ((ch = _terminal_read_char(1)) != 0)
	{
		if(ch == '\n' || ch == '\r') //���»س����߻��з�,��ʾ�������
		{
			//action handle in here
			_terminal_format_buff();
			_terminal_run_cmd(_terminal_struct->buff);
			_terminal_deinit();
			_terminal_put_string(_terminal_struct->prompt);
		}
		else if(ch == '\b')
		{
			_terminal_delete_char(_terminal_struct->cursor);
		}
		else if(ch == 0x1b && _terminal_read_char(0xffff) == 0x5b) //�����������ַ�'ESC +�� + A',ASCIIΪ0x1b + 0x5b + 0x41
		{
			_terminal_handle_direction_key(_terminal_read_char(0xffff));
		}
		else //����ʣ���������ַ�����0x21 ~ 0x7E
		{
			_terminal_insert_char(ch, _terminal_struct->cursor);
		}
	}
}

/***********************************************TERMINAL CLI HANDLE******************************************/


/*
 * ���ﲻֱ����strtok��������Ϊ�⺯��strtok���޸�ԭ�ַ�����
 * ����ÿ�β�����Ҫ����һ���ַ���,̫Ӱ��Ч�ʣ�
 */
static void _terminal_cut_cmd(char *src, char delim)
{
	char *head = src, *tail = src;

	ASSERT(_terminal_struct);

	while(*tail != '\0')
	{
		if(*tail == delim)
		{
			strncpy(_terminal_struct->cli.param.argv[_terminal_struct->cli.param.argc ++], head, tail - head);
			head = tail + 1;
		}
		tail++;
	}

	//becasue the input cmd string end with '\0', but the last paramter should also be saved
	strncpy(_terminal_struct->cli.param.argv[_terminal_struct->cli.param.argc ++], head, tail - head);

	//for(i = 0; i < _terminal_struct->cli.param.argc; i++)
	//{
	//	printf("--%u--%s", i, _terminal_struct->cli.param.argv[i]);
	//}

}

/*
static bool _terminal_find_history_cmd(uint8_t *str)
{
	uint8_t i = 0;

	if((str == NULL) || (*str == '\0'))
	{
		return false;
	}

	for (i = 0; i < _terminal_struct->cli.history.write; i++)
	{
		if(!strcmp(_terminal_struct->cli.history.buff[i], str))
		{
			return true;
		}
	}

	return false;
}
*/

static void _terminal_save_history_cmd(char *str)
{
	ASSERT(_terminal_struct);
	/*
	if(_terminal_find_history_cmd(str))
	{
		return;
	}
	else
	*/
	{
		memset(_terminal_struct->cli.history.buff[_terminal_struct->cli.history.write], 0, TERMINAL_BUFF_LEN);
		strcpy(_terminal_struct->cli.history.buff[_terminal_struct->cli.history.write],str);
		_terminal_struct->cli.history.write ++;
		_terminal_struct->cli.history.read = _terminal_struct->cli.history.write;

		if(_terminal_struct->cli.history.write >= CLI_HISTORY_CMD_NUM)
		{
			_terminal_struct->cli.history.full = true;
			_terminal_struct->cli.history.write = 0;
		}
	}
}

/*
 * ��Ϊע������ĸ�ʽ�Ǳ������ַ�'&'��ͷ�������������ַ�'#'��ͷ,read|write����set|get������ϵ�һ���Ӵ�,����Ӵ�֮�����ַ�'|'�ָ���
 * ����:
 * [opr|opw] &baseAddr &OffAddr #ParmNum(0~0xff),ʵ����������������opr 0xa0 0x0,opw 0x16 0x2 0x10��
 * [get|set] rx_los mode #[0|1],ʵ����������������get rx_los mode,set rx_los mode 0
 *
 */
static uint8_t _terminal_special_char_match(char *str)
{
	char *p = str;

	if((str == NULL) || (*str == '\0'))
	{
		return TYPE_NONE;
	}

	for(; *p != '\0'; p++)
	{
		if(*p == '&')
		{
			return TYPE_VARIABLE_STR;
		}
		else if(*p == '#')
		{
			return TYPE_CONSTANT_PARAM_STR;
		}
		else if((*p == '[') || (*p == ']') || (*p == '|'))
		{
			return TYPE_MULTI_PARAMS_STR;
		}
	}

	return TYPE_NONE;
}


/*
 * ���ﲻ���ǽ���Ҫ�и�������ַ������浽����һ
 * ��buff��Ϊ�˽�Լ����ռ�,��Լջ�ռ�
 * ����������ַ��������ַ�'['��ͷ,���ַ�']'
 * ��β��,�����ַ�����βû�����ַ�'\0'����,
 * ÿ������ͨ���ַ�'|'�ָ���
 */
static bool  _terminal_str_sect_match(char delim, char *src, char *target)
{
	char *head = src + 1, *tail = src + 1;  //src + 1��Ŀ���������ַ�����һ���ַ�'['

	while(*tail != ']')
	{
		if(*tail == delim)
		{
			if(!strncmp(head, target, tail - head))  //if match success
			{
				return true;
			}
			head = tail + 1;
		}
		tail++;
	}

	//becasue the input string end with ']', but the last paramter should also be compared
	if(!strncmp(head, target, tail - head))  //if match success
	{
		return true;
	}
	return false;
}

/*
 * �����������Ҫ����ע���������Ϣƥ�䣬����˼������:
 * 1.���������Ƚϼ�࣬��һ���ո��ַ���ÿ���ַ����ηֿ���������������ַ����Ժ��ȸ�ʽ��������ֶα�����_terminal_struct->cli.param.argv��
 * 2.����ע����ַ�����Ȼ��ʼ"�ֽ�"ע����ַ���,һ�߷ֽ�,һ�߱Ƚ�,����������ٶȻ���ӿ�,�����"�ֽ�"��,��ô��ƥ����ַ���Ҳ�ᱻ"�ֽ�"����Ȼ�������ù�
 * 3.���Կո��ַ��ֽ⣬�����ƥ�䣬�ж��Ƿ��Ǳ�ʾ����������ַ�������������ַ�&���ַ�#��ͷ���ַ�����ֱ��������ƥ��
 * 4.����������ַ�&���ַ�#��ͷ���ַ������ж��ǲ��Ƕ����������϶��ɵ��ַ����Σ�����ǣ�����ͨ��API _terminal_str_sect_match�ֽ⣬�ж��Ƿ�ƥ��
 * 5.ʵ�����������������ܶ���ע����ַ���������opw 0xa0 0x0 0x5 0x1 0x2 0x3 0x4 0x5,����ƥ���ʱ��ֻ��Ҫ��ע���ַ���ƥ�����û�в�ƥ���
 * item,�ͱ�ʾƥ��ɹ�
 */
static bool _terminal_string_match(const char *src)
{
	char  *str_sect = NULL, buff[TERMINAL_BUFF_LEN] = {0};
	uint8_t i = 0;

	if((src == NULL) || (*src == '\0'))
	{
		return false;
	}

	strcpy(buff, src);

	str_sect = strtok(buff," ");

	while(str_sect)
	{
		if(!strcmp(str_sect, _terminal_struct->cli.param.argv[i]))  //if match success
		{
			str_sect = strtok(NULL," ");  //��ȡ��һ���ַ�����
			i ++;
		}
		else
		{
			uint8_t type = _terminal_special_char_match(str_sect);

			if(type == TYPE_MULTI_PARAMS_STR)
			{
				if(strstr(str_sect,_terminal_struct->cli.param.argv[i]))  //if in multi params string include target str , indicate match success
				{
					if(_terminal_str_sect_match('|', str_sect, _terminal_struct->cli.param.argv[i]))
					{
						str_sect = strtok(NULL," ");
						i ++;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else if((type == TYPE_VARIABLE_STR) || (type == TYPE_CONSTANT_PARAM_STR)) //����������ַ�&����#��ʾƥ��ɹ�
			{
				str_sect = strtok(NULL," ");
				i ++;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

static REGISTER_CMD_T* _terminal_match_cmd()
{
	REGISTER_CMD_T *command = (REGISTER_CMD_T *)&_terminal_struct->cli.cmd.command[0];
	uint8_t i = 0;

	for(i = 0; i < _terminal_struct->cli.cmd.num; i++)
	{
		if(_terminal_string_match(command->name))
		{
			return command;
		}
		command ++;
	}

	return NULL;
}

static void _terminal_run_cmd(char *str)
{
	RETURN_VALUE_TYPE ret = RET_OK;
	REGISTER_CMD_T *command = NULL;

	if((str == NULL) || (*str == '\0'))
	{
		return;
	}

	ASSERT(_terminal_struct);

	_terminal_save_history_cmd(str);
	_terminal_cut_cmd(str,' ');

	command = _terminal_match_cmd();

	if(command != NULL)
	{
		ret = command->func(&_terminal_struct->cli.param);
		if(RET_OK != ret)
		{
			if(RET_NO_PERMISSION == ret)
			{
				_terminal_put_string("\r\nPlease write passward '0x8f 0x9e 0xad 0xec' at first");
			}

			_terminal_put_string("\r\n---ATECMDRESULT--- FAIL");
		}
		else
		{
			_terminal_put_string("\r\n---ATECMDRESULT--- OK");
		}
	}
	else
	{
		_terminal_put_string("\r\nCommand not recognised.  Enter 'help' to view a list of available commands.");
	}

}

static uint8_t _terminal_show_cmd_help(PARAM_INFO_T *param)
{
	REGISTER_CMD_T *command = (REGISTER_CMD_T *)&_terminal_struct->cli.cmd.command[0];
	uint8_t i = 0;

	param = NULL;

	for(i = 0; i < _terminal_struct->cli.cmd.num; i++)
	{
		_terminal_put_string("\r\n        ");
		_terminal_put_string(command->name);
		command ++;
	}
	return RET_OK;
}

void  terminal_register_cmd(const char *name, REG_FUN func)   //ע���������
{
	if((name == NULL) || (func == NULL))
	{
		return;
	}

	ASSERT(_terminal_struct);

	if(_terminal_struct->cli.cmd.num < CLI_CMD_NUM)
	{
		_terminal_struct->cli.cmd.command[_terminal_struct->cli.cmd.num].name   = name;
		_terminal_struct->cli.cmd.command[_terminal_struct->cli.cmd.num].func	  = func;
		_terminal_struct->cli.cmd.num ++;
	}
}

static void _terminal_read_history_cmd(void)
{
	ASSERT(_terminal_struct);

	if(_terminal_struct->cli.history.read > 0)
	{
		_terminal_del_string();
		_terminal_struct->cli.history.read --;
		_terminal_put_string(_terminal_struct->cli.history.buff[_terminal_struct->cli.history.read]);

		COM_LOCK_SEM(_terminal_struct->lock);
		memcpy(_terminal_struct->buff,_terminal_struct->cli.history.buff[_terminal_struct->cli.history.read],strlen(_terminal_struct->cli.history.buff[_terminal_struct->cli.history.read]));
		_terminal_struct->write  = _terminal_struct->buff + strlen(_terminal_struct->buff);  //update write pointer
		COM_UNLOCK_SEM(_terminal_struct->lock);

		_terminal_struct->read   = _terminal_struct->buff + strlen(_terminal_struct->buff);  //update read pointer
		_terminal_struct->len    = strlen(_terminal_struct->buff);
		_terminal_struct->cursor = strlen(_terminal_struct->buff);

		if(_terminal_struct->cli.history.read == 0x0)
		{
			if( _terminal_struct->cli.history.full)
			{
				_terminal_struct->cli.history.read = CLI_HISTORY_CMD_NUM;
			}
			else
			{
				_terminal_struct->cli.history.read = _terminal_struct->cli.history.write;
			}
		}
	}
}

#endif
