# this seems to confuse just too many things out there ... 
# guess following the standard is not there yet
# <?xml version="1.0" encoding="iso-8859-15"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD XHTML 1.1 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
# this must be the very first line to make doc-type switching work on IE
#use wml::std::logo
#use wml::std::tags
#use wml::des::space
#use wml::des::imgdot
#use wml::std::grid
#use wml::fmt::url
#include <inc/langset.inc>
#include <inc/html.inc>
#include <inc/customtags.inc>
#use wml::des::navbar
#include <inc/navbar-def.inc>
#include <$(ROOT)/navbar.inc>
<html lang="<lang:star: * />" xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="content-type" content="text/html; charset=ISO-8859-15" />
<link* REL="icon" HREF="$(ROOT)/inc/favicon.ico" TYPE="image/ico" />
..Title!>>RRDtool - Page without a Title<<..
<title*>RRDtool - {#Title#}</title*>
<!-- Andrew Clover is THE MAN. He made internet explorer more or less
     more css2 compliant!  http://www.doxdesk.com/software/ -->
<script type="text/javascript" src="$(ROOT)/inc/position.js"></script> <!-- left and right -->
<script type="text/javascript" src="$(ROOT)/inc/fixed.js"></script>    <!-- fixed positioning -->
<style type="text/css">
<!--/* <![CDATA[ */
/* ie5 for mac thinks this comment does not end here because of the backslash \*/
@import url('$(ROOT)/inc/design.css');
## @import url('$(ROOT)/inc/design_$(TYPE).css');
/* ]]> */-->
</style>
</head>
<body>
<table id="frame">
<tr>
<td valign="top" id="menucell">
<!-- ############################# -->
<!-- # MENU -->
<!-- ############################# -->
<div id="menu">
<h2 class="legacy">Menu</h2>
<navbar:render name=main menumode=inner nohints txtonly select=$(PAGE)/>
<br/>
<br/>
<br/>
<a href="http://people.ee.ethz.ch/oetiker/wish"><img border="0" src="$(ROOT)/inc/button.png"/></a><br/>
<br/>
<br/>
<div>
<iframe width="120" height="50" name="bandwidth" frameborder="0" src="$(ROOT)/hoster.html"></iframe>
</div>
<br/>
<br/>
</div>
</td>
<!-- ############################# -->
<!-- # Logo and Body -->
<!-- ############################# -->
<td id="bodycell">
# all mirrors seem dead currently
<div id="mirrors">
<A HREF="http://oss.oetiker.ch/rrdtool/<where/>">Swiss Original</A>.&nbsp;Mirrors: 
<A HREF="http://rrdtool.mirror.graemef.net/<where/>">UK</a><!--Graeme Fowler <graeme@graemef.net>-->

##<A HREF="http://rrdtool.planetmirror.com/<where/>">AU</A><!--jason andrade <jason@dstc.edu.au>-->
<A HREF="http://rrdtool.cs.pu.edu.tw/<where/>">TW</A><!--Hsun-Chang Chang <hsunchang@gmail.com>-->
##<A HREF="http://ftp.heanet.ie/mirrors/people.ee.ethz.ch/%257Eoetiker/webtools/rrdtool/<where/>">IE</A><!--Colm MacCárthaigh <colm.maccarthaigh@heanet.ie>-->
##<A HREF="http://rrdtool.cyclone.se/<where/>">SE</A><!--Hans Engren <hans@cyclone.se>-->
#<A HREF="http://rrdtool.trexle.net/<where/>">US</A><!--Hossam Hossny <hossam@trexle.com>-->
#<A HREF="http://rrdtool.paracoda.com/<where/>">CA</A><!--Hossam Hossny <admin@paracoda.com>-->
</div>
<div id="logo">
<div class="legacy">
RRDtool
<hr/>
</div>
</div>
<!-- ############################# -->
<!-- # BODY -->
<!-- ############################# -->
<div id="body">
<div id="googlead_right">
<script type="text/javascript"><!--
google_ad_client = "pub-8135238818549668";
google_ad_width = 160;
google_ad_height = 600;
google_ad_format = "160x600_as";
google_ad_type = "text";
google_ad_channel ="5627785043";
google_color_border = "1A1A1A";
google_color_bg = "303030";     
google_color_link = "66A5E0";      
google_color_url = "E0BBE8";      
google_color_text = "D0D0D0";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
{#Body#}
</div>
</td>
</tr>
<tr>
<td id="leftcornercell"></td>
<td id="footcell">
<!-- ############################# -->
<!-- # FOOTER -->
<!-- ############################# -->
<div id="address">
<small>
<perl>
<protect pass=2>
use POSIX qw(strftime LC_TIME setlocale); 
foreach my $l (qw($(LANGS))){
	my $lang = lc($l) eq 'de' ? 'de_CH' : $l;
	$lang =~ s/^en.*/sv/i;	
	setlocale(LC_TIME,$lang);
	my $year = strftime "%Y", localtime;
	my $date = strftime "%x", localtime("$(WML_SRC_TIME)"); 
	q{$(AUTHOR)} =~ m{(.+\S+)\s*(<.+>|http://.+)} || die q{No Match for $(AUTHOR)}."\n"; 
	my $text=$1; my $url=$2; $url =~ s/<(\S+)(?:\@|_at_)(\S+)>/mailto:$1\@removethis.$2/;
	my $LANG = uc("LANG_$l");
	print <<LINE
[$LANG:$date | <A HREF="$url">$text</A>:$LANG]
LINE
}
</protect>
</perl>
</small>
</div>
<perl>
if ( -s "$(WML_SRC_FILENAME)" > 4000 ) {
print <<'GOOGLEAD';
<p>
<script type="text/javascript"><!--
google_ad_client = "pub-8135238818549668";
google_ad_width = 728;
google_ad_height = 90;
google_ad_format = "728x90_as";
google_ad_type = "text";
google_ad_channel ="6085696737";
google_color_border = "616FC5";
google_color_bg = "202020";     
google_color_link = "66A5FF";      
google_color_url = "FFBBE8";      
google_color_text = "FFFFFF";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</p>
GOOGLEAD
}
</perl>
</td></tr>
</table>
<hr class="legacy"/>
<p class="legacy" style="border-width: 1px; border-color: red; padding: 5px;">
<en><b>NOTE:</b> The content of this website is accessible with any browser. The graphical
design though relies completely on CSS2 styles. If you see this text, this means
that your browser does not support CSS2. Consider upgrading to a standard conformant
browser like Mozilla <a href="http://www.getfirefox.com">Firefox</a> or
<a href="http://www.opera.com">Opera</a> but also Apple's <a href="http://www.apple.com/safari/">Safari</a>
or KDE's <a href="http://www.konqueror.org">Konqueror</a> for example.
It may also be that you are looking at a mirror page which did not copy <a href="$(ROOT)/inc/design.css">the CSS</a> for this page. Or if some pictu
res are missing, then the mirror may not have picked up the contents of the <a href="$(ROOT)/inc/">inc</a> directory.</en>
#<de><b>HINWEIS:</b> Diese Website funktioniert mit jedem Browser. Die graphische Präsentation jedoch
#basiert komplett auf CSS2. Beispiele für CSS2 konforme Browser sind zum Beispiel  <a href="http://www.getfirefox.com">Firefox</a>
#<a href="http://www.opera.com">Opera</a> oder auch <a href="http://www.apple.com/safari/">Safari</a> von Apple und
#<a href="http://www.konqueror.org">Konqueror</a> aus dem KDE Projekt.
#</de>
</p> 
</body>
</html>

# All the rest goes into Body
..Body>>

