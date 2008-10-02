#!/usr/bin/env rrdcgi
<html>
<head>
<title>Traffic Stats for oss.oetiker.ch</title>
</head>
<body>
<h1>Traffic Stats for oss.oetiker.ch</h1>

<h2>The Bytes</h2>
<table border="1" cellspacing="0" cellpadding="2">
<tr><td>Period</td>
    <td>Incoming</td>
    <td>Outgoing</td>
    <td>Total</td></tr>

<!--
<RRD::GRAPH -
       --start="midnight"
       --end="start+24h"
       --imginfo=" "
       DEF:in=lan.rrd:in:AVERAGE:step=1800
       DEF:out=lan.rrd:out:AVERAGE:step=1800
       VDEF:is=in,TOTAL
       PRINT:is:"%0.2lf %s"
       VDEF:os=out,TOTAL
       PRINT:os:"%0.2lf %S"
       CDEF:sum=in,out,+
       VDEF:ss=sum,TOTAL
       PRINT:ss:"%0.2lf %S"
>
-->

<tr><td><RRD::TIME::NOW %Y-%m-%d></td>
    <td align="right"><RRD::PRINT 0></td>
    <td align="right"><RRD::PRINT 1></td>
    <td align="right"><RRD::PRINT 2></td></tr>

<!--
<RRD::GRAPH -
       --start="<RRD::TIME::NOW %Y%m01>"
       --end="now"
       --imginfo=" "
       DEF:in=lan.rrd:in:AVERAGE:step=1800
       DEF:out=lan.rrd:out:AVERAGE:step=1800
       VDEF:is=in,TOTAL
       PRINT:is:"%0.2lf %s"
       VDEF:os=out,TOTAL
       PRINT:os:"%0.2lf %S"
       CDEF:sum=in,out,+
       VDEF:ss=sum,TOTAL
       PRINT:ss:"%0.2lf %S"
>
-->

<tr><td><RRD::TIME::NOW %Y-%m></td>
    <td align="right"><RRD::PRINT 0></td>
    <td align="right"><RRD::PRINT 1></td>
    <td align="right"><RRD::PRINT 2></td></tr>

<!--
<RRD::GRAPH -
       --start="<RRD::TIME::NOW %Y0101>"
       --end="now"
       --imginfo=" "
       DEF:in=lan.rrd:in:AVERAGE:step=1800
       DEF:out=lan.rrd:out:AVERAGE:step=1800
       VDEF:is=in,TOTAL
       PRINT:is:"%0.2lf %s"
       VDEF:os=out,TOTAL
       PRINT:os:"%0.2lf %S"
       CDEF:sum=in,out,+
       VDEF:ss=sum,TOTAL
       PRINT:ss:"%0.2lf %S"
>
-->

<tr><td><RRD::TIME::NOW %Y></td>
    <td align="right"><RRD::PRINT 0></td>
    <td align="right"><RRD::PRINT 1></td>
    <td align="right"><RRD::PRINT 2></td></tr>
</table>

<h2>Current</h2>

<RRD::SETVAR start -2h>
<RRD::SETVAR end now>
<RRD::INCLUDE graph.inc>

<h2>Day</h2>

<RRD::SETVAR start -24h>
<RRD::SETVAR end now>
<RRD::INCLUDE graph.inc>

<h2>7 Days</h2>

<RRD::SETVAR start -7d>
<RRD::SETVAR end now>
<RRD::INCLUDE graph.inc>

<h2>Month</h2>

<RRD::SETVAR start -30d>
<RRD::SETVAR end now>
<RRD::INCLUDE graph.inc>

<h2>This Year</h2>

<RRD::SETVAR start "Jan1">
<RRD::SETVAR end   "Dec31">
<RRD::INCLUDE graph.inc>

<h2>Last Year</h2>

<RRD::SETVAR start "Jan1-365d">
<RRD::SETVAR end   "Dec31-365d">
<RRD::INCLUDE graph.inc>

</body>
</html>
