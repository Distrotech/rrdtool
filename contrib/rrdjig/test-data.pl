#!/usr/bin/perl -w
require 5.008;
use lib qw(/scratch/rrd-1.4.3-test2/lib/perl);  
use RRDs;
my $start = time - 6*15*60;
$start -= $start % 60;
RRDs::create(qw(
    --step=60
    src.rrd    
    DS:in:COUNTER:65:0:1000
    DS:out:COUNTER:65:0:1000
    DS:xyz:GAUGE:65:0:1000
    RRA:AVERAGE:0.5:1:10
    RRA:MIN:0.5:1:10
    RRA:MAX:0.5:2:10
    RRA:AVERAGE:0.5:3:10
    RRA:MIN:0.99:4:20
    RRA:MAX:0.5:3:10
    RRA:AVERAGE:0.5:5:10
    RRA:MIN:0.99:6:15
    RRA:MAX:0.5:4:15
),'--start',$start);
die RRDs::error if RRDs::error;
RRDs::create(qw(
    --step=60
    dst.rrd    
    DS:newin:COUNTER:65:0:1000
    DS:newout:COUNTER:65:0:1000
    RRA:AVERAGE:0.5:1:10
    RRA:MIN:0.5:1:10
    RRA:MAX:0.5:2:10
    RRA:AVERAGE:0.5:3:10
    RRA:MIN:0.99:4:20
    RRA:MAX:0.5:3:10
    RRA:AVERAGE:0.5:5:10
    RRA:MIN:0.99:6:15
    RRA:MAX:0.5:4:15
),'--start',$start);
die RRDs::error if RRDs::error;
my $a = 0;
for (my $i=$start;$i<=time;$i+=60){
    $a++;
    my $up = "$i:".($a*$a).":".$a.":".$a;
    RRDs::update('src.rrd',$up);
}
die RRDs::error if RRDs::error;


    
