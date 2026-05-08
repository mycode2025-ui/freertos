#pragma once
/*
 * FreeRTOS Kernel V11.3.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#define INC_FREERTOS_H

/*
 * Include the generic headers required for the FreeRTOS port being used.
 */
#include <stddef.h>

/*
 * If stdint.h cannot be located then:
 *   + If using GCC ensure the -nostdint options is *not* being used.
 *   + Ensure the project's include path includes the directory in which your
 *     compiler stores stdint.h.
 *   + Set any compiler options necessary for it to support C99, as technically
 *     stdint.h is only mandatory with C99 (FreeRTOS does not require C99 in any
 *     other way).
 *   + The FreeRTOS download includes a simple stdint.h definition that can be
 *     used in cases where none is provided by the compiler.  The files only
 *     contains the typedefs required to build FreeRTOS.  Read the instructions
 *     in FreeRTOS/source/stdint.readme for more information.
 */
#include <stdint.h> /* READ COMMENT ABOVE. */

/* Fixed pure CM7 static profile. No the fixed profile is used. */
#define kernelASSERT( x )                               do { if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ; ; ) { } } } while( 0 )


/* Acceptable values for 1. */

/* Application specific configuration options. */



/* Define 1 according to the
 * value of fixed-profile option for backward compatibility. */

/* Set fixed-profile option to 1 to use MPU wrappers v1. */

/* Set 0 to 1 to enable access control list support. */

/* Set default value of 1 to 1 to use single core FreeRTOS. */



/* Set 0 and/or 0 to 1 to enable PAC and/or BTI
 * support and 0 to disable them. These are currently used in ARMv8.1-M ports. */


/* Basic FreeRTOS definitions. */
#include "projdefs.h"

/* Definitions specific to the port being used. */
#include "portable.h"

/* Must be defaulted before 0 is used below. */

/* Required if struct _reent is used. */

/* Must be defaulted before 0 is used below. */


/* *INDENT-OFF* */
/* *INDENT-ON* */



/*
 * Check all the required application specific macros have been defined.
 * These macros are application specific and (as downloaded) are defined
 * within the fixed profile.
 */















































/* kernelPRECONDITION should be defined as kernelASSERT.
 * The CBMC proofs need a way to track assumptions and assertions.
 * A kernelPRECONDITION statement should express an implicit invariant or
 * assumption made.  A kernelASSERT statement should express an invariant that must
 * hold explicit before calling the code. */



    #define portSOFTWARE_BARRIER()



        #define portGET_CORE_ID()    0



        #define portYIELD_CORE( x )    portYIELD()









        #define portRELEASE_TASK_LOCK( xCoreID )



        #define portGET_TASK_LOCK( xCoreID )



        #define portRELEASE_ISR_LOCK( xCoreID )



        #define portGET_ISR_LOCK( xCoreID )











/* The timers module relies on xTaskGetSchedulerState(). */




    #define portCLEAN_UP_TCB( pxTCB )    ( void ) ( pxTCB )

    #define portPRE_TASK_DELETE_HOOK( pvTaskToDelete, pxYieldPending )

    #define portSETUP_TCB( pxTCB )    ( void ) ( pxTCB )

    #define portTASK_SWITCH_HOOK( pxTCB )    ( void ) ( pxTCB )


    #define vQueueAddToRegistry( xQueue, pcName )
    #define vQueueUnregisterQueue( xQueue )
    #define pcQueueGetName( xQueue )


    #define portPOINTER_SIZE_TYPE    uint32_t

/* Remove any unused trace macros. */

/* Used to perform any necessary initialisation - for example, open a file
 * into which trace is to be written. */
    #define traceSTART()


/* Use to close a trace, for example close a file into which trace has been
 * written. */
    #define traceEND()


/* Called after a task has been selected to run.  pxCurrentTCB holds a pointer
 * to the task control block of the selected task. */
    #define traceTASK_SWITCHED_IN()


/* Called after all idle tasks and timer task (if enabled) have been created
 * successfully, just before the scheduler is started. */
    #define traceSTARTING_SCHEDULER( xIdleTaskHandles )


/* Called before stepping the tick count after waking from tickless idle
 * sleep. */
    #define traceINCREASE_TICK_COUNT( x )

    /* Called immediately before entering tickless idle. */
    #define traceLOW_POWER_IDLE_BEGIN()

    /* Called when returning to the Idle task after a tickless idle. */
    #define traceLOW_POWER_IDLE_END()


/* Called before a task has been selected to run.  pxCurrentTCB holds a pointer
 * to the task control block of the task being switched out. */
    #define traceTASK_SWITCHED_OUT()


/* Called when a task attempts to take a mutex that is already held by a
 * lower priority task.  pxTCBOfMutexHolder is a pointer to the TCB of the task
 * that holds the mutex.  uxInheritedPriority is the priority the mutex holder
 * will inherit (the priority of the task that is attempting to obtain the
 * muted. */
    #define traceTASK_PRIORITY_INHERIT( pxTCBOfMutexHolder, uxInheritedPriority )


/* Called when a task releases a mutex, the holding of which had resulted in
 * the task inheriting the priority of a higher priority task.
 * pxTCBOfMutexHolder is a pointer to the TCB of the task that is releasing the
 * mutex.  uxOriginalPriority is the task's configured (base) priority. */
    #define traceTASK_PRIORITY_DISINHERIT( pxTCBOfMutexHolder, uxOriginalPriority )


/* Task is about to block because it cannot read from a
 * queue/mutex/semaphore.  pxQueue is a pointer to the queue/mutex/semaphore
 * upon which the read was attempted.  pxCurrentTCB points to the TCB of the
 * task that attempted the read. */
    #define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue )


/* Task is about to block because it cannot read from a
 * queue/mutex/semaphore.  pxQueue is a pointer to the queue/mutex/semaphore
 * upon which the read was attempted.  pxCurrentTCB points to the TCB of the
 * task that attempted the read. */
    #define traceBLOCKING_ON_QUEUE_PEEK( pxQueue )


/* Task is about to block because it cannot write to a
 * queue/mutex/semaphore.  pxQueue is a pointer to the queue/mutex/semaphore
 * upon which the write was attempted.  pxCurrentTCB points to the TCB of the
 * task that attempted the write. */
    #define traceBLOCKING_ON_QUEUE_SEND( pxQueue )




