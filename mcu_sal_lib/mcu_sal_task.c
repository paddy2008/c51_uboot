#include "mcu_sal.h"

#ifdef MCU_TASK_MODULE_ENABLED

static TASK_LIST_T *task_list = NULL;

/*
 * ���Ҫ�������ȼ����ܣ���ô�����ȶ�������ȼ����б�Ȼ���������������ȼ��ģ�
 * ���뵽��Ӧ��λ��,��ô���������������µ������ʱ�򣬲���Ҫ�����ȼ��ĸ��ĺͱ䶯��
 * ���ҷ�����ã�ȱ���ǲ���������ͬ���ȼ������񣬲��ҵ���ӵ��������ֻ��һ����ʱ��
 * Ҳ��������������ȼ������б�
 */
void task_add(TASK_STRUCT_T *task)
{
	ASSERT(task);
	ASSERT(task_list);

	if(task->pri < TASK_NUM)
	{
		if((task_list->tasks[task->pri] == NULL) && (task_list->task_num < TASK_NUM))
		{
			task_list->tasks[task->pri] = task;
		}
		else
		{
			lib_log("\r\n task_add failed1");
			return;
		}
	}
	else
	{
		lib_log("\r\n task_add failed2");
		return;
	}

	task_list->task_num ++;
}


void task_list_init(TASK_LIST_T *list)
{
	uint8_t i = 0;

	ASSERT(list);

	task_list = list;

	for(i = 0; i < TASK_NUM; i++)
	{
		task_list->tasks[i] 		  = NULL;
		task_list->fast_task.id[i]    = TASK_ID_INVALID;
	}

	task_list->task_num       = 0;
	task_list->fast_task.num  = 0;

}

/*
 * ���������������������Ӧ�ж���ص����񣬵�����ĳ���жϵ�ʱ��
 * �����жϴ������е��øú�������������ǰ����������Ժ󣬼����
 * ��Ҫ���ٵ��õ����񣬻�������Ӧ���������Ҫע��������ﲢû��
 * ֱ��ʹ�ú�����������Ϊ�������������⻯ΪID����������Ŀ����Ϊ��
 * ��ܱ������ĸ澯�����ʹ�ú�������Ϊ�ú����Ĳ��룬��ô��������
 * ���Ǻ���ָ�룬���������ͨ�������жϴ������е��ã����Ա�����
 * �ᱨ���жϴ������������򶼵�����ͬһ�������ĸ澯��Ϣ
 *
 * */
void task_fast_handle(uint8_t id)
{
	uint8_t i = 0;

	ASSERT(task_list);

	if(task_list->task_num == 0)  //do nothing when no task in task list
	{
		return;
	}

	for(i = 0; i < task_list->fast_task.num; i++)
	{
		if(task_list->fast_task.id[i] == id)  //if current task id is already exist, do nothing
		{
			return;
		}
	}

	//if current id is not exist
	if((task_list->fast_task.num < task_list->task_num) && (task_list->fast_task.id[task_list->fast_task.num] == TASK_ID_INVALID))
	{
		//here there is not juduge id and gTaskInstant[id] validity,so they are judged in using
		ENTER_CRITICAL();
		task_list->fast_task.id[task_list->fast_task.num] = id;
		task_list->fast_task.num ++;
		EXIT_CRITICAL();
	}
}


static uint8_t _task_find(uint8_t id)
{
	uint8_t i = 0;

	RE_ASSERT(task_list);

	for(i = 0; i < TASK_NUM; i++)
	{
		if((task_list->tasks[i] != NULL) && (task_list->tasks[i]->id == id))
		{
			return task_list->tasks[i]->pri;
		}
	}

	return TASK_NUM;
}

void task_run()
{
	uint8_t i = 0, j = 0, index = 0;

	ASSERT(task_list);

	for(i = 0; i < TASK_NUM; i++)
	{
		for(j = 0; j < task_list->fast_task.num; j++)
		{
			index = _task_find(task_list->fast_task.id[j]);

			if((index < TASK_NUM) && (task_list->tasks[index] != NULL))
			{
				task_list->tasks[index]->func();
			}

			ENTER_CRITICAL();  //because fast_task.num and fast_task.id is updated by multiple interrupt handle function, it must disable global interrupt
			task_list->fast_task.num --;
			task_list->fast_task.id[j] = TASK_ID_INVALID;
			EXIT_CRITICAL();
		}

		if(task_list->tasks[i] != NULL)  //i is actually pri, get task id by pri,and get task pointer by id
		{
			task_list->tasks[i]->func();
		}
	}
}

#endif
