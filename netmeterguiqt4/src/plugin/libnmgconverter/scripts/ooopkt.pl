#!/usr/bin/perl -w

#
# ooopkt.pl - Script to parse DREC .dly files and extract out of order
# packet information
#
# by Lorand Jakab <ljakab@ac.upc.edu>
#

use strict;

$main::VERSION = '1.0';
my $MGEN = 0;               # Which MGEN version produced the output (3.x/4.x)
my $GREPABLE = 0;           # Do we want grepable output?
my $TABLE = 0;              # Do we want a table of expected vs. rec. packets?
my $pkt_cnt = 0;            # Total number of received packets
my $seq_max = 0;            # The highest received sequence number
my $seq_cur = 0;            # The last received sequence number
my $ooo_cnt = 0;            # Counter for ooo packets
my $ooo_max_delta = 0;      # The maximum ooo distance
my $ooo_cur_delta = 0;      # Current ooo distance
my $ooo_max_delta_seq;      # Sequence number of the most ooo packet
my @received;               # Sequence numbers of received packets
my %bucket;                 # Temporary place for expected packets that didn't
                            # arrive yet. At the end it will contain the
                            # sequence numbers of lost packets
my $flow = 0;

my $arg;
my $in_file;
my $out_file;
while (defined ($arg = shift @ARGV)) {
    if ($arg eq '3') {
        &HELP_MESSAGE if $MGEN == 4;
        $MGEN = 3;
    } elsif ($arg eq '4') {
        &HELP_MESSAGE if $MGEN == 3;
        $MGEN = 4;
    } elsif ($arg eq '-g') {
        $GREPABLE = 1;
    } elsif ($arg eq '-t') {
        $TABLE = 1;
        $out_file = shift @ARGV;
        &HELP_MESSAGE if $out_file =~ /^-/;
    } elsif ($arg eq '-f') {
        $flow = shift @ARGV;
        &HELP_MESSAGE if !($flow =~ /\d/);
    }  else {
        $in_file = $arg;
    }
}

if (!$flow) {
        $flow = 1;
}

$MGEN = 3 if $MGEN == 0;    # Set deafult MGEN 3.x format

&HELP_MESSAGE unless (defined $in_file);
open DLY, $in_file or die "Input file $in_file not found!\n";

while (<DLY>) {
    chomp;
    my $tmp2;
    my $seq;
    if ($MGEN == 3) {
        $tmp2 = sprintf("Flow>%0.4d Seq>", $flow);
#        next unless ( /^Flow>\d{4} Seq>\d{6}/ );
        next unless ( /^$tmp2\d{6}/ );
        (my $tmp, $seq) = split(/Seq>/);
        ($seq) = split(/\s+/,$seq);
    }
    
    if ($MGEN == 4) {
        $tmp2 = sprintf("RECV flow>%d ", $flow);
        next unless ( /$tmp2/ );
        (my $tmp, $seq) = split(/seq>/);
        ($seq) = split(/\s+/,$seq);
    }

    # We add 0 to get rid of leading 0s (convert to number)
    $seq_cur = $seq + 0;
    push @received, $seq_cur if $TABLE;
    $pkt_cnt++;
    if ($seq_cur < $seq_max) {                      # Out of order packet
        delete $bucket{"$seq_cur"};                 # Remove missing packet flag
        $ooo_cnt++;
        $ooo_cur_delta++;
        if ($ooo_cur_delta > $ooo_max_delta) {      # Distance increased
            $ooo_max_delta = $ooo_cur_delta;
            $ooo_max_delta_seq = $seq_cur;
        }
    } else {                                        # In-order packet
        if (($seq_cur - $seq_max) > 1) {            # Flag missing packets
            for (my $i = $seq_max + 1; $i < $seq_cur; $i++) {
                $bucket{"$i"} = "MISSING";
            }
        }
        $seq_max = $seq_cur;
        $ooo_cur_delta = 0;
    }
}

close DLY;

# Increment the sequence number for statistics (sequencing starts at 0)
$seq_max += 1;

if ($seq_max == 1) {
    print STDERR "NO RESULTS! Input file was probably not a valid ASCII DREC/MGEN4 log file!\n\n";
    exit 1;
}

unless ($GREPABLE) {
    printf "\nOut of order packet calculator v$main::VERSION\n\n";
    printf "   Total number of sent packets:      %7d\n", $seq_max;
    printf "   Total number of reiceived packets: %7d\n", $pkt_cnt;
    printf "                                      -------\n", $pkt_cnt;
    printf "   Packet loss [ratio]:               %7d [%.2e]\n", $seq_max - $pkt_cnt, ($seq_max - $pkt_cnt) / $seq_max;
    printf "\n";
    printf "   Out of order packets [ratio]:      %7d [%.2e]\n", $ooo_cnt, $ooo_cnt / $seq_max;
    printf "   Maximum ooo distance:              %7d\n", $ooo_max_delta;
    if (defined $ooo_max_delta_seq) {
        printf "      (Packet #%06d)\n\n", $ooo_max_delta_seq;
    } else {
        printf "\n";
    }
    if (keys (%bucket) < 50 && keys (%bucket) != 0) {
        printf "   Lost packet sequence numbers:\n";
        my $j = 1;
        foreach (sort keys %bucket) {
            printf "      %6d", $_;
            printf "\n" if $j % 5 == 0;
            $j++;
        }
        printf "\n";
    }
} else {
    print "$ooo_cnt\t$ooo_max_delta\t", $seq_max - $pkt_cnt, "\t$seq_max\n";
}

my $offset = 0;
my $m = 0;
if ($TABLE) {
    open OF, ">$out_file" or die "\nUnable to create $out_file: $!\n";
    for ($m=0; $m<$seq_max; $m++) {
        &check_index;
        print OF "$m\t$received[$m-$offset]\n";
    }
    close OF;
}

sub check_index {
    if (defined $bucket{"$m"}) {
        $m++;
        $offset++;
        &check_index;
    }
}

sub HELP_MESSAGE {
    die <<EO_HELP_MESSAGE

Out of order packet calculator v$main::VERSION

Usage: $0 [-3|-4] [-g] [-t <output_file>] <input_file>
        -3   <input_file> is in MGEN 3.x format
        -4   <input_file> is in MGEN 4.x format
        -g   grepable output
        -t   generate table of expected seq# vs. received seq#

EO_HELP_MESSAGE
}
