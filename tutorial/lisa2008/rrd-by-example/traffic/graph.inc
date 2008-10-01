<p>
<RRD::GRAPH lan<RRD::GETVAR start>.png
       --title="oss.oetiker.ch network traffic"
       --vertical-label=Bytes/s
       --start="<RRD::GETVAR start>"
       --end="<RRD::GETVAR end>"
       --width=600 
       --height=100  
       DEF:in=lan.rrd:in:AVERAGE
       CDEF:nin=in,-1,*
       LINE1.5:nin#00d000
       AREA:nin#90ff90:Incoming
       DEF:out=lan.rrd:out:AVERAGE
       LINE1.5:out#2020ff
       AREA:out#9090ff:Outgoing
       LINE0.5:0#000
       COMMENT:"<RRD::TIME::NOW '%Y-%m-%d %H\:%M'>\j"     
>
</p>
