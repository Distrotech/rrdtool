#!/usr/bin/perl -w
use strict;
use lib qw( /scratch/rrd4/lib/perl );
use RRDs;
my $out = RRDs::graphv(
     '-', '--start' => '00:00 20080916',
     '--end' => 'start+8d',
     '--lower-limit' => 0,
     '--imgformat' => 'PDF',
     'DEF:a=hw-demo.rrd:in:AVERAGE',
     'LINE1:a#c00:InOctets');
my $ERROR = RRDs::error;
die "ERROR: $ERROR\n" if $ERROR;
map { 
    print $_.' = '.substr($out->{$_},0,8)."\n" 
} sort keys %$out;