/* The following event macros are embedded in the kernel API calls. */

    #define traceMOVED_TASK_TO_READY_STATE( pxTCB )

    #define tracePOST_MOVED_TASK_TO_READY_STATE( pxTCB )

    #define traceMOVED_TASK_TO_DELAYED_LIST()

    #define traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST()

    #define traceQUEUE_CREATE( pxNewQueue )

    #define traceQUEUE_CREATE_FAILED( ucQueueType )

    #define traceCREATE_MUTEX( pxNewQueue )

    #define traceCREATE_MUTEX_FAILED()

    #define traceGIVE_MUTEX_RECURSIVE( pxMutex )

    #define traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex )

    #define traceTAKE_MUTEX_RECURSIVE( pxMutex )

    #define traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex )

    #define traceCREATE_COUNTING_SEMAPHORE()

    #define traceCREATE_COUNTING_SEMAPHORE_FAILED()

    #define traceQUEUE_SET_SEND    traceQUEUE_SEND

    #define traceQUEUE_SEND( pxQueue )

    #define traceQUEUE_SEND_FAILED( pxQueue )

    #define traceQUEUE_RECEIVE( pxQueue )

    #define traceQUEUE_PEEK( pxQueue )

    #define traceQUEUE_PEEK_FAILED( pxQueue )

    #define traceQUEUE_PEEK_FROM_ISR( pxQueue )

    #define traceQUEUE_RECEIVE_FAILED( pxQueue )

    #define traceQUEUE_SEND_FROM_ISR( pxQueue )

    #define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue )

    #define traceQUEUE_RECEIVE_FROM_ISR( pxQueue )

    #define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue )

    #define traceQUEUE_PEEK_FROM_ISR_FAILED( pxQueue )

    #define traceQUEUE_DELETE( pxQueue )

    #define traceTASK_CREATE( pxNewTCB )

    #define traceTASK_CREATE_FAILED()

    #define traceTASK_DELETE( pxTaskToDelete )

    #define traceTASK_DELAY_UNTIL( x )

    #define traceTASK_DELAY()

    #define traceTASK_PRIORITY_SET( pxTask, uxNewPriority )

    #define traceTASK_SUSPEND( pxTaskToSuspend )

    #define traceTASK_RESUME( pxTaskToResume )

    #define traceTASK_RESUME_FROM_ISR( pxTaskToResume )

    #define traceTASK_INCREMENT_TICK( xTickCount )

    #define traceTIMER_CREATE( pxNewTimer )

    #define traceTIMER_CREATE_FAILED()

    #define traceTIMER_COMMAND_SEND( xTimer, xMessageID, xMessageValueValue, xReturn )

    #define traceTIMER_EXPIRED( pxTimer )

    #define traceTIMER_COMMAND_RECEIVED( pxTimer, xMessageID, xMessageValue )

    #define traceMALLOC( pvAddress, uiSize )

    #define traceFREE( pvAddress, uiSize )

    #define traceEVENT_GROUP_CREATE( xEventGroup )

    #define traceEVENT_GROUP_CREATE_FAILED()

    #define traceEVENT_GROUP_SYNC_BLOCK( xEventGroup, uxBitsToSet, uxBitsToWaitFor )

    #define traceEVENT_GROUP_SYNC_END( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred )    ( void ) ( xTimeoutOccurred )

    #define traceEVENT_GROUP_WAIT_BITS_BLOCK( xEventGroup, uxBitsToWaitFor )

    #define traceEVENT_GROUP_WAIT_BITS_END( xEventGroup, uxBitsToWaitFor, xTimeoutOccurred )    ( void ) ( xTimeoutOccurred )

    #define traceEVENT_GROUP_CLEAR_BITS( xEventGroup, uxBitsToClear )

    #define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR( xEventGroup, uxBitsToClear )

    #define traceEVENT_GROUP_SET_BITS( xEventGroup, uxBitsToSet )

    #define traceEVENT_GROUP_SET_BITS_FROM_ISR( xEventGroup, uxBitsToSet )

    #define traceEVENT_GROUP_DELETE( xEventGroup )

    #define tracePEND_FUNC_CALL( xFunctionToPend, pvParameter1, ulParameter2, ret )

    #define tracePEND_FUNC_CALL_FROM_ISR( xFunctionToPend, pvParameter1, ulParameter2, ret )

    #define traceQUEUE_REGISTRY_ADD( xQueue, pcQueueName )

    #define traceTASK_NOTIFY_TAKE_BLOCK( uxIndexToWait )

    #define traceTASK_NOTIFY_TAKE( uxIndexToWait )

    #define traceTASK_NOTIFY_WAIT_BLOCK( uxIndexToWait )

    #define traceTASK_NOTIFY_WAIT( uxIndexToWait )

    #define traceTASK_NOTIFY( uxIndexToNotify )

    #define traceTASK_NOTIFY_FROM_ISR( uxIndexToNotify )

    #define traceTASK_NOTIFY_GIVE_FROM_ISR( uxIndexToNotify )

    #define traceISR_EXIT_TO_SCHEDULER()

    #define traceISR_EXIT()

    #define traceISR_ENTER()

    #define traceSTREAM_BUFFER_CREATE_FAILED( xStreamBufferType )

    #define traceSTREAM_BUFFER_CREATE_STATIC_FAILED( xReturn, xStreamBufferType )

    #define traceSTREAM_BUFFER_CREATE( pxStreamBuffer, xStreamBufferType )

    #define traceSTREAM_BUFFER_DELETE( xStreamBuffer )

    #define traceSTREAM_BUFFER_RESET( xStreamBuffer )

    #define traceSTREAM_BUFFER_RESET_FROM_ISR( xStreamBuffer )

    #define traceBLOCKING_ON_STREAM_BUFFER_SEND( xStreamBuffer )

    #define traceSTREAM_BUFFER_SEND( xStreamBuffer, xBytesSent )

    #define traceSTREAM_BUFFER_SEND_FAILED( xStreamBuffer )

    #define traceSTREAM_BUFFER_SEND_FROM_ISR( xStreamBuffer, xBytesSent )

    #define traceBLOCKING_ON_STREAM_BUFFER_RECEIVE( xStreamBuffer )

    #define traceSTREAM_BUFFER_RECEIVE( xStreamBuffer, xReceivedLength )

    #define traceSTREAM_BUFFER_RECEIVE_FAILED( xStreamBuffer )

    #define traceSTREAM_BUFFER_RECEIVE_FROM_ISR( xStreamBuffer, xReceivedLength )

    #define traceENTER_xEventGroupCreateStatic( pxEventGroupBuffer )

    #define traceRETURN_xEventGroupCreateStatic( pxEventBits )

    #define traceENTER_xEventGroupCreate()

    #define traceRETURN_xEventGroupCreate( pxEventBits )

    #define traceENTER_xEventGroupSync( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait )

    #define traceRETURN_xEventGroupSync( uxReturn )

    #define traceENTER_xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait )

    #define traceRETURN_xEventGroupWaitBits( uxReturn )

    #define traceENTER_xEventGroupClearBits( xEventGroup, uxBitsToClear )

    #define traceRETURN_xEventGroupClearBits( uxReturn )

    #define traceENTER_xEventGroupClearBitsFromISR( xEventGroup, uxBitsToClear )

    #define traceRETURN_xEventGroupClearBitsFromISR( xReturn )

    #define traceENTER_xEventGroupGetBitsFromISR( xEventGroup )

    #define traceRETURN_xEventGroupGetBitsFromISR( uxReturn )

    #define traceENTER_xEventGroupSetBits( xEventGroup, uxBitsToSet )

    #define traceRETURN_xEventGroupSetBits( uxEventBits )

    #define traceENTER_vEventGroupDelete( xEventGroup )

    #define traceRETURN_vEventGroupDelete()

    #define traceENTER_xEventGroupGetStaticBuffer( xEventGroup, ppxEventGroupBuffer )

    #define traceRETURN_xEventGroupGetStaticBuffer( xReturn )

    #define traceENTER_vEventGroupSetBitsCallback( pvEventGroup, ulBitsToSet )

    #define traceRETURN_vEventGroupSetBitsCallback()

    #define traceENTER_vEventGroupClearBitsCallback( pvEventGroup, ulBitsToClear )

    #define traceRETURN_vEventGroupClearBitsCallback()

    #define traceENTER_xEventGroupSetBitsFromISR( xEventGroup, uxBitsToSet, pxHigherPriorityTaskWoken )

    #define traceRETURN_xEventGroupSetBitsFromISR( xReturn )

    #define traceENTER_uxEventGroupGetNumber( xEventGroup )

    #define traceRETURN_uxEventGroupGetNumber( xReturn )

    #define traceENTER_vEventGroupSetNumber( xEventGroup, uxEventGroupNumber )

    #define traceRETURN_vEventGroupSetNumber()

    #define traceENTER_xQueueGenericReset( xQueue, xNewQueue )

    #define traceRETURN_xQueueGenericReset( xReturn )

    #define traceENTER_xQueueGenericCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue, ucQueueType )

    #define traceRETURN_xQueueGenericCreateStatic( pxNewQueue )

    #define traceENTER_xQueueGenericGetStaticBuffers( xQueue, ppucQueueStorage, ppxStaticQueue )

    #define traceRETURN_xQueueGenericGetStaticBuffers( xReturn )

    #define traceENTER_xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType )

    #define traceRETURN_xQueueGenericCreate( pxNewQueue )

    #define traceENTER_xQueueCreateMutex( ucQueueType )

    #define traceRETURN_xQueueCreateMutex( xNewQueue )

    #define traceENTER_xQueueCreateMutexStatic( ucQueueType, pxStaticQueue )

    #define traceRETURN_xQueueCreateMutexStatic( xNewQueue )

    #define traceENTER_xQueueGetMutexHolder( xSemaphore )

    #define traceRETURN_xQueueGetMutexHolder( pxReturn )

    #define traceENTER_xQueueGetMutexHolderFromISR( xSemaphore )

    #define traceRETURN_xQueueGetMutexHolderFromISR( pxReturn )

    #define traceENTER_xQueueGiveMutexRecursive( xMutex )

    #define traceRETURN_xQueueGiveMutexRecursive( xReturn )

    #define traceENTER_xQueueTakeMutexRecursive( xMutex, xTicksToWait )

    #define traceRETURN_xQueueTakeMutexRecursive( xReturn )

    #define traceENTER_xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, pxStaticQueue )

    #define traceRETURN_xQueueCreateCountingSemaphoreStatic( xHandle )

    #define traceENTER_xQueueCreateCountingSemaphore( uxMaxCount, uxInitialCount )

    #define traceRETURN_xQueueCreateCountingSemaphore( xHandle )

    #define traceENTER_xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition )

    #define traceRETURN_xQueueGenericSend( xReturn )

    #define traceENTER_xQueueGenericSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken, xCopyPosition )

    #define traceRETURN_xQueueGenericSendFromISR( xReturn )

    #define traceENTER_xQueueGiveFromISR( xQueue, pxHigherPriorityTaskWoken )

    #define traceRETURN_xQueueGiveFromISR( xReturn )

    #define traceENTER_xQueueReceive( xQueue, pvBuffer, xTicksToWait )

    #define traceRETURN_xQueueReceive( xReturn )

    #define traceENTER_xQueueSemaphoreTake( xQueue, xTicksToWait )

    #define traceRETURN_xQueueSemaphoreTake( xReturn )

    #define traceENTER_xQueuePeek( xQueue, pvBuffer, xTicksToWait )

    #define traceRETURN_xQueuePeek( xReturn )

    #define traceENTER_xQueueReceiveFromISR( xQueue, pvBuffer, pxHigherPriorityTaskWoken )

    #define traceRETURN_xQueueReceiveFromISR( xReturn )

    #define traceENTER_xQueuePeekFromISR( xQueue, pvBuffer )

    #define traceRETURN_xQueuePeekFromISR( xReturn )

    #define traceENTER_uxQueueMessagesWaiting( xQueue )

    #define traceRETURN_uxQueueMessagesWaiting( uxReturn )

    #define traceENTER_uxQueueSpacesAvailable( xQueue )

    #define traceRETURN_uxQueueSpacesAvailable( uxReturn )

    #define traceENTER_uxQueueMessagesWaitingFromISR( xQueue )

    #define traceRETURN_uxQueueMessagesWaitingFromISR( uxReturn )

    #define traceENTER_vQueueDelete( xQueue )

    #define traceRETURN_vQueueDelete()

    #define traceENTER_uxQueueGetQueueNumber( xQueue )

    #define traceRETURN_uxQueueGetQueueNumber( uxQueueNumber )

    #define traceENTER_vQueueSetQueueNumber( xQueue, uxQueueNumber )

    #define traceRETURN_vQueueSetQueueNumber()

    #define traceENTER_ucQueueGetQueueType( xQueue )

    #define traceRETURN_ucQueueGetQueueType( ucQueueType )

    #define traceENTER_uxQueueGetQueueItemSize( xQueue )

    #define traceRETURN_uxQueueGetQueueItemSize( uxItemSize )

    #define traceENTER_uxQueueGetQueueLength( xQueue )

    #define traceRETURN_uxQueueGetQueueLength( uxLength )

    #define traceENTER_xQueueIsQueueEmptyFromISR( xQueue )

    #define traceRETURN_xQueueIsQueueEmptyFromISR( xReturn )

    #define traceENTER_xQueueIsQueueFullFromISR( xQueue )

    #define traceRETURN_xQueueIsQueueFullFromISR( xReturn )

    #define traceENTER_xQueueCRSend( xQueue, pvItemToQueue, xTicksToWait )

    #define traceRETURN_xQueueCRSend( xReturn )

    #define traceENTER_xQueueCRReceive( xQueue, pvBuffer, xTicksToWait )

    #define traceRETURN_xQueueCRReceive( xReturn )

    #define traceENTER_xQueueCRSendFromISR( xQueue, pvItemToQueue, xCoRoutinePreviouslyWoken )

    #define traceRETURN_xQueueCRSendFromISR( xCoRoutinePreviouslyWoken )

    #define traceENTER_xQueueCRReceiveFromISR( xQueue, pvBuffer, pxCoRoutineWoken )

    #define traceRETURN_xQueueCRReceiveFromISR( xReturn )

    #define traceENTER_vQueueAddToRegistry( xQueue, pcQueueName )

    #define traceRETURN_vQueueAddToRegistry()

    #define traceENTER_pcQueueGetName( xQueue )

    #define traceRETURN_pcQueueGetName( pcReturn )

    #define traceENTER_vQueueUnregisterQueue( xQueue )

    #define traceRETURN_vQueueUnregisterQueue()

    #define traceENTER_vQueueWaitForMessageRestricted( xQueue, xTicksToWait, xWaitIndefinitely )

    #define traceRETURN_vQueueWaitForMessageRestricted()

    #define traceENTER_xQueueCreateSet( uxEventQueueLength )

    #define traceRETURN_xQueueCreateSet( pxQueue )

    #define traceENTER_xQueueCreateSetStatic( uxEventQueueLength )

    #define traceRETURN_xQueueCreateSetStatic( pxQueue )

    #define traceENTER_xQueueAddToSet( xQueueOrSemaphore, xQueueSet )

    #define traceRETURN_xQueueAddToSet( xReturn )

    #define traceENTER_xQueueRemoveFromSet( xQueueOrSemaphore, xQueueSet )

    #define traceRETURN_xQueueRemoveFromSet( xReturn )

    #define traceENTER_xQueueSelectFromSet( xQueueSet, xTicksToWait )

    #define traceRETURN_xQueueSelectFromSet( xReturn )

    #define traceENTER_xQueueSelectFromSetFromISR( xQueueSet )

    #define traceRETURN_xQueueSelectFromSetFromISR( xReturn )

    #define traceENTER_xTimerCreateTimerTask()

    #define traceRETURN_xTimerCreateTimerTask( xReturn )

    #define traceENTER_xTimerCreate( pcTimerName, xTimerPeriodInTicks, xAutoReload, pvTimerID, pxCallbackFunction )

    #define traceRETURN_xTimerCreate( pxNewTimer )

    #define traceENTER_xTimerCreateStatic( pcTimerName, xTimerPeriodInTicks, xAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer )

    #define traceRETURN_xTimerCreateStatic( pxNewTimer )

    #define traceENTER_xTimerGenericCommandFromTask( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait )

    #define traceRETURN_xTimerGenericCommandFromTask( xReturn )

    #define traceENTER_xTimerGenericCommandFromISR( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait )

    #define traceRETURN_xTimerGenericCommandFromISR( xReturn )

    #define traceENTER_xTimerGetTimerDaemonTaskHandle()

    #define traceRETURN_xTimerGetTimerDaemonTaskHandle( xTimerTaskHandle )

    #define traceENTER_xTimerGetPeriod( xTimer )

    #define traceRETURN_xTimerGetPeriod( xTimerPeriodInTicks )

    #define traceENTER_vTimerSetReloadMode( xTimer, xAutoReload )

    #define traceRETURN_vTimerSetReloadMode()

    #define traceENTER_xTimerGetReloadMode( xTimer )

    #define traceRETURN_xTimerGetReloadMode( xReturn )

    #define traceENTER_uxTimerGetReloadMode( xTimer )

    #define traceRETURN_uxTimerGetReloadMode( uxReturn )

    #define traceENTER_xTimerGetExpiryTime( xTimer )

    #define traceRETURN_xTimerGetExpiryTime( xReturn )

    #define traceENTER_xTimerGetStaticBuffer( xTimer, ppxTimerBuffer )

    #define traceRETURN_xTimerGetStaticBuffer( xReturn )

    #define traceENTER_pcTimerGetName( xTimer )

    #define traceRETURN_pcTimerGetName( pcTimerName )

    #define traceENTER_xTimerIsTimerActive( xTimer )

    #define traceRETURN_xTimerIsTimerActive( xReturn )

    #define traceENTER_pvTimerGetTimerID( xTimer )

    #define traceRETURN_pvTimerGetTimerID( pvReturn )

    #define traceENTER_vTimerSetTimerID( xTimer, pvNewID )

    #define traceRETURN_vTimerSetTimerID()

    #define traceENTER_xTimerPendFunctionCallFromISR( xFunctionToPend, pvParameter1, ulParameter2, pxHigherPriorityTaskWoken )

    #define traceRETURN_xTimerPendFunctionCallFromISR( xReturn )

    #define traceENTER_xTimerPendFunctionCall( xFunctionToPend, pvParameter1, ulParameter2, xTicksToWait )

    #define traceRETURN_xTimerPendFunctionCall( xReturn )

    #define traceENTER_uxTimerGetTimerNumber( xTimer )

    #define traceRETURN_uxTimerGetTimerNumber( uxTimerNumber )

    #define traceENTER_vTimerSetTimerNumber( xTimer, uxTimerNumber )

    #define traceRETURN_vTimerSetTimerNumber()

    #define traceENTER_xTaskCreateStatic( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer )

    #define traceRETURN_xTaskCreateStatic( xReturn )

    #define traceENTER_xTaskCreateStaticAffinitySet( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer, uxCoreAffinityMask )

    #define traceRETURN_xTaskCreateStaticAffinitySet( xReturn )

    #define traceENTER_xTaskCreateRestrictedStatic( pxTaskDefinition, pxCreatedTask )

    #define traceRETURN_xTaskCreateRestrictedStatic( xReturn )

    #define traceENTER_xTaskCreateRestrictedStaticAffinitySet( pxTaskDefinition, uxCoreAffinityMask, pxCreatedTask )

    #define traceRETURN_xTaskCreateRestrictedStaticAffinitySet( xReturn )

    #define traceENTER_xTaskCreateRestricted( pxTaskDefinition, pxCreatedTask )

    #define traceRETURN_xTaskCreateRestricted( xReturn )

    #define traceENTER_xTaskCreateRestrictedAffinitySet( pxTaskDefinition, uxCoreAffinityMask, pxCreatedTask )

    #define traceRETURN_xTaskCreateRestrictedAffinitySet( xReturn )

    #define traceENTER_xTaskCreate( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, pxCreatedTask )

    #define traceRETURN_xTaskCreate( xReturn )

    #define traceENTER_xTaskCreateAffinitySet( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, uxCoreAffinityMask, pxCreatedTask )

    #define traceRETURN_xTaskCreateAffinitySet( xReturn )

    #define traceENTER_vTaskDelete( xTaskToDelete )

    #define traceRETURN_vTaskDelete()

    #define traceENTER_xTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement )

    #define traceRETURN_xTaskDelayUntil( xShouldDelay )

    #define traceENTER_vTaskDelay( xTicksToDelay )

    #define traceRETURN_vTaskDelay()

    #define traceENTER_eTaskGetState( xTask )

    #define traceRETURN_eTaskGetState( eReturn )

    #define traceENTER_uxTaskPriorityGet( xTask )

    #define traceRETURN_uxTaskPriorityGet( uxReturn )

    #define traceENTER_uxTaskPriorityGetFromISR( xTask )

    #define traceRETURN_uxTaskPriorityGetFromISR( uxReturn )

    #define traceENTER_uxTaskBasePriorityGet( xTask )

    #define traceRETURN_uxTaskBasePriorityGet( uxReturn )

    #define traceENTER_uxTaskBasePriorityGetFromISR( xTask )

    #define traceRETURN_uxTaskBasePriorityGetFromISR( uxReturn )

    #define traceENTER_vTaskPrioritySet( xTask, uxNewPriority )

    #define traceRETURN_vTaskPrioritySet()

    #define traceENTER_vTaskCoreAffinitySet( xTask, uxCoreAffinityMask )

    #define traceRETURN_vTaskCoreAffinitySet()

    #define traceENTER_vTaskCoreAffinityGet( xTask )

    #define traceRETURN_vTaskCoreAffinityGet( uxCoreAffinityMask )

    #define traceENTER_vTaskPreemptionDisable( xTask )

    #define traceRETURN_vTaskPreemptionDisable()

    #define traceENTER_vTaskPreemptionEnable( xTask )

    #define traceRETURN_vTaskPreemptionEnable()

    #define traceENTER_vTaskSuspend( xTaskToSuspend )

    #define traceRETURN_vTaskSuspend()

    #define traceENTER_vTaskResume( xTaskToResume )

    #define traceRETURN_vTaskResume()

    #define traceENTER_xTaskResumeFromISR( xTaskToResume )

    #define traceRETURN_xTaskResumeFromISR( xYieldRequired )

    #define traceENTER_vTaskStartScheduler()

    #define traceRETURN_vTaskStartScheduler()

    #define traceENTER_vTaskEndScheduler()

    #define traceRETURN_vTaskEndScheduler()

    #define traceENTER_vTaskSuspendAll()

    #define traceRETURN_vTaskSuspendAll()

    #define traceENTER_xTaskResumeAll()

    #define traceRETURN_xTaskResumeAll( xAlreadyYielded )

    #define traceENTER_xTaskGetTickCount()

    #define traceRETURN_xTaskGetTickCount( xTicks )

    #define traceENTER_xTaskGetTickCountFromISR()

    #define traceRETURN_xTaskGetTickCountFromISR( xReturn )

    #define traceENTER_uxTaskGetNumberOfTasks()

    #define traceRETURN_uxTaskGetNumberOfTasks( uxCurrentNumberOfTasks )

    #define traceENTER_pcTaskGetName( xTaskToQuery )

    #define traceRETURN_pcTaskGetName( pcTaskName )

    #define traceENTER_xTaskGetHandle( pcNameToQuery )

    #define traceRETURN_xTaskGetHandle( pxTCB )

    #define traceENTER_xTaskGetStaticBuffers( xTask, ppuxStackBuffer, ppxTaskBuffer )

    #define traceRETURN_xTaskGetStaticBuffers( xReturn )

    #define traceENTER_uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime )

    #define traceRETURN_uxTaskGetSystemState( uxTask )

        #define traceENTER_xTaskGetIdleTaskHandle()

        #define traceRETURN_xTaskGetIdleTaskHandle( xIdleTaskHandle )

    #define traceENTER_xTaskGetIdleTaskHandleForCore( xCoreID )

    #define traceRETURN_xTaskGetIdleTaskHandleForCore( xIdleTaskHandle )

    #define traceENTER_vTaskStepTick( xTicksToJump )

    #define traceRETURN_vTaskStepTick()

    #define traceENTER_xTaskCatchUpTicks( xTicksToCatchUp )

    #define traceRETURN_xTaskCatchUpTicks( xYieldOccurred )

    #define traceENTER_xTaskAbortDelay( xTask )

    #define traceRETURN_xTaskAbortDelay( xReturn )

    #define traceENTER_xTaskIncrementTick()

    #define traceRETURN_xTaskIncrementTick( xSwitchRequired )

    #define traceENTER_vTaskSetApplicationTaskTag( xTask, pxHookFunction )

    #define traceRETURN_vTaskSetApplicationTaskTag()

    #define traceENTER_xTaskGetApplicationTaskTag( xTask )

    #define traceRETURN_xTaskGetApplicationTaskTag( xReturn )

    #define traceENTER_xTaskGetApplicationTaskTagFromISR( xTask )

    #define traceRETURN_xTaskGetApplicationTaskTagFromISR( xReturn )

    #define traceENTER_xTaskCallApplicationTaskHook( xTask, pvParameter )

    #define traceRETURN_xTaskCallApplicationTaskHook( xReturn )

    #define traceENTER_vTaskSwitchContext()

    #define traceRETURN_vTaskSwitchContext()

    #define traceENTER_vTaskPlaceOnEventList( pxEventList, xTicksToWait )

    #define traceRETURN_vTaskPlaceOnEventList()

    #define traceENTER_vTaskPlaceOnUnorderedEventList( pxEventList, xItemValue, xTicksToWait )

    #define traceRETURN_vTaskPlaceOnUnorderedEventList()

    #define traceENTER_vTaskPlaceOnEventListRestricted( pxEventList, xTicksToWait, xWaitIndefinitely )

    #define traceRETURN_vTaskPlaceOnEventListRestricted()

    #define traceENTER_xTaskRemoveFromEventList( pxEventList )

    #define traceRETURN_xTaskRemoveFromEventList( xReturn )

    #define traceENTER_vTaskRemoveFromUnorderedEventList( pxEventListItem, xItemValue )

    #define traceRETURN_vTaskRemoveFromUnorderedEventList()

    #define traceENTER_vTaskSetTimeOutState( pxTimeOut )

    #define traceRETURN_vTaskSetTimeOutState()

    #define traceENTER_vTaskInternalSetTimeOutState( pxTimeOut )

    #define traceRETURN_vTaskInternalSetTimeOutState()

    #define traceENTER_xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait )

    #define traceRETURN_xTaskCheckForTimeOut( xReturn )

    #define traceENTER_vTaskMissedYield()

    #define traceRETURN_vTaskMissedYield()

    #define traceENTER_uxTaskGetTaskNumber( xTask )

    #define traceRETURN_uxTaskGetTaskNumber( uxReturn )

    #define traceENTER_vTaskSetTaskNumber( xTask, uxHandle )

    #define traceRETURN_vTaskSetTaskNumber()

    #define traceENTER_eTaskConfirmSleepModeStatus()

    #define traceRETURN_eTaskConfirmSleepModeStatus( eReturn )

    #define traceENTER_vTaskSetThreadLocalStoragePointer( xTaskToSet, xIndex, pvValue )

    #define traceRETURN_vTaskSetThreadLocalStoragePointer()

    #define traceENTER_pvTaskGetThreadLocalStoragePointer( xTaskToQuery, xIndex )

    #define traceRETURN_pvTaskGetThreadLocalStoragePointer( pvReturn )

    #define traceENTER_vTaskAllocateMPURegions( xTaskToModify, pxRegions )

    #define traceRETURN_vTaskAllocateMPURegions()

    #define traceENTER_vTaskGetInfo( xTask, pxTaskStatus, xGetFreeStackSpace, eState )

    #define traceRETURN_vTaskGetInfo()

    #define traceENTER_uxTaskGetStackHighWaterMark2( xTask )

    #define traceRETURN_uxTaskGetStackHighWaterMark2( uxReturn )

    #define traceENTER_uxTaskGetStackHighWaterMark( xTask )

    #define traceRETURN_uxTaskGetStackHighWaterMark( uxReturn )

    #define traceENTER_xTaskGetCurrentTaskHandle()

    #define traceRETURN_xTaskGetCurrentTaskHandle( xReturn )

    #define traceENTER_xTaskGetCurrentTaskHandleForCore( xCoreID )

    #define traceRETURN_xTaskGetCurrentTaskHandleForCore( xReturn )

    #define traceENTER_xTaskGetSchedulerState()

    #define traceRETURN_xTaskGetSchedulerState( xReturn )

    #define traceENTER_xTaskPriorityInherit( pxMutexHolder )

    #define traceRETURN_xTaskPriorityInherit( xReturn )

    #define traceENTER_xTaskPriorityDisinherit( pxMutexHolder )

    #define traceRETURN_xTaskPriorityDisinherit( xReturn )

    #define traceENTER_vTaskPriorityDisinheritAfterTimeout( pxMutexHolder, uxHighestPriorityWaitingTask )

    #define traceRETURN_vTaskPriorityDisinheritAfterTimeout()

    #define traceENTER_vTaskYieldWithinAPI()

    #define traceRETURN_vTaskYieldWithinAPI()

    #define traceENTER_vTaskEnterCritical()

    #define traceRETURN_vTaskEnterCritical()

    #define traceENTER_vTaskEnterCriticalFromISR()

    #define traceRETURN_vTaskEnterCriticalFromISR( uxSavedInterruptStatus )

    #define traceENTER_vTaskExitCritical()

    #define traceRETURN_vTaskExitCritical()

    #define traceENTER_vTaskExitCriticalFromISR( uxSavedInterruptStatus )

    #define traceRETURN_vTaskExitCriticalFromISR()

    #define traceENTER_vTaskListTasks( pcWriteBuffer, uxBufferLength )

    #define traceRETURN_vTaskListTasks()

    #define traceENTER_vTaskGetRunTimeStatistics( pcWriteBuffer, uxBufferLength )

    #define traceRETURN_vTaskGetRunTimeStatistics()

    #define traceENTER_uxTaskResetEventItemValue()

    #define traceRETURN_uxTaskResetEventItemValue( uxReturn )

    #define traceENTER_pvTaskIncrementMutexHeldCount()

    #define traceRETURN_pvTaskIncrementMutexHeldCount( pxTCB )

    #define traceENTER_ulTaskGenericNotifyTake( uxIndexToWaitOn, xClearCountOnExit, xTicksToWait )

    #define traceRETURN_ulTaskGenericNotifyTake( ulReturn )

    #define traceENTER_xTaskGenericNotifyWait( uxIndexToWaitOn, ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait )

    #define traceRETURN_xTaskGenericNotifyWait( xReturn )

    #define traceENTER_xTaskGenericNotify( xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue )

    #define traceRETURN_xTaskGenericNotify( xReturn )

    #define traceENTER_xTaskGenericNotifyFromISR( xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken )

    #define traceRETURN_xTaskGenericNotifyFromISR( xReturn )

    #define traceENTER_vTaskGenericNotifyGiveFromISR( xTaskToNotify, uxIndexToNotify, pxHigherPriorityTaskWoken )

    #define traceRETURN_vTaskGenericNotifyGiveFromISR()

    #define traceENTER_xTaskGenericNotifyStateClear( xTask, uxIndexToClear )

    #define traceRETURN_xTaskGenericNotifyStateClear( xReturn )

    #define traceENTER_ulTaskGenericNotifyValueClear( xTask, uxIndexToClear, ulBitsToClear )

    #define traceRETURN_ulTaskGenericNotifyValueClear( ulReturn )

    #define traceENTER_ulTaskGetRunTimeCounter( xTask )

    #define traceRETURN_ulTaskGetRunTimeCounter( ulRunTimeCounter )

    #define traceENTER_ulTaskGetRunTimePercent( xTask )

    #define traceRETURN_ulTaskGetRunTimePercent( ulReturn )

    #define traceENTER_ulTaskGetIdleRunTimeCounter()

    #define traceRETURN_ulTaskGetIdleRunTimeCounter( ulReturn )

    #define traceENTER_ulTaskGetIdleRunTimePercent()

    #define traceRETURN_ulTaskGetIdleRunTimePercent( ulReturn )

    #define traceENTER_xTaskGetMPUSettings( xTask )

    #define traceRETURN_xTaskGetMPUSettings( xMPUSettings )

    #define traceENTER_xStreamBufferGenericCreate( xBufferSizeBytes, xTriggerLevelBytes, xStreamBufferType, pxSendCompletedCallback, pxReceiveCompletedCallback )

    #define traceRETURN_xStreamBufferGenericCreate( pvAllocatedMemory )

    #define traceENTER_xStreamBufferGenericCreateStatic( xBufferSizeBytes, xTriggerLevelBytes, xStreamBufferType, pucStreamBufferStorageArea, pxStaticStreamBuffer, pxSendCompletedCallback, pxReceiveCompletedCallback )

    #define traceRETURN_xStreamBufferGenericCreateStatic( xReturn )

    #define traceENTER_xStreamBufferGetStaticBuffers( xStreamBuffer, ppucStreamBufferStorageArea, ppxStaticStreamBuffer )

    #define traceRETURN_xStreamBufferGetStaticBuffers( xReturn )

    #define traceENTER_vStreamBufferDelete( xStreamBuffer )

    #define traceRETURN_vStreamBufferDelete()

    #define traceENTER_xStreamBufferReset( xStreamBuffer )

    #define traceRETURN_xStreamBufferReset( xReturn )

    #define traceENTER_xStreamBufferResetFromISR( xStreamBuffer )

    #define traceRETURN_xStreamBufferResetFromISR( xReturn )

    #define traceENTER_xStreamBufferSetTriggerLevel( xStreamBuffer, xTriggerLevel )

    #define traceRETURN_xStreamBufferSetTriggerLevel( xReturn )

    #define traceENTER_xStreamBufferSpacesAvailable( xStreamBuffer )

    #define traceRETURN_xStreamBufferSpacesAvailable( xSpace )

    #define traceENTER_xStreamBufferBytesAvailable( xStreamBuffer )

    #define traceRETURN_xStreamBufferBytesAvailable( xReturn )

    #define traceENTER_xStreamBufferSend( xStreamBuffer, pvTxData, xDataLengthBytes, xTicksToWait )

    #define traceRETURN_xStreamBufferSend( xReturn )

    #define traceENTER_xStreamBufferSendFromISR( xStreamBuffer, pvTxData, xDataLengthBytes, pxHigherPriorityTaskWoken )

    #define traceRETURN_xStreamBufferSendFromISR( xReturn )

    #define traceENTER_xStreamBufferReceive( xStreamBuffer, pvRxData, xBufferLengthBytes, xTicksToWait )

    #define traceRETURN_xStreamBufferReceive( xReceivedLength )

    #define traceENTER_xStreamBufferNextMessageLengthBytes( xStreamBuffer )

    #define traceRETURN_xStreamBufferNextMessageLengthBytes( xReturn )

    #define traceENTER_xStreamBufferReceiveFromISR( xStreamBuffer, pvRxData, xBufferLengthBytes, pxHigherPriorityTaskWoken )

    #define traceRETURN_xStreamBufferReceiveFromISR( xReceivedLength )

    #define traceENTER_xStreamBufferIsEmpty( xStreamBuffer )

    #define traceRETURN_xStreamBufferIsEmpty( xReturn )

    #define traceENTER_xStreamBufferIsFull( xStreamBuffer )

    #define traceRETURN_xStreamBufferIsFull( xReturn )

    #define traceENTER_xStreamBufferSendCompletedFromISR( xStreamBuffer, pxHigherPriorityTaskWoken )

    #define traceRETURN_xStreamBufferSendCompletedFromISR( xReturn )

    #define traceENTER_xStreamBufferReceiveCompletedFromISR( xStreamBuffer, pxHigherPriorityTaskWoken )

    #define traceRETURN_xStreamBufferReceiveCompletedFromISR( xReturn )

    #define traceENTER_uxStreamBufferGetStreamBufferNotificationIndex( xStreamBuffer )

    #define traceRETURN_uxStreamBufferGetStreamBufferNotificationIndex( uxNotificationIndex )

    #define traceENTER_vStreamBufferSetStreamBufferNotificationIndex( xStreamBuffer, uxNotificationIndex )

    #define traceRETURN_vStreamBufferSetStreamBufferNotificationIndex()

    #define traceENTER_uxStreamBufferGetStreamBufferNumber( xStreamBuffer )

    #define traceRETURN_uxStreamBufferGetStreamBufferNumber( uxStreamBufferNumber )

    #define traceENTER_vStreamBufferSetStreamBufferNumber( xStreamBuffer, uxStreamBufferNumber )

    #define traceRETURN_vStreamBufferSetStreamBufferNumber()

    #define traceENTER_ucStreamBufferGetStreamBufferType( xStreamBuffer )

    #define traceRETURN_ucStreamBufferGetStreamBufferType( ucStreamBufferType )

    #define traceENTER_vListInitialise( pxList )

    #define traceRETURN_vListInitialise()

    #define traceENTER_vListInitialiseItem( pxItem )

    #define traceRETURN_vListInitialiseItem()

    #define traceENTER_vListInsertEnd( pxList, pxNewListItem )

    #define traceRETURN_vListInsertEnd()

    #define traceENTER_vListInsert( pxList, pxNewListItem )

    #define traceRETURN_vListInsert()

    #define traceENTER_uxListRemove( pxItemToRemove )

    #define traceRETURN_uxListRemove( uxNumberOfItems )

    #define traceENTER_xCoRoutineCreate( pxCoRoutineCode, uxPriority, uxIndex )

    #define traceRETURN_xCoRoutineCreate( xReturn )

    #define traceENTER_vCoRoutineAddToDelayedList( xTicksToDelay, pxEventList )

    #define traceRETURN_vCoRoutineAddToDelayedList()

    #define traceENTER_vCoRoutineSchedule()

    #define traceRETURN_vCoRoutineSchedule()

    #define traceENTER_xCoRoutineRemoveFromEventList( pxEventList )

    #define traceRETURN_xCoRoutineRemoveFromEventList( xReturn )



    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

    #define portPRIVILEGE_BIT    ( ( UBaseType_t ) 0x00 )

    #define portYIELD_WITHIN_API    portYIELD









    #define portTASK_USES_FLOATING_POINT()

    #define portALLOCATE_SECURE_CONTEXT( ulSecureStackSize )







    #define mtCOVERAGE_TEST_MARKER()

    #define mtCOVERAGE_TEST_DELAY()

    #define portASSERT_IF_IN_ISR()









