#!/usr/bin/perl
#$ENV{PATH}='/scratch/rrd4/bin';
$ENV{PATH}=$ENV{HOME}.'/checkouts/rrdtool/branches/1.3/program/src:'.$ENV{PATH};
my $R=rrdtool;
my $w=1600 ;
my $h=100 ;
my $start = 1199999700;
sub cr {
    system $R,'create','h.rrd',
          '--step' => 300,
          '--start' => ($start-1),
          'DS:a:GAUGE:600:U:U',
          'RRA:AVERAGE:0.5:1:3100',
          'RRA:HWPREDICT:3100:0.2:0.01:48';
    my @updates;
    for (my $i = 1; $i < 3000;$i++){
        push @updates, ($i*300+$start).':'.(sin($i*3.14159265/24)*10*(cos($i/100)+3));

    }
    system $R,'update','h.rrd',@updates;
}

sub rg {
    my $file = shift;
    print STDERR $file,"\t";
#    if (-f $file){
#        print STDERR " skip\n";
#        return;
#    }
    my @G = ( '--start' => $start-3600,
              '--end'   => 'start+200h',
#              '--full-size-mode',
              '--color=BACK#ffff',
              '--color=CANVAS#ffff',
              '--color=SHADEA#ffff',
              '--color=SHADEB#ffff',
              '--lower-limit' => 0,
              '--pango-markup',
              '--height' => $h, '--width' => $w,
              '--imgformat' => 'PDF');
   system $R, 'graph', $file, @G, @_;
}
cr;

rg 'HW-LINE.pdf', 
               'DEF:a=h.rrd:a:AVERAGE',
               'DEF:pred=h.rrd:a:HWPREDICT',
               'DEF:conf=h.rrd:a:DEVPREDICT',
               'CDEF:lowc=pred,conf,2,*,-',
               'CDEF:widc=conf,4,*',
               'LINE1:lowc',
               'AREA:widc#cfc::STACK',
               'LINE1:0#3a1::STACK',
               'LINE1:lowc#3a1',             
               'LINE1:a#a00:a\l';            
#               'LINE1:pred#0a0:pred\l';

