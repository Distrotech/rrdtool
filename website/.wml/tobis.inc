#use wml::des::space
#use wml::std::tags
#use wml::fmt::url
#use wml::std::box

<define-container TITLE>
..TITLE>>%body<<..
</define-container>

<define-container LI>
<LI*>%body</LI*><DIV><vspace 7></DIV>
</define-container>

<define-container MAGICK>
# <MAGICK ButtonSmall|ButtonLarge|Title|Section  optfilename>
<perl>
BEGIN{unshift @INC, "/usr/pack/imagemagick-4.2.9-to/solaris/perl/"};
use Image::Magick;
my $type = lc("%0");
my $text = <<'sdosidufgtjseflsdj';
%body
sdosidufgtjseflsdj
$text =~ s/%/%%/g;

my $fontpath = "/home/oetiker/data/projects/AABN-rrdtool/website/.ttf";
my %props = (
	"buttonsmall" =>["trebuc","20","250x34",  "+40+0"],
	"buttonlarge" =>["trebuc","21","250x38",  "+24+3"],
	"title" => 	["trebucbd","50","940x400", "+0+1"],
	"section" => 	["trebucbd","32","940x60",  "+0+1"]
);

my %color = ( "back" => "#6c9bcd",
	       "fore" => "#14195d",
	       "activ" =>   "#fdfbcc",
	       "marker" =>  "#14195d",
	       "tfore" =>   "#14195d",
	       "tactiv" => "#5786bf",
	       "tback" =>   "#ffffff",	       
);

my $file = $text;
$file =~ s/\s+|\/|<|>|"|\?/_/g;
$file = "%1" if "%1";

if ($type =~ /^button/) {
      @filetask = ("$file-sel","$file-std","$file-ovr");
} else {
      @filetask = ($file);
}
foreach $file (@filetask){
       next if -e "$(ROOT)/.img/$file.gif";
       my ($q);
	
       $q=Image::Magick->new;     
       $q->Set(size=>$props{$type}[2]);
       if ($type =~ /^title/ ||$type =~ /^section/ ){
        $q->ReadImage("xc:$color{tback}");
	$col=$color{tfore};
       } else {
        $q->ReadImage("xc:$color{back}");
	$col=$color{fore};
       }
       $border=0;

       if ($type =~ /^title/){
        $q->Annotate(text=>$text,
	       geometry=>"+3+3",
	       font=>"\@${fontpath}/$props{$type}[0].ttf",
	       pointsize=>$props{$type}[1],
	       pen=>$color{tactiv},
	       align=>'left');
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
        $q->Blur(factor=>99);
       }

       if ($type =~ /^button/) {
       SWITCH: for ($file) {
            /-sel$/ && do { $col = $color{activ}; $border=1; last;};
            /-ovr$/ && do { $col = $color{activ};
		if ($type =~ /small$/) {
			    $q->Draw(primitive=>'FillPolygon',
				     pen=>$color{marker},
				     points => "10,4 24,14 10,24");
		} else {
			    $q->Draw(primitive=>'FillPolygon',
				     pen=>$color{marker},
				     points => "0,10 14,20 0,30");
		}
			    last;};
            /-std$/ && do { $col = $color{fore}; last;};
          }
        }


        $q->Annotate(text=>$text,
	       geometry=>$props{$type}[3],
	       font=>"\@${fontpath}/$props{$type}[0].ttf",
	       pointsize=>$props{$type}[1],
	       pen=>$col,
	       align=>'left');

       $q->Scale(geometry=>"50%");
       $q->Crop(geometry=>"0x0+1+1") unless $type =~ /^button/;
       $q->Sharpen(factor=>"30%") if $type =~ /^button/;
       $q->Quantize(colors=>15,olorspace=>"RGB", dither=>"true");	
       $q->Write(filename=>"gif:$(ROOT)/.img/$file.gif", 
                 colors=>16,							 
                 interlace=>"line",
	         compress=>'lzw');
}

print "$(ROOT)/.img/$file.gif" unless "%1";

</perl>
</define-container>

<define-container H1>
..!H1>>
<H1*><IMG SRC="<MAGICK Title>%body</MAGICK>" ALT="<: print qq{%body} :> "></H1*>
<<..
..!TITLE>><: print qq{%body} :><<..
</define-container>

<define-container H2>
<DIV><vspace 6><BR><IMG SRC="<MAGICK Section>%body</MAGICK>" ALT="%body"><BR><vspace 5></DIV>
</define-container>

#<define-container P>
#<DIV>%body<BR><vspace 4></DIV>
#</define-container>

<define-container DD>
<DD*>%BODY</DD*><P*>
</define-container>

<define-container DT>
<DT*><B>%BODY</B></DT*>
</define-container>

