# vim:sw=4 sta et showmatch

# db2x_manxml - convert Man-XML to Texinfo
#               (See docbook2X documentation for details)
#
# (C) 2000-2004 Steve Cheng <stevecheng@users.sourceforge.net>
#
# See the COPYING file in the docbook2X distribution
# for the copyright status of this software.
#
# Note: db2x_manxml.pl does not run by itself!
#       It must be configured by including a config.pl file
#       which is done when building docbook2X.
#       In addition, the non-standard module
#       XML::Handler::SGMLSpl must be explicitly loaded
#       when docbook2X is not installed.

use strict;


##################################################
#
# Option parsing
#
##################################################

use Getopt::Long;
Getopt::Long::Configure('gnu_getopt');
my $cmdoptions = {
    'encoding' => 'us-ascii',
    'list-files' => 0,
    'to-stdout' => 0,
    'symlinks' => 0,
    'solinks' => 0,
    'no-links' => 0,
    'utf8trans-program' => $db2x_config{'utf8trans-program'},
    'utf8trans-map' => $db2x_config{'utf8trans-map-roff'},
    'iconv-program' => $db2x_config{'iconv-program'},
};

sub options_help {
    print "Usage: $0 [OPTION]... [FILE]...\n";
    print <<'end';
Make man pages from Man-XML

  --encoding=ENCODING   Character encoding for man pages
                        Default is US-ASCII
  --list-files          Write list of output files to stdout
  --to-stdout           Write output to stdout instead of to files

  Some man pages are made available under multiple names. Use one of
  the following to select how should these names be made available:
  --symlinks            Symbolic links to principal man page
  --solinks             Stub pages with .so requests to principal man page
  --no-links            Make man page available only under principal name

  These options set the location of auxiliary programs:
  --utf8trans-program=PATH, --utf8trans-map=PATH, --iconv-program=PATH

  --help                Show this help and exit
  --version             Show version and exit

See the db2x_manxml(1) manual page and the docbook2X documentation for
more details.
end
    exit 0;
}

sub options_version
{
    print "db2x_manxml (part of docbook2X " .
            $db2x_config{'docbook2X-version'} . ")\n";
    print <<'end';
$Revision$ $Date$
<URL:http://docbook2x.sourceforge.net/>

Copyright (C) 2000-2004 Steve Cheng
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
end
    exit 0;
}

$SIG{__WARN__} = sub { print STDERR "$0: " . $_[0]; };
if(!GetOptions($cmdoptions,
    'encoding=s',
    'list-files',
    'to-stdout',
    'symlinks',
    'solinks',
    'no-links',
    'utf8trans-program=s',
    'utf8trans-map=s',
    'iconv-program=s',
    'help', \&options_help,
    'version', \&options_version))
{
    print STDERR "Try \"$0 --help\" for more information.\n";
    exit 1;
}
$SIG{__WARN__} = undef;

if($cmdoptions->{'symlinks'} +
   $cmdoptions->{'solinks'} +
   $cmdoptions->{'no-links'} > 1)
{
    print STDERR "$0: Only one of --symlinks, --solinks or --no-links options is allowed!\n";
    exit 1;
} elsif($cmdoptions->{'symlinks'} +
   $cmdoptions->{'solinks'} +
   $cmdoptions->{'no-links'} == 0)
{
   $cmdoptions->{'no-links'} = 1;
}

use XML::Handler::SGMLSpl;
$manxml::templates = XML::Handler::SGMLSpl->new(
                        { 'options' => $cmdoptions });
$manxml::templates->push_mode('file-unselected');



##################################################
#
# RoffWriter: Sanitized output routines
#
##################################################

package RoffWriter;
require Exporter;
@RoffWriter::ISA = qw(Exporter);
@RoffWriter::EXPORT_OK = qw(man_escape man_normalize_ws);

#
# Use RoffWriter on specified file
# Params: fh - an IO::Handle to send the output
#
sub new
{
    my ($class, $fh) = @_;
    my $self = { fh => $fh, line_start => 1, whitespace_last => 0 };
    return bless($self, $class);
}

#
# Write output to filehandle
# or the string buffer (see buffer_{on,off}).
#
sub write
{
    my ($self, $text) = @_;
    if(defined $self->{stringbuffer}) {
        $self->{stringbuffer} .= $text;
    } else {
        $self->{fh}->print($text);
    }
}

#
# buffer_on, buffer_off
#
# Hack to allow the user to buffer output temporarily
# to a string which can be back pasted in later with write().
#
# This is used, in particular, for tbl output:
# The column and span width information must precede
# the cell data for each row, but the cell data may be
# arbitrarily complex text mixed with any number of
# roff requests.
#
# Don't take this facility lightly :)
# It is low-level: it intentionally does not save or restore
# the line_start internal state --- you must do that yourself,
# or use this facility only at well-defined points
# (e.g. only at line_start = 1).
#
sub buffer_on
{
    my ($self) = @_;
    $self->{stringbuffer} = '';
}

sub buffer_off
{
    my ($self) = @_;
    my $stringbuffer = $self->{stringbuffer};
    $self->{stringbuffer} = undef;
    return $stringbuffer;
}

