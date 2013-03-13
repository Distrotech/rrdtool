#!/bin/sh
PATH=/scratch/rrd4/bin:$PATH
R=rrdtool
$R create real.rrd \
   --step=300 \
   --start=1199999699 \
   DS:distance:COUNTER:600:-40:100 \
   RRA:AVERAGE:0.4:1:5

u(){
 $R update real.rrd $1
}

u 1200000000:0
u 1200000150:15
u 1200000310:31
u 1200000640:64
u 1200000910:91

$R fetch real.rrd -s 1200000000 -e 1200000899 AVERAGE
