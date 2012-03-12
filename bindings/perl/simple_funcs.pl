#!/usr/bin/perl -w
# Copyright (C) 2011-2012 D. V. Wiebe
#
##########################################################################
#
# This file is part of the GetData project.
#
# GetData is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation; either version 2.1 of the License, or (at your
# option) any later version.
#
# GetData is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with GetData; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
use strict;
use warnings;

my %rec;
my $i;

sub printcleanup {
  $_ = shift;
  my ($t, $n) = @$_;

  return "\t\tsafefree($n);\n" if (
    $t eq "gdp_complex_in *" or $t eq "const char **" or $t eq "gdp_data_in *"
      or $t eq "gdp_double_in *"
  );
  return "";
}

sub printmunge {
  $_ = shift;
  my ($t, $n) = @$_;

  return "&$n" if ($t =~ /&$/);
  return "(long long)$n" if ($t eq "off64_t" or $t eq "gd_shift_t" or
    $t eq "gdpu_shift_t" or $t eq "int64_t");
  return "(unsigned long long)$n" if ($t eq "uint64_t");
  return "creal($n), cimag($n)" if ($t eq "_Complex double" or
    $t eq "gdpu_complex");
  return "$n.r, (unsigned long long)$n.u, (long long)$n.i"
  if ($t eq "gd_triplet_t");
  return $n;
}

sub printfmt {
  $_ = shift;
  s/^const //g;

  if (/char \*$/) {
    return "\\\"%s\\\"";
  } elsif (/[\*&]$/) {
    return "%p";
  } elsif ($_ eq "_Complex double" or $_ eq "gdpu_complex") {
    return "%g;%g";
  } elsif ($_ eq "double") {
    return "%g";
  } elsif ($_ eq "gd_bit_t" or $_ eq "gdpu_bitnum_t" or $_ eq "gdpu_numbits_t"
      or $_ eq "gd_windop_t")
  {
    return "%i";
  } elsif ($_ eq "gd_entype_t") {
    return "%02x";
  } elsif ($_ eq "gd_shift_t" or $_ eq "gdpu_shift_t") {
    return "%lli";
  } elsif ($_ eq "gd_spf_t" or $_ eq "gdpu_spf_t" or $_ eq "gd_count_t") {
    return "%u";
  } elsif ($_ eq "gd_type_t" or $_ eq "gdpu_type_t") {
    return "%03x";
  } elsif ($_ eq "int" or $_ eq "gdpu_int") {
    return "%i";
  } elsif ($_ eq "off64_t") {
    return "%lli";
  } elsif ($_ eq "size_t") {
    return "%zi";
  } elsif ($_ eq "unsigned int") {
    return "%u";
  } elsif ($_ eq "unsigned long int") {
    return "%lu";
  } elsif ($_ eq "gd_triplet_t") {
    return "{%g,%llX,%lli}";
  } else {
    die $_;
  }
}

while (<>) {
  chomp;
  until (/\)/) {
    $_ .= " " . <>;
    chomp;
  }

  my ($ret, $func, $args) = /(.*?) *([:\w]+)\((.*)\)/;
  die "Spurious gd_" if ($func =~ /^gd_/);
  my $lfs = ($func =~ /64$/);
  $func =~ s/64$// if ($lfs);
  my $name = $func;
  if ($func =~ /:/) {
    ($func, $name) = $func =~ /(.*):(.*)/;
  }

  die $_ unless $args;
  my @args = split ",", $args;

  my (@argtype, @arg);
  my $arglist = undef;
  my $prmlist = undef;
  for (@args) {
    my ($t, $n, $d) = /\s*(.*?)\s*(\w+)(?:=(.*))?$/;
    push @argtype, $t;
    push @arg, [ $t, $n ];
    $_ = "$t $n";
    $arglist .= ", " if $arglist;
    $prmlist .= ", " if $prmlist;
    $arglist .= "&" if ($t =~ /&$/);
    $arglist .= $n;
    $prmlist .= $n;
    $prmlist .= "=$d" if defined $d;
  }
  s/^\s*// for @args;

  print "$ret\n$name($prmlist)\n\t", join("\n\t", @args), "\n";
  print "\tPREINIT:\n\t\tGDP_DIRFILE_ALIAS;\n" . 
        "\tALIAS:\n\t\tGetData::Dirfile::$name = 1\n"
    if ($args[0] =~ /DIRFILE/);

  print "\tCODE:\n\t\tdtrace(\"", join(", ", map(printfmt($_), @argtype));
  print "\", ", join(", ", map(printmunge($_), @arg)), ");\n";
  print "\t\tRETVAL = ";
  print (($lfs) ? "gdp64(gd_$func)" : "gd_$func");
  print "($arglist);\n\t\tGDP_UNDEF_ON_ERROR();\n\tOUTPUT:\n";
  print "\t\tRETVAL\n\tCLEANUP:\n", map(printcleanup($_), @arg);
  print "\t\tdreturn(\"", &printfmt($ret), "\", ";
  print &printmunge([$ret, "RETVAL"]), ");\n\n";
}
