#!/bin/sh
export PATH=/scratch/rrd4/bin:$PATH
R=rrdtool
$R create first.rrd \
   --step=300 \
   --start=1199999699 \
   DS:temperature:GAUGE:600:-40:100 \
   RRA:AVERAGE:0.4:1:5 \
   RRA:AVERAGE:0.4:3:2 \
   RRA:MIN:0.4:3:2 \
   RRA:MAX:0.4:3:2


#!/bin/sh
R=rrdtool
u(){
 $R update first.rrd $1
}

u 1199999700:00 
u 1200000000:10
u 1200000300:20
u 1200000600:30
u 1200000900:40


$R dump first.rrd