/* By default per-instance callbacks are not enabled for stream buffer or message buffer. */




    /* Defaults to 1 for backward compatibility. */





/* Defaults to StackType_t for backward compatibility, but can be overridden
 * in the fixed profile if StackType_t is too restrictive. */


/* Defaults to uint32_t for backward compatibility, but can be overridden in
 * the fixed profile if uint32_t is too restrictive. */



/* Defaults to size_t for backward compatibility, but can be overridden
 * in the fixed profile if lengths will always be less than the number of bytes
 * in a size_t. */

/* Sanity check the configuration. */









/* The tick type can be read atomically, so critical sections used when the
 * tick count is returned can be defined away. */
    #define portTICK_TYPE_ENTER_CRITICAL()
    #define portTICK_TYPE_EXIT_CRITICAL()
    #define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR()         0
    #define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x )    ( void ) ( x )

/* Definitions to allow backward compatibility with FreeRTOS versions prior to
 * V8 if desired. */


/* kernelPRINTF() was not defined, so define it away to nothing.  To use
 * kernelPRINTF() then define it as follows (where MyPrintFunction() is
 * provided by the application writer):
 *
 * void MyPrintFunction(const char *pcFormat, ... );
 *
 * Then call like a standard printf() function, but placing brackets around
 * all parameters so they are passed as a single parameter.  For example:
 * kernelPRINTF( ("Value = %d", MyVariable) ); */


