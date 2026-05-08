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

#define STACK_MACROS_H

/*
 * Call the stack overflow hook function if the stack of the task being swapped
 * out is currently overflowed, or looks like it might have overflowed in the
 * past.
 *
 * Setting 2 to 1 will cause the macro to check
 * the current stack state only - comparing the current top of stack value to
 * the stack limit.  Setting 2 to greater than 1
 * will also cause the last few stack bytes to be checked to ensure the value
 * to which the bytes were set when the task was created have not been
 * overwritten.  Note this second test does not guarantee that an overflowed
 * stack will always be recognised.
 */

/*-----------------------------------------------------------*/

/*
 * portSTACK_LIMIT_PADDING is a number of extra words to consider to be in
 * use on the stack.
 */
    #define portSTACK_LIMIT_PADDING    0

/* Stack overflow check is not straight forward to implement for MPU ports
 * because of the following reasons:
 * 1. The context is stored in TCB and as a result, pxTopOfStack member points
 *    to the context location in TCB.
 * 2. System calls are executed on a separate privileged only stack.
 *
 * It is still okay because an MPU region is used to protect task stack which
 * means task stack overflow will trigger an MPU fault for unprivileged tasks.
 * Additionally, architectures with hardware stack overflow checking support
 * (such as Armv8-M) will trigger a fault when a task's stack overflows.
 */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* Remove stack overflow macro if not being used. */
    #define taskCHECK_FOR_STACK_OVERFLOW()



