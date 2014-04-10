#!/usr/bin/perl

@kqin = ("../include/itemdefs.h","../include/progress.h");
@d_s = ();
@d_v = ();
$ni = 0;

foreach $a (@kqin)
{
  open (INFILE,$a) or die ("could not open include file");
  while (<INFILE>)
  {
    if (substr($_,0,7) eq "#define")
    {
      @b = split(" ",$_);
      @d_s[$ni] = $b[1];
      @d_v[$ni] = $b[2];
      $ni++;
    }
  }
}

while (<STDIN>)
{
  for $a (0..$ni-1)
  {
    $_ =~ s/\b$d_s[$a]\b/$d_v[$a]/g;
  }
  print $_;
}
