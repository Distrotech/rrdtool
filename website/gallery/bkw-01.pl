#!/usr/local/bin/perl -w
#From: Brian K. West <brian@bkw.org>
#
# snmpwalks all nas boxen specified in the configuration file,
# tallies the number of connect speeds, and returns a percentage
# of various data sets in the form:
#
# N:blah:blah:blah:blah:blah:blah
#
# requires a configuration file specified by $naslist
# config file format is:
# nas name:community
# i.e.
# pm1-stigler:communitystring
#
#
# mad propz to teep
# Need to redo graphing but hey it works..
#
use lib qw( /usr/local/rrdtool-1.0.8/lib/perl ../lib/perl );
use RRDs;
use strict;

my($totlines)   = 248; # Total Ports
my($workdir) = "/export/home/brian/public_html"; # Directory where you want
the images copied too
my($uptime) = `uptime`; # Uptime is printed on graph
my($rrd) = "overall.rrd"; # RRD file name
my($target) = "overall"; # Target name(Used for Images)
my($option) = "GAUGE"; # This can get COUNTER, GAUGE
my($naslist) = "./nas.dat"; # path and name of configuration file
my($domain) = "cwis.net"; # your domain name (used for snmpwalk)
my($path_to_snmpwalk) = "/usr/local/bin"; # no trailing slash

# speed ranges are totally dynamic
# ranges are  range[x] to ((range[x+1]) - 1)
# i.e.  1 to 14399, 14400 to 28799, 28800 to 33599, et al
#


my(@speed_range) = qw(1 14400 28800 33600 40000 43000 45000 50000 65000);

### nothing else should need configured below this line ###
my($cpyfiles);
my($ERROR);
my($rrdata);
my($filetest);

$filetest = stat($rrd);
if(!$filetest) {
RRDs::create ($rrd,  "--step",300,
        "DS:a:$option:600:0:100",
        "DS:b:$option:600:0:100",
        "DS:c:$option:600:0:100",
        "DS:d:$option:600:0:100",
        "DS:e:$option:600:0:100",
        "DS:f:$option:600:0:100",
        "DS:g:$option:600:0:100",
        "DS:h:$option:600:0:100",
        "DS:i:$option:600:0:100",
        "DS:j:$option:600:0:100",
        "RRA:AVERAGE:0.5:1:600",
        "RRA:AVERAGE:0.5:6:700",
        "RRA:AVERAGE:0.5:24:775",
        "RRA:AVERAGE:0.5:288:797",
        "RRA:MAX:0.5:1:600 ",
        "RRA:MAX:0.5:6:700 ",
        "RRA:MAX:0.5:24:775",
        "RRA:MAX:0.5:288:797")
;

my $ERROR = RRDs::error;
die "$0: unable to create `$rrd': $ERROR\n" if $ERROR;
}

my($num_ranges) = 0;
$num_ranges = @speed_range;
my(%speeds);

open(NASLIST, "<$naslist");
my($line);
my($foo) = 0;
my(@nases);
my(%comms);
my(@raw_data);
my($line_count) = 0;

while(defined ($line = <NASLIST>)) {
chop $line;
my($nas,$community) = split(/:/,$line);
$nases[$foo++] = $nas;
$comms{$nas} = $community;
}

my($speed_list);
foreach $speed_list (@speed_range) {
$speeds{$speed_list} = 0;
}
undef($speed_list);

my(@data,$cnt);

foreach $cnt (0 .. $#nases) {
my($nas) = $nases[$cnt];
@data = `$path_to_snmpwalk/snmpwalk -v 1 $nas.$domain $comms{$nas}
interfaces.ifTable.ifEntry.ifSpeed`;


my($blah);
foreach $blah (@data) {
my($a);
chomp $blah;
(undef,$a) = split(/:/,$blah);
$a =~ s/ file://g;
if($a > 1 && $a < 65000) {
$raw_data[$line_count++] = $a;
}
}

}

my($foo3) = 0;
my($foo2);
foreach $foo2 (@raw_data) {
chomp $foo2;
my($carrier);
($carrier = $foo2) =~ s/ file://g;
my($test);
my($i) = 0;
foreach $i (0 .. $num_ranges) {
if($i+1 < $num_ranges && ($carrier >= $speed_range[$i] && $carrier <=
(($speed_range[$i+1])-1)))
{
$speeds{$speed_range[$i]}++;
$foo3++;

}

}
}

