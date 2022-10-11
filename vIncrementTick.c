BaseType_t xTaskIncrementTick( void )
{
    TCB_t * pxTCB;
    TickType_t xItemValue;
    BaseType_t xSwitchRequired = pdFALSE;

    if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
    {
        const TickType_t xConstTickCount = xTickCount + ( TickType_t ) 1;
        xTickCount = xConstTickCount;

        if( xConstTickCount == ( TickType_t ) 0U )
        {
            taskSWITCH_DELAYED_LISTS();
        }
        if( xConstTickCount >= xNextTaskUnblockTime )// 具有可唤醒任务
        {
            for( ; ; )
            {
                if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
                {
                    xNextTaskUnblockTime = portMAX_DELAY;
                    break;
                }
                else
                {
                    pxTCB = listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
                    /**
                     *  下面这行代码很反人类，很不符合人的直觉，当然大概率是我不懂它的设计思路啦
                     *  他的目的是查出最早可以唤醒的任务，这就涉及到什么时候唤醒的问题。
                     *  在vDelayTask()中有如下代码：
                     *     xTimeToWake = xConstTickCount + xTicksToWait; // 当前tick数量加上延时tick数得到唤醒时刻（更标准说法是tick刻）。
                     *     listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xStateListItem ), xTimeToWake );
                     *     vListInsert( pxDelayedTaskList, &( pxCurrentTCB->xStateListItem ) );  // 前面讨论过，这个函数是排序插入的
                     *  这里他居然把一个唤醒时刻放到了一个ListItem中，为什么要和List扯上关系 ？？？？？？
                     */
                    xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );
                    // 根据上面的分析，这里就是把最早的可唤醒任务取了出来（这里却是对于List的一些函数，很不理解）
                    if( xConstTickCount < xItemValue )// 还没到唤醒时刻
                    {
                        xNextTaskUnblockTime = xItemValue; // 更新最早可唤醒时刻
                        break;// 注意当前是处在循环中的
                    }
                    // 可唤醒
                    listREMOVE_ITEM( &( pxTCB->xStateListItem ) ); // 在这里似乎明白了，为什么这么设计
                    /**
                     *  listINSERT_END( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xStateListItem ) );
                     *  这句代码来自任务创建过程中的靠后代码，值的注意的是，就绪队列的放的不是TCB 。
                     *  第一遍分析没分析出来，这里居然储存的是xStateListItem而不是TCB ！！！！！
                     *  这样想来，似乎明白了为什么这样设计，那如何在就绪队列里面反向查找TCB ？？？
                     */
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL ) // 任务被挂起有多种原因，可能是等待事件
                    { // 为什么这么写因为我目前没有研究Event， 这里没法分析
                        listREMOVE_ITEM( &( pxTCB->xEventListItem ) );
                    }
                    prvAddTaskToReadyList( pxTCB ); // 加入就绪队列，详细见创建任务代码分析
                    #if ( configUSE_PREEMPTION == 1 )
                    {
                        if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
                        {
                            xSwitchRequired = pdTRUE; // 支持抢占式调度的话，标记需要立刻调度，另外这里调度的意义相当于恢复延时结束的任务。
                        }
                    }
                }
                // 关于这里为什么需要循环，同一个tick时刻，多个任务可能会唤醒，直到找到不能唤醒的任务后就退出
            }
        }
        #if ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 1 ) ) // 同优先级时间切片任务轮转
        {
            if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ pxCurrentTCB->uxPriority ] ) ) > ( UBaseType_t ) 1 )
            {
                xSwitchRequired = pdTRUE;
            }
        }
        #endif

        #if ( configUSE_PREEMPTION == 1 )
        {
            if( xYieldPending != pdFALSE )
            {
                xSwitchRequired = pdTRUE;
            }
        }
        #endif
    }
    else
    {
        ++xPendedTicks; // 标记有几个tick未能处理，这个有什么用途暂时未弄清楚
    }
    return xSwitchRequired;
    }