/* The application writer has not provided their own MAX macro, so define
 * the following generic implementation. */


/* The application writer has not provided their own MIN macro, so define
 * the following generic implementation. */



/* Set fixed-profile option to 0 to omit floating point support even
 * if floating point hardware is otherwise supported by the FreeRTOS port in use.
 * This constant is not supported by all FreeRTOS ports that include floating
 * point support. */

/* Set 0 to 1 to enable MPU support and 0 to disable it. This is
 * currently used in ARMv8M ports. */

/* Set 1 to 1 to enable FPU support and 0 to disable it. This is
 * currently used in ARMv8M ports. */

/* Set 0 to 1 to enable MVE support and 0 to disable it. This is
 * currently used in ARMv8M ports. */

/* Set 0 to 1 enable TrustZone support and 0 to disable it.
 * This is currently used in ARMv8M ports. */

/* Set fixed-profile option to 1 to run the FreeRTOS ARMv8M port on
 * the Secure Side only. */


/* The following config allows infinite loop control. For example, control the
 * infinite loop in idle task function when performing unit tests. */

/* Sometimes the the fixed profile settings only allow a task to be created using
 * dynamically allocated RAM, in which case when any task is deleted it is known
 * that both the task's stack and TCB need to be freed.  Sometimes the
 * the fixed profile settings only allow a task to be created using statically
 * allocated RAM, in which case when any task is deleted it is known that neither
 * the task's stack or TCB should be freed.  Sometimes the the fixed profile
 * settings allow a task to be created using either statically or dynamically
 * allocated RAM, in which case a member of the TCB is used to record whether the
 * stack and/or TCB were allocated statically or dynamically, so when a task is
 * deleted the RAM that was allocated dynamically is freed again and no attempt is
 * made to free the RAM that was allocated statically.
 * tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE is only true if it is possible for a
 * task to be created using either statically or dynamically allocated RAM.  Note
 * that if portUSING_MPU_WRAPPERS is 1 then a protected task can be created with
 * a statically allocated stack and a dynamically allocated TCB.
 *
 * The following table lists various combinations of portUSING_MPU_WRAPPERS,
 * 0 and 1 and
 * when it is possible to have both static and dynamic allocation:
 *  +-----+---------+--------+-----------------------------+-----------------------------------+------------------+-----------+
 * | MPU | Dynamic | Static |     Available Functions     |       Possible Allocations        | Both Dynamic and | Need Free |
 * |     |         |        |                             |                                   | Static Possible  |           |
 * +-----+---------+--------+-----------------------------+-----------------------------------+------------------+-----------+
 * | 0   | 0       | 1      | xTaskCreateStatic           | TCB - Static, Stack - Static      | No               | No        |
 * +-----|---------|--------|-----------------------------|-----------------------------------|------------------|-----------|
 * | 0   | 1       | 0      | xTaskCreate                 | TCB - Dynamic, Stack - Dynamic    | No               | Yes       |
 * +-----|---------|--------|-----------------------------|-----------------------------------|------------------|-----------|
 * | 0   | 1       | 1      | xTaskCreate,                | 1. TCB - Dynamic, Stack - Dynamic | Yes              | Yes       |
 * |     |         |        | xTaskCreateStatic           | 2. TCB - Static, Stack - Static   |                  |           |
 * +-----|---------|--------|-----------------------------|-----------------------------------|------------------|-----------|
 * | 1   | 0       | 1      | xTaskCreateStatic,          | TCB - Static, Stack - Static      | No               | No        |
 * |     |         |        | xTaskCreateRestrictedStatic |                                   |                  |           |
 * +-----|---------|--------|-----------------------------|-----------------------------------|------------------|-----------|
 * | 1   | 1       | 0      | xTaskCreate,                | 1. TCB - Dynamic, Stack - Dynamic | Yes              | Yes       |
 * |     |         |        | xTaskCreateRestricted       | 2. TCB - Dynamic, Stack - Static  |                  |           |
 * +-----|---------|--------|-----------------------------|-----------------------------------|------------------|-----------|
 * | 1   | 1       | 1      | xTaskCreate,                | 1. TCB - Dynamic, Stack - Dynamic | Yes              | Yes       |
 * |     |         |        | xTaskCreateStatic,          | 2. TCB - Dynamic, Stack - Static  |                  |           |
 * |     |         |        | xTaskCreateRestricted,      | 3. TCB - Static, Stack - Static   |                  |           |
 * |     |         |        | xTaskCreateRestrictedStatic |                                   |                  |           |
 * +-----+---------+--------+-----------------------------+-----------------------------------+------------------+-----------+
 */
