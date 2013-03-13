#!/usr/bin/perl
#$ENV{PATH}='/scratch/rrd4/bin';
use Math::BigInt;
$ENV{PATH}=$ENV{HOME}.'/checkouts/rrdtool/branches/1.3/program/src:'.$ENV{PATH};
my $R=rrdtool;
my $w=320 ;
my $h=200 ;
my $start = 1199999700;
if (not -f 'b.rrd'){
    system $R,'create','b.rrd',
          '--step' => 300,
          '--start' => ($start-1),
          'DS:a:COUNTER:600:U:U',
          'RRA:AVERAGE:0.4:1:2100';

    my @updates;
    my $count = Math::BigInt->new(0);
    my $add =  Math::BigInt->new('70000000000000') ;
    for (my $i = 1; $i < 100;$i++){
        $count = $count +  $add * Math::BigInt->new(''.int(rand(10)));
        print "$count\n";
        push @updates, ($i*300+$start).':'.$count;
    }
    system $R,'update','b.rrd',@updates;
}

sub rg {
    my $file = shift;
    print STDERR $file,"\t";
    if (-f $file){
        print STDERR " skip\n";
        return;
    }
    my @G = ( '--start' => $start+3600,
              '--end'   => $start + 100 * 280,
#              '--full-size-mode',
              '--color=BACK#ffff',
              '--color=CANVAS#ffff',
              '--color=SHADEA#ffff',
              '--color=SHADEB#ffff',
              '--lower-limit' => 0,
              '--pango-markup',
              '--height' => $h, '--width' => $w,
              '--imgformat' => 'PDF',
              'DEF:a=b.rrd:a:AVERAGE');
   system $R, 'graph', $file, @G, @_;
}

rg 'bigLINE.pdf', 
               '--lower-limit' => 1000,
               'LINE:a#11a03b:DEF\:a=x.rrd\:a\:AVERAGE',
