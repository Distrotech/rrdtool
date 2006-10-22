<define-tag h1 whitespace=delete endtag=required>
<preserve subtitle />
<set-var %attributes />
<h1*>%body</h1*>
{#H1_FLAT#:<: $x = <<kjhasdflkjhasdf;
%body
kjhasdflkjhasdf
$x =~ s/\\n/ /g; print $x; :>:#H1_FLAT#}
<if <get-var title_is_set /> ""
   {#Title#:{#H1_FLAT#}:#Title#}
/>
<div id="googlead_right">
<script type="text/javascript"><!--
google_ad_client = "pub-8135238818549668";
google_ad_width = 160; 
google_ad_height = 600;
google_ad_format = "160x600_as";
google_ad_type = "text_image";
google_ad_channel ="5627785043";
google_color_border = "e6e6e6";      
google_color_bg = "f8f8f8";
google_color_link = "0017C1"; 
google_color_url = "270B83"; 
google_color_text = "000000";
#google_color_border = "717fe5";
#google_color_bg = "F0F0F0";
#google_color_link = "2c3fbf";
#google_color_url = "6851b0";
#google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</div>
<restore subtitle />
</define-tag>

<define-tag title whitespace=delete endtag=required>
<set-var title_is_set=1/>
{#!Title#:<: $_ = <<kjhasdflkjhasdf;
%body
kjhasdflkjhasdf
s/\\n/ /g; print :>:#Title#}
</define-tag>

<perl>my $addcount=0;</perl>
<define-tag h2 whitespace=delete endtag=required>
<perl>
$addcount++;
if ($addcount == 6){
print <<'GOOGLEAD';
<p>
<script type="text/javascript"><!--
google_ad_client = "pub-8135238818549668";
google_ad_width = 336;
google_ad_height = 280;
google_ad_format = "336x280_as";
google_ad_type = "text_image";
google_ad_channel ="9159032853";
google_color_border = "E6E6E6";
google_color_bg = "f8f8f8";
google_color_link = "2c3fbf";
google_color_url = "6851b0";
google_color_text = "000000";
//--></script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
</p>
GOOGLEAD
}
</perl>
<h2*>%body</h2*>
</define-tag>
