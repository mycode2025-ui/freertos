/*
 * FreeRTOS Kernel V11.3.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "stack_macros.h"

/* The default definitions are only available for non-MPU ports. The
 * reason is that the stack alignment requirements vary for different
 * architectures.*/

/* The MPU ports require MPU_WRAPPERS_INCLUDED_FROM_API_FILE to be defined
 * for the header files above, but not in this file, in order to generate the
 * correct privileged Vs unprivileged linkage and placement. */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Set 0 to 2 to include the stats formatting
 * functions but without including stdio.h here. */



/* This macro requests the running task pxTCB to yield. In single core
 * scheduler, a running task always runs on core 0 and portYIELD_WITHIN_API()
 * can be used to request the task running on core 0 to yield. Therefore, pxTCB
 * is not used in this macro. */
        #define taskYIELD_TASK_CORE_IF_USING_PREEMPTION( pxTCB ) \
    do {                                                         \
        ( void ) ( pxTCB );                                      \
        portYIELD_WITHIN_API();                                  \
    } while( 0 )

        #define taskYIELD_ANY_CORE_IF_USING_PREEMPTION( pxTCB ) \
    do {                                                        \
        if( pxCurrentTCB->uxPriority < ( pxTCB )->uxPriority )  \
        {                                                       \
            portYIELD_WITHIN_API();                             \
        }                                                       \
        else                                                    \
        {                                                       \
            mtCOVERAGE_TEST_MARKER();                           \
        }                                                       \
    } while( 0 )



/* Values that can be assigned to the ucNotifyState member of the TCB. */
#define taskNOT_WAITING_NOTIFICATION              ( ( uint8_t ) 0 ) /* Must be zero as it is the initialised value. */
#define taskWAITING_NOTIFICATION                  ( ( uint8_t ) 1 )
#define taskNOTIFICATION_RECEIVED                 ( ( uint8_t ) 2 )

/*
 * The value used to fill the stack of a task when the task is created.  This
 * is used purely for checking the high water mark for tasks.
 */
#define tskSTACK_FILL_BYTE                        ( 0xa5U )

/* Bits used to record how a task's stack and TCB were allocated. */
#define tskDYNAMICALLY_ALLOCATED_STACK_AND_TCB    ( ( uint8_t ) 0 )
#define tskSTATICALLY_ALLOCATED_STACK_ONLY        ( ( uint8_t ) 1 )
#define tskSTATICALLY_ALLOCATED_STACK_AND_TCB     ( ( uint8_t ) 2 )

/* If any of the following are set then task stacks are filled with a known
 * value so the high water mark can be determined.  If none of the following are
 * set then don't fill the stack so there is no unnecessary dependency on memset. */
    #define tskSET_NEW_STACKS_TO_KNOWN_VALUE    0

/*
 * Macros used by vListTask to indicate which state a task is in.
 */
#define tskRUNNING_CHAR      ( 'X' )
#define tskBLOCKED_CHAR      ( 'B' )
#define tskREADY_CHAR        ( 'R' )
#define tskDELETED_CHAR      ( 'D' )
#define tskSUSPENDED_CHAR    ( 'S' )

/*
 * Some kernel aware debuggers require the data the debugger needs access to be
 * global, rather than file scope.
 */

/* The name allocated to the Idle task.  This can be overridden by defining
 * "IDLE" in the fixed profile. */
    
/* Reserve space for Core ID and null termination. */
    /* Reserve space for null termination. */
    #define taskRESERVED_TASK_NAME_LENGTH    1U


/* If 1 is 1 then task selection is
 * performed in a way that is tailored to the particular microcontroller
 * architecture being used. */

/* A port optimised version is provided.  Call the port defined macros. */
    #define taskRECORD_READY_PRIORITY( uxPriority )    portRECORD_READY_PRIORITY( ( uxPriority ), uxTopReadyPriority )

/*-----------------------------------------------------------*/

    #define taskSELECT_HIGHEST_PRIORITY_TASK()                                                  \
    do {                                                                                        \
        UBaseType_t uxTopPriority;                                                              \
                                                                                                \
        /* Find the highest priority list that contains ready tasks. */                         \
        portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );                          \
        kernelASSERT( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ uxTopPriority ] ) ) > 0 ); \
        listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );   \
    } while( 0 )

/*-----------------------------------------------------------*/

/* A port optimised version is provided, call it only if the TCB being reset
 * is being referenced from a ready list.  If it is referenced from a delayed
 * or suspended list then it won't be in a ready list. */
    #define taskRESET_READY_PRIORITY( uxPriority )                                                     \
    do {                                                                                               \
        if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ] ) ) == ( UBaseType_t ) 0 ) \
        {                                                                                              \
            portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );                        \
        }                                                                                              \
    } while( 0 )


/*-----------------------------------------------------------*/

/* pxDelayedTaskList and pxOverflowDelayedTaskList are switched when the tick
 * count overflows. */
#define taskSWITCH_DELAYED_LISTS()                                                \
    do {                                                                          \
        List_t * pxTemp;                                                          \
                                                                                  \
        /* The delayed tasks list should be empty when the lists are switched. */ \
        kernelASSERT( ( listLIST_IS_EMPTY( pxDelayedTaskList ) ) );               \
                                                                                  \
        pxTemp = pxDelayedTaskList;                                               \
        pxDelayedTaskList = pxOverflowDelayedTaskList;                            \
        pxOverflowDelayedTaskList = pxTemp;                                       \
        xNumOfOverflows = ( BaseType_t ) ( xNumOfOverflows + 1 );                 \
        prvResetNextTaskUnblockTime();                                            \
    } while( 0 )

/*-----------------------------------------------------------*/

/*
 * Place the task represented by pxTCB into the appropriate ready list for
 * the task.  It is inserted at the end of the list.
 */
#define prvAddTaskToReadyList( pxTCB )                                                                     \
    do {                                                                                                   \
        traceMOVED_TASK_TO_READY_STATE( pxTCB );                                                           \
        taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );                                                \
        listINSERT_END( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xStateListItem ) ); \
        tracePOST_MOVED_TASK_TO_READY_STATE( pxTCB );                                                      \
    } while( 0 )
/*-----------------------------------------------------------*/

/*
 * Several functions take a TaskHandle_t parameter that can optionally be NULL,
 * where NULL is used to indicate that the handle of the currently executing
 * task should be used in place of the parameter.  This macro simply checks to
 * see if the parameter is NULL and returns a pointer to the appropriate TCB.
 */
#define prvGetTCBFromHandle( pxHandle )    ( ( ( pxHandle ) == NULL ) ? pxCurrentTCB : ( pxHandle ) )

/* The item value of the event list item is normally used to hold the priority
 * of the task to which it belongs (coded to allow it to be held in reverse
 * priority order).  However, it is occasionally borrowed for other purposes.  It
 * is important its value is not updated due to a task priority change while it is
 * being used for another purpose.  The following bit definition is used to inform
 * the scheduler that the value should not be changed - in which case it is the
 * responsibility of whichever module is using the value to ensure it gets set back
 * to its original value when it is released. */
    #define taskEVENT_LIST_ITEM_VALUE_IN_USE    ( ( uint32_t ) 0x80000000U )

/* Indicates that the task is not actively running on any core. */
#define taskTASK_NOT_RUNNING           ( ( BaseType_t ) ( -1 ) )

/* Indicates that the task is actively running but scheduled to yield. */
#define taskTASK_SCHEDULED_TO_YIELD    ( ( BaseType_t ) ( -2 ) )

/* Returns pdTRUE if the task is actively running and not scheduled to yield. */
    #define taskTASK_IS_RUNNING( pxTCB )                          ( ( ( pxTCB ) == pxCurrentTCB ) ? ( pdTRUE ) : ( pdFALSE ) )
    #define taskTASK_IS_RUNNING_OR_SCHEDULED_TO_YIELD( pxTCB )    ( ( ( pxTCB ) == pxCurrentTCB ) ? ( pdTRUE ) : ( pdFALSE ) )

/* Indicates that the task is an Idle task. */
#define taskATTRIBUTE_IS_IDLE    ( UBaseType_t ) ( 1U << 0U )


#define taskBITS_PER_BYTE    ( ( size_t ) 8 )

/*-----------------------------------------------------------*/

/*
 * Task control block.  A task control block (TCB) is allocated for each task,
 * and stores task state information, including a pointer to the task's context
 * (the task's run time environment, including register values)
 */
typedef struct tskTaskControlBlock       /* The old naming convention is used to prevent breaking kernel aware debuggers. */
{
    volatile StackType_t * pxTopOfStack; /**< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */



    ListItem_t xStateListItem;                  /**< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
    ListItem_t xEventListItem;                  /**< Used to reference a task from an event list. */
    UBaseType_t uxPriority;                     /**< The priority of the task.  0 is the lowest priority. */
    StackType_t * pxStack;                      /**< Points to the start of the stack. */
    char pcTaskName[ 16 ]; /**< Descriptive name given to the task when created.  Facilitates debugging only. */





        UBaseType_t uxBasePriority; /**< The priority last assigned to the task - used by the priority inheritance mechanism. */
        UBaseType_t uxMutexesHeld;





        volatile uint32_t ulNotifiedValue[ 1 ];
        volatile uint8_t ucNotifyState[ 1 ];

    /* See the comments in FreeRTOS.h with the definition of
     * tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */


} tskTCB;

/* The old tskTCB name is maintained above then typedefed to the new TCB_t name
 * below to enable the use of older kernel aware debuggers. */
typedef tskTCB TCB_t;

    /* MISRA Ref 8.4.1 [Declaration shall be visible] */
    /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-84 */
    /* coverity[misra_c_2012_rule_8_4_violation] */
    portDONT_DISCARD PRIVILEGED_DATA TCB_t * volatile pxCurrentTCB = NULL;

/* Lists for ready and blocked tasks. --------------------
 * xDelayedTaskList1 and xDelayedTaskList2 could be moved to function scope but
 * doing so breaks some kernel aware debuggers and debuggers that rely on removing
 * the static qualifier. */
PRIVILEGED_DATA static List_t pxReadyTasksLists[ 8 ]; /**< Prioritised ready tasks. */
PRIVILEGED_DATA static List_t xDelayedTaskList1;                         /**< Delayed tasks. */
PRIVILEGED_DATA static List_t xDelayedTaskList2;                         /**< Delayed tasks (two lists are used - one for delays that have overflowed the current tick count. */
PRIVILEGED_DATA static List_t * volatile pxDelayedTaskList;              /**< Points to the delayed task list currently being used. */
PRIVILEGED_DATA static List_t * volatile pxOverflowDelayedTaskList;      /**< Points to the delayed task list currently being used to hold tasks that have overflowed the current tick count. */
PRIVILEGED_DATA static List_t xPendingReadyList;                         /**< Tasks that have been readied while the scheduler was suspended.  They will be moved to the ready list when the scheduler is resumed. */



/* Global POSIX errno. Its value is changed upon context switching to match
 * the errno of the currently running task. */

/* Other file private variables. --------------------------------*/
PRIVILEGED_DATA static volatile UBaseType_t uxCurrentNumberOfTasks = ( UBaseType_t ) 0U;
PRIVILEGED_DATA static volatile TickType_t xTickCount = ( TickType_t ) 0;
PRIVILEGED_DATA static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;
PRIVILEGED_DATA static volatile BaseType_t xSchedulerRunning = pdFALSE;
PRIVILEGED_DATA static volatile TickType_t xPendedTicks = ( TickType_t ) 0U;
PRIVILEGED_DATA static volatile BaseType_t xYieldPendings[ 1 ] = { pdFALSE };
PRIVILEGED_DATA static volatile BaseType_t xNumOfOverflows = ( BaseType_t ) 0;
PRIVILEGED_DATA static UBaseType_t uxTaskNumber = ( UBaseType_t ) 0U;
PRIVILEGED_DATA static volatile TickType_t xNextTaskUnblockTime = ( TickType_t ) 0U; /* Initialised to portMAX_DELAY before the scheduler starts. */
PRIVILEGED_DATA static TaskHandle_t xIdleTaskHandles[ 1 ];       /**< Holds the handles of the idle tasks.  The idle tasks are created automatically when the scheduler is started. */

/* Improve support for OpenOCD. The kernel tracks Ready tasks via priority lists.
 * For tracking the state of remote threads, OpenOCD uses uxTopUsedPriority
 * to determine the number of priority lists to read back from the remote target. */
