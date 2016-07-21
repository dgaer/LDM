LDM send util with retry and notify me
Last modfied: 07/21/2016

Rewrite of ldmsend.c that will allow send retries if an error occurs
during the file uploads. Adds an optional notification to verify that
the product was added to the LDM queue on the remote host. Due to some
network issues, several SR/NWS sites sites had product upload failures
using ldmsend. As some critical products are only sent at specific
times we needed a way to resend the product during periods when the
network was saturated. Due to firewall configurations SRH cannot
request from our downstream servers so we could not do a pqinsert at
the site and request from the downstream queue. Ldmsend works most of
the time but was failing during periods of when the network was
saturated. This version of ldmsend allow NWS sites to resend and
verify the upload, for example:

    su - ldm
    /bin/bash
    dd if=/dev/urandom of=/tmp/testfile.1 bs=1024 count=8192
    cd /tmp
    status=$(/usr/local/ldm/util/ldmsend_nws -vxnl- -h srh-ls-cpnrs.srh.noaa.gov -f EXP -o 3600 -r 5 -R 60 -T 25 -p '^testfile.*' testfile.1)
    echo $status

NOTE: This version of was compiled as ldmsend_nws and copied to the
util directory. 

If status variable equals PASS the file made is to the upstream LDM
queue. If status equals anything else, the file did not upload and our
scripts can be adjusted to send alert messages to the forecasters if
any critical upload failed.
