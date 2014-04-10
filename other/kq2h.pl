#!/usr/bin/perl -w

die "Need a file name!" if ($#ARGV != 0);

$ft = 0;
@vh = ("item","spell","nf","tnum", "mapno");

open (IFILE,$ARGV[0]) or die "Could not open input file.";

sub quote {
    $_[0] =~ s/^\s+//;
    $_[0] =~ s/\s+$//;
    $_[0]="\"".$_[0]."\"";
}

while (<IFILE>)
{
  chomp;
  @ff = split(",");
  if ($ft == 0)
  {
    for $a (0 .. $#vh)
    {
      $ft = $a+1 if ($ff[0] eq $vh[$a]);
    }
    if ($ft == 0)
    {
      print STDERR ("Unrecognized header: ",$ff[0],"\n");
      exit;
    }
    else
    {
      next;
    }
  }
  elsif ($ft == 1)
  {
    next if ($#ff < 51);
    $ff[0] = "   {\"".$ff[0]."\"";
    $ff[3] = "\"".$ff[3]."\"";
    $ff[15] = "{".$ff[15];
    $ff[22] = $ff[22]."}";
    $ff[23] = "\n   {".$ff[23];
    $ff[35] = $ff[35]."}";
    $ff[36] = "{".$ff[36];
    $ff[51] = $ff[51]."}},\n";
  }
  elsif ($ft == 2)
  {
    next if ($#ff < 20);
    $ff[0] = "  {\"".$ff[0]."\"";
    $ff[2] = "\"".$ff[2]."\"";
    $ff[13] = "{".$ff[13];
    $ff[20] = $ff[20]."}},\n";
  }
  elsif ($ft == 3)
  {
    next if ($#ff < 7);
    $ff[0] = "  {".$ff[0];
    $ff[7] = "\"".$ff[7]."\"},\n";
  }
  elsif ($ft==4)
  {
    next if ($#ff < 7);
    $ff[0] = "  {".$ff[0];
    $ff[3] = "{".$ff[3];
    $ff[7] = $ff[7]."}},\n";
  }
  elsif ($ft==5)
  {
      next if ($#ff<6);
      $ff[0]=" {".$ff[0];
      quote($ff[5]);
      quote($ff[6]);
      $ff[6]=$ff[6]."},\n";
  }
  print (join(",",@ff));
  $c++;
}
print (STDERR $c," lines.","\n");


