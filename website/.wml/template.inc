#include "$(ROOT)/.wml/nestednavbar.inc"
#include "$(ROOT)/.wml/tobis.inc"
#include "$(ROOT)/.wml/navbar.inc"



# The Template
<HTML>
<HEAD>
<!-- The Template for this webpage has been created by Tobi Oetiker -->
<TITLE*>RRD TOOL -- <<TITLE>></TITLE*>
<STYLE TYPE="text/css">
<protect>
<!--
H1, H2, DIV, P, LI, 
UL, OL, TABLE, TD, TR {font-family: trebuchet ms, lucida sans, helvetica, sans-serif}
PRE {font-family: courier, courier-new, lucidatypewriter}
-->
</protect>
</STYLE>
</HEAD>
<BODY bgcolor=white 
      text=#00000 LINK=#0000a0 
      VLINK=#0000a0 ALINK=#0000a0
      MARGINWIDTH=0 MARGINHEIGHT=0>
<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>
<TR><TD width=8 ALIGN=left VALIGN=TOP 
        BACKGROUND="$(ROOT)/.pics/b.gif" BGCOLOR=#6c9bcd><hspace 8></TD>
<TD width=126 ALIGN=left VALIGN=TOP 
        BACKGROUND="$(ROOT)/.pics/b.gif" BGCOLOR=#6c9bcd>
<DIV><vspace 10></DIV>
<A HREF="http://ee-staff.ethz.ch/~oetiker/webtools/rrdtool/"
><img border=0 src="$(ROOT)/.pics/rrdtool.gif"></A><hspace 5>
<DIV><vspace 10></DIV>
# Nav Bar
<nnb:render $(section) $(subsection)>
<BR>
<vspace 40><BR>
<small>
Website and<br>
RRD TOOL<br>
created by<br>
<B><A HREF="http://www.caida.org/~tobi">Tobi Oetiker</A></B><BR>
<vspace 10><BR>
For more information<BR>
about CAIDA write to<BR>
<TABLE CELLPADDING=0 CELLSPACING=0 BORDER=0><TR><TD><small>info</small></TD><TD><small>@</small></TD><TD><small>caida.org</small></TD></TR></TABLE><BR>
<vspace 30><BR>
<A HREF="http://www.engelschall.com/sw/wml"><IMG border=0 SRC="$(ROOT)/.pics/logo-wml.gif"></A><BR>
<vspace 10><BR>
</TD>
<TD WIDTH=22 background="$(ROOT)/.pics/bro.gif"><hspace 22></TD><TD><hspace 5></TD>
<TD valign=top align=left>
<vspace 10><BR>
<A HREF="http://www.caida.org"><IMG ALIGN=right border=0 SRC="$(ROOT)/.pics/CAIDAlogo.120.gif"></A>
<<H1>>
<<Body>>
<DIV><vspace 20></DIV>
<HR noshade size=1>
<DIV align=right><FONT color="#bfbfbf">
<small><script type="text/javascript" language="JavaScript">
<!--
  document.write("Original location of this page: ");
  document.write(document.location);
  document.write("<BR>");
//  -->
</script>
This page was updated on, <:print "".localtime(time):></small></font></DIV>
</TD></TR>
<TR><TD colspan=2 background="$(ROOT)/.pics/bbo.gif"><vspace 22></TD><TD><IMG SRC="$(ROOT)/.pics/bco.gif"></TD>
<TD colspan=2></TD></TR>
# The End
</TABLE>
</BODY>

# All the rest goes into Body
..Body>>