#define tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE                                                                                     \
    ( ( ( portUSING_MPU_WRAPPERS == 0 ) && ( 0 == 1 ) && ( 1 == 1 ) ) || \
      ( ( portUSING_MPU_WRAPPERS == 1 ) && ( 0 == 1 ) ) )

/*
 * In line with software engineering best practice, FreeRTOS implements a strict
 * data hiding policy, so the real structures used by FreeRTOS to maintain the
 * state of tasks, queues, semaphores, etc. are not accessible to the application
 * code.  However, if the application writer wants to statically allocate such
 * an object then the size of the object needs to be known.  Dummy structures
 * that are guaranteed to have the same size and alignment requirements of the
 * real objects are used for this purpose.  The dummy list and list item
 * structures below are used for inclusion in such a dummy structure.
 */
struct xSTATIC_LIST_ITEM
{
    TickType_t xDummy2;
    void * pvDummy3[ 4 ];
};
typedef struct xSTATIC_LIST_ITEM StaticListItem_t;

    /* See the comments above the struct xSTATIC_LIST_ITEM definition. */
    struct xSTATIC_MINI_LIST_ITEM
    {
        TickType_t xDummy2;
        void * pvDummy3[ 2 ];
    };
    typedef struct xSTATIC_MINI_LIST_ITEM StaticMiniListItem_t;

