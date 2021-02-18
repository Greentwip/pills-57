#!/usr/bin/perl -w

#die "Usage: extractseg rom seg0 [seg1] ...\n" unless $#ARGV > 0;
$rom = $ARGV[0];
%sym=();
while(<STDIN>) {
    @x = split;
    $sym{$x[2]} = $x[0] if (defined $x[2]);
}
shift;
while($#ARGV >= 0) {
    $name = $ARGV[0];
    $start = hex($sym{"_" . "$name" . "SegmentRomStart"});
    $end = hex($sym{"_" . "$name" . "SegmentRomEnd"});
    #print "$name $start $end\n";
    $end = $end - $start;
    shift;
    `dd if=$rom bs=1 skip=$start count=$end of=$name.seg`
}
