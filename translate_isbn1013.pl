 use Business::ISBN;
 use strict;
 use warnings;
 use Business::ISBN qw(%ERROR_TEXT);
 use Data::Dumper;

 # For each ISBN found in a single string containing a set of ISBNs:
 # * Normalize an incoming ISBN to have the correct checksum and no hyphens
 # * Convert an incoming ISBN10 or ISBN13 to its counterpart and return

 my $input = shift;
 my $output = '';

 foreach my $word (split(/\s/, $input)) {
     my $isbn = Business::ISBN->new($word);
     print "Constructor validation: ".$ERROR_TEXT{$isbn->error}."\n";
     print Dumper($isbn);

     my $result = $isbn->{'valid'};
     print "initial validation result: ".$ERROR_TEXT{$result}."\n";

     # First check the checksum; if it is not valid, fix it and add the original
     # bad-checksum ISBN to the output
     if ($isbn && $isbn->is_valid_checksum() == Business::ISBN::BAD_CHECKSUM) {
         $output .= $isbn->isbn() . " ";
         $isbn->fix_checksum();
	my $result = $isbn->{'valid'};
     	print "next validation result: ".$ERROR_TEXT{$result}."\n";

     }
	print Dumper($isbn);

     $result = $isbn->is_valid();
     print "validation result: ".$ERROR_TEXT{$result}."\n";

     # If we now have a valid ISBN, convert it to its counterpart ISBN10/ISBN13
     # and add the normalized original ISBN to the output
     if ($isbn && $isbn->is_valid()) {
         my $isbn_xlated = ($isbn->type eq "ISBN13") ? $isbn->as_isbn10 : $isbn->as_isbn13;
         $output .= $isbn->isbn . " ";

         # If we successfully converted the ISBN to its counterpart, add the
         # converted ISBN to the output as well
         $output .= ($isbn_xlated->isbn . " ") if ($isbn_xlated);
     }
 }
 print $output."\n" if $output;
                                                                                           
 # If there were no valid ISBNs, just return the raw input +
 #print $input."\n";