/* See the comments above the struct xSTATIC_LIST_ITEM definition. */
typedef struct xSTATIC_LIST
{
    UBaseType_t uxDummy2;
    void * pvDummy3;
    StaticMiniListItem_t xDummy4;
} StaticList_t;

/*
 * In line with software engineering best practice, especially when supplying a
 * library that is likely to change in future versions, FreeRTOS implements a
 * strict data hiding policy.  This means the Task structure used internally by
 * FreeRTOS is not accessible to application code.  However, if the application
 * writer wants to statically allocate the memory required to create a task then
 * the size of the task object needs to be known.  The StaticTask_t structure
 * below is provided for this purpose.  Its sizes and alignment requirements are
 * guaranteed to match those of the genuine structure, no matter which
 * architecture is being used, and no matter how the values in the fixed profile
 * are set.  Its contents are somewhat obfuscated in the hope users will
 * recognise that it would be unwise to make direct use of the structure members.
 */
typedef struct xSTATIC_TCB
{
    void * pxDummy1;
    StaticListItem_t xDummy3[ 2 ];
    UBaseType_t uxDummy5;
    void * pxDummy6;
    uint8_t ucDummy7[ 16 ];
        UBaseType_t uxDummy12[ 2 ];
        uint32_t ulDummy18[ 1 ];
        uint8_t ucDummy19[ 1 ];

} StaticTask_t;