my($total,$num) = 0;
my($key,$value);
$rrdata = "N";
#print "N";
foreach $key (sort keys %speeds) {
$num = $speeds{$key};
$total = ($num/$line_count)*100;
$total = sprintf("%3.2f",$total);
$rrdata .=":$total";
}
#print "$rrdata";
my($utilization);
$utilization = ($foo3/$totlines)*100;
$utilization = sprintf("%3.2f",$utilization);
$rrdata .=":$utilization";
close(NASLIST);

RRDs::update $rrd, "$rrdata";
        if ($ERROR = RRDs::error) {
                die "$0: unable to update `$rrd': $ERROR\n";
        };

RRDs::graph "$target-day.gif",
        "--start", " -86400",
        "--title", "Modem Carrier Stats.(Daily) - $target",
        "--rigid",
        "--width=600",
        "--height=250",
        "--upper-limit","100",
        "--lower-limit","0",
        "--vertical-label","Percent(%)",
        "DEF:a=$rrd:a:AVERAGE",
        "DEF:b=$rrd:b:AVERAGE",
        "DEF:c=$rrd:c:AVERAGE",
        "DEF:d=$rrd:d:AVERAGE",
        "DEF:e=$rrd:e:AVERAGE",
        "DEF:f=$rrd:f:AVERAGE",
        "DEF:g=$rrd:g:AVERAGE",
        "DEF:h=$rrd:h:AVERAGE",
"DEF:j=$rrd:j:AVERAGE",
        "AREA:a#FFFF00:    1-14399",
        "GPRINT:a:LAST:Current \\: %8.2lf %s",
        "GPRINT:a:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:a:MIN:Min \\: %8.2lf %s",
        "GPRINT:a:MAX:Max \\: %8.2lf %s\\n",
        "STACK:b#C0C0C0:14400-28799",
        "GPRINT:b:LAST:Current \\: %8.2lf %s",
        "GPRINT:b:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:b:MIN:Min \\: %8.2lf %s",
        "GPRINT:b:MAX:Max \\: %8.2lf %s\\n",
        "STACK:c#FF7E6C:28800-33599",
        "GPRINT:c:LAST:Current \\: %8.2lf %s",
        "GPRINT:c:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:c:MIN:Min \\: %8.2lf %s",
        "GPRINT:c:MAX:Max \\: %8.2lf %s\\n",
        "STACK:d#6DC8FE:33600-39999",
        "GPRINT:d:LAST:Current \\: %8.2lf %s",
        "GPRINT:d:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:d:MIN:Min \\: %8.2lf %s",
        "GPRINT:d:MAX:Max \\: %8.2lf %s\\n",
        "STACK:e#00FF00:40000-42999",
        "GPRINT:e:LAST:Current \\: %8.2lf %s",
        "GPRINT:e:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:e:MIN:Min \\: %8.2lf %s",
        "GPRINT:e:MAX:Max \\: %8.2lf %s\\n",
        "STACK:f#FF00FF:43000-44999",
        "GPRINT:f:LAST:Current \\: %8.2lf %s",
        "GPRINT:f:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:f:MIN:Min \\: %8.2lf %s",
        "GPRINT:f:MAX:Max \\: %8.2lf %s\\n",
        "STACK:g#FF0000:45000-49999",
        "GPRINT:g:LAST:Current \\: %8.2lf %s",
        "GPRINT:g:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:g:MIN:Min \\: %8.2lf %s",
        "GPRINT:g:MAX:Max \\: %8.2lf %s\\n",
        "STACK:h#4444FF:50000-65000",
        "GPRINT:h:LAST:Current \\: %8.2lf %s",
        "GPRINT:h:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:h:MIN:Min \\: %8.2lf %s",
        "GPRINT:h:MAX:Max \\: %8.2lf %s\\n",
"LINE1:j#000000:Utilization",
        "GPRINT:j:LAST:Current \\: %8.2lf %s",
        "GPRINT:j:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:j:MIN:Min \\: %8.2lf %s",
        "GPRINT:j:MAX:Max \\: %8.2lf %s\\n",
        "COMMENT:\\n",
        "COMMENT:                                 Graph Shows Percent of
user connected at X Speed.\\n",
        "COMMENT:\\n",
        "COMMENT:                  $uptime",
