#!/usr/bin/perl -w

#
# delayfile.pl - Script to parse DREC .dly files and extract delays
# This tool uses the timestamps for calculating delays, jointly with
# outputting the time since the tests started, this way the sequence
# numbers become times, which is much more useful
# 
# by René Serral-Gracià <rserral@ac.upc.edu>
#

use strict;

$main::VERSION = '1.0';
my $MGEN = 0;               # Which MGEN version produced the output (3.x/4.x)
my $GREPABLE = 0;           # Do we want grepable output?
my $TABLE = 0;              # Do we want a table of expected vs. rec. packets?

my $arg;
my $in_file;
my $out_file;
my $first_packet = 0;
my $flow = 1;
my $resolution = 0;
my $onlydelay = 0;
my $ignoreNegative = 0;

my $acc_delay;              # In case of resolution aggregation we need the partial accumulated delay
my $acc_delay2;
my $acc_delay3;
my $packetsMS;              # Yes, it is for the mean
my $currentMsec = -1;       # First case :(
my $bin_first_packet;       # More needed data :)
my $showPLR = 0;            # By default we don't show PLRs
my %seqBucket = ();
my $plr;
my $firstSeq;
my $maxSeq;
my $putSeq = 0;

my @list;

while (defined ($arg = shift @ARGV)) {
    if ($arg eq '3') {
        &HELP_MESSAGE if $MGEN == 4;
        $MGEN = 3;
    } elsif ($arg eq '4') {
        &HELP_MESSAGE if $MGEN == 3;
        $MGEN = 4;
    } elsif ($arg eq '-g') {
        $GREPABLE = 1;
    } elsif ($arg eq '-s') {
        $putSeq = 1;
    } elsif ($arg eq '-t') {
        $TABLE = 1;
        $out_file = shift @ARGV;
        &HELP_MESSAGE if $out_file =~ /^-/;
    } elsif ($arg eq '-f') {
        $flow = shift @ARGV;
        &HELP_MESSAGE if !($flow =~ /\d/);
    } elsif ($arg eq '-r') {
        $resolution = shift @ARGV;
        &HELP_MESSAGE if !($flow =~ /\d/);
    } elsif ($arg eq '-d') {
        $onlydelay = 1;
    } elsif ($arg eq '-p') {
        $showPLR = 1;
    } elsif ($arg eq '-i') {
        $ignoreNegative = 1;
    } else {
        $in_file = $arg;
    }
}

$MGEN = 3 if $MGEN == 0;    # Set deafult MGEN 3.x format

&HELP_MESSAGE unless (defined $in_file);
open DLY, $in_file or die "Input file $in_file not found!\n";

