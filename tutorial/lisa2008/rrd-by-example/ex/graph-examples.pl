#!/usr/bin/perl
#$ENV{PATH}='/scratch/rrd4/bin';
$ENV{PATH}=$ENV{HOME}.'/checkouts/rrdtool/branches/1.3/program/src:'.$ENV{PATH};
my $R=rrdtool;
my $w=320 ;
my $h=200 ;
my $start = 1199999700;
if (not -f 'x.rrd'){
    system $R,'create','x.rrd',
          '--step' => 300,
          '--start' => ($start-1),
          'DS:a:GAUGE:600:-40:2100',
          'DS:b:GAUGE:600:-40:2100',
          'DS:r:GAUGE:600:-40:2100',
          'RRA:AVERAGE:0.4:1:2100',
#         'RRA:AVERAGE:0.4:3:100',
          'RRA:MIN:0.4:3:2100',
          'RRA:MAX:0.4:3:2100';

    my @updates;
    for (my $i = 1; $i < 100;$i++){
        push @updates, ($i*300+$start).':'.(sin($i/10)*40+sin($i/19)*10+50).':'.(cos($i/10)*40+cos($i/33)*15+70).':'.(cos($i/10)*10+sin($i/3)*35+70+rand(40));
    }
    system $R,'update','x.rrd',@updates;
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
              'DEF:a=x.rrd:a:AVERAGE',
              'DEF:r=x.rrd:r:AVERAGE',
              'DEF:b=x.rrd:b:AVERAGE');
   system $R, 'graph', $file, @G, @_;
}

rg 'LINE.pdf', 
               '--lower-limit' => 1000,
               'LINE:a#11a03b:DEF\:a=x.rrd\:a\:AVERAGE',
               'LINE:b#a1003b:DEF\:b=x.rrd\:b\:AVERAGE\l';

rg 'LINE-lower.pdf', 
           'LINE:a#11a03b',
           'LINE:b#a1003b',
           'COMMENT:--lower-limit=<b>0</b>';

rg 'LINE-slope.pdf',
          '--slope-mode',
          'LINE:a#11a03b',
          'LINE:b#a1003b',
          'COMMENT:<b>--slope-mode</b>';

rg 'LINE-graph-monos.png',
          '--graph-render-mode' => 'mono',
          '--imgformat' => 'PNG',
          '--zoom'=>1,
          'LINE:a#11a03b',
          'LINE3:b#a1003b',
          'COMMENT:--graph-render-mode=<b>mono</b>';

system "convert -scale 800% LINE-graph-monos.png LINE-graph-mono.png" unless -f "LINE-graph-mono.png";

rg 'LINE-font-monos.png',
          '--font-render-mode' => 'mono',
          '--zoom'=>1,
          '--imgformat' => 'PNG',
          'LINE:a#11a03b',
          'LINE3:b#a1003b',
          'COMMENT:--font-render-mode=<b>mono</b>';

system "convert -scale 800% LINE-font-monos.png LINE-font-mono.png" unless -f "LINE-font-mono.png";

rg "LINE-width.pdf",
               'LINE1:b#ff00ff:LINE<b>1</b>\:b#ff00ff',
               'LINE4:a#ffaa00:LINE<b>4</b>\:a#ffaa00\l';

rg "LINE-dash.pdf",
           'LINE1:a#ff00ff:LINE1\:a#ff00ff\:\:<b>dashes=10,10,80,10</b>\n:dashes=10,20,80,20',
           'LINE2:b#ffaa00:LINE2\:b#ffaa00\:\:<b>dashes=1,3</b>\:<b>dash-offset=10</b>:dashes=1,3:dash-offset=3';


rg "DEF-step.pdf",
           'DEF:c=x.rrd:a:AVERAGE:step=1800',
           'LINE3:a#ccc:DEF\:a=x.rrd\:a\:AVERAGE\n',
           'LINE1:c#f00:DEF\:b=x.rrd\:a\:AVERAGE\:<b>step=1800</b>';

rg "DEF-reduce.pdf",
           'DEF:c=x.rrd:a:AVERAGE:step=1800:reduce=MIN',
           "DEF:d=x.rrd:a:AVERAGE:step=1800:reduce=MAX",
           'LINE1:c#f00:DEF\:b=x.rrd\:a\:AVERAGE\:step=1800\:<b>reduce=MIN</b>\n',
           'LINE1:d#0a0:DEF\:c=x.rrd\:a\:AVERAGE\:step=1800\:<b>reduce=MAX</b>\n',
           'LINE1:a#888:DEF\:a=x.rrd\:a\:AVERAGE';

