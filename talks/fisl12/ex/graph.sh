#!/bin/sh
rrdtool graphv first.png \
   --title="FISL12 RRDtool Demo" \
   --start=1199999000 --end=1200001300 \
   --lower-limit=0 \
   DEF:t=first.rrd:temperature:AVERAGE \
   'AREA:t#880011:Temperature'
