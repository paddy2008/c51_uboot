/*
 * mcu_sal_terminal的主要功能:
 * (1)终端库可以应用于窗口通信，USB通信，网口通信等
 * (2)主要处理终端数据，包括数据的显示，光标移动，增减字符等
 * (3)提供注册命令的API
 * (4)保留历史命令
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
 * 1.TERMINAL_BUFF的大小是100 + 1个字符，最后一个空间用于保存字符串结束符'\0'
 * 2.为了避免输入字符超过100个字符，函数里面增加特殊处理，只保存前97个可回显字符，剩下3
 * 个字符保存控制字符，如'\b','\r','\n'和方向键，因为方向键是组合键，包含3个字符,所以实际可回显的字符是97个
 * 即输入97个字符以后，可以移动光标，但不能继续输入新的字符，当然删除一个字符以后，又可以输入一个新的字符
 */
static void _terminal_recv_char(const char recv)
{
	static bool key_flag = false;

	ASSERT(_terminal_struct);
	if(_terminal_struct->write < _terminal_struct->buff + TERMINAL_BUFF_LEN - 3)  //只保存前97个字符，最后面3个字符只能是控制字符如'\b','\r','\n'和方向键，因为方向键是组合键，包含3个字符
	{
		if((*_terminal_struct->write == '\0') && _terminal_struct->lock) //if current position is empty, 如果当前锁是打开的，接收该字符，否则直接丢弃
		{
			_terminal_write_char(recv);
		}
	}
	else  //最后面的3个字符
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

			if(_terminal_struct->write == _terminal_struct->buff + TERMINAL_BUFF_LEN) //如果buff已经写满
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
 * 读字符函数必须增加一个延时参数，因为像方向键是一个组合键，
 * 是三个字符组合而成，读到第一个字符以后，有时候并不能里面
 * 读取第二个字符，因为mcu内部的频率是远远高于uart的通信速率的
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
		if(_terminal_struct->read < _terminal_struct->write) //因为是非循环buff,所以read指针一定是小于等于write指针
		{
			if(*_terminal_struct->read != '\0')  //if not equal NUL(空字符)
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
 * 因为在初始化的过程中并且全局中断未打开之前，有可能需要打印信息
 * 例如检测SDA和SCL的电平状态等等，所以这里在输出信息之前，先将
 * 全局中断打开
 */
static void _terminal_put_string(const char *str)
{
	lib_put_string(str);
}

/*
 * 格式化字符串的目的:
 * 删除那些控制字符，只包含命令组合的可回显字符
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
 *注意一些特殊字符的处理，这些特殊字符并不需要保存起来
 */
static void _terminal_handle_direction_key(uint8_t ch)
{
	switch(ch)
	{
		case 'A': //up direction key, 是组合字符'ESC +【 + A',ASCII为0x1b + 0x5b + 0x41
		{
			_terminal_read_history_cmd();
			break;
		}
		case 'B': //down direction key，是组合字符'ESC +【 + B',ASCII为0x1b + 0x5b + 0x42
		{
			_terminal_read_history_cmd();
			break;
		}
		case 'C': //right direciton key，是组合字符'ESC +【 + C',ASCII为0x1b + 0x5b + 0x43
		{
			_terminal_format_buff();
			if(_terminal_struct->cursor < _terminal_struct->len)
			{
				_terminal_putchar(0x1b);
				_terminal_putchar(0x5b);
				_terminal_putchar('C');   //这三句的作用就是光标右移
				_terminal_struct->cursor ++;
			}
			break;
		}
		case 'D': //left direciton key，是组合字符'ESC +【 + D',ASCII为0x1b + 0x5b + 0x44
		{
			_terminal_format_buff();
			if(_terminal_struct->cursor > 0)
			{
				_terminal_putchar(0x1b);
				_terminal_putchar(0x5b);
				_terminal_putchar('D'); //这三句的作用就是光标左移
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
 * 插入字符的处理流程:
 * 1.首先判断是在字符串结尾还是中间
 * 2.如果是字符串结尾，那么很容易，直接输出就好
 * 3.如果是字符串中间，处理流程如下，假设当前字符串是12345,要在23直接插入字符a，那么光标在23之间
 * 4.那么显示变为12a345，实际buff中的数据为12345a，因为输入a的时候首先接收并存放到了buff中
 * 5.先拷贝保留光标后面的字符串，即345存放到buff
 * 6.然后将新插入的字符插入到光标当前位置
 * 7.再将buff拷贝到光标后的位置，完成对原buf的更新，接下来就是显示的处理了，注意输入字符或者字符串以后，光标位置改变了要及时更新_uart_struct.cursor的值
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

		/*更新原buff*/
		strncpy(buff, cur_pos, _terminal_struct->len - pos);  	//保存光标后面的字符串
		*cur_pos = ch;
		cur_pos ++;    																//加1的目的是为了给新插入字符预留空间
		memcpy(cur_pos, buff, strlen(buff) + 1);  			//更新原buff，拷贝strlen(buff) + 1个字符的目的是为了增加字符串末尾的'\0'字符,剩余长度为UART0_BUFF_LEN - pos - 1

		/*显示处理*/
		_terminal_putchar(ch);
		_terminal_put_string(buff);
		_terminal_struct->cursor = strlen(_terminal_struct->buff);    //更新光标位置
		_terminal_cursor_left_move(strlen(buff)); 				//恢复光标到输入字符的位置
	}
	else  //indicate the cursor at end
	{
		_terminal_putchar(ch);
		_terminal_struct->cursor ++;
	}

	_terminal_struct->len ++;
}


/*
 * 删除字符的处理流程
 * 与插入字符的处理思路一样，先删除指定字符，然后字符串拼接
 * 123456789,假设当前光标位于56之间
 * 1234 6789,执行语句_terminal_put_string("\b \b");后的效果，并且当前光标在4后面，而不是5后面
 * 123467899,执行语句_terminal_put_string(str);后的效果,并且当前光标在两个9之间
 * 12346789, 执行语句_terminal_put_string(" \b");后的效果,当前光标在9后面
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
		/*更新原buff*/
		strcpy(buff, cur_pos);  	//保存光标后面的字符串
		cur_pos --;  				//减1的目的是为了覆盖被删除的那个字符
		COM_LOCK_SEM(_terminal_struct->lock);
		memcpy(cur_pos, buff, strlen(buff) + 1);  	//更新原buff，拷贝strlen(buff) + 1个字符的目的是为了增加字符串末尾的'\0'字符,剩余长度为UART0_BUFF_LEN - pos + 1
		_terminal_struct->write --;	//因为删除了一个字符，所以write指针需要减1
		COM_UNLOCK_SEM(_terminal_struct->lock);

		_terminal_struct->read -- ;  //因为删除了一个字符，所以read指针需要减1
		_terminal_struct->len = strlen(_terminal_struct->buff);

		/*显示处理*/
		_terminal_put_string("\b \b");   //打印'\b',表示光标前移一个字符，然后打印空格，删除光标后的字符，再打印'\b', 光标再次前移一个字符，综合效果就是删除字符后，光标位置不变
		_terminal_put_string(buff);      //光标后面的字符同步前移，光标已经移到了字符串的倒数第二位
		_terminal_put_string(" \b");	  //光标在字符串的最后面
		_terminal_struct->cursor = _terminal_struct->len;
		_terminal_cursor_left_move(strlen(buff));  //将光标恢复到删除字符的位置
	}
}

static void _terminal_del_string(void)
{
	uint16_t i = 0;

	_terminal_format_buff();

	ASSERT(_terminal_struct);

	//将光标移动到字符串的最后面
	for(i = 0; _terminal_struct->cursor < _terminal_struct->len; i ++)
	{
		_terminal_putchar(0x1b);
		_terminal_putchar(0x5b);
		_terminal_putchar('C');   //这三句的作用就是光标右移
		_terminal_struct->cursor ++;
	}

	for(i = 0; i < _terminal_struct->len; i++)
	{
		_terminal_put_string("\b \b");   //打印'\b',表示光标前移一个字符，然后打印空格，删除光标后的字符，再打印'\b', 光标再次前移一个字符，综合效果就是删除字符后，光标位置不变
	}

	_terminal_deinit();
}

/*
 * UART的处理流程
 * 1.UART只负责接收数据，并将数据保存到UART_BUFF,这样处理的优势是全双工，提高串口的响应速度，如果接收一个字符显示一个字符，处理速度会慢很多
 * 2.CMD_BUFF的数据是从UART_BUFF中读取的，其实是可以共用的，我们这里就采取共用的策略，节约RAM
 * 3.如果每输入一个字符，就立即从UART_BUFF读取到CMD_BUFF,那么删除字符的时候，两份数据都要删除，所以应该等输入结束以后，才将数据更新到CMD_BUFF
 * 4.写字符和读字符基本同步，目的就是为了回显
 * 5.定义一个非循环UART_BUFF,当写满的时候，将写指针重新指向UART_BUFF的起始位置，因为是非循环buff，所以read指针永远小于等于write指针
 * */
void terminal_event_handle(void)
{
	char ch = 0;

	ASSERT(_terminal_struct);

	if ((ch = _terminal_read_char(1)) != 0)
	{
		if(ch == '\n' || ch == '\r') //按下回车或者换行符,表示输入结束
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
		else if(ch == 0x1b && _terminal_read_char(0xffff) == 0x5b) //方向键是组合字符'ESC +【 + A',ASCII为0x1b + 0x5b + 0x41
		{
			_terminal_handle_direction_key(_terminal_read_char(0xffff));
		}
		else //处理剩余其他的字符，即0x21 ~ 0x7E
		{
			_terminal_insert_char(ch, _terminal_struct->cursor);
		}
	}
}

/***********************************************TERMINAL CLI HANDLE******************************************/


/*
 * 这里不直接用strtok函数是因为库函数strtok会修改原字符串，
 * 所以每次操作都要备份一份字符串,太影响效率，
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
 * 因为注册命令的格式是变量以字符'&'开头，常量参数以字符'#'开头,read|write或者set|get命令组合到一个子串,这个子串之间用字符'|'分隔开
 * 例如:
 * [opr|opw] &baseAddr &OffAddr #ParmNum(0~0xff),实际输入的命令可能是opr 0xa0 0x0,opw 0x16 0x2 0x10等
 * [get|set] rx_los mode #[0|1],实际输入的命令可能是get rx_los mode,set rx_los mode 0
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
 * 这里不考虑将需要切割出来的字符串保存到另外一
 * 个buff是为了节约代码空间,节约栈空间
 * 传入进来的字符串是以字符'['开头,以字符']'
 * 结尾的,并且字符串结尾没有以字符'\0'结束,
 * 每个参数通过字符'|'分隔开
 */
static bool  _terminal_str_sect_match(char delim, char *src, char *target)
{
	char *head = src + 1, *tail = src + 1;  //src + 1的目的是跳过字符串第一个字符'['

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
 * 输入的命令需要根据注册的命令信息匹配，基本思想如下:
 * 1.输入的命令比较简洁，以一个空格字符将每个字符串段分开，当接收输入的字符串以后，先格式化，将其分段保存在_terminal_struct->cli.param.argv中
 * 2.遍历注册的字符串，然后开始"分解"注册的字符串,一边分解,一边比较,这样处理的速度会更加快,如果先"分解"好,那么不匹配的字符串也会被"分解"，显然这是无用功
 * 3.先以空格字符分解，如果不匹配，判断是否是表示特殊参数的字符串，如果是以字符&和字符#开头的字符串，直接跳过不匹配
 * 4.如果不是以字符&和字符#开头的字符串，判断是不是多命令参数组合而成的字符串段，如果是，继续通过API _terminal_str_sect_match分解，判断是否匹配
 * 5.实际输入的命令参数可能多于注册的字符串，例如opw 0xa0 0x0 0x5 0x1 0x2 0x3 0x4 0x5,所以匹配的时候，只需要将注册字符串匹配完成没有不匹配的
 * item,就表示匹配成功
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
			str_sect = strtok(NULL," ");  //获取下一个字符串段
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
			else if((type == TYPE_VARIABLE_STR) || (type == TYPE_CONSTANT_PARAM_STR)) //如果有特殊字符&或者#表示匹配成功
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

void  terminal_register_cmd(const char *name, REG_FUN func)   //注册命令处理函数
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
