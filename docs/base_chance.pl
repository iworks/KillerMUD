#!/usr/bin/perl -w

$start =  1;
$end   = 32;
$end_i = 60;
$step  =  2;


$content = "\n\e[0m  ";

for ($i = $start;$i<$end;$i+=$step) {
	$content .= sprintf "%4d", $i;
	
}
$content .= "\n";

$max = 0;
$min = 999;


#base_chance = 50 + 10 * ( victim->level - ( ch->level - ( ch->level > 26? 4: ( ch->level > 22? 2: 0 ) ) ) );
$fx = '40+$i-$j';
#f2 = '40 - $j';

for ($i=$start;$i<$end_i;$i+=$step) {
	for ($j=$start;$j<$end;$j+=$step) {
		if ($i>$j) {
			$h = $i;
			$l = $j;
		}
		else {
			$h = $j;
			$l = $i;
		}
        $x = eval($f2) if $f2;
		$value = eval($fx);

#        $value = 95 if $value > 95;
#        $value = 10 if $value < 10;

		$max = $value if ($value > $max);
		$min = $value if ($value < $max);
	}
}

for ($i=$start;$i<$end_i;$i+=$step) {
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
        
        $x = eval($f2) if $f2;
		$value = eval($fx);

#        $value = 95 if $value > 95;
#        $value = 10 if $value < 10;



		$color = $value/$max * ($max-$min)/5 + 2;
		$color = 2 if ($color < 2);
		$color = 7 if ($color > 7);
		$value = 0 if ($value < 0);
		
		$content .= sprintf "\e[3%dm%4d", $color, $value;
	}
	$content .= "\n";
}
$content .= "\e[0m";

print $content;

open FILE, ">do_examine.txt";
print FILE $content;

