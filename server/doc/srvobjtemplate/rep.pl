# ===========================================================
# Copyright (C) 2012 PYR Software Ltd. All rights reserved.
# ===========================================================

use strict;
use warnings;
use File::DosGlob 'glob';
#
# Required by perl2exe
#
my @flist = glob "*.*";
############################

my $what = shift || die;
my $to = shift || die;
my @filelist = glob("*.*");
my $curfile;
my $tmp1;
my $tmp2;
my $tmp3=0;

foreach (@filelist)
{
  $curfile=$_;
  next if (-d $curfile);
  if ($curfile eq "$0")    #skiping ourself
  {
    print "skipped $_\n";
    next;
  } 


#  while (-e "$curfile.$tmp1") 
#  { 
#    $tmp1=int(rand*1000000); 
#  } 
#  print "tempname: $curfile.$tmp1\n";

  $tmp1=$curfile;
  if ( $tmp1 =~ /$what/ )
  {
    $tmp1 =~ s/$what/$to/g;
  }
  else
  {
    while (-e "$tmp3") 
    { 
      $tmp3=int(rand*1000000); 
    }
    $tmp1=$tmp3; 
  }

print "$curfile - $tmp1\n";
  open (INFILE, "$curfile") || die;
  open (OUTFILE, ">$tmp1") || die;

  while (<INFILE>)
  {
    $tmp2 = $_;
    if ($tmp2 =~ /$what/)
    { 
#      print "was:  $tmp2";
      $tmp2 =~ s/$what/$to/g;
#      print "changed: $tmp2";
    }
    print OUTFILE "$tmp2";
  }

  close INFILE;
  close OUTFILE;
  unlink "$curfile" || die; 
  if ( $curfile =~ /$what/ )
  {
  }
  else
  {
    rename "$tmp1","$curfile";
  }
  

}