;
if ($ERROR = RRDs::error) {
  print "ERROR: $ERROR\n";
};
RRDs::graph "$target-week.gif",
        "--start", " -604800",
        "--title", "Modem Carrier Stats.(Weekly) - $target",
        "--rigid",
        "--width=600",
        "--height=250",
        "--upper-limit","100",
        "--lower-limit","0",
        "--vertical-label","Percent(%)",
        "DEF:a=$rrd:a:AVERAGE",
        "DEF:b=$rrd:b:AVERAGE",
        "DEF:c=$rrd:c:AVERAGE",
        "DEF:d=$rrd:d:AVERAGE",
        "DEF:e=$rrd:e:AVERAGE",
        "DEF:f=$rrd:f:AVERAGE",
        "DEF:g=$rrd:g:AVERAGE",
        "DEF:h=$rrd:h:AVERAGE",
"DEF:j=$rrd:j:AVERAGE",
        "AREA:a#FFFF00:    1-14399",
        "GPRINT:a:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:a:MIN:Min \\: %8.2lf %s",
        "GPRINT:a:MAX:Max \\: %8.2lf %s\\n",
        "STACK:b#C0C0C0:14400-28799",
        "GPRINT:b:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:b:MIN:Min \\: %8.2lf %s",
        "GPRINT:b:MAX:Max \\: %8.2lf %s\\n",
        "STACK:c#FF7E6C:28800-33599",
        "GPRINT:c:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:c:MIN:Min \\: %8.2lf %s",
        "GPRINT:c:MAX:Max \\: %8.2lf %s\\n",
        "STACK:d#6DC8FE:33600-39999",
        "GPRINT:d:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:d:MIN:Min \\: %8.2lf %s",
        "GPRINT:d:MAX:Max \\: %8.2lf %s\\n",
        "STACK:e#00FF00:40000-42999",
        "GPRINT:e:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:e:MIN:Min \\: %8.2lf %s",
        "GPRINT:e:MAX:Max \\: %8.2lf %s\\n",
        "STACK:f#FF00FF:43000-44999",
        "GPRINT:f:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:f:MIN:Min \\: %8.2lf %s",
        "GPRINT:f:MAX:Max \\: %8.2lf %s\\n",
        "STACK:g#FF0000:45000-49999",
        "GPRINT:g:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:g:MIN:Min \\: %8.2lf %s",
        "GPRINT:g:MAX:Max \\: %8.2lf %s\\n",
        "STACK:h#4444FF:50000-65000",
        "GPRINT:h:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:h:MIN:Min \\: %8.2lf %s",
        "GPRINT:h:MAX:Max \\: %8.2lf %s\\n",
"LINE1:j#000000:Utilization",
        "GPRINT:j:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:j:MIN:Min \\: %8.2lf %s",
        "GPRINT:j:MAX:Max \\: %8.2lf %s\\n",
        "COMMENT:\\n",
        "COMMENT:                                 Graph Shows Percent of
user connected at X Speed.\\n",
        "COMMENT:\\n",
        "COMMENT:                  $uptime",
;
if ($ERROR = RRDs::error) {
  print "ERROR: $ERROR\n";
};
RRDs::graph "$target-month.gif",
        "--start", " -2600640",
        "--title", "Modem Carrier Stats.(Monthly) - $target",
        "--rigid",
        "--width=600",
        "--height=250",
        "--upper-limit","100",
        "--lower-limit","0",
        "--vertical-label","Percent(%)",
        "DEF:a=$rrd:a:AVERAGE",
        "DEF:b=$rrd:b:AVERAGE",
        "DEF:c=$rrd:c:AVERAGE",
        "DEF:d=$rrd:d:AVERAGE",
        "DEF:e=$rrd:e:AVERAGE",
        "DEF:f=$rrd:f:AVERAGE",
        "DEF:g=$rrd:g:AVERAGE",
        "DEF:h=$rrd:h:AVERAGE",
"DEF:j=$rrd:j:AVERAGE",
        "AREA:a#FFFF00:    1-14399",
        "GPRINT:a:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:a:MIN:Min \\: %8.2lf %s",
        "GPRINT:a:MAX:Max \\: %8.2lf %s\\n",
        "STACK:b#C0C0C0:14400-28799",
        "GPRINT:b:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:b:MIN:Min \\: %8.2lf %s",
        "GPRINT:b:MAX:Max \\: %8.2lf %s\\n",
        "STACK:c#FF7E6C:28800-33599",
        "GPRINT:c:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:c:MIN:Min \\: %8.2lf %s",
        "GPRINT:c:MAX:Max \\: %8.2lf %s\\n",
        "STACK:d#6DC8FE:33600-39999",
        "GPRINT:d:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:d:MIN:Min \\: %8.2lf %s",
        "GPRINT:d:MAX:Max \\: %8.2lf %s\\n",
        "STACK:e#00FF00:40000-42999",
        "GPRINT:e:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:e:MIN:Min \\: %8.2lf %s",
        "GPRINT:e:MAX:Max \\: %8.2lf %s\\n",
        "STACK:f#FF00FF:43000-44999",
        "GPRINT:f:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:f:MIN:Min \\: %8.2lf %s",
        "GPRINT:f:MAX:Max \\: %8.2lf %s\\n",
        "STACK:g#FF0000:45000-49999",
        "GPRINT:g:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:g:MIN:Min \\: %8.2lf %s",
        "GPRINT:g:MAX:Max \\: %8.2lf %s\\n",
        "STACK:h#4444FF:50000-65000",
        "GPRINT:h:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:h:MIN:Min \\: %8.2lf %s",
        "GPRINT:h:MAX:Max \\: %8.2lf %s\\n",