#
# Print text with whitespace folding
# Usually need to escape text first
# Params: text - string to print
#
sub print_ws
{
    my ($self, $text) = @_;

    my @lines = split(/(\n)/, $text);

    foreach my $line (@lines) {
        # Go to beginning of next line
        # unless already at beginning of a line
        if($line eq "\n") {
            $self->write("\n")
                unless $self->{line_start}++;
        } else {
            # No spaces at the beginning of a line.
            #
            # The second if condition ensures that whenever the
            # 'logical' input contains a string of whitespace, but
            # the 'logical' input is split into two separate print_ws
            # in the middle of the string of whitespace, like this:
            #
            # print_ws('text      ') ; print_ws('     more text')
            #
            # Then the output should be 'text more text'
            # with one space in between the (first two) words.
            #
            # NB: The use of 'whitespace' here excludes the newline.
            # NB: whitespace_last is only meaningful when
            #     line_start == 0.  This interpretation
            #     allows us to not have to update
            #     the whitespace_last status as much.
            #
            $line =~ s/^[ \t]+// if $self->{line_start}
                or $self->{whitespace_last};

            # "." and "'" get misinterpreted as a request
            # at beginning of lines, so use a no-width space
            # to prevent that.
            $line =~ s/^([.'])/\\\&$1/ if $self->{line_start};

            # Fold whitespaces in the middle of the line
            $line =~ tr/ \t/ /s;

            if($line ne '') {
                $self->write($line);
                $self->{line_start} = 0;
                $self->{whitespace_last} = ($line =~ /[ \t]$/);
            }
        }
    }
}

#
# Print text without folding whitespace
# Usually need to escape text first
# Params: text - string to print
#
sub print
{
    my ($self, $text) = @_;

    my @lines = split(/(\n)/, $text);

    foreach my $line (@lines) {
        if($line eq "\n") {
            $self->write("\n");
            $self->{line_start} = 1;
        } else {
            # "." and "'" get misinterpreted as a request
            # at beginning of lines, so use a no-width space
            # to prevent that.
            $line =~ s/^([.'])/\\\&$1/ if $self->{line_start};

            $self->{line_start} = ($line eq '');
            $self->{whitespace_last} = ($line =~ /[ \t]$/);
            $self->write($line);
        }
    }
}

sub skip_line
{
    my ($self) = @_;
    $self->write(($self->{line_start}++? "\n" : "\n\n"));
}

#
# Make a roff "request"
# Params: name - request identifier.  The beginning '.' is optional.
#         args - request arguments, in raw form.
#                If there are characters to be escaped,
#                wrap the string in a call to string_escape.
# Does not support using a different request start character.
#
sub request
{
    my ($self, $name, @args) = @_;

    $name = ".$name" if $name !~ /^[\.']/;

    $self->write
        ($self->{line_start} ? "$name" : "\n$name");

    foreach my $arg (@args) {
        $arg =~ tr/\n/ /;

        # Suggestion from groff Info manual.
        # Escape double quotes using \(dq
        $arg =~ s/"/\\\(dq/g;

        $arg = '"' . $arg . '"' if $arg =~ / /;
        $self->write(' ' . $arg);
    }

    $self->write("\n");
    $self->{line_start} = 1;
}

#
# Print a comment in the output.
# Params: comment - the comment text.
#                   May use any characters; they need not be escaped.
#
sub comment
{
    my ($self, $comment) = @_;
    $self->write("\n") unless $self->{line_start};

    foreach my $line (split(/\n/, $comment)) {
        $self->write('.\" ');
        $self->write($line);
        $self->write("\n");
    }

    $self->{line_start} = 1;
}


#
# Use a roff "escape" i.e. commands embedded in text starting with \
# Params: escape - the escape sequence, excluding \
# Does not support using a different escape character.
#
sub escape
{
    my ($self, $escape) = @_;
    $self->write("\\$escape");
}

#
# Escape characters special to roff so they are displayed literally
# Params: s - the string to escape
# Returns: new string, with offending characters escaped
#
sub man_escape
{
    my $s = shift;
    $s =~ s/\\/\\\\/g;
    return $s;
}

sub man_normalize_ws
{
    my $s = shift;
    $s =~ tr/[ \t\n]/ /s;
    $s =~ s/(^ )|( $)//;
    return $s;
}





package manxml;
import RoffWriter qw(man_escape man_normalize_ws);

use IO::File;
use vars qw($templates);

##################################################
#
# Man page management
#
##################################################

# Remove leading and trailing blanks.
sub strip_string
{
    my $str = shift;

    $str = $1 if ($str =~ m#^\s*(\S.*)#);
    $str = $1 if ($str =~ m#^(.*\S)\s*$#);

    return $str;
}

#
# Generate a good file name for a man page, given a title and section
#
# Params: title - man page title
#         section - 1, 2, 3, 4, etc.
#
sub man_filename
{
    my $title = strip_string(shift);
    my $sect = strip_string(shift);

    # Escape the path separator '/' which is not allowed
    # in Unix file names. (NUL is not allowed either, but
    # it can't occur in XML data anyway.)
    # We deliberate do NOT escape any other characters
    # (e.g. space) because the user who happens to want
    # to use a space in his file names, for whatever reason,
    # would be justifiably annoyed at us if we had hard-coded
    # any unnecessary escaping here.
    #
    # On the other hand, for people who want escaping, they
    # can either fix their XML sources or stylesheets, very easily.

    $title =~ tr/\//_/;
    $sect =~ tr/\//_/;
    return "$title.$sect";
}

sub shell_quote
{
    join(' ', map { my $u = $_;
                    $u =~ s#([\$`"\\\n])#\\$1#g;
                    '"' . $u . '"' } @_);
}

sub man_openfile
{
    my ($self, $filename) = @_;
    my $encoding = $self->{options}->{'encoding'};

    my $openstr = '';

    if($encoding !~ /^utf|ucs/i
        and $self->{options}->{'utf8trans-program'} ne '')
    {
        $openstr .= '| ' .
            shell_quote($self->{options}->{'utf8trans-program'},
                        $self->{options}->{'utf8trans-map'})
            . ' ';
    }

    if($encoding !~ /^utf-?8$/i
        and $self->{options}->{'iconv-program'} ne '')
    {
        $openstr .= '| ' .
            shell_quote($self->{options}->{'iconv-program'},
                        '-f', 'utf-8',
                        '-t', $encoding)
            . ' ';
    }

    if($openstr eq '') {
        if(!$self->{options}->{'to-stdout'}) {
            $openstr = $filename;
            # Trick from Perl FAQ to open file with arbitrary characters
            $openstr =~ s#^(\s)#./$1#;
            $openstr = ">${openstr}\0";
            print "$filename\n"
                if $self->{options}->{'list-files'};
        } else {
            $openstr = '>-';
        }
    } else {
        if(!$self->{options}->{'to-stdout'}) {
            $openstr .= '> ' . shell_quote($filename);
            print "$filename\n"
                if $self->{options}->{'list-files'};
        }
    }

    my $iof = new IO::File($openstr)
        or die "$0: error opening $openstr: $!\n";

    # Set output encoding to UTF-8 on Perl >=5.8.0
    # so it doesn't complain
    binmode($iof, ":utf8") unless $] < 5.008;

    return $iof;
}

$templates->add_rule('manpage<', 'file-unselected', sub {
    my ($self, $elem, $templates) = @_;

    my $filename = man_filename(
                    $elem->attr('title'),
                    $elem->attr('sect'));

    $self->{fh} = man_openfile($self, $filename);
    $self->{rw} = new RoffWriter($self->{fh});

    $self->{'adjust-stack'} = [ 'b' ];

    $self->{rw}->comment($elem->attr('preprocessors'))
        if($elem->attr('preprocessors') ne '');

    # I've dug through the Internet to see if there was any
    # standard way to specify encoding with man pages.
    # The following seems to be a reasonable proposal:
    # <URL:http://mail.nl.linux.org/linux-utf8/2001-04/msg00168.html>
    my $encoding = $self->{options}->{'encoding'};
    $encoding =~ s#//TRANSLIT$##i;
    $self->{rw}->comment("-*- coding: $encoding -*-");

    $self->{rw}->request('TH',
        # Nothing in the man macros say this has to be the same as
        # the $file and $sect.  While it is best to follow convention,
        # some stylesheets may want to uppercase/lowercase the name,
        # so it is best leave this to them.
        man_normalize_ws(man_escape($elem->attr('h1'))),
        man_normalize_ws(man_escape($elem->attr('h2'))),
        man_normalize_ws(man_escape($elem->attr('h3'))),
        man_normalize_ws(man_escape($elem->attr('h4'))),
        man_normalize_ws(man_escape($elem->attr('h5'))));

    $templates->pop_mode();
});

$templates->add_rule('manpage>', 'file-unselected', sub {
    my ($self, $elem, $templates) = @_;

    $self->{rw}->print_ws("\n");        # ensure file ends in eol
    $self->{fh}->close
        or die $! ? "$0: error closing file/pipe: $!\n"
                  : "$0: program in pipeline exited with an error\n";
    $self->{fh} = undef;
    $self->{rw} = undef;

    my $mainfilename = man_filename($elem->attr('title'),
                                    $elem->attr('sect'));

    # Make the files for the alternate names for the man page
    foreach my $refname (@{$elem->ext->{'refnames'}})
    {
        my $filename = man_filename($refname, $elem->attr('sect'));

        if($filename eq $mainfilename || $self->{options}->{'no-links'}
            || $self->{options}->{'to-stdout'})
        {
            # Same as main man page, don't make link.
        }
        else {
            print STDOUT "$filename\n"
                if $self->{options}->{'list-files'};

            if($self->{options}->{'symlinks'}) {
                symlink($mainfilename, $filename);

            } else {
                my $sectnum = $1 if $mainfilename =~ /^.+\.(\d)/;

                my $fh = new IO::File $filename, "w";
                my $rw = new RoffWriter($fh);

                $rw->request('so',
                    man_escape("man${sectnum}/$mainfilename"));

                $fh->close;
            }
        }
    }

    $templates->push_mode('file-unselected');
});

$templates->add_rule('manpageset<', 'file-unselected', sub {});
$templates->add_rule('manpageset>', 'file-unselected', sub {});

$templates->add_rule('text()', 'file-unselected', \&illegal_text_handler);
$templates->add_rule('*<', 'file-unselected', \&illegal_element_handler);



##################################################
#
# A clean solution to the extra-newlines problem
#
##################################################

# Call before almost block-level element; needed to ensure
# hanging indent and line-breaking before and after
# is handled correctly.
sub block_start
{
    my ($self, $elem) = @_;
    my $lastchild = $elem->parent->ext->{lastchild};


    # There's no macro to stop the hang indent without
    # starting a new paragraph (.PP), so it has to be
    # done at the beginning of the next block.
    if($lastchild eq 'hanging') {
        $self->{rw}->request('PP');
    }

    # Conversely, here we are supposed to be hanging,
    # so do not use .PP, which would nullify it
    elsif($elem->within('TPitem')) {
        $self->{rw}->skip_line()
            unless $lastchild eq '';    # Don't put blank before first block
    }

    # Start a new 'paragraph'
    elsif($lastchild eq 'block') {
        # .PP macro messes up the indentation when
        # used inside a table entry.
        if($elem->parent->name eq 'entry') {
            $self->{rw}->skip_line();
        } else {
            $self->{rw}->request('PP');
        }
    }

    # Could also be done with .PP.
    elsif($lastchild eq 'inline') {
        $self->{rw}->skip_line();
    }

    $elem->parent->ext->{lastchild} = 'block';
}

sub mixed_inline_start
{
    my ($self, $node) = @_;
    if($node->parent->ext->{lastchild} eq 'block') {
        $self->{rw}->skip_line();
    }
    elsif($node->parent->ext->{lastchild} eq 'hanging') {
        # Skipping a line after a hanging paragraph
        # doesn't work, because we want to stop
        # the indent
        $self->{rw}->request('PP');
    }

    $node->parent->ext->{lastchild} = 'inline';
}




##################################################
#
# Changing fonts
#
##################################################

$templates->add_rule('b<', \&bold_start_handler);
$templates->add_rule('b>', \&bold_end_handler);
$templates->add_rule('i<', \&italic_start_handler);
$templates->add_rule('i>', \&italic_end_handler);

$templates->add_rule('b<', 'single-line-mode', \&bold_start_handler);
$templates->add_rule('b>', 'single-line-mode', \&bold_end_handler);
$templates->add_rule('i<', 'single-line-mode', \&italic_start_handler);
$templates->add_rule('i>', 'single-line-mode', \&italic_end_handler);

$templates->add_rule('b<', 'table-mode', \&bold_start_handler);
$templates->add_rule('b>', 'table-mode', \&bold_end_handler);
$templates->add_rule('i<', 'table-mode', \&italic_start_handler);
$templates->add_rule('i>', 'table-mode', \&italic_end_handler);

$templates->add_rule('b<', 'verbatim-mode', \&bold_start_handler);
$templates->add_rule('b>', 'verbatim-mode', \&bold_end_handler);
$templates->add_rule('i<', 'verbatim-mode', \&italic_start_handler);
$templates->add_rule('i>', 'verbatim-mode', \&italic_end_handler);

sub bold_start_handler {
    my ($self, $elem, $templates) = @_;
    mixed_inline_start($self, $elem);

    # If the last font is also bold, don't change anything.
    # Basically this is to just get more readable man output.
    $self->{rw}->escape('fB')
        unless $elem->in('b');
}
sub bold_end_handler {
    my ($self, $elem, $templates) = @_;

    if($elem->in('i')) {
        $self->{rw}->escape('fI');
    }
    elsif($elem->in('b')) { }
    else {
        $self->{rw}->escape('fR');
    }
}

sub italic_start_handler {
    my ($self, $elem, $templates) = @_;
    mixed_inline_start($self, $elem);

    # If the last font is also bold, don't change anything.
    # Basically this is to just get more readable man output.
    $self->{rw}->escape('fI')
        unless $elem->in('i');
}
sub italic_end_handler {
    my ($self, $elem, $templates) = @_;

    if($elem->in('b')) {
        $self->{rw}->escape('fB');
    }
    elsif($elem->in('i')) { }
    else {
        $self->{rw}->escape('fR');
    }
}

# Fixed-with fonts
$templates->add_rule('tt<', sub {});
$templates->add_rule('tt<', 'single-line-mode', sub {});
$templates->add_rule('tt<', 'table-mode', sub {});
$templates->add_rule('tt<', 'verbatim-mode', sub {});

# roff distinguishes between the ASCII hyphen-minus
# and a normal hyphen.
# See <URL:http://www.cs.tut.fi/~jkorpela/dashes.html>

sub disambiguate_hyphen_minus
{
    my ($text, $elem) = @_;

    if(!$elem or $elem->within('tt') or $elem->within('verbatim')) {
        $text =~ s/-/\\-/g;
    }

    return $text;
}

##################################################
#
# NAME section
#
##################################################

# The man page database parser needs the names and
# description all on one line, so enter a special mode to do that.
$templates->add_rule('refnameline<', sub {
    my ($self, $elem, $templates) = @_;
    $elem->parent->ext->{lastchild} = 'block';
    $templates->push_mode('refnameline-mode');
});

$templates->add_rule('refnameline>', sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();
    $self->{rw}->print_ws("\n");
    $elem->ext->{'refnames'} = 0;
});

$templates->add_rule('text()', 'refnameline-mode',
    \&single_line_text_handler);

$templates->add_rule('refname<', 'refnameline-mode', sub {
    my ($self, $elem, $templates) = @_;
    $templates->push_mode('refname');
    $self->{output_save} = '';
});
$templates->add_rule('refname>', 'refnameline-mode', sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();

    if($elem->parent->ext->{'refnames'}++) {
        $self->{rw}->print_ws(', ');
    }

    $self->{rw}->print_ws(man_escape($self->{output_save}));

    # Store the refname; needed later to make the links.
    push(@{$elem->parent->parent->ext->{refnames}},
            $self->{output_save});
    $self->{output_save} = undef;
});
$templates->add_rule('text()', 'refname',
    \&save_text_handler);

$templates->add_rule('refpurpose<', 'refnameline-mode', sub {
    my ($self, $elem, $templates) = @_;
    $self->{rw}->print_ws(' \- ');
});






##################################################
#
# Section headings
#
##################################################

$templates->add_rule("SH<", sub {
    my ($self, $elem, $templates) = @_;
    $elem->parent->ext->{lastchild} = '';
    $templates->push_mode('section-mode');
    $self->{output_save} = '';
});

$templates->add_rule("SH>", sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();
    $self->{rw}->request('SH',
        man_normalize_ws(man_escape($self->{output_save})));
    $self->{output_save} = undef;
});

$templates->add_rule("SS<", sub {
    my ($self, $elem, $templates) = @_;
    $elem->parent->ext->{lastchild} = '';
    $templates->push_mode('section-mode');
    $self->{output_save} = '';
});

$templates->add_rule("SS>", sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();
    $self->{rw}->request('SS',
        man_normalize_ws(man_escape($self->{output_save})));
    $self->{output_save} = undef;
});

$templates->add_rule('text()', 'section-mode',
    \&save_text_handler);

$templates->add_rule('b<', 'section-mode', sub {});
$templates->add_rule('i<', 'section-mode', sub {});
$templates->add_rule('tt<', 'section-mode', sub {});
$templates->add_rule('*<', 'section-mode', \&illegal_element_handler);



##################################################
#
# Paragraph
#
##################################################

$templates->add_rule('para<', sub {
    my ($self, $elem, $templates) = @_;
    block_start($self, $elem);

    my $adstack = $self->{'adjust-stack'};
    if($elem->attr('align') ne '') {
        $self->{rw}->request('ad', $elem->attr('align'));
        push(@$adstack, $elem->attr('align'));
    }
});

$templates->add_rule('para>', sub {
    my ($self, $elem, $templates) = @_;
    my $adstack = $self->{'adjust-stack'};
    if($elem->attr('align') ne '') {
        pop(@$adstack);
        $self->{rw}->request('ad', $adstack->[$#$adstack]);
    }
});


##################################################
#
# Indented paragraphs of various sorts
#
##################################################

$templates->add_rule('TP<', sub {
    my ($self, $elem, $templates) = @_;
    $elem->parent->ext->{lastchild} = 'hanging';
});

$templates->add_rule('TPtag<', sub {
    my ($self, $elem, $templates) = @_;
    $self->{rw}->request('TP',
        $elem->parent->attr('indent'));
    $templates->push_mode('single-line-mode');
});
$templates->add_rule('TPtag>', sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();
    $self->{rw}->print_ws("\n");
});

$templates->add_rule('TPitem<', sub {});




##################################################
#
# Verbatim sections (disable filling and adjusting)
#
##################################################

$templates->add_rule('verbatim<', sub {
    my ($self, $elem, $templates) = @_;
    block_start($self, $elem);
    $templates->push_mode('verbatim-mode');
    $self->{rw}->request('nf');
});
$templates->add_rule('verbatim>', sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();
    $self->{rw}->request('fi');
});




##################################################
#
# Plain old indent
#
##################################################

$templates->add_rule('indent<', sub {
    my ($self, $elem, $templates) = @_;

    $elem->parent->ext->{lastchild} = 'block';
    #block_start($self, $elem);
    #
    my $indent = $elem->attr('indent');
    $self->{rw}->request('RS', $indent);
});
$templates->add_rule('indent>', sub {
    my ($self, $elem, $templates) = @_;
    $self->{rw}->request('RE');
});

$templates->add_rule('leftindent<', sub {
    my ($self, $elem, $templates) = @_;

    # This is from Bernd Westphal's old patch
    # to docbook2man-sgmlspl.

    $self->{rw}->escape('kx');          # Save current horiz. position to x
    $self->{rw}->request('if',          # if too far left,
        qw{ (\nxu > (\n(.lu / 2)) .nr x (\n(.lu / 5) });
                                        #    set x to 0.2 * line-length
    $self->{rw}->request("'in",         #  modify indent, suppress newline
        qw{ \n(.iu+\nxu });
});

$templates->add_rule('leftindent>', sub {
    my ($self, $elem, $templates) = @_;

    $self->{rw}->request("'in",         # restore indent
        qw{ \n(.iu-\nxu });
});


##################################################
#
# Vertical spacing
#
##################################################

$templates->add_rule('sp<', sub {
    my ($self, $elem, $templates) = @_;
    $self->{rw}->request('sp',
        $elem->attr('length'));
});
$templates->add_rule('br<', sub {
    my ($self, $elem, $templates) = @_;
    $self->{rw}->request('br');
});

##################################################
#
# Tables
#
##################################################

=head1 Tables

db2x_manxml implements CALS tables directly,
or rather, a XML description of tables that is really
similar to how DocBook CALS tables work.

The differences are:

=over 4
=item *
Some presentational attributes may not work,
because they are not supported by tbl.

=item *
Character-based alignment does not work.

=item *
tfoot goes after the tbody, not before.
This deviation mainly comes from the fact
that db2x_manxml is stream-based, so content that
goes at the end of the output has to appear later on
the input.  (It could be worked around by buffering
the tfoot data, but since tbl does not actually support
table footers it is not worth the effort.)

=item *
A table cell entry is either an entry or entry element.
The latter uses tbl's T{ ... T} facility to include
entire blocks of text, whereas the former contains only inline
entries.  Theoretically all table cell entries can be considered
as blocks, but for short inline entries using the entry element
makes the output look nicer.

Actually SGML DocBook (but not XML DocBook) makes this
inline/block distinction also in the DTD content model
for entry.  For a reference, see "pernicious content"
in the TDG.

=item *
entrytbl is not supported, obviously.

=back

I am happy to report though that
both horizontal and vertical spans, and sparse cells[*]
are supported.

[*] That is, specifying that an individual table cell should
go into a specific column (previously specified by colspec).

=cut

$templates->add_rule('table<', sub {
    my ($self, $elem, $templates) = @_;
    $self->{rw}->request('TS');

    my $global_options;
    if($elem->attr('frame') eq ''
        or $elem->attr('frame') eq 'none')
    {
    } elsif($elem->attr('frame') eq 'all') {
        $global_options .= 'allbox ';
    } else {
        $templates->warn_location($elem, "only a frame of 'all' or 'none' is supported");
    }

    if($elem->attr('pgwide')) {
        $global_options .= 'expand';
    }

    $self->{rw}->print_ws($global_options . ";\n")
        if defined $global_options;

    $templates->push_mode('table-mode');
    # Global options line
});
$templates->add_rule('table>', sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode();
    $self->{rw}->request('TE');
});

=head2 Algorithm for rendering tables

For simplicity in this description I will concentrate on rendering a
tbody.  Rendering thead and tfoot is similar.  I will also assume that
you know how tbl works and its syntax.

First, when colspec or spanspec is encountered, the data on the column
number, alignment, etc. is stored in the tgroup's node ext structure.
colspecs and spanspecs that occur in thead and tfoot override the ones
in tgroup, and tbody uses the colspecs and spanspecs that physically
occur under tgroup, unchanged, as per the CALS specification.

The most critical information is the column number.  The running column
number is always kept, and is used whenever a colspec does not specify a
column number explicitly.  (Similarly for entry and entrytbl.)

tbl requires that the formatting information for I<all> the table rows
to come before any of the actual data.  However, this information must
be obtained as each row and cell is processed.  (This information mainly
consists of the length of horizontal or vertical spans, if any, as well
as text alignment, which may be customized differently for each cell.)

So all the rows have to be buffered as they are processed, and then, at
the end of the table, the formatting information is output, then all the
buffered rows.  A special buffering mode in RoffWriter is used to do
this.

There is no problem with this buffering when processing thead or tfoot,
since there are usually only one or two lines in those sections.  But
the tbody may have thousands of lines.  Even if we don't exactly run out
of memory, the output would not be very nice because we would have a
thousand format-lines (the formatting information for one particular
row) that are nearly identical.

Therefore we add a refinement to our procedure: we watch the
format-lines, and if they are the same for the last few rows we flush
our buffer.  If we have a different format-line coming later in our
input, we use the table continuation request (C<.T&>) to change it (in
blocks). So our output looks like this:

 l l l l             # Format lines
 l l l s
 l l l l.            # All the lines from the third line on have this format
 Data Data Data Data
 Data Data Data
 Data Data Data Data
 Data Data Data Data
 Data Data Data Data
 Data Data Data Data
 Data Data Data Data
 Data Data Data Data
 .T&
 l l l s
 l l l l.
 ...
 Data
 ...
 .T&
 l l l r
 l l l l.
 ...
 Data
 ...

Of course, theoretically we could use C<.T&> for each any every row, but
this bloats the output.

As I have mentioned, the format-lines contain span information.
Actually cell spans can be handled fairly easily with an
iterative/imperative algorithm.  (As opposed to a functional/recursive
one, which makes it a pain to implement in XSLT, and so we do it here
instead.)

First, there is a default format-line, which is constructed with the
help of the colspec information.  The format-line is represented as a
flat array with each element corresponding to one column, in order from
left-to-right (assume LTR natural languages here, since roff does not
support other writing directions).  The list elements are simply strings
like "l", "r", "c", etc.  i.e. the same sort of column specifiers as
used in tbl.  To make a format-line in the output it is only needed to
C<join> this list.

When a table entry comes along and specifies some horizontal span, the
default format-line is taken as the initial template for the current
line's format-line, and the columns in this format-line array that are
affected by the span are assigned a different specifier (something like
"s").  The newly-changed format-line is the one that is used for the
current line.

Of course, when there is a horizontal span, care must be taken that
we skip over the correct number of 'physical' cells.

For vertical spans, the format-line handling is the same, except with a
different tbl column specifier ("^").  Vertical spanning does require
that subsequent rows know about the vertically spanning cells and
reserve space for them:  this is easily handled with another flat array
in a similar fashion to the format-line array.  Each element of the
current_vspans array would hold a non-negative integer indicating how
many more rows to reserve for each column.  (Zero means there is no
vertical span for that column, naturally.) Once each row is finished,
each number of this list is decremented by one, unless it is zero, in
which it stays at zero.  Whenever a table cell requests vertical span, the
appropriate column of the current_vspans array is re-assigned to.

The current_vspans array is also consulted when advancing columns
(as one row's table cells are processed) so that the current column
is not advanced in the middle of some vertical span from a previous row.

=cut

$templates->add_rule('tgroup<', 'table-mode', sub {
    my ($self, $elem, $templates) = @_;

    $elem->ext->{total_cols} = $elem->attr('cols');

    if($elem->attr('cols') !~ /^\d+$/
        or $elem->attr('cols') < 1)
    {
        $templates->warn_location($elem, "fatal error: invalid number of columns for table");
    }

    $elem->ext->{column_data} = [];

    $elem->ext->{colnames} = {};
    $elem->ext->{spannames} = {};

    $elem->ext->{colspec_current_colnum} = 0;

    $elem->ext->{align} = $elem->attr('align') || 'left';
    $elem->ext->{rowsep} =
        ( (defined $elem->attr('rowsep'))?
            $elem->attr('rowsep') :
            $elem->parent->attr('rowsep') );
    $elem->ext->{colsep} =
        ( (defined $elem->attr('colsep'))?
            $elem->attr('colsep') :
            $elem->parent->attr('colsep') );
});

$templates->add_rule('tgroup>', 'table-mode', sub {});

$templates->add_rule('colspec<', 'table-mode', sub {
    my ($self, $elem, $templates) = @_;
    my $t_elem = $elem->parent;
    my $tgroup = $t_elem->parent;

    if($t_elem->name ne 'tgroup') {
        $t_elem->ext->{total_cols} = $tgroup->ext->{total_cols};
        $t_elem->ext->{column_data} = [];
        $t_elem->ext->{colnames} = [];
        $t_elem->ext->{spannames} = {};
    }

    my $colnum;
    if($elem->attr('colnum')) {
        $colnum = $elem->attr('colnum');
        if($colnum !~ /^\d+$/ or $colnum < 1) {
            $templates->warn_location($elem, "invalid column number --- ignoring\n");
            $colnum = $t_elem->ext->{colspec_current_colnum} + 1;
        } elsif($colnum <= $t_elem->ext->{colspec_current_colnum}) {
            $templates->warn_location($elem, "column numbers of colspecs are not given in an increasing sequence\n");
        }
    } else {
        $colnum = $t_elem->ext->{colspec_current_colnum} + 1;
    }

    if($colnum > $t_elem->ext->{total_cols}) {
        $templates->warn_location($elem, "column number exceeds total number of columns --- ignoring this colspec\n");
        return;
    }

    $t_elem->ext->{colspec_current_colnum} = $colnum;

    my $column_data_entry =
        { colwidth => $elem->attr('colwidth'),
          colname => $elem->attr('colname'),
          colnum => $colnum,

          rowsep => $elem->attr('rowsep'),
          colsep => $elem->attr('colsep'),
          align => $elem->attr('align') || $tgroup->ext->{align}
        };

    $t_elem->ext->{column_data}->[$colnum-1] = $column_data_entry;

    $t_elem->ext->{colnames}->{$elem->attr('colname')}
        = $column_data_entry
            if $elem->attr('colname') ne '';
});

$templates->add_rule('spanspec<', 'table-mode', sub {
    my ($self, $elem, $templates) = @_;
    my $t_elem = $elem->parent;

    $t_elem->ext->{spannames}->{$elem->attr('spanname')}
        = {
            start => $t_elem->ext->{colnames}->{$elem->attr('namest')},
            end => $t_elem->ext->{colnames}->{$elem->attr('nameend')},

            rowsep => $elem->attr('rowsep'),
            colsep => $elem->attr('colsep'),
            align => $elem->attr('align') ||
                $t_elem->ext->{colnames}->{$elem->attr('namest')}->{align}
          };
});

sub tbl_build_format_line_helper
{
    my ($self, $elem) = @_;
    my $tgroup = $elem->parent;

    my @format_line = ();

    for(my $i = 0; $i < $elem->ext->{total_cols}; $i++)
    {
        my $cd = $elem->ext->{column_data}->[$i];

        my $f = tbl_align_to_tbl(
                (defined $cd ? $cd->{align} : $tgroup->ext->{align}));
        $f .= tbl_colwidth_to_tbl($cd->{colwidth})
            if (defined $cd and defined $cd->{colwidth});
        $f .= ' |'
            if ((defined $cd and $cd->{colsep}) or
                    $tgroup->ext->{colsep})
                and ($i != $elem->ext->{total_cols}-1);

        push(@format_line, $f);
    }

    return \@format_line;
}

sub tbl_colwidth_to_tbl
{
    my ($s) = @_;
    # FIXME!
    return '';
}

sub tbl_build_format_line
{
    my ($self, $elem) = @_;

    if($elem->name eq 'tbody'
        or !defined $elem->ext->{column_data})
    {
        foreach my $x (qw(total_cols column_data colnames spannames)) {
            $elem->ext->{$x} = $elem->parent->ext->{$x};
        }
    }

    $elem->ext->{default_format_line} =
        tbl_build_format_line_helper($self, $elem);

    my @vspans = (0) x $elem->ext->{total_cols};
    $elem->ext->{current_vspans} = \@vspans;

    my @vspan_template_format = ('^') x $elem->ext->{total_cols};
    $elem->ext->{vspan_template_format} = \@vspan_template_format;

    my @current_rowseps = ('^') x $elem->ext->{total_cols};
    $elem->ext->{current_rowseps} = \@current_rowseps;
}


sub t_elem_start_handler { my ($self, $elem, $templates) = @_;
                           tbl_format_line_buffer_start($elem, $self->{rw}); }
sub t_elem_end_handler   { my ($self, $elem, $templates) = @_;
                           tbl_format_line_buffer_flush($elem, $self->{rw}); }

$templates->add_rule('thead<', 'table-mode', \&t_elem_start_handler);
$templates->add_rule('thead>', 'table-mode', \&t_elem_end_handler);
$templates->add_rule('tfoot<', 'table-mode', \&t_elem_start_handler);
$templates->add_rule('tfoot>', 'table-mode', \&t_elem_end_handler);
$templates->add_rule('tbody<', 'table-mode', \&t_elem_start_handler);
$templates->add_rule('tbody>', 'table-mode', \&t_elem_end_handler);

sub tbl_advance_column
{
    my ($row, $rw, $new_colnum, $relative_advance) = @_;

    my $old_colnum = $row->ext->{current_colnum};
    my $total_cols = $row->parent->ext->{total_cols};

    if($relative_advance) {
        my $vspans = $row->parent->ext->{current_vspans};
        for($new_colnum = $old_colnum + $relative_advance;
            $new_colnum <= $total_cols && ($vspans->[$new_colnum - 1] > 0);
            $new_colnum++)
        {}
    }
    elsif($new_colnum == -1) {
        $new_colnum = $total_cols + 1;
    }

    $row->ext->{current_colnum} = $new_colnum;

    $new_colnum = $total_cols if $new_colnum > $total_cols;
    $old_colnum = 1           if $old_colnum == 0;

    $rw->print("\t" x ($new_colnum - $old_colnum));
}


$templates->add_rule('row<', 'table-mode', sub {
    my ($self, $elem, $templates) = @_;
    my $t_elem = $elem->parent;
    my $tgroup = $t_elem->parent;

    if(! $t_elem->ext->{num_rows}++) {
        tbl_build_format_line($self, $t_elem);
    }

    if(defined $tgroup->ext->{last_rowseps}) {
        # tbl has this funny (seemingly undocumented)
        # behavior where if a format line is all underscores
        # then the corresponding (blank) data line _must_ be omitted.
        $self->{rw}->print("\n")
            if grep(/[^_]/, @{$tgroup->ext->{last_rowseps}});

        if($t_elem->name ne 'tbody') {
            tbl_format_line_buffer_push($t_elem,
                $tgroup->ext->{last_rowseps});
        } else {
            tbl_format_line_buffer_push_ex($t_elem, $self->{rw},
                $tgroup->ext->{last_rowseps});
        }

        $tgroup->ext->{last_rowseps} = undef;
    }

    $elem->ext->{current_colnum} = 0;
    tbl_advance_column($elem, $self->{rw}, 0, 1);
    $elem->ext->{this_format_line} =
        list_copy($t_elem->ext->{default_format_line});

    tbl_copy_vspan_template_format(
        $elem->ext->{this_format_line},
        $t_elem->ext->{current_vspans},
        $t_elem->ext->{vspan_template_format});

    for(my $i = 0; $i < @{$t_elem->ext->{current_vspans}}; $i++)
    {
        if($t_elem->ext->{current_vspans}->[$i] > 0) {
        } elsif(defined $elem->attr('rowsep')) {
            $t_elem->ext->{current_rowseps}->[$i] =
                ( $elem->attr('rowsep') ? '_' : '^' );
        } elsif(defined $t_elem->ext->{column_data}->[$i]) {
            $t_elem->ext->{current_rowseps}->[$i] =
                ( $t_elem->ext->{column_data}->[$i]->{rowsep} ? '_' : '^' );
        } else {
            $t_elem->ext->{current_rowseps}->[$i] =
                ( $tgroup->ext->{rowsep} ? '_' : '^' );
        }
    }
});

sub tbl_copy_vspan_template_format
{
    my ($format_line, $vspans, $vspan_template_format) = @_;
    for(my $i = 0; $i < @$vspans; $i++)
    {
        $format_line->[$i] = $vspan_template_format->[$i]
            if $vspans->[$i] > 0;
    }

    return $format_line;
}

$templates->add_rule('row>', 'table-mode', sub {
    my ($self, $elem, $templates) = @_;
    my $t_elem = $elem->parent;
    my $tgroup = $t_elem->parent;

    tbl_advance_column($elem, $self->{rw}, -1);

    $self->{rw}->print_ws("\n");

    if($t_elem->name ne 'tbody') {
        tbl_format_line_buffer_push($t_elem, $elem->ext->{this_format_line});
    } else {
        tbl_format_line_buffer_push_ex($t_elem, $self->{rw},
            $elem->ext->{this_format_line});
    }

    foreach my $c (@{$t_elem->ext->{current_vspans}}) {
        $c-- if $c > 0;
    }

    my @current_rowseps;
    for(my $i = 0; $i < $t_elem->ext->{total_cols}; $i++) {
        $current_rowseps[$i] =
            ($t_elem->ext->{current_vspans}->[$i] > 0 ?
                '^' :
                $t_elem->ext->{current_rowseps}->[$i]);
    }

    # We don't output the borders yet, because they are supposed
    # to be separators and we do not want one on the last row.
    # So output them on the next row.
    $tgroup->ext->{last_rowseps} = \@current_rowseps
        if grep { $_ eq '_' } @current_rowseps;
});

# Compare two lists (given as references),
# checking they have the same number of elements
# and each corresponding element is string-wise equal.
#
sub list_eq
{
    my ($a, $b) = @_;
    return 0 if (!defined $a or !defined $b or @$a != @$b);

    for(my $i = 0; $i < @$a; $i++) {
        return 0 if $a->[$i] ne $b->[$i];
    }

    return 1;
}

# Shallow-copy a list, given a reference,
# into a new list, with a new reference.
#
sub list_copy
{
    if(wantarray) {
        return map { my @x = @$_; \@x } @_;
    } else {
        my @x = @{$_[0]};
        return \@x;
    }
}

sub tbl_format_line_buffer_start
{
    my ($t_elem, $rw) = @_;
    $t_elem->ext->{format_lines} = [];
    $t_elem->ext->{same_format_lines} = 0;
    $t_elem->ext->{current_format_line} = undef;

    $rw->buffer_on();
}

sub tbl_format_line_buffer_push
{
    my ($t_elem, $format_line) = @_;
    my $format_lines = $t_elem->ext->{format_lines};

    if(scalar(@$format_lines) != 0 and
        list_eq(
            $format_lines->[$#$format_lines],
            $format_line))
    {
        $t_elem->ext->{same_format_lines}++;
    } else {
        $t_elem->ext->{same_format_lines} = 0;
    }

    push(@$format_lines, $format_line);

}

sub tbl_format_line_buffer_flush
{
    my ($t_elem, $rw, $n) = @_;
    my $format_lines = $t_elem->ext->{format_lines};
    my $buffered_rows = $rw->buffer_off();

    $t_elem->ext->{same_format_lines} = 0;
    $t_elem->ext->{current_format_line} = undef;

    return if @$format_lines == 0;

    $rw->request('T&')
        if $t_elem->parent->parent->ext->{'T&'}++;

    $rw->print(
        join("\n",
            map { join(' ', @$_) } @$format_lines[0 .. ($#$format_lines - $n)])
        . ".\n");

    $rw->write($buffered_rows);

    $t_elem->ext->{format_lines} = [];
}

sub tbl_format_line_buffer_push_ex
{
    my ($t_elem, $rw, $format_line) = @_;

    if(defined $t_elem->ext->{current_format_line})
    {
        if(list_eq($t_elem->ext->{current_format_line}, $format_line)) {
            $rw->write($rw->buffer_off());
            $rw->buffer_on();
        }
        else {
            $t_elem->ext->{format_lines} = [];
            $t_elem->ext->{same_format_lines} = 0;
            $t_elem->ext->{current_format_line} = undef;

            tbl_format_line_buffer_push($t_elem, $format_line);
        }
    }
    else {
        tbl_format_line_buffer_push($t_elem, $format_line);
        if($t_elem->ext->{same_format_lines} == 3) {
            tbl_format_line_buffer_flush($t_elem, $rw,
                $t_elem->ext->{same_format_lines});
            $t_elem->ext->{current_format_line} = $format_line;

            $rw->buffer_on();
        }
    }
}

sub tbl_align_to_tbl {
    my ($align) = @_;
    if   ($align eq 'left')  { return 'l'; }
    elsif($align eq 'right') { return 'r'; }
    elsif($align eq 'center') { return 'c'; }
    else { return 'l' }
}


sub tbl_entry_start_handler {
    my ($self, $elem, $templates) = @_;
    my $row = $elem->parent;
    my $t_elem = $elem->parent->parent;
    my $tgroup = $t_elem->parent;

    my $start_col; my $end_col;
    my $align; my $colsep;

    if($elem->attr('colname')) {
        $start_col = $t_elem->{colnames}->{$elem->attr('colname')};
    } elsif($elem->attr('spanname')) {
        my $span = $t_elem->{spannames}->{$elem->attr('spanname')};
        $start_col = $span->{start};
        $end_col = $span->{end};
        $align = $span->{align};
        $colsep = $span->{colsep};
    } elsif($elem->attr('namest')) {
        $start_col = $t_elem->{colnames}->{$elem->attr('namest')};
        $end_col = $t_elem->{colnames}->{$elem->attr('nameend')};
    }

    $align = $elem->attr('align')
        if defined $elem->attr('align');
    $colsep = $elem->attr('colsep')
        if defined $elem->attr('colsep');


    if(defined $start_col) {
        tbl_advance_column($row, $self->{rw}, $start_col->{colnum});
    }

    if(defined $elem->attr('rowsep')) {
        $t_elem->ext->{current_rowseps}->[$row->ext->{current_colnum} - 1] =
            ( $elem->attr('rowsep') ? '_' : '^' );
    }

    if(defined $align) {
        my $t = tbl_align_to_tbl($align);
        $row->ext->{this_format_line}->[$row->ext->{current_colnum} - 1]
            =~ s/^[lrc]/$t/;
    }
    if(defined $colsep) {
        my $t = ($colsep and
                    ($row->ext->{current_colnum} !=
                        $tgroup->ext->{total_cols})) ? ' |' : '';
        $row->ext->{this_format_line}->[$row->ext->{current_colnum} - 1]
            =~ s/( \|)?$/$t/;
    }

    my $relative_advance = 1;
    if(defined $end_col) {
        for(my $i = $start_col->{colnum}; $i < $end_col->{colnum}; $i++) {
            $row->ext->{this_format_line}->[$i] = 's';
        }
        $relative_advance = $end_col->{colnum} - $start_col->{colnum} + 1;
    }
    $elem->ext->{relative_advance} = $relative_advance;

    if($elem->attr('morerows')) {
        if($elem->attr('morerows') !~ /^\d+$/) {
            $templates->warn_location($elem, "invalid morerows value --- ignoring\n");
        } else {

            for(my $i = 0; $i < $relative_advance; $i++) {
                my $c = $row->ext->{current_colnum} + $i;
                $t_elem->ext->{current_vspans}->[$c - 1]
                        = $elem->attr('morerows') + 1;
                $t_elem->ext->{vspan_template_format}->[$c - 1]
                        = '^';
                $t_elem->ext->{vspan_template_format}->[$c - 1]
                        .= ' |'
                    if $row->ext->{this_format_line}->[$row->ext->{current_colnum} - 1] =~ /\|$/;
            }
        }
    }

    if($elem->name eq 'entry') {
        $self->{rw}->print("T{\n");
        $templates->push_mode('');
    } else {
        $self->{rw}->print("\\&");
        $templates->push_mode('single-line-mode');
    }
}

sub tbl_entry_end_handler
{
    my ($self, $elem, $templates) = @_;
    my $row = $elem->parent;

    $templates->pop_mode();
    if($elem->name eq 'entry') {
        $self->{rw}->print_ws("\nT}");
    }

    tbl_advance_column($row, $self->{rw}, 0,
        $elem->ext->{relative_advance});
}

$templates->add_rule('simentry<', 'table-mode', \&tbl_entry_start_handler);
$templates->add_rule('simentry>', 'table-mode', \&tbl_entry_end_handler);
$templates->add_rule('entry<', 'table-mode', \&tbl_entry_start_handler);
$templates->add_rule('entry>', 'table-mode', \&tbl_entry_end_handler);

$templates->add_rule('*<', 'table-mode', \&illegal_element_handler);
$templates->add_rule('text()', 'table-mode', \&illegal_text_handler);


##################################################
#
# Character data
#
##################################################

sub save_text_handler
{
    my ($self, $node, $templates) = @_;
    $self->{output_save} .= $node->{Data};
}

sub single_line_text_handler {
    my ($self, $node, $templates) = @_;
    my $s = $node->{Data};

    $s =~ tr/\n/ /;
    $s = disambiguate_hyphen_minus(man_escape($s), $node);

    mixed_inline_start($self, $node);

    $self->{rw}->print_ws($s);
}

sub illegal_text_handler {
    my ($self, $node, $templates) = @_;

    if($node->{Data} =~ /[^\s]/) {
        $templates->warn_location($node, "character data is not allowed here");
    }
}

$templates->add_rule('*<', 'single-line-mode', \&illegal_element_handler);
$templates->add_rule('text()', 'single-line-mode',
    \&single_line_text_handler);

$templates->add_rule('*<', 'verbatim-mode', \&illegal_element_handler);
$templates->add_rule('text()', 'verbatim-mode', sub {
    my ($self, $node, $templates) = @_;
    $self->{rw}->print(
        disambiguate_hyphen_minus(man_escape($node->{Data})));
});

$templates->add_rule('text()', sub {
    my ($self, $node, $templates) = @_;
    mixed_inline_start($self, $node);
    $self->{rw}->print_ws(
        disambiguate_hyphen_minus(man_escape($node->{Data}), $node));
});




##################################################
#
# Comments
#
##################################################

$templates->add_rule('comment<', sub {
    my ($self, $elem, $templates) = @_;
    $templates->push_mode('comment-mode');
});

$templates->add_rule('comment>', sub {
    my ($self, $elem, $templates) = @_;
    $templates->pop_mode('comment-mode');
    $self->{rw}->comment($elem->ext->{content});
});

$templates->add_rule('*<', 'comment-mode', \&illegal_element_handler);
$templates->add_rule('text()', 'comment-mode', sub {
    my ($self, $node, $templates) = @_;
    $node->parent->ext->{content} .= $node->{Data};
});




##################################################
#
# Processing instructions
#
##################################################

$templates->add_rule('processing-instruction()', sub {
    my ($self, $node, $templates) = @_;

    if($node->{Target} eq 'man') {
        my $data = $node->{Data};
        $data =~ s/\&#xA;/\n/g;
        $data =~ s/\&#10;/\n/g;
        $self->{rw}->print_ws($data);
    }
});

$templates->add_rule('*<', \&illegal_element_handler);
sub illegal_element_handler {
    my ($self, $node, $templates) = @_;
    $templates->warn_location($node, "element not allowed here\n");
};





##################################################
#
# Main
#
##################################################

package main;

use XML::SAX::ParserFactory;

unshift(@ARGV, '-') unless @ARGV;
my $parser = XML::SAX::ParserFactory->parser(DocumentHandler => $manxml::templates);

foreach my $file (@ARGV)
{
    if($file eq '-') {
        $parser->parse_file(\*STDIN);
    } else {
        $parser->parse_uri($file);
    }
}