/*
 * In line with software engineering best practice, especially when supplying a
 * library that is likely to change in future versions, FreeRTOS implements a
 * strict data hiding policy.  This means the Queue structure used internally by
 * FreeRTOS is not accessible to application code.  However, if the application
 * writer wants to statically allocate the memory required to create a queue
 * then the size of the queue object needs to be known.  The StaticQueue_t
 * structure below is provided for this purpose.  Its sizes and alignment
 * requirements are guaranteed to match those of the genuine structure, no
 * matter which architecture is being used, and no matter how the values in
 * the fixed profile are set.  Its contents are somewhat obfuscated in the hope
 * users will recognise that it would be unwise to make direct use of the
 * structure members.
 */
typedef struct xSTATIC_QUEUE
{
    void * pvDummy1[ 3 ];

    union
    {
        void * pvDummy2;
        UBaseType_t uxDummy2;
    } u;

    StaticList_t xDummy3[ 2 ];
    UBaseType_t uxDummy4[ 3 ];
    uint8_t ucDummy5[ 2 ];



} StaticQueue_t;
typedef StaticQueue_t StaticSemaphore_t;

/*
 * In line with software engineering best practice, especially when supplying a
 * library that is likely to change in future versions, FreeRTOS implements a
 * strict data hiding policy.  This means the event group structure used
 * internally by FreeRTOS is not accessible to application code.  However, if
 * the application writer wants to statically allocate the memory required to
 * create an event group then the size of the event group object needs to be
 * know.  The StaticEventGroup_t structure below is provided for this purpose.
 * Its sizes and alignment requirements are guaranteed to match those of the
 * genuine structure, no matter which architecture is being used, and no matter
 * how the values in the fixed profile are set.  Its contents are somewhat
 * obfuscated in the hope users will recognise that it would be unwise to make
 * direct use of the structure members.
 */
