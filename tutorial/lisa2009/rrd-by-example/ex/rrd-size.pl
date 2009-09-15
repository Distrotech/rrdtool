#!/usr/bin/perl
sub rrd_sizer {
  my ($ds_cnt,$rra_sz,$rra_cnt) = @_;
  system 'rrdtool', 'create', 'sizer.rrd',
      map({ "DS:d${_}:GAUGE:600:U:U" } 1..$ds_cnt),
      map({ "RRA:AVERAGE:0.5:1:$rra_sz" } 1..$rra_cnt);
  my $size = -s 'sizer.rrd';
  printf "DSs: %1d  RRA Row: %1d  RRAs: %1d == %3d byte\n", 
      $ds_cnt,$rra_sz,$rra_cnt,$size;
  return $size;
  }
#                       DSs RRAs RRA Rows  
my $base    = rrd_sizer  1,    1,    1;
my $ds      = rrd_sizer  2,    1,    1;
my $rra_sz  = rrd_sizer  1,    2,    1;
my $rra_cnt = rrd_sizer  1,    1,    2;
printf "+1 DS:      %3d byte\n",($ds - $base);
printf "+1 RRA Row: %3d byte\n",($rra_sz - $base);
printf "+1 RRA:     %3d byte\n",($rra_cnt - $base);
