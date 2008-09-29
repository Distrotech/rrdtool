#!/bin/sh
export PATH=/scratch/rrd4/bin:$PATH
R=rrdtool
$R create x.rrd \
   --step=300 \
   --start=1199999699 \
   DS:temperature:GAUGE:600:-40:100 \
   RRA:AVERAGE:0.5:1:2 \
   RRA:AVERAGE:0.5:2:3

#!/bin/sh
R=rrdtool
u(){
 $R update x.rrd $1
}

u 1199999700:00 
u 1200000000:10
u 1200000300:20
u 1200000600:30
u 1200000900:40
u 1200001200:50

set -x 

$R fetch x.rrd -r300 -s1200000000 -e1200000900 AVERAGE
$R fetch x.rrd -r300 -s1200000600 -e1200000900 AVERAGE