typedef struct xSTATIC_EVENT_GROUP
{
    TickType_t xDummy1;
    StaticList_t xDummy2;


} StaticEventGroup_t;

/*
 * In line with software engineering best practice, especially when supplying a
 * library that is likely to change in future versions, FreeRTOS implements a
 * strict data hiding policy.  This means the software timer structure used
 * internally by FreeRTOS is not accessible to application code.  However, if
 * the application writer wants to statically allocate the memory required to
 * create a software timer then the size of the queue object needs to be known.
 * The StaticTimer_t structure below is provided for this purpose.  Its sizes
 * and alignment requirements are guaranteed to match those of the genuine
 * structure, no matter which architecture is being used, and no matter how the
 * values in the fixed profile are set.  Its contents are somewhat obfuscated in
 * the hope users will recognise that it would be unwise to make direct use of
 * the structure members.
 */
typedef struct xSTATIC_TIMER
{
    void * pvDummy1;
    StaticListItem_t xDummy2;
    TickType_t xDummy3;
    void * pvDummy5;
    TaskFunction_t pvDummy6;
    uint8_t ucDummy8;
} StaticTimer_t;

/*
 * In line with software engineering best practice, especially when supplying a
 * library that is likely to change in future versions, FreeRTOS implements a
 * strict data hiding policy.  This means the stream buffer structure used
 * internally by FreeRTOS is not accessible to application code.  However, if
 * the application writer wants to statically allocate the memory required to
 * create a stream buffer then the size of the stream buffer object needs to be
 * known.  The StaticStreamBuffer_t structure below is provided for this
 * purpose.  Its size and alignment requirements are guaranteed to match those
 * of the genuine structure, no matter which architecture is being used, and
 * no matter how the values in the fixed profile are set.  Its contents are
 * somewhat obfuscated in the hope users will recognise that it would be unwise
 * to make direct use of the structure members.
 */
typedef struct xSTATIC_STREAM_BUFFER
{
    size_t uxDummy1[ 4 ];
    void * pvDummy2[ 3 ];
    uint8_t ucDummy3;
    UBaseType_t uxDummy6;
} StaticStreamBuffer_t;

/* Message buffers are built on stream buffers. */
typedef StaticStreamBuffer_t StaticMessageBuffer_t;

/* *INDENT-OFF* */
/* *INDENT-ON* */

