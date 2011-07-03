#!/bin/sh
rrdtool graphv first.png \
   --zoom=2 \
   --title="FISL12 RRDtool Demo" \
   --start=1199999000 --end=1200001300 \
   DEF:t=first.rrd:temperature:AVERAGE \
   'AREA:t#ff8888:Temperature' \
   'LINE0.5:0#880000::STACK'
