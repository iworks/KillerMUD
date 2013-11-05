#!/usr/bin/perl -w

use strict;

my $starty =  1;
my $endy   = 40;
my $stepy  =  2;

my $startx = 13;
my $endx   = 32;
my $stepx  =  2;


my $normalizacja = 1;
my $nl = 0;
my $nh = 100;

my $content = "\n";

my $ox = "\e[0m  ";
for (my $x = $startx;$x<$endx;$x+=$stepx) {
	$ox .= sprintf "%3d", $x;
}
$ox .= "\n";


my $max = 0;
my $min = 999;

my ($h,$l);

my $fx = '4*$x - 2*$y';


$content .= $fx . "\n";
$content =~ s/\$//g;
$content .= $ox;


for (my $y=$starty;$y<$endy;$y+=$stepy) {
	for (my $x=$startx;$x<$endx;$x+=$stepx) {
		if ($y>$x) {
			$h = $y;
			$l = $x;
		}
		else {
			$h = $x;
			$l = $y;
		}
		my $value = eval($fx);
		$max = $value if ($value > $max);
		$min = $value if ($value < $max);
	}
}

if ($normalizacja) {
	$l = $nl if $l < $nl;
	$h = $nh if $h < $nh;
}

for (my $y=$starty;$y<$endy;$y+=$stepy) {
	$content .= sprintf "\e[0m%2d", $y;
	for (my $x=$startx;$x<$endx;$x+=$stepx) {
		if ($y>$x) {
			$h = $y;
			$l = $x;
		}
		else {
			$h = $x;
			$l = $y;
		}

		my $value = eval($fx);
		my $color = $value/$max * ($max-$min)/5 + 2;
		$color = 2 if ($color < 2);
		$color = 7 if ($color > 7);
		$value = 0 if ($value < 0);
		
		$content .= sprintf "\e[3%dm%3s", $color, ($value>99)? '--':$value;
	}
	$content .= "\n";
}
$content .= "\e[0m";

$content .= $ox . "\n\n";

print $content;

open FILE, ">do_undead_resaenblance.txt";
print FILE $content;

