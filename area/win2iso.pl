#!/usr/bin/perl -w
##########################################################################
#                                                                        #
#  KILLER MUD is copyright 2005 Killer MUD Staff (alphabetical)          #
#                                                                        #
#     Pietrzak Marcin    (marcin.pietrzak@mud.pl        ) [Gurthg  ]     #
#                                                                        #
##########################################################################
# $Id: win2iso.pl,v 1.1 2005/06/12 15:19:14 gurthg Exp $*/
##########################################################################
use strict;
##########################################################################
my @lf;
if (opendir DIR, ".") {
	@lf = grep {/\.are$/} readdir DIR;
	close DIR;
}
else {
	die;
}
foreach my $file (@lf) {
	if ($file eq 'merc.h') {
		print "Pomijam plik merc.h!\n";
	}
	else {
		my $nf = '';
		if (open FILE, "<".$file) {
			while (<FILE>) {
				$nf .= $_;
			}
			close FILE;
			if ($nf =~ /[¹Ÿœ]/i) {
				print 'Plik: ' . $file . " zawiera polskie znaki CP-1250 - konweruje.\n";
				if (open FILE, ">".$file) {
					print FILE Win2ISO($nf);
					close FILE;
				}
			}
		}
	}
}
sub Win2ISO {
	my $tmp = shift;
	$tmp =~ s/\xa5/\xa1/g; # A
	$tmp =~ s/\x8c/\xa6/g; # S
	$tmp =~ s/\x8f/\xac/g; # Zi
	$tmp =~ s/\xb9/\xb1/g; # a
	$tmp =~ s/\x9c/\xb6/g; # s
	$tmp =~ s/\x9f/\xbc/g; # zi
	return $tmp;
}
