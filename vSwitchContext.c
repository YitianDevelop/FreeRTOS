void vTaskSwitchContext( void )
{
  if( uxSchedulerSuspended != ( UBaseType_t ) pdFALSE )
    {
        xYieldPending = pdTRUE; // 标记切换放弃 ， 具体用还不清楚
    }
  else
    {
        xYieldPending = pdFALSE;
        taskSELECT_HIGHEST_PRIORITY_TASK(); // 选择优先级最高的任务。
    }
}

