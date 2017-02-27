/**
 * Assertion Checking: Asserts will print a message, turn on the red light, and halt execution
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#ifndef _ASSERT_h_
#define _ASSERT_h_

#define DEBUG

#ifdef DEBUG
void __assert(const char *file, int line, const char *funct, const char *expres, const char * message);
// Assertion assistance via: https://gist.github.com/jlesech/3089916
// (GNU Assert def from assert.h)
# define assert(__e, message) ((__e) ? (void)0 : __assert (__FILE__, __LINE__, \
                                                       __FUNCTION__, #__e, message))

#else
# define assert(__e, message) ;
#endif

#endif
