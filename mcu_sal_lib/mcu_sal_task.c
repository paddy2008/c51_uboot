#include "mcu_sal.h"

#ifdef MCU_TASK_MODULE_ENABLED

static TASK_LIST_T *task_list = NULL;

/*
 * 如果要增加优先级功能，那么必须先定义好优先级的列表，然后根据添加任务优先级的，
 * 插入到相应的位置,这么做的优势是增加新的任务的时候，不需要做优先级的更改和变动，
 * 而且方便调用，缺点是不允许有相同优先级的任务，并且当添加的任务个数只有一个的时候，
 * 也必须遍历整个优先级任务列表
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
 * 这个函数的作用是优先响应中断相关的任务，当发生某种中断的时候，
 * 会在中断处理函数中调用该函数，这样当当前的任务结束以后，检测有
 * 需要快速调用的任务，会优先响应这个任务，需要注意的是这里并没有
 * 直接使用函数函数名作为参数，而是虚拟化为ID，这样做的目的是为了
 * 规避编译器的告警，如果使用函数名作为该函数的参与，那么参与类型
 * 就是函数指针，而这个函数通常是在中断处理函数中调用，所以编译器
 * 会报告中断处理函数和主程序都调用了同一个函数的告警信息
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