my $newstart = $start + 40*300;
rg "DEF-start.pdf",
           'DEF:c=x.rrd:a:AVERAGE:start='.$newstart,
           'LINE5:a#ccc:DEF\:a=x.rrd\:a\:AVERAGE\n',
           'LINE1:c#f00:DEF\:b=x.rrd\:a\:AVERAGE\:<b>start='.$newstart.'</b>';

rg 'AREA-simple.pdf',
          'AREA:a#f1805b:<b>AREA</b>\:a#a1003b',
          'LINE2:b#1180fb:LINE\:b#11a03b\l';

rg 'AREA-two.pdf',
          'AREA:a#f1805b:<b>AREA</b>\:a#a1003b',
          'AREA:b#1180fb:<b>AREA</b>\:b#11a03b\l';

rg 'AREA-trans.pdf',
          'AREA:a#f1805bff:AREA\:a#a1003b<b>ff</b>',
          'AREA:b#1180fb60:AREA\:b#11a03b<b>60</b>\l';

rg 'AREA-stack.pdf',
          'AREA:a#f1805b:AREA\:a#a1003b',
          'AREA:b#1180fb:AREA\:b#11a03b\:...\:<b>STACK</b>\l:STACK';

rg 'SHIFT-simple.pdf',
          'DEF:c=x.rrd:a:AVERAGE',
          'CDEF:d=c',
          'SHIFT:d:3600',
          'LINE:c#1f9',
          'LINE:d#417:CDEF\:b=a <b>SHIFT</b>\:b\:3600\l';


rg 'SHIFT-startdef.pdf',
          'DEF:c=x.rrd:a:AVERAGE:start='.($start-3600),
          'CDEF:d=c',
          'SHIFT:d:3600',
          'LINE:c#1f9',
          'LINE:d#417:CDEF\:b=a <b>SHIFT</b>\:b\:3600\l',
          'COMMENT:DEF\:a=x.rrd\:a\:AVERAGE\:<b>start='.($start-3600).'</b>\l';

rg 'RPN-simple.pdf',
          'CDEF:c=a,20,+',
          'LINE:a#1f9',
          'LINE:c#417:<b>CDEF</b>\:b=a,20,+\l';

rg 'RPN-max.pdf',
          'CDEF:c=a,b,MAX',
          'AREA:a#1f9:a',
          'AREA:b#41f:b',
          'LINE2:c#f00:c',
          'COMMENT:c=a,b,<b>MAX</b>\l';

rg 'RPN-limit.pdf',         
          'CDEF:c=a,30,70,LIMIT',     
          'LINE4:a#1f9:a',   
          'LINE2:c#41f:b',                     
          'COMMENT:b=a,30,70,<b>LIMIT</b>\l';

rg 'RPN-trend.pdf',         
          'CDEF:k=r,3600,TREND',
          'LINE1:r#3a1:a',   
          'LINE1:k#21f:b',                     
          'COMMENT:b=a,3600,<b>TREND</b>\l';

rg 'RPN-trend-start.pdf',         
          'DEF:rr=x.rrd:r:AVERAGE:start='.($start-3600),
          'CDEF:k=rr,3600,TREND',
          'COMMENT:DEF\:a=x.rrd\:a\:AVERAGE\:<b>start='.($start-3600).'</b>\l',
          'LINE1:r#3a1:a',   
          'LINE1:k#21f:b',                     
          'COMMENT:b=a,3600,TREND\l';

rg 'RPN-trend-shift.pdf',         
          'DEF:rr=x.rrd:r:AVERAGE:start='.($start-3600),
          'CDEF:k=rr,3600,TREND',
          'SHIFT:k:-1800',
          'LINE1:r#3a1:a',   
          'COMMENT:CDEF\:b=a,3600,TREND <b>SHIFT</b>\:b\:-1800',
          'LINE1:k#21f:b\l';                    

if ( ! -f '1.rrd'){
        system $R,'create','1.rrd',
                  '--step' => 1,
                  '--start' => ($start-1),
                  'DS:a:GAUGE:2:U:U',
                  'RRA:AVERAGE:0.4:1:1' };


