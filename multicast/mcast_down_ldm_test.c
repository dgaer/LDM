/**
 * Copyright 2014 University Corporation for Atmospheric Research. All rights
 * reserved. See the the file COPYRIGHT in the top-level source-directory for
 * licensing conditions.
 *
 * @file mcast_down_ldm_test.c
 *
 * This file performs a unit-test of the mcast_down_ldm module.
 *
 * @author: Steven R. Emmerson
 */


#include "config.h"

#include "mcast_down_ldm.h"
#include "log.h"

#include "vcmtp_c_api_stub.h"
#include "pq_stub.h"

#include <errno.h>
#include <opmock.h>
#include <stdlib.h>

static void missed_product_func(
    Mdl* const  mdl,
    const void* file_entry)
{
}

void test_mdl_createAndExecute()
{
    int                 status;
    pqueue*             pq = (pqueue*)1;
    const char* const   addr = "224.0.0.1";
    const int           port = 1;
    int                 (*int_func)() = (int(*)())1;
    void                (*void_func)() = (void(*)())2;

    status = mdl_createAndExecute(NULL, (void*)1, addr, 0);
    OP_ASSERT_EQUAL_INT(EINVAL, status);
    log_clear();

    status = mdl_createAndExecute((void*)1, NULL, (void*)1, 0);
    OP_ASSERT_EQUAL_INT(EINVAL, status);
    log_clear();

    vcmtpReceiver_new_ExpectAndReturn(
            NULL, int_func, int_func, void_func, addr, port,      NULL,  EINVAL,
            NULL, NULL,     NULL,     NULL,      NULL, cmp_short, NULL);
    status = mdl_createAndExecute((void*)1, (void*)1, NULL, port);
    OP_ASSERT_EQUAL_INT(EINVAL, status);
    log_clear();

    vcmtpReceiver_new_ExpectAndReturn(
            NULL, int_func, int_func, void_func, addr,     port,      NULL,  0,
            NULL, NULL,     NULL,     NULL,      cmp_cstr, cmp_short, NULL);
    vcmtpReceiver_free_ExpectAndReturn(NULL, NULL);
    status = mdl_createAndExecute(pq, missed_product_func, addr, port);
    OP_ASSERT_EQUAL_INT(0, status);

    OP_VERIFY();
}

int main(
    int		argc,
    char**	argv)
{
    opmock_test_suite_reset();
    opmock_register_test(test_mdl_createAndExecute, "test_mdl_createAndExecute");
    opmock_test_suite_run();
    return opmock_get_number_of_errors() != 0;
}