my $orenetaMeters = 0;
while (<DLY>) {
    chomp;
    my $tmp2;
    my $txtime;
    my $rxtime;
    my $rxtime2;
    my $rxtime3;
    my $seq;

    if ($MGEN == 3) {
        $tmp2 = sprintf("Flow>%0.4d Seq>", $flow);
#        next unless ( /^Flow>\d{4} Seq>\d{6}/ );
        next unless ( /^$tmp2\d{6}/ );
        (my @line) = split(/ +/);
        ($tmp2, $seq) = split(/>/,$line[1]);
	($tmp2, $txtime) = split(/>/,$line[6]);
	($tmp2, $rxtime) = split(/>/,$line[7]);
	($tmp2, $rxtime2) = split(/>/,$line[8]);
	if ($tmp2 eq "RxTime2") {
          $orenetaMeters++;
	  ($tmp2, $rxtime3) = split(/>/,$line[9]);
          if ($tmp2 eq "RxTime3") {
            $orenetaMeters++;
          }
	}
    } elsif ($MGEN == 4) {
	$tmp2 = sprintf("RECV flow>%d ", $flow);
        next unless ( /$tmp2/ );
        (my @line) = split(/ +/);
        ($tmp2, $seq) = split(/>/,$line[3]);
	($tmp2, $txtime) = split(/>/,$line[6]);
	$rxtime = $line[0];
    }
    $txtime = date2int($txtime);
    if ($first_packet == 0) {
    	$first_packet = $txtime;
        $firstSeq = $seq;
        $maxSeq = $seq;
    }
    $rxtime = date2int($rxtime);
    (my $delay) = $rxtime - $txtime;
    my $delay2;
    my $delay3;
    if ($delay < 0) {
        if ($ignoreNegative) {
	  print STDERR "Negative delay, continuing as user requested\n";
	} else {
	  print STDERR "Negative delay, aborting\n";
	  exit -1;
	}
    }
    if ($orenetaMeters >= 1) {
      $rxtime2 = date2int($rxtime2);
      $delay2 = $rxtime2 - $rxtime;
    }
    if ($orenetaMeters >= 2) {
      $rxtime3 = date2int($rxtime3);
      $delay3 = $rxtime3 - $rxtime2;
    }

    if ($resolution) {
      if ($showPLR) {
        if ($seq > $maxSeq) {
          $maxSeq = $seq;
        }
        $seqBucket{$seq} = 1;
      }
      my $thisPacket = sprintf("%d",($txtime - $first_packet)/1000);
      $thisPacket += 0;
      if ( ($thisPacket == 0 && $currentMsec == -1) ||
            $thisPacket >= ($currentMsec + $resolution) ) { # Beginning of the msec+resolution
         if ($thisPacket >= ($currentMsec + $resolution) && $currentMsec != -1 ) {
	   if ($onlydelay) {
           	printf ("%.3f", ($acc_delay/$packetsMS)/1000);
	   } elsif ($putSeq) {
           	printf ("%d %.3f", $seq, ($acc_delay/$packetsMS)/1000);
           } else {
           	printf ("%d %.3f", $bin_first_packet, ($acc_delay/$packetsMS)/1000);
	   }
           printf(" %.3f",($acc_delay2/$packetsMS)/1000) if ($orenetaMeters >= 1);
           printf(" %.3f",($acc_delay3/$packetsMS)/1000) if ($orenetaMeters >= 2);

           if ($showPLR) {
             my $tmp;
             my $losses = 0;
	     my @tmp = (sort keys %seqBucket);
	     $losses = ($maxSeq - $firstSeq + 1) - @tmp;
             $plr = $losses / ($maxSeq - $firstSeq + 1);
             %seqBucket = ();
             $maxSeq++;
             $firstSeq = $maxSeq;
             printf(" %.3f", $plr);
           }
           printf("\n");
         }
         $packetsMS = 1;
         $currentMsec = $thisPacket;
         $acc_delay = $delay; # In usec!
         $acc_delay2 = $delay2 if ($orenetaMeters >= 1); # In usec!
         $acc_delay3 = $delay3 if ($orenetaMeters >= 2); # In usec!
         $bin_first_packet = $thisPacket;
      } else { # Continuation of the "bin"
         $packetsMS++;
         $acc_delay += $delay;
         $acc_delay2 += $delay2 if ($orenetaMeters >= 1); # In usec!
         $acc_delay3 += $delay3 if ($orenetaMeters >= 2); # In usec!
      }
    } else {
      $maxSeq = $seq if ($seq > $maxSeq);
      $list[$seq] = { DELAY => $delay,
                      DELAY2 => $delay2,
                      DELAY3 => $delay3,
                      TIME => ($txtime - $first_packet)
      };
    }
}
if (!$resolution) {
  for(my $i = 0; $i < $maxSeq; $i++) {
    my $time1;
    my $time2;
    my $increment;
    my $burst = 0;
    # Detect Loss burst
    while (!defined $list[$i + $burst]->{DELAY} && ($burst + $i) < $maxSeq) {
      $burst++;
    }
    if ($burst) {
      my $tmp = 0;
      if (!$putSeq) {  # Prepare list of fake timestamps
        $time1 = $list[$i-1]->{TIME};
        $time2 = $list[$i+$burst]->{TIME};
        $increment = ($time2 - $time1) / ($burst+1);
      }
      while ($tmp < $burst) { 
        if ($putSeq) {
          printf("%d ", $i+$tmp);
        } else {
          printf("%.3f ", ($time1 + ($increment * ($tmp+1)))/1000 );
        }
        printf("0\n");
        $tmp++;
      }
      $i += $burst;
    }
    my $delay  = $list[$i]->{DELAY};
    my $delay2 = $list[$i]->{DELAY2};
    my $delay3 = $list[$i]->{DELAY3};
    my $fromStart = $list[$i]->{TIME};
    my $seq = $i;
    if ($onlydelay) {
      printf ("%.3f", $delay/1000);
    } elsif  ($putSeq) {
      printf ("%d %.3f", $seq, $delay/1000);
    } else {
      printf ("%.3f %.3f", $fromStart/1000, $delay/1000);
    }
    printf(" %.3f", $delay2/1000) if ($orenetaMeters >= 1);
    printf(" %.3f", $delay3/1000) if ($orenetaMeters >= 2);
    printf("\n");
  }
} elsif ($showPLR && $resolution) {
  my $tmp;
  my $losses = 0;
  for ($tmp = $firstSeq, $tmp <= $maxSeq, $tmp++) {
    $losses++ if ($seqBucket{$tmp} != 1);
  }
  $plr = $losses / ($maxSeq - $firstSeq + 1);
}

if ($resolution && $packetsMS) {
    printf ("%d %.3f", $bin_first_packet, ($acc_delay/$packetsMS)/1000);
    if ($showPLR) {
      printf(" %.3f\n", $plr);
    } else {
      printf("\n");
    }
}

close DLY;

sub date2int {
        my $tmp;
        (my @ints) = split(/:/, $_[0]);
        (my @lowpart) = split(/\./, $ints[2]);
        $tmp= $ints[0]*3600;
        $tmp+=$ints[1]*60+$lowpart[0];
        $tmp=$tmp*1000000+$lowpart[1];
}

sub HELP_MESSAGE {
    die <<EO_HELP_MESSAGE

Out of order packet calculator v$main::VERSION

Usage: $0 [3|4] [-f <flow number>] [-r <resolution in ms>] [-t <output_file>] <input_file>
        3   <input_file> is in MGEN 3.x format
        4   <input_file> is in MGEN 4.x format
        -f   <flow number> specifies the flow to analyse (default = 1)
	-r   <resolution> Packet aggregation in miliseconds
        -t   generate table of expected seq# vs. received seq#
        -s   put sequence number not delay since start
        -d   only put delay values
        -p   insert packet loss ratio also

EO_HELP_MESSAGE
}