rg 'RPN-if.pdf',
          'CDEF:c=a,b,LT,a,b,IF,4,-',
          'LINE1:a#3a1:a',   
          'LINE1:b#21f:b',                     
          'AREA:c#f00a:c=a,b,<b>LT</b>,a,b,<b>IF</b>,4,-\l';

rg 'RPN-UNKN.pdf',
          'CDEF:c=a,b,GT,a,UNKN,IF',
          'CDEF:d=a,b,LT,b,UNKN,IF',
          'AREA:c#8a1:c=a,b,GT,a,<b>UNKN</b>,IF',   
          'AREA:d#91f:d=a,b,LT,b,<b>UNKN</b>,IF\l';

rg 'RPN-count.pdf',
          'CDEF:c=COUNT,3,%,0,EQ,a,UNKN,IF',
          'AREA:c#8a1:c=<b>COUNT</b>,3,%,0,EQ,a,UNKN,IF';

rg 'RPN-time.pdf',
          'CDEF:c=TIME,1800,%,900,GE,a,UNKN,IF',
          'AREA:c#8a1:c=<b>TIME</b>,1800,%,900,GE,a,UNKN,IF';

rg 'RPN-time-minus.pdf',
          'CDEF:c=TIME,1,-,1800,%,900,GE,a,UNKN,IF',
          'AREA:c#8a1:c=TIME,<b>1,-</b>,1800,%,900,GE,a,UNKN,IF';

if ( ! -f '1.rrd'){
        system $R,'create','1.rrd',
                  '--step' => 1,
                  '--start' => ($start-1),
                  'DS:a:GAUGE:2:U:U',
                  'RRA:AVERAGE:0.4:1:1' };

rg 'RPN-time-odd.pdf',
          'CDEF:c=TIME,1756,%,180,GE,a,UNKN,IF',
          'AREA:c#8a1:c=TIME,1756,%,180,GE,a,UNKN,IF';

rg 'RPN-time-odd-hires.pdf',
          'DEF:s=1.rrd:a:AVERAGE',
          'CDEF:c=s,POP,TIME,1756,%,180,GE,a,UNKN,IF',
          'COMMENT:DEF\:one=1.rrd\:one\:AVERAGE',
          'AREA:c#8a1:c=one,POP,TIME,1756,%,180,GE,a,UNKN,IF';

rg 'RPN-prev.pdf',
          'CDEF:c=COUNT,3,%,0,EQ,a,UNKN,IF',
          'CDEF:d=COUNT,3,%,1,EQ,PREV,c,IF',
          'COMMENT:CDEF\:c=COUNT,3,%,0,EQ,a,UNKN,IF',
          'AREA:d#8a1:d=COUNT,3,%,1,EQ,<b>PREV</b>,c,IF';
        
rg 'VDEF-average.pdf', 
               'VDEF:aavg=a,AVERAGE',
               'LINE2:a#11a03b88',
               'LINE:aavg#01902b:b',
               'GPRINT:aavg:avg %.1lf',
               'COMMENT:<b>VDEF</b>\:b=a,AVERAGE <b>GPRINT</b>\:b\:avg %.1lf\l';

rg 'VDEF-minmax.pdf', 
               'LINE2:a#4f4',
               'VDEF:amax=a,MAXIMUM',
               'LINE:amax#123',
               'VRULE:amax#123:max',          
               'GPRINT:amax:%.1lf',
               'GPRINT:amax:%H\:%M:strftime',
               'COMMENT:VDEF\:max=a,MAXIMUM\l',
               'VDEF:amin=a,MINIMUM',
               'LINE:amin#48f',
               'VRULE:amin#48f:min',
               'GPRINT:amin:%.1lf',
               'GPRINT:amin:%H\:%M:strftime',
               'COMMENT:VDEF\:min=a,MINIMUM\l';

rg 'VDEF-lsl.pdf', 
               'VDEF:slope=a,LSLSLOPE',
               'VDEF:int=a,LSLINT',                       
               'CDEF:lsl=a,POP,COUNT,slope,*,int,+',
               'GPRINT:slope:VDEF\:slope=a,LSLSLOPE (%.3lf)\l',
               'GPRINT:int:VDEF\:int=a,LSLINT (%.1lf)\l',
               'LINE2:a#8f1:a',
               'LINE2:lsl#f71:lsl=a,POP,COUNT,slope,*,int,+\l';