static const volatile UBaseType_t uxTopUsedPriority = 8 - 1U;

/* Context switches are held pending while the scheduler is suspended.  Also,
 * interrupts must not manipulate the xStateListItem of a TCB, or any of the
 * lists the xStateListItem can be referenced from, if the scheduler is suspended.
 * If an interrupt needs to unblock a task while the scheduler is suspended then it
 * moves the task's event list item into the xPendingReadyList, ready for the
 * kernel to move the task from the pending ready list into the real ready list
 * when the scheduler is unsuspended.  The pending ready list itself can only be
 * accessed from a critical section.
 *
 * Updates to uxSchedulerSuspended must be protected by both the task lock and the ISR lock
 * and must not be done from an ISR. Reads must be protected by either lock and may be done
 * from either an ISR or a task. */
PRIVILEGED_DATA static volatile UBaseType_t uxSchedulerSuspended = ( UBaseType_t ) 0U;


/*-----------------------------------------------------------*/

/* File private functions. --------------------------------*/

/*
 * Creates the idle tasks during scheduler start.
 */
static BaseType_t prvCreateIdleTasks( void );




/**
 * Utility task that simply returns pdTRUE if the task referenced by xTask is
 * currently in the Suspended state, or pdFALSE if the task referenced by xTask
 * is in any other state.
 */

/*
 * Utility to ready all the lists used by the scheduler.  This is called
 * automatically upon the creation of the first task.
 */
static void prvInitialiseTaskLists( void ) PRIVILEGED_FUNCTION;

static size_t prvTaskCheckFreeStackSpace( const uint8_t * pucStackByte ) PRIVILEGED_FUNCTION;

/*
 * The idle task, which as all tasks is implemented as a never ending loop.
 * The idle task is automatically created and added to the ready lists upon
 * creation of the first user task.
 *
 * In the FreeRTOS SMP, 1 - 1 passive idle tasks are also
 * created to ensure that each core has an idle task to run when no other
 * task is available to run.
 *
 * The portTASK_FUNCTION_PROTO() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for these functions are:
 *
 * void prvIdleTask( void *pvParameters );
 * void prvPassiveIdleTask( void *pvParameters );
 *
 */
static portTASK_FUNCTION_PROTO( prvIdleTask, pvParameters ) PRIVILEGED_FUNCTION;

/*
 * Utility to free all memory allocated by the scheduler to hold a TCB,
 * including the stack pointed to by the TCB.
 *
 * This does not free memory allocated by the task itself (i.e. memory
 * allocated by calls to pvPortMalloc from within the tasks application code).
 */

/*
 * Used only by the idle task.  This checks to see if anything has been placed
 * in the list of tasks waiting to be deleted.  If so the task is cleaned up
 * and its TCB deleted.
 */
static void prvCheckTasksWaitingTermination( void ) PRIVILEGED_FUNCTION;

/*
 * The currently executing task is entering the Blocked state.  Add the task to
 * either the current or the overflow delayed task list.
 */
static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait,
                                            const BaseType_t xCanBlockIndefinitely ) PRIVILEGED_FUNCTION;

/*
 * Fills an TaskStatus_t structure with information on each task that is
 * referenced from the pxList list (which may be a ready list, a delayed list,
 * a suspended list, etc.).
 *
 * THIS FUNCTION IS INTENDED FOR DEBUGGING ONLY, AND SHOULD NOT BE CALLED FROM
 * NORMAL APPLICATION CODE.
 */

/*
 * Searches pxList for a task with name pcNameToQuery - returning a handle to
 * the task if it is found, or NULL if the task is not found.
 */

/*
 * When a task is created, the stack of the task is filled with a known value.
 * This function determines the 'high water mark' of the task stack by
 * determining how much of the stack remains at the original preset value.
 */

/*
 * Return the amount of time, in ticks, that will pass before the kernel will
 * next move a task from the Blocked state to the Running state or before the
 * tick count overflows (whichever is earlier).
 *
 * This conditional compilation should use inequality to 0, not equality to 1.
 * This is to ensure portSUPPRESS_TICKS_AND_SLEEP() can be called when user
 * defined low power mode implementations require 0 to be
 * set to a value other than 1.
 */

/*
 * Set xNextTaskUnblockTime to the time at which the next Blocked state task
 * will exit the Blocked state.
 */
static void prvResetNextTaskUnblockTime( void ) PRIVILEGED_FUNCTION;


/*
 * Called after a Task_t structure has been allocated either statically or
 * dynamically to fill in the structure's members.
 */
static void prvInitialiseNewTask( TaskFunction_t pxTaskCode,
                                  const char * const pcName,
                                  const size_t uxStackDepth,
                                  void * const pvParameters,
                                  UBaseType_t uxPriority,
                                  TaskHandle_t * const pxCreatedTask,
                                  TCB_t * pxNewTCB,
                                  const MemoryRegion_t * const xRegions ) PRIVILEGED_FUNCTION;

/*
 * Called after a new task has been created and initialised to place the task
 * under the control of the scheduler.
 */
static void prvAddNewTaskToReadyList( TCB_t * pxNewTCB ) PRIVILEGED_FUNCTION;

/*
 * Create a task with static buffer for both TCB and stack. Returns a handle to
 * the task if it is created successfully. Otherwise, returns NULL.
 */
    static TCB_t * prvCreateStaticTask( TaskFunction_t pxTaskCode,
                                        const char * const pcName,
                                        const size_t uxStackDepth,
                                        void * const pvParameters,
                                        UBaseType_t uxPriority,
                                        StackType_t * const puxStackBuffer,
                                        StaticTask_t * const pxTaskBuffer,
                                        TaskHandle_t * const pxCreatedTask ) PRIVILEGED_FUNCTION;

/*
 * Create a restricted task with static buffer for both TCB and stack. Returns
 * a handle to the task if it is created successfully. Otherwise, returns NULL.
 */

/*
 * Create a restricted task with static buffer for task stack and allocated buffer
 * for TCB. Returns a handle to the task if it is created successfully. Otherwise,
 * returns NULL.
 */

/*
 * Create a task with allocated buffer for both TCB and stack. Returns a handle to
 * the task if it is created successfully. Otherwise, returns NULL.
 */

/*
 * freertos_tasks_c_additions_init() should only be called if the user definable
 * macro FREERTOS_TASKS_C_ADDITIONS_INIT() is defined, as that is the only macro
 * called by the function.
 */


/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/


    static TCB_t * prvCreateStaticTask( TaskFunction_t pxTaskCode,
                                        const char * const pcName,
                                        const size_t uxStackDepth,
                                        void * const pvParameters,
                                        UBaseType_t uxPriority,
                                        StackType_t * const puxStackBuffer,
                                        StaticTask_t * const pxTaskBuffer,
                                        TaskHandle_t * const pxCreatedTask )
    {
        TCB_t * pxNewTCB;

        kernelASSERT( puxStackBuffer != NULL );
        kernelASSERT( pxTaskBuffer != NULL );

        {
            /* Sanity check that the size of the structure used to declare a
             * variable of type StaticTask_t equals the size of the real task
             * structure. */
            volatile size_t xSize = sizeof( StaticTask_t );
            kernelASSERT( xSize == sizeof( TCB_t ) );
            ( void ) xSize; /* Prevent unused variable warning when kernelASSERT() is not used. */
        }

        if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
        {
            /* The memory used for the task's TCB and stack are passed into this
             * function - use them. */
            /* MISRA Ref 11.3.1 [Misaligned access] */
            /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-113 */
            /* coverity[misra_c_2012_rule_11_3_violation] */
            pxNewTCB = ( TCB_t * ) pxTaskBuffer;
            ( void ) memset( ( void * ) pxNewTCB, 0x00, sizeof( TCB_t ) );
            pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;


            prvInitialiseNewTask( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, pxCreatedTask, pxNewTCB, NULL );
        }
        else
        {
            pxNewTCB = NULL;
        }

        return pxNewTCB;
    }
/*-----------------------------------------------------------*/

    TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                    const char * const pcName,
                                    const size_t uxStackDepth,
                                    void * const pvParameters,
                                    UBaseType_t uxPriority,
                                    StackType_t * const puxStackBuffer,
                                    StaticTask_t * const pxTaskBuffer )
    {
        TaskHandle_t xReturn = NULL;
        TCB_t * pxNewTCB;

        traceENTER_xTaskCreateStatic( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );

        pxNewTCB = prvCreateStaticTask( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer, &xReturn );

        if( pxNewTCB != NULL )
        {

            prvAddNewTaskToReadyList( pxNewTCB );
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_xTaskCreateStatic( xReturn );

        return xReturn;
    }
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvInitialiseNewTask( TaskFunction_t pxTaskCode,
                                  const char * const pcName,
                                  const size_t uxStackDepth,
                                  void * const pvParameters,
                                  UBaseType_t uxPriority,
                                  TaskHandle_t * const pxCreatedTask,
                                  TCB_t * pxNewTCB,
                                  const MemoryRegion_t * const xRegions )
{
    StackType_t * pxTopOfStack;
    UBaseType_t x;


    /* Avoid dependency on memset() if it is not required. */

    /* Calculate the top of stack address.  This depends on whether the stack
     * grows from high memory to low (as per the 80x86) or vice versa.
     * portSTACK_GROWTH is used to make the result positive or negative as required
     * by the port. */
    {
        pxTopOfStack = &( pxNewTCB->pxStack[ uxStackDepth - ( size_t ) 1 ] );
        pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );

        /* Check the alignment of the calculated top of stack is correct. */
        kernelASSERT( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack & ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) == 0U ) );

    }

    /* Store the task name in the TCB. */
    if( pcName != NULL )
    {
        for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) 16; x++ )
        {
            pxNewTCB->pcTaskName[ x ] = pcName[ x ];

            /* Don't copy all 16 if the string is shorter than
             * 16 characters just in case the memory after the
             * string is not accessible (extremely unlikely). */
            if( pcName[ x ] == ( char ) 0x00 )
            {
                break;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        /* Ensure the name string is terminated in the case that the string length
         * was greater or equal to 16. */
        pxNewTCB->pcTaskName[ 16 - 1U ] = '\0';
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    /* This is used as an array index so must ensure it's not too large. */
    kernelASSERT( uxPriority < 8 );

    if( uxPriority >= ( UBaseType_t ) 8 )
    {
        uxPriority = ( UBaseType_t ) 8 - ( UBaseType_t ) 1U;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    pxNewTCB->uxPriority = uxPriority;
    {
        pxNewTCB->uxBasePriority = uxPriority;
    }

    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
    vListInitialiseItem( &( pxNewTCB->xEventListItem ) );

    /* Set the pxNewTCB as a link back from the ListItem_t.  This is so we can get
     * back to  the containing TCB from a generic item in a list. */
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );

    /* Event lists are always in priority order. */
    listSET_LIST_ITEM_VALUE( &( pxNewTCB->xEventListItem ), ( TickType_t ) 8 - ( TickType_t ) uxPriority );
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xEventListItem ), pxNewTCB );

    {
        /* Avoid compiler warning about unreferenced parameter. */
        ( void ) xRegions;
    }


    /* Initialize the TCB stack to look as if the task was already running,
     * but had been interrupted by the scheduler.  The return address is set
     * to the start of the task function. Once the stack has been initialised
     * the top of stack variable is updated. */
    {
        /* If the port has capability to detect stack overflow,
         * pass the stack end address to the stack initialization
         * function as well. */
        {
            pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );
        }

        {
            kernelASSERT( ( ( portPOINTER_SIZE_TYPE ) ( pxTopOfStack - pxNewTCB->pxTopOfStack ) ) < ( ( portPOINTER_SIZE_TYPE ) uxStackDepth ) );
        }
    }

    /* Initialize task state and task attributes. */

    if( pxCreatedTask != NULL )
    {
        /* Pass the handle out in an anonymous way.  The handle can be used to
         * change the created task's priority, delete the created task, etc.*/
        *pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}
