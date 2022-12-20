#!/usr/bin/perl -w

#    Extracts messages from simple Lua scripts
#    Copyright (C) 2004, 2005 Yann Dirson (the original wmlxgettext version for wesnoth)
#    Copyright (C) 2007 Günther Brammer (the luaxgettext version for KQ)

#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

use strict;
use File::Basename;
use POSIX qw(strftime);
use Getopt::Long;

sub raw2postring {
  my $str = shift;

  $str =~ s/^(.*)$/"$1\\n"/mg;
  $str =~ s/\n$/\n"\\n"/mg;
  $str =~ s/\\n\"$/\"\n/g;

  return $str;
}

sub po2rawstring {
  my $str = shift;
  my @lines = split (/\n/, $str);

  $str = "";
  foreach my $line (@lines) {
    $line =~ m/"(.*)"/;
    $str .= $1;
  }
  $str =~ s/\\n/\n/gm;

  return $str;
}

our $toplevel = '.';
our $initialdomain = 'kq';
our $domain = undef;
GetOptions ('directory=s' => \$toplevel,
	    'initialdomain=s' => \$initialdomain,
	    'domain=s' => \$domain);

$domain = $initialdomain unless defined $domain;

## extract strings with their refs into %messages

our ($str,$translatable,$line,%messages);
chdir $toplevel;
foreach my $file (@ARGV) {
  next if $file !~ m/\.lua$/;
  open (FILE, "<$file") or die "cannot read from $file";
 LINE: while (<FILE>) {
    # skip -- lines as comments
    next LINE if m/^\s*--/;

    if (!defined $str and m/^(?:[^\"]*?)((?:_\s*)?)\"([^\"]*)\"(.*)/) {
      # single-line quoted string

      push @{$messages{raw2postring($2)}}, "$file:$."
	if ($1 ne ''); # ie. translatable

      # process remaining of the line
      $_ = $3 . "\n";
      redo LINE;

    } elsif (!defined $str and m/^(?:[^\"]*?)((?:_\s*)?)\s*\"([^\"]*)/) {
      # start of multi-line

      $translatable = ($1 ne '');
      $_ = $2;
      if (m/(.*)\r/) { $_ = "$1\n"; }
      $str = $_;
      $line = $.;

    } elsif (m/(.*?)\"(.*)/) {
      # end of multi-line
      die "end of string without a start in $file" if !defined $str;

      $str .= $1;

      push @{$messages{"\"\"\n" . raw2postring($str)}}, "$file:$."
	if $translatable;
      $str = undef;

      # process remaining of the line
      $_ = $2 . "\n";
      redo LINE;

    } elsif (defined $str) {
      # part of multi-line
      if (m/(.*)\r/) { $_ = "$1\n"; }
      $str .= $_;

    }
  }

  close FILE;
}


## index strings by their location in the source so we can sort them

our @revmessages;
foreach my $key (keys %messages) {
  foreach my $line (@{$messages{$key}}) {
    my ($file, $lineno) = split /:/, $line;
    push @revmessages, [ $file, $lineno, $key ];
  }
}

# sort them
@revmessages = sort { $a->[0] cmp $b->[0] or $a->[1] <=> $b->[1] } @revmessages;


## output

my $date = strftime "%F %R%z", localtime();

print <<EOH
msgid ""
msgstr ""
"Project-Id-Version: PACKAGE VERSION\\n"
"Report-Msgid-Bugs-To: http://sourceforge.net/projects/kqlives/\\n"
"POT-Creation-Date: $date\\n"
EOH
;
# we must break this string to avoid triggering a bug in some po-mode
# installations, at save-time for this file
print "\"PO-Revision-Date: YEAR-MO-DA ", "HO:MI+ZONE\\n\"\n";
print <<EOH
"Last-Translator: FULL NAME <EMAIL\@ADDRESS>\\n"
"Language-Team: LANGUAGE <LL\@li.org>\\n"
"MIME-Version: 1.0\\n"
"Content-Type: text/plain; charset=UTF-8\\n"
"Content-Transfer-Encoding: 8bit\\n"

EOH
;

foreach my $occurence (@revmessages) {
  my $key = $occurence->[2];
  if (defined $messages{$key}) {
    print "#:";
    foreach my $line (@{$messages{$key}}) {
      print " $line";
    }
    print "\nmsgid $key",
      "msgstr \"\"\n\n";

    # be sure we don't output the same message twice
    delete $messages{$key};
  }
}
