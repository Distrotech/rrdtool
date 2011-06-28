#!/bin/sh
rrdtool create first.rrd \
   --step=300 \
   --start=1199999699 \
   DS:temperature:GAUGE:600:-40:100 \
   RRA:AVERAGE:0.4:1:12 \
   RRA:AVERAGE:0.4:12:24 \
   RRA:MIN:0.4:12:24 \
   RRA:MAX:0.4:12:24