/*-----------------------------------------------------------*/


    static void prvAddNewTaskToReadyList( TCB_t * pxNewTCB )
    {
        /* Ensure interrupts don't access the task lists while the lists are being
         * updated. */
        taskENTER_CRITICAL();
        {
            uxCurrentNumberOfTasks = ( UBaseType_t ) ( uxCurrentNumberOfTasks + 1U );

            if( pxCurrentTCB == NULL )
            {
                /* There are no other tasks, or all the other tasks are in
                 * the suspended state - make this the current task. */
                pxCurrentTCB = pxNewTCB;

                if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
                {
                    /* This is the first task to be created so do the preliminary
                     * initialisation required.  We will not recover if this call
                     * fails, but we will report the failure. */
                    prvInitialiseTaskLists();
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                /* If the scheduler is not already running, make this task the
                 * current task if it is the highest priority task to be created
                 * so far. */
                if( xSchedulerRunning == pdFALSE )
                {
                    if( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority )
                    {
                        pxCurrentTCB = pxNewTCB;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }

            uxTaskNumber++;

            traceTASK_CREATE( pxNewTCB );

            prvAddTaskToReadyList( pxNewTCB );

            portSETUP_TCB( pxNewTCB );
        }
        taskEXIT_CRITICAL();

        if( xSchedulerRunning != pdFALSE )
        {
            /* If the created task is of a higher priority than the current task
             * then it should run now. */
            taskYIELD_ANY_CORE_IF_USING_PREEMPTION( pxNewTCB );
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


    BaseType_t xTaskDelayUntil( TickType_t * const pxPreviousWakeTime,
                                const TickType_t xTimeIncrement )
    {
        TickType_t xTimeToWake;
        BaseType_t xAlreadyYielded, xShouldDelay = pdFALSE;

        traceENTER_xTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );

        kernelASSERT( pxPreviousWakeTime );
        kernelASSERT( ( xTimeIncrement > 0U ) );

        vTaskSuspendAll();
        {
            /* Minor optimisation.  The tick count cannot change in this
             * block. */
            const TickType_t xConstTickCount = xTickCount;

            kernelASSERT( uxSchedulerSuspended == 1U );

            /* Generate the tick time at which the task wants to wake. */
            xTimeToWake = *pxPreviousWakeTime + xTimeIncrement;

            if( xConstTickCount < *pxPreviousWakeTime )
            {
                /* The tick count has overflowed since this function was
                 * lasted called.  In this case the only time we should ever
                 * actually delay is if the wake time has also  overflowed,
                 * and the wake time is greater than the tick time.  When this
                 * is the case it is as if neither time had overflowed. */
                if( ( xTimeToWake < *pxPreviousWakeTime ) && ( xTimeToWake > xConstTickCount ) )
                {
                    xShouldDelay = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                /* The tick time has not overflowed.  In this case we will
                 * delay if either the wake time has overflowed, and/or the
                 * tick time is less than the wake time. */
                if( ( xTimeToWake < *pxPreviousWakeTime ) || ( xTimeToWake > xConstTickCount ) )
                {
                    xShouldDelay = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }

            /* Update the wake time ready for the next call. */
            *pxPreviousWakeTime = xTimeToWake;

            if( xShouldDelay != pdFALSE )
            {
                traceTASK_DELAY_UNTIL( xTimeToWake );

                /* prvAddCurrentTaskToDelayedList() needs the block time, not
                 * the time to wake, so subtract the current tick count. */
                prvAddCurrentTaskToDelayedList( xTimeToWake - xConstTickCount, pdFALSE );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        xAlreadyYielded = xTaskResumeAll();

        /* Force a reschedule if xTaskResumeAll has not already done so, we may
         * have put ourselves to sleep. */
        if( xAlreadyYielded == pdFALSE )
        {
            taskYIELD_WITHIN_API();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_xTaskDelayUntil( xShouldDelay );

        return xShouldDelay;
    }

/*-----------------------------------------------------------*/


    void vTaskDelay( const TickType_t xTicksToDelay )
    {
        BaseType_t xAlreadyYielded = pdFALSE;

        traceENTER_vTaskDelay( xTicksToDelay );

        /* A delay time of zero just forces a reschedule. */
        if( xTicksToDelay > ( TickType_t ) 0U )
        {
            vTaskSuspendAll();
            {
                kernelASSERT( uxSchedulerSuspended == 1U );

                traceTASK_DELAY();

                /* A task that is removed from the event list while the
                 * scheduler is suspended will not get placed in the ready
                 * list or removed from the blocked list until the scheduler
                 * is resumed.
                 *
                 * This task cannot be in an event list as it is the currently
                 * executing task. */
                prvAddCurrentTaskToDelayedList( xTicksToDelay, pdFALSE );
            }
            xAlreadyYielded = xTaskResumeAll();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        /* Force a reschedule if xTaskResumeAll has not already done so, we may
         * have put ourselves to sleep. */
        if( xAlreadyYielded == pdFALSE )
        {
            taskYIELD_WITHIN_API();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_vTaskDelay();
    }

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


    UBaseType_t uxTaskPriorityGet( const TaskHandle_t xTask )
    {
        TCB_t const * pxTCB;
        UBaseType_t uxReturn;

        traceENTER_uxTaskPriorityGet( xTask );

        portBASE_TYPE_ENTER_CRITICAL();
        {
            /* If null is passed in here then it is the priority of the task
             * that called uxTaskPriorityGet() that is being queried. */
            pxTCB = prvGetTCBFromHandle( xTask );
            kernelASSERT( pxTCB != NULL );

            uxReturn = pxTCB->uxPriority;
        }
        portBASE_TYPE_EXIT_CRITICAL();

        traceRETURN_uxTaskPriorityGet( uxReturn );

        return uxReturn;
    }

/*-----------------------------------------------------------*/


    UBaseType_t uxTaskPriorityGetFromISR( const TaskHandle_t xTask )
    {
        TCB_t const * pxTCB;
        UBaseType_t uxReturn;
        UBaseType_t uxSavedInterruptStatus;

        traceENTER_uxTaskPriorityGetFromISR( xTask );

        /* RTOS ports that support interrupt nesting have the concept of a
         * maximum  system call (or maximum API call) interrupt priority.
         * Interrupts that are  above the maximum system call priority are keep
         * permanently enabled, even when the RTOS kernel is in a critical section,
         * but cannot make any calls to FreeRTOS API functions.  If kernelASSERT()
         * is defined in the fixed profile then
         * portASSERT_IF_INTERRUPT_PRIORITY_INVALID() will result in an assertion
         * failure if a FreeRTOS API function is called from an interrupt that has
         * been assigned a priority above the configured maximum system call
         * priority.  Only FreeRTOS functions that end in FromISR can be called
         * from interrupts  that have been assigned a priority at or (logically)
         * below the maximum system call interrupt priority.  FreeRTOS maintains a
         * separate interrupt safe API to ensure interrupt entry is as fast and as
         * simple as possible.  More information (albeit Cortex-M specific) is
         * provided on the following link:
         * https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
        portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

        /* MISRA Ref 4.7.1 [Return value shall be checked] */
        /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#dir-47 */
        /* coverity[misra_c_2012_directive_4_7_violation] */
        uxSavedInterruptStatus = ( UBaseType_t ) taskENTER_CRITICAL_FROM_ISR();
        {
            /* If null is passed in here then it is the priority of the calling
             * task that is being queried. */
            pxTCB = prvGetTCBFromHandle( xTask );
            kernelASSERT( pxTCB != NULL );

            uxReturn = pxTCB->uxPriority;
        }
        taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

        traceRETURN_uxTaskPriorityGetFromISR( uxReturn );

        return uxReturn;
    }

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static BaseType_t prvCreateIdleTasks( void )
{
    BaseType_t xReturn = pdPASS;
    BaseType_t xCoreID;
    char cIdleName[ 16 ] = { 0 };
    TaskFunction_t pxIdleTaskFunction = NULL;
    UBaseType_t xIdleTaskNameIndex;

    /* MISRA Ref 14.3.1 [Configuration dependent invariant] */
    /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-143. */
    /* coverity[misra_c_2012_rule_14_3_violation] */
    for( xIdleTaskNameIndex = 0U; xIdleTaskNameIndex < ( 16 - taskRESERVED_TASK_NAME_LENGTH ); xIdleTaskNameIndex++ )
    {
        /* MISRA Ref 18.1.1 [Configuration dependent bounds checking] */
        /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-181. */
        /* coverity[misra_c_2012_rule_18_1_violation] */
        cIdleName[ xIdleTaskNameIndex ] = "IDLE"[ xIdleTaskNameIndex ];

        if( cIdleName[ xIdleTaskNameIndex ] == ( char ) 0x00 )
        {
            break;
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }

    /* Ensure null termination. */
    cIdleName[ xIdleTaskNameIndex ] = '\0';

    /* Add each idle task at the lowest priority. */
    for( xCoreID = ( BaseType_t ) 0; xCoreID < ( BaseType_t ) 1; xCoreID++ )
    {
        {
            pxIdleTaskFunction = &prvIdleTask;
        }

        /* Update the idle task name with suffix to differentiate the idle tasks.
         * This function is not required in single core FreeRTOS since there is
         * only one idle task. */

        {
            StaticTask_t * pxIdleTaskTCBBuffer = NULL;
            StackType_t * pxIdleTaskStackBuffer = NULL;
            size_t uxIdleTaskStackSize;

            /* The Idle task is created using user provided RAM - obtain the
             * address of the RAM then create the idle task. */
            {
                vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, &pxIdleTaskStackBuffer, &uxIdleTaskStackSize );
            }
            xIdleTaskHandles[ xCoreID ] = xTaskCreateStatic( pxIdleTaskFunction,
                                                             cIdleName,
                                                             uxIdleTaskStackSize,
                                                             ( void * ) NULL,
                                                             portPRIVILEGE_BIT, /* In effect ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), but tskIDLE_PRIORITY is zero. */
                                                             pxIdleTaskStackBuffer,
                                                             pxIdleTaskTCBBuffer );

            if( xIdleTaskHandles[ xCoreID ] != NULL )
            {
                xReturn = pdPASS;
            }
            else
            {
                xReturn = pdFAIL;
            }
        }

        /* Break the loop if any of the idle task is failed to be created. */
        if( xReturn != pdPASS )
        {
            break;
        }
        else
        {
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

void vTaskStartScheduler( void )
{
    BaseType_t xReturn;

    traceENTER_vTaskStartScheduler();


    xReturn = prvCreateIdleTasks();


    if( xReturn == pdPASS )
    {
        /* freertos_tasks_c_additions_init() should only be called if the user
         * definable macro FREERTOS_TASKS_C_ADDITIONS_INIT() is defined, as that is
         * the only macro called by the function. */

        /* Interrupts are turned off here, to ensure a tick does not occur
         * before or during the call to xPortStartScheduler().  The stacks of
         * the created tasks contain a status word with interrupts switched on
         * so interrupts will automatically get re-enabled when the first task
         * starts to run. */
        portDISABLE_INTERRUPTS();


        xNextTaskUnblockTime = portMAX_DELAY;
        xSchedulerRunning = pdTRUE;
        xTickCount = ( TickType_t ) 0;

        /* If 0 is defined then the following
         * macro must be defined to configure the timer/counter used to generate
         * the run time counter time base.   NOTE:  If 0
         * is set to 0 and the following line fails to build then ensure you do not
         * have portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() defined in your
         * the fixed profile file. */
        portCONFIGURE_TIMER_FOR_RUN_TIME_STATS();

        traceTASK_SWITCHED_IN();

        traceSTARTING_SCHEDULER( xIdleTaskHandles );

        /* Setting up the timer tick is hardware specific and thus in the
         * portable interface. */

        /* The return value for xPortStartScheduler is not required
         * hence using a void datatype. */
        ( void ) xPortStartScheduler();

        /* In most cases, xPortStartScheduler() will not return. If it
         * returns pdTRUE then there was not enough heap memory available
         * to create either the Idle or the Timer task. If it returned
         * pdFALSE, then the application called xTaskEndScheduler().
         * Most ports don't implement xTaskEndScheduler() as there is
         * nothing to return to. */
    }
    else
    {
        /* This line will only be reached if the kernel could not be started,
         * because there was not enough FreeRTOS heap to create the idle task
         * or the timer task. */
        kernelASSERT( xReturn != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY );
    }

    /* Prevent compiler warnings if 0 is set to 0,
     * meaning xIdleTaskHandles are not used anywhere else. */
    ( void ) xIdleTaskHandles;

    /* OpenOCD makes use of uxTopUsedPriority for thread debugging. Prevent uxTopUsedPriority
     * from getting optimized out as it is no longer used by the kernel. */
    ( void ) uxTopUsedPriority;

    traceRETURN_vTaskStartScheduler();
}
/*-----------------------------------------------------------*/

void vTaskEndScheduler( void )
{
    traceENTER_vTaskEndScheduler();


    /* Stop the scheduler interrupts and call the portable scheduler end
     * routine so the original ISRs can be restored if necessary.  The port
     * layer must ensure interrupts enable  bit is left in the correct state. */
    portDISABLE_INTERRUPTS();
    xSchedulerRunning = pdFALSE;

    /* This function must be called from a task and the application is
     * responsible for deleting that task after the scheduler is stopped. */
    vPortEndScheduler();

    traceRETURN_vTaskEndScheduler();
}
/*----------------------------------------------------------*/

void vTaskSuspendAll( void )
{
    traceENTER_vTaskSuspendAll();

    {
        /* A critical section is not required as the variable is of type
         * BaseType_t. Each task maintains its own context, and a context switch
         * cannot occur if the variable is non zero. So, as long as the writing
         * from the register back into the memory is atomic, it is not a
         * problem.
         *
         * Consider the following scenario, which starts with
         * uxSchedulerSuspended at zero.
         *
         * 1. load uxSchedulerSuspended into register.
         * 2. Now a context switch causes another task to run, and the other
         *    task uses the same variable. The other task will see the variable
         *    as zero because the variable has not yet been updated by the
         *    original task. Eventually the original task runs again. **That can
         *    only happen when uxSchedulerSuspended is once again zero**. When
         *    the original task runs again, the contents of the CPU registers
         *    are restored to exactly how they were when it was switched out -
         *    therefore the value it read into the register still matches the
         *    value of the uxSchedulerSuspended variable.
         *
         * 3. increment register.
         * 4. store register into uxSchedulerSuspended. The value restored to
         *    uxSchedulerSuspended will be the correct value of 1, even though
         *    the variable was used by other tasks in the mean time.
         */

        /* portSOFTWARE_BARRIER() is only implemented for emulated/simulated ports that
         * do not otherwise exhibit real time behaviour. */
        portSOFTWARE_BARRIER();

        /* The scheduler is suspended if uxSchedulerSuspended is non-zero.  An increment
         * is used to allow calls to vTaskSuspendAll() to nest. */
        uxSchedulerSuspended = ( UBaseType_t ) ( uxSchedulerSuspended + 1U );

        /* Enforces ordering for ports and optimised compilers that may otherwise place
         * the above increment elsewhere. */
        portMEMORY_BARRIER();
    }

    traceRETURN_vTaskSuspendAll();
}

/*----------------------------------------------------------*/

/*----------------------------------------------------------*/

BaseType_t xTaskResumeAll( void )
{
    TCB_t * pxTCB = NULL;
    BaseType_t xAlreadyYielded = pdFALSE;

    traceENTER_xTaskResumeAll();

    {
        /* It is possible that an ISR caused a task to be removed from an event
         * list while the scheduler was suspended.  If this was the case then the
         * removed task will have been added to the xPendingReadyList.  Once the
         * scheduler has been resumed it is safe to move all the pending ready
         * tasks from this list into their appropriate ready list. */
        taskENTER_CRITICAL();
        {
            const BaseType_t xCoreID = ( BaseType_t ) portGET_CORE_ID();

            /* If uxSchedulerSuspended is zero then this function does not match a
             * previous call to vTaskSuspendAll(). */
            kernelASSERT( uxSchedulerSuspended != 0U );

            uxSchedulerSuspended = ( UBaseType_t ) ( uxSchedulerSuspended - 1U );
            portRELEASE_TASK_LOCK( xCoreID );

            if( uxSchedulerSuspended == ( UBaseType_t ) 0U )
            {
                if( uxCurrentNumberOfTasks > ( UBaseType_t ) 0U )
                {
                    /* Move any readied tasks from the pending list into the
                     * appropriate ready list. */
                    while( listLIST_IS_EMPTY( &xPendingReadyList ) == pdFALSE )
                    {
                        /* MISRA Ref 11.5.3 [Void pointer assignment] */
                        /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
                        /* coverity[misra_c_2012_rule_11_5_violation] */
                        pxTCB = listGET_OWNER_OF_HEAD_ENTRY( ( &xPendingReadyList ) );
                        listREMOVE_ITEM( &( pxTCB->xEventListItem ) );
                        portMEMORY_BARRIER();
                        listREMOVE_ITEM( &( pxTCB->xStateListItem ) );
                        prvAddTaskToReadyList( pxTCB );

                        {
                            /* If the moved task has a priority higher than the current
                             * task then a yield must be performed. */
                            if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                            {
                                xYieldPendings[ xCoreID ] = pdTRUE;
                            }
                            else
                            {
                                mtCOVERAGE_TEST_MARKER();
                            }
                        }
                    }

                    if( pxTCB != NULL )
                    {
                        /* A task was unblocked while the scheduler was suspended,
                         * which may have prevented the next unblock time from being
                         * re-calculated, in which case re-calculate it now.  Mainly
                         * important for low power tickless implementations, where
                         * this can prevent an unnecessary exit from low power
                         * state. */
                        prvResetNextTaskUnblockTime();
                    }

                    /* If any ticks occurred while the scheduler was suspended then
                     * they should be processed now.  This ensures the tick count does
                     * not  slip, and that any delayed tasks are resumed at the correct
                     * time.
                     *
                     * It should be safe to call xTaskIncrementTick here from any core
                     * since we are in a critical section and xTaskIncrementTick itself
                     * protects itself within a critical section. Suspending the scheduler
                     * from any core causes xTaskIncrementTick to increment uxPendedCounts. */
                    {
                        TickType_t xPendedCounts = xPendedTicks; /* Non-volatile copy. */

                        if( xPendedCounts > ( TickType_t ) 0U )
                        {
                            do
                            {
                                if( xTaskIncrementTick() != pdFALSE )
                                {
                                    /* Other cores are interrupted from
                                     * within xTaskIncrementTick(). */
                                    xYieldPendings[ xCoreID ] = pdTRUE;
                                }
                                else
                                {
                                    mtCOVERAGE_TEST_MARKER();
                                }

                                --xPendedCounts;
                            } while( xPendedCounts > ( TickType_t ) 0U );

                            xPendedTicks = 0;
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }

                    if( xYieldPendings[ xCoreID ] != pdFALSE )
                    {
                        {
                            xAlreadyYielded = pdTRUE;
                        }

                        {
                            taskYIELD_TASK_CORE_IF_USING_PREEMPTION( pxCurrentTCB );
                        }
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        taskEXIT_CRITICAL();
    }

    traceRETURN_xTaskResumeAll( xAlreadyYielded );

    return xAlreadyYielded;
}
/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCount( void )
{
    TickType_t xTicks;

    traceENTER_xTaskGetTickCount();

    /* Critical section required if running on a 16 bit processor. */
    portTICK_TYPE_ENTER_CRITICAL();
    {
        xTicks = xTickCount;
    }
    portTICK_TYPE_EXIT_CRITICAL();

    traceRETURN_xTaskGetTickCount( xTicks );

    return xTicks;
}
/*-----------------------------------------------------------*/

TickType_t xTaskGetTickCountFromISR( void )
{
    TickType_t xReturn;
    UBaseType_t uxSavedInterruptStatus;

    traceENTER_xTaskGetTickCountFromISR();

    /* RTOS ports that support interrupt nesting have the concept of a maximum
     * system call (or maximum API call) interrupt priority.  Interrupts that are
     * above the maximum system call priority are kept permanently enabled, even
     * when the RTOS kernel is in a critical section, but cannot make any calls to
     * FreeRTOS API functions.  If kernelASSERT() is defined in the fixed profile
     * then portASSERT_IF_INTERRUPT_PRIORITY_INVALID() will result in an assertion
     * failure if a FreeRTOS API function is called from an interrupt that has been
     * assigned a priority above the configured maximum system call priority.
     * Only FreeRTOS functions that end in FromISR can be called from interrupts
     * that have been assigned a priority at or (logically) below the maximum
     * system call  interrupt priority.  FreeRTOS maintains a separate interrupt
     * safe API to ensure interrupt entry is as fast and as simple as possible.
     * More information (albeit Cortex-M specific) is provided on the following
     * link: https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
    portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

    uxSavedInterruptStatus = portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR();
    {
        xReturn = xTickCount;
    }
    portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    traceRETURN_xTaskGetTickCountFromISR( xReturn );

    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t uxTaskGetNumberOfTasks( void )
{
    traceENTER_uxTaskGetNumberOfTasks();

    /* A critical section is not required because the variables are of type
     * BaseType_t. */
    traceRETURN_uxTaskGetNumberOfTasks( uxCurrentNumberOfTasks );

    return uxCurrentNumberOfTasks;
}
/*-----------------------------------------------------------*/

char * pcTaskGetName( TaskHandle_t xTaskToQuery )
{
    TCB_t * pxTCB;

    traceENTER_pcTaskGetName( xTaskToQuery );

    /* If null is passed in here then the name of the calling task is being
     * queried. */
    pxTCB = prvGetTCBFromHandle( xTaskToQuery );
    kernelASSERT( pxTCB != NULL );

    traceRETURN_pcTaskGetName( &( pxTCB->pcTaskName[ 0 ] ) );

    return &( pxTCB->pcTaskName[ 0 ] );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


    BaseType_t xTaskGetStaticBuffers( TaskHandle_t xTask,
                                      StackType_t ** ppuxStackBuffer,
                                      StaticTask_t ** ppxTaskBuffer )
    {
        BaseType_t xReturn;
        TCB_t * pxTCB;

        traceENTER_xTaskGetStaticBuffers( xTask, ppuxStackBuffer, ppxTaskBuffer );

        kernelASSERT( ppuxStackBuffer != NULL );
        kernelASSERT( ppxTaskBuffer != NULL );

        pxTCB = prvGetTCBFromHandle( xTask );
        kernelASSERT( pxTCB != NULL );

        {
            *ppuxStackBuffer = pxTCB->pxStack;
            *ppxTaskBuffer = ( StaticTask_t * ) pxTCB;
            xReturn = pdTRUE;
        }

        traceRETURN_xTaskGetStaticBuffers( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/

/*----------------------------------------------------------*/

/*----------------------------------------------------------*/

/* This conditional compilation should use inequality to 0, not equality to 1.
 * This is to ensure vTaskStepTick() is available when user defined low power mode
 * implementations require 0 to be set to a value other than
 * 1. */
/*----------------------------------------------------------*/

BaseType_t xTaskCatchUpTicks( TickType_t xTicksToCatchUp )
{
    BaseType_t xYieldOccurred;

    traceENTER_xTaskCatchUpTicks( xTicksToCatchUp );

    /* Must not be called with the scheduler suspended as the implementation
     * relies on xPendedTicks being wound down to 0 in xTaskResumeAll(). */
    kernelASSERT( uxSchedulerSuspended == ( UBaseType_t ) 0U );

    /* Use xPendedTicks to mimic xTicksToCatchUp number of ticks occurring when
     * the scheduler is suspended so the ticks are executed in xTaskResumeAll(). */
    vTaskSuspendAll();

    /* Prevent the tick interrupt modifying xPendedTicks simultaneously. */
    taskENTER_CRITICAL();
    {
        xPendedTicks += xTicksToCatchUp;
    }
    taskEXIT_CRITICAL();
    xYieldOccurred = xTaskResumeAll();

    traceRETURN_xTaskCatchUpTicks( xYieldOccurred );

    return xYieldOccurred;
}
/*----------------------------------------------------------*/

/*----------------------------------------------------------*/

BaseType_t xTaskIncrementTick( void )
{
    TCB_t * pxTCB;
    TickType_t xItemValue;
    BaseType_t xSwitchRequired = pdFALSE;

    traceENTER_xTaskIncrementTick();

    /* Called by the portable layer each time a tick interrupt occurs.
     * Increments the tick then checks to see if the new tick value will cause any
     * tasks to be unblocked. */
    traceTASK_INCREMENT_TICK( xTickCount );

    /* Tick increment should occur on every kernel timer event. Core 0 has the
     * responsibility to increment the tick, or increment the pended ticks if the
     * scheduler is suspended.  If pended ticks is greater than zero, the core that
     * calls xTaskResumeAll has the responsibility to increment the tick. */
    if( uxSchedulerSuspended == ( UBaseType_t ) 0U )
    {
        /* Minor optimisation.  The tick count cannot change in this
         * block. */
        const TickType_t xConstTickCount = xTickCount + ( TickType_t ) 1;

        /* Increment the RTOS tick, switching the delayed and overflowed
         * delayed lists if it wraps to 0. */
        xTickCount = xConstTickCount;

        if( xConstTickCount == ( TickType_t ) 0U )
        {
            taskSWITCH_DELAYED_LISTS();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        /* See if this tick has made a timeout expire.  Tasks are stored in
         * the  queue in the order of their wake time - meaning once one task
         * has been found whose block time has not expired there is no need to
         * look any further down the list. */
        if( xConstTickCount >= xNextTaskUnblockTime )
        {
            for( ; ; )
            {
                if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
                {
                    /* The delayed list is empty.  Set xNextTaskUnblockTime
                     * to the maximum possible value so it is extremely
                     * unlikely that the
                     * if( xTickCount >= xNextTaskUnblockTime ) test will pass
                     * next time through. */
                    xNextTaskUnblockTime = portMAX_DELAY;
                    break;
                }
                else
                {
                    /* The delayed list is not empty, get the value of the
                     * item at the head of the delayed list.  This is the time
                     * at which the task at the head of the delayed list must
                     * be removed from the Blocked state. */
                    /* MISRA Ref 11.5.3 [Void pointer assignment] */
                    /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
                    /* coverity[misra_c_2012_rule_11_5_violation] */
                    pxTCB = listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
                    xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );

                    if( xConstTickCount < xItemValue )
                    {
                        /* It is not time to unblock this item yet, but the
                         * item value is the time at which the task at the head
                         * of the blocked list must be removed from the Blocked
                         * state -  so record the item value in
                         * xNextTaskUnblockTime. */
                        xNextTaskUnblockTime = xItemValue;
                        break;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* It is time to remove the item from the Blocked state. */
                    listREMOVE_ITEM( &( pxTCB->xStateListItem ) );

                    /* Is the task waiting on an event also?  If so remove
                     * it from the event list. */
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
                    {
                        listREMOVE_ITEM( &( pxTCB->xEventListItem ) );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* Place the unblocked task into the appropriate ready
                     * list. */
                    prvAddTaskToReadyList( pxTCB );

                    /* A task being unblocked cannot cause an immediate
                     * context switch if preemption is turned off. */
                    {
                        {
                            /* Preemption is on, but a context switch should
                             * only be performed if the unblocked task's
                             * priority is higher than the currently executing
                             * task.
                             * The case of equal priority tasks sharing
                             * processing time (which happens when both
                             * preemption and time slicing are on) is
                             * handled below.*/
                            if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                            {
                                xSwitchRequired = pdTRUE;
                            }
                            else
                            {
                                mtCOVERAGE_TEST_MARKER();
                            }
                        }
                    }
                }
            }
        }

        /* Tasks of equal priority to the currently running task will share
         * processing time (time slice) if preemption is on, and the application
         * writer has not explicitly turned time slicing off. */


        {
            {
                /* For single core the core ID is always 0. */
                if( xYieldPendings[ 0 ] != pdFALSE )
                {
                    xSwitchRequired = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        }
    }
    else
    {
        xPendedTicks += 1U;

        /* The tick hook gets called at regular intervals, even if the
         * scheduler is locked. */
    }

    traceRETURN_xTaskIncrementTick( xSwitchRequired );

    return xSwitchRequired;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

    void vTaskSwitchContext( void )
    {
        traceENTER_vTaskSwitchContext();

        if( uxSchedulerSuspended != ( UBaseType_t ) 0U )
        {
            /* The scheduler is currently suspended - do not allow a context
             * switch. */
            xYieldPendings[ 0 ] = pdTRUE;
        }
        else
        {
            xYieldPendings[ 0 ] = pdFALSE;
            traceTASK_SWITCHED_OUT();


            /* Check for stack overflow, if configured. */
            taskCHECK_FOR_STACK_OVERFLOW();

            /* Before the currently running task is switched out, save its errno. */

            /* Select a new task to run using either the generic C or port
             * optimised asm code. */
            /* MISRA Ref 11.5.3 [Void pointer assignment] */
            /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
            /* coverity[misra_c_2012_rule_11_5_violation] */
            taskSELECT_HIGHEST_PRIORITY_TASK();
            traceTASK_SWITCHED_IN();

            /* Macro to inject port specific behaviour immediately after
             * switching tasks, such as setting an end of stack watchpoint
             * or reconfiguring the MPU. */
            portTASK_SWITCH_HOOK( pxCurrentTCB );

            /* After the new task is switched in, update the global errno. */

        }

        traceRETURN_vTaskSwitchContext();
    }
/*-----------------------------------------------------------*/

void vTaskPlaceOnEventList( List_t * const pxEventList,
                            const TickType_t xTicksToWait )
{
    traceENTER_vTaskPlaceOnEventList( pxEventList, xTicksToWait );

    kernelASSERT( pxEventList );

    /* THIS FUNCTION MUST BE CALLED WITH THE
     * SCHEDULER SUSPENDED AND THE QUEUE BEING ACCESSED LOCKED. */

    /* Place the event list item of the TCB in the appropriate event list.
     * This is placed in the list in priority order so the highest priority task
     * is the first to be woken by the event.
     *
     * Note: Lists are sorted in ascending order by ListItem_t.xItemValue.
     * Normally, the xItemValue of a TCB's ListItem_t members is:
     *      xItemValue = ( 8 - uxPriority )
     * Therefore, the event list is sorted in descending priority order.
     *
     * The queue that contains the event list is locked, preventing
     * simultaneous access from interrupts. */
    vListInsert( pxEventList, &( pxCurrentTCB->xEventListItem ) );

    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );

    traceRETURN_vTaskPlaceOnEventList();
}
/*-----------------------------------------------------------*/

void vTaskPlaceOnUnorderedEventList( List_t * pxEventList,
                                     const TickType_t xItemValue,
                                     const TickType_t xTicksToWait )
{
    traceENTER_vTaskPlaceOnUnorderedEventList( pxEventList, xItemValue, xTicksToWait );

    kernelASSERT( pxEventList );

    /* THIS FUNCTION MUST BE CALLED WITH THE SCHEDULER SUSPENDED.  It is used by
     * the event groups implementation. */
    kernelASSERT( uxSchedulerSuspended != ( UBaseType_t ) 0U );

    /* Store the item value in the event list item.  It is safe to access the
     * event list item here as interrupts won't access the event list item of a
     * task that is not in the Blocked state. */
    listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xEventListItem ), xItemValue | taskEVENT_LIST_ITEM_VALUE_IN_USE );

    /* Place the event list item of the TCB at the end of the appropriate event
     * list.  It is safe to access the event list here because it is part of an
     * event group implementation - and interrupts don't access event groups
     * directly (instead they access them indirectly by pending function calls to
     * the task level). */
    listINSERT_END( pxEventList, &( pxCurrentTCB->xEventListItem ) );

    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );

    traceRETURN_vTaskPlaceOnUnorderedEventList();
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList )
{
    TCB_t * pxUnblockedTCB;
    BaseType_t xReturn;

    traceENTER_xTaskRemoveFromEventList( pxEventList );

    /* THIS FUNCTION MUST BE CALLED FROM A CRITICAL SECTION.  It can also be
     * called from a critical section within an ISR. */

    /* The event list is sorted in priority order, so the first in the list can
     * be removed as it is known to be the highest priority.  Remove the TCB from
     * the delayed list, and add it to the ready list.
     *
     * If an event is for a queue that is locked then this function will never
     * get called - the lock count on the queue will get modified instead.  This
     * means exclusive access to the event list is guaranteed here.
     *
     * This function assumes that a check has already been made to ensure that
     * pxEventList is not empty. */
    /* MISRA Ref 11.5.3 [Void pointer assignment] */
    /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
    /* coverity[misra_c_2012_rule_11_5_violation] */
    pxUnblockedTCB = listGET_OWNER_OF_HEAD_ENTRY( pxEventList );
    kernelASSERT( pxUnblockedTCB );
    listREMOVE_ITEM( &( pxUnblockedTCB->xEventListItem ) );

    if( uxSchedulerSuspended == ( UBaseType_t ) 0U )
    {
        listREMOVE_ITEM( &( pxUnblockedTCB->xStateListItem ) );
        prvAddTaskToReadyList( pxUnblockedTCB );

    }
    else
    {
        /* The delayed and ready lists cannot be accessed, so hold this task
         * pending until the scheduler is resumed. */
        listINSERT_END( &( xPendingReadyList ), &( pxUnblockedTCB->xEventListItem ) );
    }

    {
        if( pxUnblockedTCB->uxPriority > pxCurrentTCB->uxPriority )
        {
            /* Return true if the task removed from the event list has a higher
             * priority than the calling task.  This allows the calling task to know if
             * it should force a context switch now. */
            xReturn = pdTRUE;

            /* Mark that a yield is pending in case the user is not using the
             * "xHigherPriorityTaskWoken" parameter to an ISR safe FreeRTOS function. */
            xYieldPendings[ 0 ] = pdTRUE;
        }
        else
        {
            xReturn = pdFALSE;
        }
    }

    traceRETURN_xTaskRemoveFromEventList( xReturn );
    return xReturn;
}
/*-----------------------------------------------------------*/

void vTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem,
                                        const TickType_t xItemValue )
{
    TCB_t * pxUnblockedTCB;

    traceENTER_vTaskRemoveFromUnorderedEventList( pxEventListItem, xItemValue );

    /* THIS FUNCTION MUST BE CALLED WITH THE SCHEDULER SUSPENDED.  It is used by
     * the event flags implementation. */
    kernelASSERT( uxSchedulerSuspended != ( UBaseType_t ) 0U );

    /* Store the new item value in the event list. */
    listSET_LIST_ITEM_VALUE( pxEventListItem, xItemValue | taskEVENT_LIST_ITEM_VALUE_IN_USE );

    /* Remove the event list form the event flag.  Interrupts do not access
     * event flags. */
    /* MISRA Ref 11.5.3 [Void pointer assignment] */
    /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#rule-115 */
    /* coverity[misra_c_2012_rule_11_5_violation] */
    pxUnblockedTCB = listGET_LIST_ITEM_OWNER( pxEventListItem );
    kernelASSERT( pxUnblockedTCB );
    listREMOVE_ITEM( pxEventListItem );


    /* Remove the task from the delayed list and add it to the ready list.  The
     * scheduler is suspended so interrupts will not be accessing the ready
     * lists. */
    listREMOVE_ITEM( &( pxUnblockedTCB->xStateListItem ) );
    prvAddTaskToReadyList( pxUnblockedTCB );

    {
        if( pxUnblockedTCB->uxPriority > pxCurrentTCB->uxPriority )
        {
            /* The unblocked task has a priority above that of the calling task, so
             * a context switch is required.  This function is called with the
             * scheduler suspended so xYieldPending is set so the context switch
             * occurs immediately that the scheduler is resumed (unsuspended). */
            xYieldPendings[ 0 ] = pdTRUE;
        }
    }

    traceRETURN_vTaskRemoveFromUnorderedEventList();
}
/*-----------------------------------------------------------*/

void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut )
{
    traceENTER_vTaskSetTimeOutState( pxTimeOut );

    kernelASSERT( pxTimeOut );
    taskENTER_CRITICAL();
    {
        pxTimeOut->xOverflowCount = xNumOfOverflows;
        pxTimeOut->xTimeOnEntering = xTickCount;
    }
    taskEXIT_CRITICAL();

    traceRETURN_vTaskSetTimeOutState();
}
/*-----------------------------------------------------------*/

void vTaskInternalSetTimeOutState( TimeOut_t * const pxTimeOut )
{
    traceENTER_vTaskInternalSetTimeOutState( pxTimeOut );

    /* For internal use only as it does not use a critical section. */
    pxTimeOut->xOverflowCount = xNumOfOverflows;
    pxTimeOut->xTimeOnEntering = xTickCount;

    traceRETURN_vTaskInternalSetTimeOutState();
}
/*-----------------------------------------------------------*/

BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut,
                                 TickType_t * const pxTicksToWait )
{
    BaseType_t xReturn;

    traceENTER_xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait );

    kernelASSERT( pxTimeOut );
    kernelASSERT( pxTicksToWait );

    taskENTER_CRITICAL();
    {
        /* Minor optimisation.  The tick count cannot change in this block. */
        const TickType_t xConstTickCount = xTickCount;
        const TickType_t xElapsedTime = xConstTickCount - pxTimeOut->xTimeOnEntering;



        if( ( xNumOfOverflows != pxTimeOut->xOverflowCount ) && ( xConstTickCount >= pxTimeOut->xTimeOnEntering ) )
        {
            /* The tick count is greater than the time at which
             * vTaskSetTimeout() was called, but has also overflowed since
             * vTaskSetTimeOut() was called.  It must have wrapped all the way
             * around and gone past again. This passed since vTaskSetTimeout()
             * was called. */
            xReturn = pdTRUE;
            *pxTicksToWait = ( TickType_t ) 0;
        }
        else if( xElapsedTime < *pxTicksToWait )
        {
            /* Not a genuine timeout. Adjust parameters for time remaining. */
            *pxTicksToWait -= xElapsedTime;
            vTaskInternalSetTimeOutState( pxTimeOut );
            xReturn = pdFALSE;
        }
        else
        {
            *pxTicksToWait = ( TickType_t ) 0;
            xReturn = pdTRUE;
        }
    }
    taskEXIT_CRITICAL();

    traceRETURN_xTaskCheckForTimeOut( xReturn );

    return xReturn;
}
/*-----------------------------------------------------------*/

void vTaskMissedYield( void )
{
    traceENTER_vTaskMissedYield();

    /* Must be called from within a critical section. */
    xYieldPendings[ portGET_CORE_ID() ] = pdTRUE;

    traceRETURN_vTaskMissedYield();
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*
 * -----------------------------------------------------------
 * The passive idle task.
 * ----------------------------------------------------------
 *
 * The passive idle task is used for all the additional cores in a SMP
 * system. There must be only 1 active idle task and the rest are passive
 * idle tasks.
 *
 * The portTASK_FUNCTION() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvPassiveIdleTask( void *pvParameters );
 */


/*
 * -----------------------------------------------------------
 * The idle task.
 * ----------------------------------------------------------
 *
 * The portTASK_FUNCTION() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */

static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
    /* Stop warnings. */
    ( void ) pvParameters;

    /** THIS IS THE RTOS IDLE TASK - WHICH IS CREATED AUTOMATICALLY WHEN THE
     * SCHEDULER IS STARTED. **/

    /* In case a task that has a secure context deletes itself, in which case
     * the idle task is responsible for deleting the task's secure context, if
     * any. */
    portALLOCATE_SECURE_CONTEXT( 0 );


    for( ; 1; )
    {
        /* See if any tasks have deleted themselves - if so then the idle task
         * is responsible for freeing the deleted task's TCB and stack. */
        prvCheckTasksWaitingTermination();




        /* This conditional compilation should use inequality to 0, not equality
         * to 1.  This is to ensure portSUPPRESS_TICKS_AND_SLEEP() is called when
         * user defined low power mode  implementations require
         * 0 to be set to a value other than 1. */

    }
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvInitialiseTaskLists( void )
{
    UBaseType_t uxPriority;

    for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) 8; uxPriority++ )
    {
        vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
    }

    vListInitialise( &xDelayedTaskList1 );
    vListInitialise( &xDelayedTaskList2 );
    vListInitialise( &xPendingReadyList );



    /* Start with pxDelayedTaskList using list1 and the pxOverflowDelayedTaskList
     * using list2. */
    pxDelayedTaskList = &xDelayedTaskList1;
    pxOverflowDelayedTaskList = &xDelayedTaskList2;
}
/*-----------------------------------------------------------*/

static size_t prvTaskCheckFreeStackSpace( const uint8_t * pucStackByte )
{
    size_t uxCount = 0U;

    while( *pucStackByte == ( uint8_t ) tskSTACK_FILL_BYTE )
    {
        pucStackByte -= portSTACK_GROWTH;
        uxCount++;
    }

    return uxCount / ( size_t ) sizeof( StackType_t );
}
/*-----------------------------------------------------------*/

static void prvCheckTasksWaitingTermination( void )
{
    /** THIS FUNCTION IS CALLED FROM THE RTOS IDLE TASK **/

}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


    UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask )
    {
        TCB_t * pxTCB;
        uint8_t * pucEndOfStack;
        UBaseType_t uxReturn;

        traceENTER_uxTaskGetStackHighWaterMark( xTask );

        pxTCB = prvGetTCBFromHandle( xTask );
        kernelASSERT( pxTCB != NULL );

        {
            pucEndOfStack = ( uint8_t * ) pxTCB->pxStack;
        }

        uxReturn = ( UBaseType_t ) prvTaskCheckFreeStackSpace( pucEndOfStack );

        traceRETURN_uxTaskGetStackHighWaterMark( uxReturn );

        return uxReturn;
    }

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvResetNextTaskUnblockTime( void )
{
    if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
    {
        /* The new current delayed list is empty.  Set xNextTaskUnblockTime to
         * the maximum possible value so it is  extremely unlikely that the
         * if( xTickCount >= xNextTaskUnblockTime ) test will pass until
         * there is an item in the delayed list. */
        xNextTaskUnblockTime = portMAX_DELAY;
    }
    else
    {
        /* The new current delayed list is not empty, get the value of
         * the item at the head of the delayed list.  This is the time at
         * which the task at the head of the delayed list should be removed
         * from the Blocked state. */
        xNextTaskUnblockTime = listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxDelayedTaskList );
    }
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


    BaseType_t xTaskPriorityInherit( TaskHandle_t const pxMutexHolder )
    {
        TCB_t * const pxMutexHolderTCB = pxMutexHolder;
        BaseType_t xReturn = pdFALSE;

        traceENTER_xTaskPriorityInherit( pxMutexHolder );

        /* If the mutex is taken by an interrupt, the mutex holder is NULL. Priority
         * inheritance is not applied in this scenario. */
        if( pxMutexHolder != NULL )
        {
            /* If the holder of the mutex has a priority below the priority of
             * the task attempting to obtain the mutex then it will temporarily
             * inherit the priority of the task attempting to obtain the mutex. */
            if( pxMutexHolderTCB->uxPriority < pxCurrentTCB->uxPriority )
            {
                /* Adjust the mutex holder state to account for its new
                 * priority.  Only reset the event list item value if the value is
                 * not being used for anything else. */
                if( ( listGET_LIST_ITEM_VALUE( &( pxMutexHolderTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == ( ( TickType_t ) 0U ) )
                {
                    listSET_LIST_ITEM_VALUE( &( pxMutexHolderTCB->xEventListItem ), ( TickType_t ) 8 - ( TickType_t ) pxCurrentTCB->uxPriority );
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }

                /* If the task being modified is in the ready state it will need
                 * to be moved into a new list. */
                if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ pxMutexHolderTCB->uxPriority ] ), &( pxMutexHolderTCB->xStateListItem ) ) != pdFALSE )
                {
                    if( uxListRemove( &( pxMutexHolderTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                    {
                        /* It is known that the task is in its ready list so
                         * there is no need to check again and the port level
                         * reset macro can be called directly. */
                        portRESET_READY_PRIORITY( pxMutexHolderTCB->uxPriority, uxTopReadyPriority );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* Inherit the priority before being moved into the new list. */
                    pxMutexHolderTCB->uxPriority = pxCurrentTCB->uxPriority;
                    prvAddTaskToReadyList( pxMutexHolderTCB );
                }
                else
                {
                    /* Just inherit the priority. */
                    pxMutexHolderTCB->uxPriority = pxCurrentTCB->uxPriority;
                }

                traceTASK_PRIORITY_INHERIT( pxMutexHolderTCB, pxCurrentTCB->uxPriority );

                /* Inheritance occurred. */
                xReturn = pdTRUE;
            }
            else
            {
                if( pxMutexHolderTCB->uxBasePriority < pxCurrentTCB->uxPriority )
                {
                    /* The base priority of the mutex holder is lower than the
                     * priority of the task attempting to take the mutex, but the
                     * current priority of the mutex holder is not lower than the
                     * priority of the task attempting to take the mutex.
                     * Therefore the mutex holder must have already inherited a
                     * priority, but inheritance would have occurred if that had
                     * not been the case. */
                    xReturn = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_xTaskPriorityInherit( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/


    BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder )
    {
        TCB_t * const pxTCB = pxMutexHolder;
        BaseType_t xReturn = pdFALSE;

        traceENTER_xTaskPriorityDisinherit( pxMutexHolder );

        if( pxMutexHolder != NULL )
        {
            /* A task can only have an inherited priority if it holds the mutex.
             * If the mutex is held by a task then it cannot be given from an
             * interrupt, and if a mutex is given by the holding task then it must
             * be the running state task. */
            kernelASSERT( pxTCB == pxCurrentTCB );
            kernelASSERT( pxTCB->uxMutexesHeld );
            ( pxTCB->uxMutexesHeld )--;

            /* Has the holder of the mutex inherited the priority of another
             * task? */
            if( pxTCB->uxPriority != pxTCB->uxBasePriority )
            {
                /* Only disinherit if no other mutexes are held. */
                if( pxTCB->uxMutexesHeld == ( UBaseType_t ) 0 )
                {
                    /* A task can only have an inherited priority if it holds
                     * the mutex.  If the mutex is held by a task then it cannot be
                     * given from an interrupt, and if a mutex is given by the
                     * holding task then it must be the running state task.  Remove
                     * the holding task from the ready list. */
                    if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                    {
                        portRESET_READY_PRIORITY( pxTCB->uxPriority, uxTopReadyPriority );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* Disinherit the priority before adding the task into the
                     * new  ready list. */
                    traceTASK_PRIORITY_DISINHERIT( pxTCB, pxTCB->uxBasePriority );
                    pxTCB->uxPriority = pxTCB->uxBasePriority;

                    /* Reset the event list item value.  It cannot be in use for
                     * any other purpose if this task is running, and it must be
                     * running to give back the mutex. */
                    listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) 8 - ( TickType_t ) pxTCB->uxPriority );
                    prvAddTaskToReadyList( pxTCB );

                    /* Return true to indicate that a context switch is required.
                     * This is only actually required in the corner case whereby
                     * multiple mutexes were held and the mutexes were given back
                     * in an order different to that in which they were taken.
                     * If a context switch did not occur when the first mutex was
                     * returned, even if a task was waiting on it, then a context
                     * switch should occur when the last mutex is returned whether
                     * a task is waiting on it or not. */
                    xReturn = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_xTaskPriorityDisinherit( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/


    void vTaskPriorityDisinheritAfterTimeout( TaskHandle_t const pxMutexHolder,
                                              UBaseType_t uxHighestPriorityWaitingTask )
    {
        TCB_t * const pxTCB = pxMutexHolder;
        UBaseType_t uxPriorityUsedOnEntry, uxPriorityToUse;
        const UBaseType_t uxOnlyOneMutexHeld = ( UBaseType_t ) 1;

        traceENTER_vTaskPriorityDisinheritAfterTimeout( pxMutexHolder, uxHighestPriorityWaitingTask );

        if( pxMutexHolder != NULL )
        {
            /* If pxMutexHolder is not NULL then the holder must hold at least
             * one mutex. */
            kernelASSERT( pxTCB->uxMutexesHeld );

            /* Determine the priority to which the priority of the task that
             * holds the mutex should be set.  This will be the greater of the
             * holding task's base priority and the priority of the highest
             * priority task that is waiting to obtain the mutex. */
            if( pxTCB->uxBasePriority < uxHighestPriorityWaitingTask )
            {
                uxPriorityToUse = uxHighestPriorityWaitingTask;
            }
            else
            {
                uxPriorityToUse = pxTCB->uxBasePriority;
            }

            /* Does the priority need to change? */
            if( pxTCB->uxPriority != uxPriorityToUse )
            {
                /* Only disinherit if no other mutexes are held.  This is a
                 * simplification in the priority inheritance implementation.  If
                 * the task that holds the mutex is also holding other mutexes then
                 * the other mutexes may have caused the priority inheritance. */
                if( pxTCB->uxMutexesHeld == uxOnlyOneMutexHeld )
                {
                    /* If a task has timed out because it already holds the
                     * mutex it was trying to obtain then it cannot of inherited
                     * its own priority. */
                    kernelASSERT( pxTCB != pxCurrentTCB );

                    /* Disinherit the priority, remembering the previous
                     * priority to facilitate determining the subject task's
                     * state. */
                    traceTASK_PRIORITY_DISINHERIT( pxTCB, uxPriorityToUse );
                    uxPriorityUsedOnEntry = pxTCB->uxPriority;
                    pxTCB->uxPriority = uxPriorityToUse;

                    /* Only reset the event list item value if the value is not
                     * being used for anything else. */
                    if( ( listGET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == ( ( TickType_t ) 0U ) )
                    {
                        listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) 8 - ( TickType_t ) uxPriorityToUse );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* If the running task is not the task that holds the mutex
                     * then the task that holds the mutex could be in either the
                     * Ready, Blocked or Suspended states.  Only remove the task
                     * from its current state list if it is in the Ready state as
                     * the task's priority is going to change and there is one
                     * Ready list per priority. */
                    if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ uxPriorityUsedOnEntry ] ), &( pxTCB->xStateListItem ) ) != pdFALSE )
                    {
                        if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                        {
                            /* It is known that the task is in its ready list so
                             * there is no need to check again and the port level
                             * reset macro can be called directly. */
                            portRESET_READY_PRIORITY( uxPriorityUsedOnEntry, uxTopReadyPriority );
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }

                        prvAddTaskToReadyList( pxTCB );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceRETURN_vTaskPriorityDisinheritAfterTimeout();
    }

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*----------------------------------------------------------*/

/*-----------------------------------------------------------*/

TickType_t uxTaskResetEventItemValue( void )
{
    TickType_t uxReturn;

    traceENTER_uxTaskResetEventItemValue();

    uxReturn = listGET_LIST_ITEM_VALUE( &( pxCurrentTCB->xEventListItem ) );

    /* Reset the event list item to its normal value - so it can be used with
     * queues and semaphores. */
    listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xEventListItem ), ( ( TickType_t ) 8 - ( TickType_t ) pxCurrentTCB->uxPriority ) );

    traceRETURN_uxTaskResetEventItemValue( uxReturn );

    return uxReturn;
}
/*-----------------------------------------------------------*/


    TaskHandle_t pvTaskIncrementMutexHeldCount( void )
    {
        TCB_t * pxTCB;

        traceENTER_pvTaskIncrementMutexHeldCount();

        pxTCB = pxCurrentTCB;

        /* If xSemaphoreCreateMutex() is called before any tasks have been created
         * then pxCurrentTCB will be NULL. */
        if( pxTCB != NULL )
        {
            ( pxTCB->uxMutexesHeld )++;
        }

        traceRETURN_pvTaskIncrementMutexHeldCount( pxTCB );

        return pxTCB;
    }

/*-----------------------------------------------------------*/


    uint32_t ulTaskGenericNotifyTake( UBaseType_t uxIndexToWaitOn,
                                      BaseType_t xClearCountOnExit,
                                      TickType_t xTicksToWait )
    {
        uint32_t ulReturn;
        BaseType_t xAlreadyYielded, xShouldBlock = pdFALSE;

        traceENTER_ulTaskGenericNotifyTake( uxIndexToWaitOn, xClearCountOnExit, xTicksToWait );

        kernelASSERT( uxIndexToWaitOn < 1 );

        /* If the notification count is zero, and if we are willing to wait for a
         * notification, then block the task and wait. */
        if( ( pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ] == 0U ) && ( xTicksToWait > ( TickType_t ) 0 ) )
        {
            /* We suspend the scheduler here as prvAddCurrentTaskToDelayedList is a
             * non-deterministic operation. */
            vTaskSuspendAll();
            {
                /* We MUST enter a critical section to atomically check if a notification
                 * has occurred and set the flag to indicate that we are waiting for
                 * a notification. If we do not do so, a notification sent from an ISR
                 * will get lost. */
                taskENTER_CRITICAL();
                {
                    /* Only block if the notification count is not already non-zero. */
                    if( pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ] == 0U )
                    {
                        /* Mark this task as waiting for a notification. */
                        pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] = taskWAITING_NOTIFICATION;

                        /* Arrange to wait for a notification. */
                        xShouldBlock = pdTRUE;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
                taskEXIT_CRITICAL();

                /* We are now out of the critical section but the scheduler is still
                 * suspended, so we are safe to do non-deterministic operations such
                 * as prvAddCurrentTaskToDelayedList. */
                if( xShouldBlock == pdTRUE )
                {
                    traceTASK_NOTIFY_TAKE_BLOCK( uxIndexToWaitOn );
                    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            xAlreadyYielded = xTaskResumeAll();

            /* Force a reschedule if xTaskResumeAll has not already done so. */
            if( ( xShouldBlock == pdTRUE ) && ( xAlreadyYielded == pdFALSE ) )
            {
                taskYIELD_WITHIN_API();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        taskENTER_CRITICAL();
        {
            traceTASK_NOTIFY_TAKE( uxIndexToWaitOn );
            ulReturn = pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ];

            if( ulReturn != 0U )
            {
                if( xClearCountOnExit != pdFALSE )
                {
                    pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ] = ( uint32_t ) 0U;
                }
                else
                {
                    pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ] = ulReturn - ( uint32_t ) 1;
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] = taskNOT_WAITING_NOTIFICATION;
        }
        taskEXIT_CRITICAL();

        traceRETURN_ulTaskGenericNotifyTake( ulReturn );

        return ulReturn;
    }

/*-----------------------------------------------------------*/


    BaseType_t xTaskGenericNotifyWait( UBaseType_t uxIndexToWaitOn,
                                       uint32_t ulBitsToClearOnEntry,
                                       uint32_t ulBitsToClearOnExit,
                                       uint32_t * pulNotificationValue,
                                       TickType_t xTicksToWait )
    {
        BaseType_t xReturn, xAlreadyYielded, xShouldBlock = pdFALSE;

        traceENTER_xTaskGenericNotifyWait( uxIndexToWaitOn, ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );

        kernelASSERT( uxIndexToWaitOn < 1 );

        /* If the task hasn't received a notification, and if we are willing to wait
         * for it, then block the task and wait. */
        if( ( pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] != taskNOTIFICATION_RECEIVED ) && ( xTicksToWait > ( TickType_t ) 0 ) )
        {
            /* We suspend the scheduler here as prvAddCurrentTaskToDelayedList is a
             * non-deterministic operation. */
            vTaskSuspendAll();
            {
                /* We MUST enter a critical section to atomically check and update the
                 * task notification value. If we do not do so, a notification from
                 * an ISR will get lost. */
                taskENTER_CRITICAL();
                {
                    /* Only block if a notification is not already pending. */
                    if( pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] != taskNOTIFICATION_RECEIVED )
                    {
                        /* Clear bits in the task's notification value as bits may get
                         * set by the notifying task or interrupt. This can be used
                         * to clear the value to zero. */
                        pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ] &= ~ulBitsToClearOnEntry;

                        /* Mark this task as waiting for a notification. */
                        pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] = taskWAITING_NOTIFICATION;

                        /* Arrange to wait for a notification. */
                        xShouldBlock = pdTRUE;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
                taskEXIT_CRITICAL();

                /* We are now out of the critical section but the scheduler is still
                 * suspended, so we are safe to do non-deterministic operations such
                 * as prvAddCurrentTaskToDelayedList. */
                if( xShouldBlock == pdTRUE )
                {
                    traceTASK_NOTIFY_WAIT_BLOCK( uxIndexToWaitOn );
                    prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            xAlreadyYielded = xTaskResumeAll();

            /* Force a reschedule if xTaskResumeAll has not already done so. */
            if( ( xShouldBlock == pdTRUE ) && ( xAlreadyYielded == pdFALSE ) )
            {
                taskYIELD_WITHIN_API();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        taskENTER_CRITICAL();
        {
            traceTASK_NOTIFY_WAIT( uxIndexToWaitOn );

            if( pulNotificationValue != NULL )
            {
                /* Output the current notification value, which may or may not
                 * have changed. */
                *pulNotificationValue = pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ];
            }

            /* If ucNotifyValue is set then either the task never entered the
             * blocked state (because a notification was already pending) or the
             * task unblocked because of a notification.  Otherwise the task
             * unblocked because of a timeout. */
            if( pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] != taskNOTIFICATION_RECEIVED )
            {
                /* A notification was not received. */
                xReturn = pdFALSE;
            }
            else
            {
                /* A notification was already pending or a notification was
                 * received while the task was waiting. */
                pxCurrentTCB->ulNotifiedValue[ uxIndexToWaitOn ] &= ~ulBitsToClearOnExit;
                xReturn = pdTRUE;
            }

            pxCurrentTCB->ucNotifyState[ uxIndexToWaitOn ] = taskNOT_WAITING_NOTIFICATION;
        }
        taskEXIT_CRITICAL();

        traceRETURN_xTaskGenericNotifyWait( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/


    BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify,
                                   UBaseType_t uxIndexToNotify,
                                   uint32_t ulValue,
                                   eNotifyAction eAction,
                                   uint32_t * pulPreviousNotificationValue )
    {
        TCB_t * pxTCB;
        BaseType_t xReturn = pdPASS;
        uint8_t ucOriginalNotifyState;

        traceENTER_xTaskGenericNotify( xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue );

        kernelASSERT( uxIndexToNotify < 1 );
        kernelASSERT( xTaskToNotify );
        pxTCB = xTaskToNotify;

        taskENTER_CRITICAL();
        {
            if( pulPreviousNotificationValue != NULL )
            {
                *pulPreviousNotificationValue = pxTCB->ulNotifiedValue[ uxIndexToNotify ];
            }

            ucOriginalNotifyState = pxTCB->ucNotifyState[ uxIndexToNotify ];

            pxTCB->ucNotifyState[ uxIndexToNotify ] = taskNOTIFICATION_RECEIVED;

            switch( eAction )
            {
                case eSetBits:
                    pxTCB->ulNotifiedValue[ uxIndexToNotify ] |= ulValue;
                    break;

                case eIncrement:
                    ( pxTCB->ulNotifiedValue[ uxIndexToNotify ] )++;
                    break;

                case eSetValueWithOverwrite:
                    pxTCB->ulNotifiedValue[ uxIndexToNotify ] = ulValue;
                    break;

                case eSetValueWithoutOverwrite:

                    if( ucOriginalNotifyState != taskNOTIFICATION_RECEIVED )
                    {
                        pxTCB->ulNotifiedValue[ uxIndexToNotify ] = ulValue;
                    }
                    else
                    {
                        /* The value could not be written to the task. */
                        xReturn = pdFAIL;
                    }

                    break;

                case eNoAction:

                    /* The task is being notified without its notify value being
                     * updated. */
                    break;

                default:

                    /* Should not get here if all enums are handled.
                     * Artificially force an assert by testing a value the
                     * compiler can't assume is const. */
                    kernelASSERT( xTickCount == ( TickType_t ) 0 );

                    break;
            }

            traceTASK_NOTIFY( uxIndexToNotify );

            /* If the task is in the blocked state specifically to wait for a
             * notification then unblock it now. */
            if( ucOriginalNotifyState == taskWAITING_NOTIFICATION )
            {
                listREMOVE_ITEM( &( pxTCB->xStateListItem ) );
                prvAddTaskToReadyList( pxTCB );

                /* The task should not have been on an event list. */
                kernelASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );


                /* Check if the notified task has a priority above the currently
                 * executing task. */
                taskYIELD_ANY_CORE_IF_USING_PREEMPTION( pxTCB );
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        taskEXIT_CRITICAL();

        traceRETURN_xTaskGenericNotify( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/


    BaseType_t xTaskGenericNotifyFromISR( TaskHandle_t xTaskToNotify,
                                          UBaseType_t uxIndexToNotify,
                                          uint32_t ulValue,
                                          eNotifyAction eAction,
                                          uint32_t * pulPreviousNotificationValue,
                                          BaseType_t * pxHigherPriorityTaskWoken )
    {
        TCB_t * pxTCB;
        uint8_t ucOriginalNotifyState;
        BaseType_t xReturn = pdPASS;
        UBaseType_t uxSavedInterruptStatus;

        traceENTER_xTaskGenericNotifyFromISR( xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken );

        kernelASSERT( xTaskToNotify );
        kernelASSERT( uxIndexToNotify < 1 );

        /* RTOS ports that support interrupt nesting have the concept of a
         * maximum  system call (or maximum API call) interrupt priority.
         * Interrupts that are  above the maximum system call priority are keep
         * permanently enabled, even when the RTOS kernel is in a critical section,
         * but cannot make any calls to FreeRTOS API functions.  If kernelASSERT()
         * is defined in the fixed profile then
         * portASSERT_IF_INTERRUPT_PRIORITY_INVALID() will result in an assertion
         * failure if a FreeRTOS API function is called from an interrupt that has
         * been assigned a priority above the configured maximum system call
         * priority.  Only FreeRTOS functions that end in FromISR can be called
         * from interrupts  that have been assigned a priority at or (logically)
         * below the maximum system call interrupt priority.  FreeRTOS maintains a
         * separate interrupt safe API to ensure interrupt entry is as fast and as
         * simple as possible.  More information (albeit Cortex-M specific) is
         * provided on the following link:
         * https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
        portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

        pxTCB = xTaskToNotify;

        /* MISRA Ref 4.7.1 [Return value shall be checked] */
        /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#dir-47 */
        /* coverity[misra_c_2012_directive_4_7_violation] */
        uxSavedInterruptStatus = ( UBaseType_t ) taskENTER_CRITICAL_FROM_ISR();
        {
            if( pulPreviousNotificationValue != NULL )
            {
                *pulPreviousNotificationValue = pxTCB->ulNotifiedValue[ uxIndexToNotify ];
            }

            ucOriginalNotifyState = pxTCB->ucNotifyState[ uxIndexToNotify ];
            pxTCB->ucNotifyState[ uxIndexToNotify ] = taskNOTIFICATION_RECEIVED;

            switch( eAction )
            {
                case eSetBits:
                    pxTCB->ulNotifiedValue[ uxIndexToNotify ] |= ulValue;
                    break;

                case eIncrement:
                    ( pxTCB->ulNotifiedValue[ uxIndexToNotify ] )++;
                    break;

                case eSetValueWithOverwrite:
                    pxTCB->ulNotifiedValue[ uxIndexToNotify ] = ulValue;
                    break;

                case eSetValueWithoutOverwrite:

                    if( ucOriginalNotifyState != taskNOTIFICATION_RECEIVED )
                    {
                        pxTCB->ulNotifiedValue[ uxIndexToNotify ] = ulValue;
                    }
                    else
                    {
                        /* The value could not be written to the task. */
                        xReturn = pdFAIL;
                    }

                    break;

                case eNoAction:

                    /* The task is being notified without its notify value being
                     * updated. */
                    break;

                default:

                    /* Should not get here if all enums are handled.
                     * Artificially force an assert by testing a value the
                     * compiler can't assume is const. */
                    kernelASSERT( xTickCount == ( TickType_t ) 0 );
                    break;
            }

            traceTASK_NOTIFY_FROM_ISR( uxIndexToNotify );

            /* If the task is in the blocked state specifically to wait for a
             * notification then unblock it now. */
            if( ucOriginalNotifyState == taskWAITING_NOTIFICATION )
            {
                /* The task should not have been on an event list. */
                kernelASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

                if( uxSchedulerSuspended == ( UBaseType_t ) 0U )
                {
                    listREMOVE_ITEM( &( pxTCB->xStateListItem ) );
                    prvAddTaskToReadyList( pxTCB );

                }
                else
                {
                    /* The delayed and ready lists cannot be accessed, so hold
                     * this task pending until the scheduler is resumed. */
                    listINSERT_END( &( xPendingReadyList ), &( pxTCB->xEventListItem ) );
                }

                {
                    if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                    {
                        /* The notified task has a priority above the currently
                         * executing task so a yield is required. */
                        if( pxHigherPriorityTaskWoken != NULL )
                        {
                            *pxHigherPriorityTaskWoken = pdTRUE;
                        }

                        /* Mark that a yield is pending in case the user is not
                         * using the "xHigherPriorityTaskWoken" parameter to an ISR
                         * safe FreeRTOS function. */
                        xYieldPendings[ 0 ] = pdTRUE;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
            }
        }
        taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

        traceRETURN_xTaskGenericNotifyFromISR( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/


    void vTaskGenericNotifyGiveFromISR( TaskHandle_t xTaskToNotify,
                                        UBaseType_t uxIndexToNotify,
                                        BaseType_t * pxHigherPriorityTaskWoken )
    {
        TCB_t * pxTCB;
        uint8_t ucOriginalNotifyState;
        UBaseType_t uxSavedInterruptStatus;

        traceENTER_vTaskGenericNotifyGiveFromISR( xTaskToNotify, uxIndexToNotify, pxHigherPriorityTaskWoken );

        kernelASSERT( xTaskToNotify );
        kernelASSERT( uxIndexToNotify < 1 );

        /* RTOS ports that support interrupt nesting have the concept of a
         * maximum  system call (or maximum API call) interrupt priority.
         * Interrupts that are  above the maximum system call priority are keep
         * permanently enabled, even when the RTOS kernel is in a critical section,
         * but cannot make any calls to FreeRTOS API functions.  If kernelASSERT()
         * is defined in the fixed profile then
         * portASSERT_IF_INTERRUPT_PRIORITY_INVALID() will result in an assertion
         * failure if a FreeRTOS API function is called from an interrupt that has
         * been assigned a priority above the configured maximum system call
         * priority.  Only FreeRTOS functions that end in FromISR can be called
         * from interrupts  that have been assigned a priority at or (logically)
         * below the maximum system call interrupt priority.  FreeRTOS maintains a
         * separate interrupt safe API to ensure interrupt entry is as fast and as
         * simple as possible.  More information (albeit Cortex-M specific) is
         * provided on the following link:
         * https://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
        portASSERT_IF_INTERRUPT_PRIORITY_INVALID();

        pxTCB = xTaskToNotify;

        /* MISRA Ref 4.7.1 [Return value shall be checked] */
        /* More details at: https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/MISRA.md#dir-47 */
        /* coverity[misra_c_2012_directive_4_7_violation] */
        uxSavedInterruptStatus = ( UBaseType_t ) taskENTER_CRITICAL_FROM_ISR();
        {
            ucOriginalNotifyState = pxTCB->ucNotifyState[ uxIndexToNotify ];
            pxTCB->ucNotifyState[ uxIndexToNotify ] = taskNOTIFICATION_RECEIVED;

            /* 'Giving' is equivalent to incrementing a count in a counting
             * semaphore. */
            ( pxTCB->ulNotifiedValue[ uxIndexToNotify ] )++;

            traceTASK_NOTIFY_GIVE_FROM_ISR( uxIndexToNotify );

            /* If the task is in the blocked state specifically to wait for a
             * notification then unblock it now. */
            if( ucOriginalNotifyState == taskWAITING_NOTIFICATION )
            {
                /* The task should not have been on an event list. */
                kernelASSERT( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL );

                if( uxSchedulerSuspended == ( UBaseType_t ) 0U )
                {
                    listREMOVE_ITEM( &( pxTCB->xStateListItem ) );
                    prvAddTaskToReadyList( pxTCB );

                }
                else
                {
                    /* The delayed and ready lists cannot be accessed, so hold
                     * this task pending until the scheduler is resumed. */
                    listINSERT_END( &( xPendingReadyList ), &( pxTCB->xEventListItem ) );
                }

                {
                    if( pxTCB->uxPriority > pxCurrentTCB->uxPriority )
                    {
                        /* The notified task has a priority above the currently
                         * executing task so a yield is required. */
                        if( pxHigherPriorityTaskWoken != NULL )
                        {
                            *pxHigherPriorityTaskWoken = pdTRUE;
                        }

                        /* Mark that a yield is pending in case the user is not
                         * using the "xHigherPriorityTaskWoken" parameter in an ISR
                         * safe FreeRTOS function. */
                        xYieldPendings[ 0 ] = pdTRUE;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
            }
        }
        taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

        traceRETURN_vTaskGenericNotifyGiveFromISR();
    }

/*-----------------------------------------------------------*/


    BaseType_t xTaskGenericNotifyStateClear( TaskHandle_t xTask,
                                             UBaseType_t uxIndexToClear )
    {
        TCB_t * pxTCB;
        BaseType_t xReturn;

        traceENTER_xTaskGenericNotifyStateClear( xTask, uxIndexToClear );

        kernelASSERT( uxIndexToClear < 1 );

        /* If null is passed in here then it is the calling task that is having
         * its notification state cleared. */
        pxTCB = prvGetTCBFromHandle( xTask );
        kernelASSERT( pxTCB != NULL );

        taskENTER_CRITICAL();
        {
            if( pxTCB->ucNotifyState[ uxIndexToClear ] == taskNOTIFICATION_RECEIVED )
            {
                pxTCB->ucNotifyState[ uxIndexToClear ] = taskNOT_WAITING_NOTIFICATION;
                xReturn = pdPASS;
            }
            else
            {
                xReturn = pdFAIL;
            }
        }
        taskEXIT_CRITICAL();

        traceRETURN_xTaskGenericNotifyStateClear( xReturn );

        return xReturn;
    }

/*-----------------------------------------------------------*/


    uint32_t ulTaskGenericNotifyValueClear( TaskHandle_t xTask,
                                            UBaseType_t uxIndexToClear,
                                            uint32_t ulBitsToClear )
    {
        TCB_t * pxTCB;
        uint32_t ulReturn;

        traceENTER_ulTaskGenericNotifyValueClear( xTask, uxIndexToClear, ulBitsToClear );

        kernelASSERT( uxIndexToClear < 1 );

        /* If null is passed in here then it is the calling task that is having
         * its notification state cleared. */
        pxTCB = prvGetTCBFromHandle( xTask );
        kernelASSERT( pxTCB != NULL );

        taskENTER_CRITICAL();
        {
            /* Return the notification as it was before the bits were cleared,
             * then clear the bit mask. */
            ulReturn = pxTCB->ulNotifiedValue[ uxIndexToClear ];
            pxTCB->ulNotifiedValue[ uxIndexToClear ] &= ~ulBitsToClear;
        }
        taskEXIT_CRITICAL();

        traceRETURN_ulTaskGenericNotifyValueClear( ulReturn );

        return ulReturn;
    }

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait,
                                            const BaseType_t xCanBlockIndefinitely )
{
    TickType_t xTimeToWake;
    const TickType_t xConstTickCount = xTickCount;
    List_t * const pxDelayedList = pxDelayedTaskList;
    List_t * const pxOverflowDelayedList = pxOverflowDelayedTaskList;


    /* Remove the task from the ready list before adding it to the blocked list
     * as the same list item is used for both lists. */
    if( uxListRemove( &( pxCurrentTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
    {
        /* The current task must be in a ready list, so there is no need to
         * check, and the port reset macro can be called directly. */
        portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    {
        /* Calculate the time at which the task should be woken if the event
         * does not occur.  This may overflow but this doesn't matter, the kernel
         * will manage it correctly. */
        xTimeToWake = xConstTickCount + xTicksToWait;

        /* The list item will be inserted in wake time order. */
        listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xStateListItem ), xTimeToWake );

        if( xTimeToWake < xConstTickCount )
        {
            traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST();
            /* Wake time has overflowed.  Place this item in the overflow list. */
            vListInsert( pxOverflowDelayedList, &( pxCurrentTCB->xStateListItem ) );
        }
        else
        {
            traceMOVED_TASK_TO_DELAYED_LIST();
            /* The wake time has not overflowed, so the current block list is used. */
            vListInsert( pxDelayedList, &( pxCurrentTCB->xStateListItem ) );

            /* If the task entering the blocked state was placed at the head of the
             * list of blocked tasks then xNextTaskUnblockTime needs to be updated
             * too. */
            if( xTimeToWake < xNextTaskUnblockTime )
            {
                xNextTaskUnblockTime = xTimeToWake;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        /* Avoid compiler warning when 0 is not 1. */
        ( void ) xCanBlockIndefinitely;
    }
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Code below here allows additional code to be inserted into this source file,
 * especially where access to file scope functions and data is needed (for example
 * when performing module tests). */



/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*
 * Reset the state in this file. This state is normally initialized at start up.
 * This function must be called by the application before restarting the
 * scheduler.
 */
void vTaskResetState( void )
{
    BaseType_t xCoreID;

    /* Task control block. */
    {
        pxCurrentTCB = NULL;
    }



    /* Other file private variables. */
    uxCurrentNumberOfTasks = ( UBaseType_t ) 0U;
    xTickCount = ( TickType_t ) 0;
    uxTopReadyPriority = tskIDLE_PRIORITY;
    xSchedulerRunning = pdFALSE;
    xPendedTicks = ( TickType_t ) 0U;

    for( xCoreID = 0; xCoreID < 1; xCoreID++ )
    {
        xYieldPendings[ xCoreID ] = pdFALSE;
    }

    xNumOfOverflows = ( BaseType_t ) 0;
    uxTaskNumber = ( UBaseType_t ) 0U;
    xNextTaskUnblockTime = ( TickType_t ) 0U;

    uxSchedulerSuspended = ( UBaseType_t ) 0U;

}
/*-----------------------------------------------------------*/
