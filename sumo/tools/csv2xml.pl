#!/usr/bin/perl
# Converts a csv-file into a XML-representation
# The parameters are:
# 1. parameter: The name of the type to build ("Type" or "Edge")
# 2. parameter: The input file to use
# The columns of the file must be divided by an ';' while the rows must
# be divided by a single '\n'.
# The first line must contain the names of the fields, also divided with ';'
if($ARGV[1]) {
  $data_name = $ARGV[0];
  # use the second file to generate the output
  print "<".$data_name."s>\n";
  open(INDAT, "< $ARGV[1]") || die "DATA_FILE \"".$ARGV[1]."\" not found!\n";
  $a = <INDAT>;
  $a =~ s/[\n]*$//g;
  @field_names = split("\;", $a);
  while($a=<INDAT>) {
    $a =~ s/[\n]*$//g;
    undef @data;
    @data = split("\;", $a);
    print "   <".$data_name." ";
    for($i=0; defined($field_names[$i]); $i++) {
      print $field_names[$i]."=\"".$data[$i]."\"";
      if(defined($data[$i+1])&&defined($field_names[$i+1])) {
	print " ";
      }
    }
    print "/>\n";
  }
  print "</".$data_name."s>\n";
  close INDAT;
} else {
  print "Syntax Error!!!\n";
  print "Syntax: toxml.pl <DATA_NAME> <DATA_FILE>\n"
}

