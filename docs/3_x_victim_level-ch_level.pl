#!/usr/bin/perl -w

$start =  1;
$end   = 32;
$step  =  2;


$content = "\n\e[0m  ";

for ($i = $start;$i<$end;$i+=$step) {
	$content .= sprintf "%3d", $i;
	
}
$content .= "\n";

$max = 0;
$min = 999;


$fx = '3*$i-$j';

for ($i=$start;$i<$end;$i+=$step) {
	for ($j=$start;$j<$end;$j+=$step) {
		if ($i>$j) {
			$h = $i;
			$l = $j;
		}
		else {
			$h = $j;
			$l = $i;
		}
		$value = eval($fx);
		$max = $value if ($value > $max);
		$min = $value if ($value < $max);
	}
}

for ($i=$start;$i<$end;$i+=$step) {
	$content .= sprintf "\e[0m%2d", $i;
	for ($j=$start;$j<$end;$j+=$step) {
		if ($i>$j) {
			$h = $i;
			$l = $j;
		}
		else {
			$h = $j;
			$l = $i;
		}

		$value = eval($fx);
		$color = $value/$max * ($max-$min)/5 + 2;
		$color = 2 if ($color < 2);
		$color = 7 if ($color > 7);
		$value = 0 if ($value < 0);
		
		$content .= sprintf "\e[3%dm%3d", $color, $value;
	}
	$content .= "\n";
}
$content .= "\e[0m";

print $content;

open FILE, ">do_examine.txt";
print FILE $content;

