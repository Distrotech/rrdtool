#!/bin/sh
# use from cron
# * * * * *  /path/to/ifbyteget.sh eth0

PATH=/bin:/usr/bin
export PATH

cd /home/oposs/public_html/stats

if [ ! -f $1.rrd ]; then

rrdtool create $1.rrd \
   --step=60 \
   DS:in:DERIVE:70:0:100000000 \
   DS:out:DERIVE:70:0:100000000 \
   RRA:AVERAGE:0.5:1:1500 \
   RRA:AVERAGE:0.5:60:10000 \
   RRA:MIN:0.5:60:10000 \
   RRA:MAX:0.5:60:10000 \
   RRA:AVERAGE:0.5:1440:1000 \
   RRA:MIN:0.5:1440:1000 \
   RRA:MAX:0.5:1440:1000
fi

rrdtool update $1.rrd \
   N:`grep $1: /proc/net/dev \
      | sed 's/.*://' | awk '{print $1":"$9}'`
