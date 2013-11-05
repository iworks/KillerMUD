#!/usr/bin/perl -w
system('clear');
print ("-" x 74) . "\n";
print "\nPocz±tek prenoszenia krain, które nie s± w area.lst.\n";

$area_path      = '../../area/';
$area_path_move = $area_path.'areas_not_in_area_lst/';

%area = ();
if (open FILE, "<".$area_path."area.lst") {
	while (<FILE>) {
		if ($_ =~ /\.are$/) {
			$a = $_;
			$a =~ s/[\r\n]+//gms;
			$area{$a} = 'defined';
		}
	}
	close FILE;
}
opendir DIR, $area_path;
@temp = grep { /\.are$/i } readdir DIR;
close DIR;

foreach (@temp) {
	unless (defined($area{$_})) {
		$a = 'mv ' . $area_path . $_ . ' ' . $area_path_move;
		print 'przenosze: ' . $a . "\n";
 		system($a);
	}
}

print "\nPrzenoszenie krainy zakonczone sukcesem.\n\n";
print ("-" x 74) . "\n";
print "\n           -- yog 07 lutego 2002 --\n\n";