"LINE1:j#000000:Utilization",
        "GPRINT:j:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:j:MIN:Min \\: %8.2lf %s",
        "GPRINT:j:MAX:Max \\: %8.2lf %s\\n",
        "COMMENT:\\n",
        "COMMENT:                                 Graph Shows Percent of
user connected at X Speed.\\n",
        "COMMENT:\\n",
        "COMMENT:                  $uptime",
;
if ($ERROR = RRDs::error) {
  print "ERROR: $ERROR\n";
};
RRDs::graph "$target-year.gif",
        "--start", " -31557600",
        "--title", "Modem Carrier Stats.(Yearly) - $target",
        "--rigid",
        "--width=600",
        "--height=250",
        "--upper-limit","100",
        "--lower-limit","0",
        "--vertical-label","Percent(%)",
        "DEF:a=$rrd:a:AVERAGE",
        "DEF:b=$rrd:b:AVERAGE",
        "DEF:c=$rrd:c:AVERAGE",
        "DEF:d=$rrd:d:AVERAGE",
        "DEF:e=$rrd:e:AVERAGE",
        "DEF:f=$rrd:f:AVERAGE",
        "DEF:g=$rrd:g:AVERAGE",
        "DEF:h=$rrd:h:AVERAGE",
"DEF:j=$rrd:j:AVERAGE",
        "AREA:a#FFFF00:    1-14399",
        "GPRINT:a:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:a:MIN:Min \\: %8.2lf %s",
        "GPRINT:a:MAX:Max \\: %8.2lf %s\\n",
        "STACK:b#C0C0C0:14400-28799",
        "GPRINT:b:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:b:MIN:Min \\: %8.2lf %s",
        "GPRINT:b:MAX:Max \\: %8.2lf %s\\n",
        "STACK:c#FF7E6C:28800-33599",
        "GPRINT:c:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:c:MIN:Min \\: %8.2lf %s",
        "GPRINT:c:MAX:Max \\: %8.2lf %s\\n",
        "STACK:d#6DC8FE:33600-39999",
        "GPRINT:d:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:d:MIN:Min \\: %8.2lf %s",
        "GPRINT:d:MAX:Max \\: %8.2lf %s\\n",
        "STACK:e#00FF00:40000-42999",
        "GPRINT:e:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:e:MIN:Min \\: %8.2lf %s",
        "GPRINT:e:MAX:Max \\: %8.2lf %s\\n",
        "STACK:f#FF00FF:43000-44999",
        "GPRINT:f:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:f:MIN:Min \\: %8.2lf %s",
        "GPRINT:f:MAX:Max \\: %8.2lf %s\\n",
        "STACK:g#FF0000:45000-49999",
        "GPRINT:g:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:g:MIN:Min \\: %8.2lf %s",
        "GPRINT:g:MAX:Max \\: %8.2lf %s\\n",
        "STACK:h#4444FF:50000-65000",
        "GPRINT:h:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:h:MIN:Min \\: %8.2lf %s",
        "GPRINT:h:MAX:Max \\: %8.2lf %s\\n",
"LINE1:j#000000:Utilization",
        "GPRINT:j:AVERAGE:Avg \\: %8.2lf %s",
        "GPRINT:j:MIN:Min \\: %8.2lf %s",
        "GPRINT:j:MAX:Max \\: %8.2lf %s\\n",
        "COMMENT:\\n",
        "COMMENT:                                 Graph Shows Percent of
user connected at X Speed.\\n",
        "COMMENT:\\n",
        "COMMENT:                  $uptime",
;
if ($ERROR = RRDs::error) {
  print "ERROR: $ERROR\n";
};


$cpyfiles = `cp -f *.gif $workdir`;
exit;






