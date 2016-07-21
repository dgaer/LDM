LDM utility used to read the LDM queue and write XML info files
Last modfied: 07/21/2016

LDM queue monitor program used to display LDM queue data on internal
Web servers. Used by NWS/SRH to track LDM products.

Building:
--------
After building and installing LDM distrib, from dev_queue source
directory:

make
make install

Testing:
-------
Make your DEV queue monitor output directory:

$ mkdir -p /data.local/dev_queue
$ /usr/local/ldm/util/dev_queue -v -d5 -l"$LDMHOME/var/logs/devqueue.log" -q"/dev/shm/ldm.pq" -o"/data.local/dev_queue"

Press Ctrl-c to exit test.

For a list of options:

$ /usr/local/ldm/util/dev_queue -h

Adding startup:
-------------
vi $LDMHOME/etc/ldmd.conf

EXEC	"/usr/local/ldm/util/dev_queue -q/dev/shm/ldm.pq -o/data.local/dev_queue"

$ ldmadmin restart
$ ps -ef | grep dev_queue

Web monitor setup:
-----------------
As root, install Apache and PHP:

# yum install httpd php php-cli php-common

Install the monitor PHP scripts from your dev_queue source directory:

# mkdir -pv /var/www/html/ldm
# chmod 755 /var/www/html/ldm
# cp -fv /usr/local/ldm/dev_queue/php/*.php /var/www/html/ldm/. 
# chmod 644 /var/www/html/ldm/*.php

Create a sym link to your queue XML files:

# cd /var/www/html/ldm
# ln -s /data.local/dev_queue queue_data

# service httpd start

The link to view the queue monitor will be:

# echo "http://$(hostname)/ldm"

After testing the internal site:

# chkconfig httpd on

NOTE: The serice and chkconfig commands will still work for httpd
under RHEL7.

NOTE: If required, don't for get to replace the Apache test home page
by putting an index page in the "/var/www/html" directory.



