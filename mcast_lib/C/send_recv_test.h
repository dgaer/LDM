/**
 * Copyright 2014 University Corporation for Atmospheric Research. All rights
 * reserved. See the the file COPYRIGHT in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: send_recv_test.h
 * @author: Steven R. Emmerson
 *
 * This file contains common definitions for `send_test.c` and `recv_test.c`.
 */

#ifndef SEND_RECV_TEST_H_
#define SEND_RECV_TEST_H_

//#define HELLO_PORT  12345
#define HELLO_PORT  38800
//#define HELLO_PORT  1 // Requires superuser privileges
//#define HELLO_GROUP "239.0.0.37" // Works
#define HELLO_GROUP "234.128.117.0" // UCAR unicast-based multicast address. Works


#ifdef __cplusplus
    extern "C" {
#endif


#ifdef __cplusplus
    }
#endif

#endif /* SEND_RECV_TEST_H_ */
