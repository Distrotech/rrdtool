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
<protect>
<!--[if lt IE 7]><script src="$(ROOT)/inc/IE7/ie7-standard-p.js" type="text/javascript"></script><![endif]-->
</protect>
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
<p><b>Gold Sponsors</b></p>
<p><a href="http://www.groundworkopensource.com/products/components/rrdtool.html"><img title="GroundWork Open Source" border="0" alt="GroundWork Open Source Logo" src="$(ROOT)/logos/gw200x100.gif"/></a></p>
<p><a href="http://www.zenoss.com/rrdtool?utm_source=rrdtool&amp;utm_medium=logolink&amp;utm_campaign=inbound"><img border="0" title="Zenoss Open Source IT Monitoring" alt="Zenoss Open Source IT Monitoring" src="$(ROOT)/logos/zenoss.png"/></a></p>
<p><a href="$(ROOT)/sponsor.<lang:star: */>.html">Silver Sponsors ...</a></p>
<br/>
<br/>
<div>
<a href="http://tobi.oetiker.ch/wish/"><img border="0" alt="Make Tobi Happy" src="$(ROOT)/inc/happy-tobi.png"/></a>
</div>
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
<A HREF="http://ftp.heanet.ie/mirrors/oss.oetiker.ch/rrdtool/<where/>">IE</A><!-- <mirrors@heanet.ie>-->
<A HREF="http://rrdtool.codemonkey.se/<where/>">SE</A><!--Hans Engren <hans@cyclone.se>-->
<A HREF="http://rrdtool.basemirror.de/<where/>">DE</A><!--Baseportal Mirrors <mirrors@baseportal.de>-->
<A HREF="http://rrdtool.mirroraty.org/<where/>">US</A><!--Hossam Hossny <hossam@trexle.com>-->
<A HREF="http://rrdtool.paracoda.com/<where/>">CA</A><!--Hossam Hossny <admin@paracoda.com>-->
<A HREF="http://rrdtool.PlaySystems.ro/<where/>">RO</a><!--dan.draghici@ulbsibiu.ro-->
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
<!-- google_ad_section_start -->
{#Body#}
<!-- google_ad_section_end -->
</div>
</td>
</tr>
<tr>
<td id="leftcornercell"></td>
<td id="footcell">
<!-- ############################# -->
<!-- # FOOTER -->
<!-- ############################# -->
<perl>
if ( -s "$(WML_SRC_BASENAME).wml" > 4000 ){
print <<'GOOGLEAD';
<div style="clear:right">
<script type="text/javascript"><!--
google_ad_client = "pub-8135238818549668";
google_ad_width = 300;
google_ad_height = 250;
google_ad_format = "300x250_as";
google_ad_type = "text_image";
google_ad_channel ="6085696737";
google_color_border = "ffffff";
google_color_bg = "f0f8FF";
google_color_link = "191919";
google_color_url = "0066CC";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
GOOGLEAD
}
</perl>
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
[$LANG:$date | <A HREF="$url">$text</A> | <A HREF="http://it.oetiker.ch/monitoring/">OETIKER+PARTNER AG</a>:$LANG]
LINE
}
</protect>
</perl>
</small>
</div>
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
<script src="http://www.google-analytics.com/urchin.js" type="text/javascript">
</script>
<script type="text/javascript">
_uacct = "UA-374525-1";
urchinTracker();
</script>
</body>
</html>

# All the rest goes into Body
..Body>>

