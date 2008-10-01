#!/usr/bin/perl
#$ENV{PATH}='/scratch/rrd4/bin';
$ENV{PATH}=$ENV{HOME}.'/checkouts/rrdtool/branches/1.3/program/src:'.$ENV{PATH};
my $R='rrdtool';
my $width=600 ;
my $height=200 ;

$ENV{TZ}='MET';

sub create {   
    my %p = (@_);
    my $start;
    open my $fh,'<hw-demo.data';
    my $end = (split /\s/, <$fh>)[0]; # drop the first line;
    my @updates;
    $start = $end;    
    while (<$fh>){
        my @line = split;
        last if $start - $p{step} > $line[0];
        $start = $line[0];
        unshift @updates, join ":",@line[0,1,2];
    }
    system $R,'create','hw-demo.rrd',
          '--step' => $p{step},
          '--start' => ($start-1),
          'DS:in:GAUGE:'.($p{step}*1.5).':U:U',
          'DS:out:GAUGE:'.($p{step}*1.5).':U:U',
          'RRA:AVERAGE:0.5:1:'.$p{rows},
          'RRA:HWPREDICT:'.$p{rows}.':'.$p{alpha}.':'.$p{beta}.':'.$p{period};            
    # it seems that resize is a bit broken ... will have to look into this
    system $R,'resize','hw-demo.rrd',5,'GROW',$p{rows};
    system $R,'dump','hw-demo.rrd','x';
    system $R,'restore','x','x.rrd';
    rename 'x.rrd','hw-demo.rrd';
    unlink 'x';
    system $R,'update','hw-demo.rrd',@updates;
    return ($start,$end);
}

sub graph {
    my $file = shift;
    my $start = shift;
    my $end = shift;
    system $R, 'graph', $file,
              '--lazy',
              '--start' => $start,
              '--end'   => $end,
              '--color=BACK#ffff',
              '--color=CANVAS#ffff',
              '--color=SHADEA#ffff',
              '--color=SHADEB#ffff',
              '--lower-limit' => 0,
              '--pango-markup',
              '--height' => $height,
              '--width' => $width,
              '--imgformat' => 'PDF', @_;
}

sub quick {
    my $period = shift;
    my $alpha = shift;
    my $beta = shift;
    create ( step => 1800,
             rows => 16*24*12,
             period => $period,
             alpha => $alpha,
             beta => $beta );
   my $file = "HW-p${period}-a${alpha}-b${beta}";
   $file =~ s/\./_/g;
   graph  $file.'.pdf', 
          '00:00 20080916',
          'start+8d',
          'DEF:a=hw-demo.rrd:in:AVERAGE',
          'DEF:pred=hw-demo.rrd:in:HWPREDICT',
          'DEF:conf=hw-demo.rrd:in:DEVPREDICT',
          'DEF:fail=hw-demo.rrd:in:FAILURES',
          'TICK:fail#ff8:1:Failures',
          'CDEF:lowc=pred,conf,2,*,-',
          'CDEF:widc=conf,4,*',
          'LINE1:lowc',
          'AREA:widc#cfc:Band:STACK',
          'LINE0.1:0#3a1::STACK',
          'LINE0.1:lowc#3a1',             
          'LINE1:a#c00:InOctets',
          "LINE1:pred#0a0:Predicted p\\:$period, a\\:$alpha, b\\:$beta\\l";
}
    
my ($start,$end) = create ( step => 1800,
                            rows => 16*24*12,
                            period => 24*2,
                            alpha => 0.5,
                            beta => 0.001 );

$width=800;
$height=100;
graph  'HW-input.pdf', 
       '00:00 20080916',
       'start+14d',
       'DEF:a=hw-demo.rrd:in:AVERAGE',
       'LINE2:a#c00:InOctets';


$width=450;
$height=120;
quick 1,0.5,0.001;
quick 1,0.1,0.001;
quick 1,0.1,0.1;
quick 48,0.5,0.001;
quick 48,0.2,0.001;
quick 48,0.03,0.001;
quick 48,0.03,0.1;
