#!/usr/bin/perl
use XMLNMMgenParser;
use strict;
use 5.010;

my $filename = '/home/rbarreda/proves/mgen/test_owd_20070329_160755_vtjm.xff';
my $xml = XMLNMMgenParser::readFile( $filename );

# my $xml = XMLNMMgenParser::readFile( $ARGV[0] );

XMLNMMgenParser::parseTest( $xml );

say "RESULTS (1,1,'tx') direct access >> ", $XMLNMMgenParser::flows{ 1 }{ 1 }{'tx'};
say "RESULTS (1,1,'tx') via function  >> ", XMLNMMgenParser::getData( 1, 1, 'tx');
do XMLNMMgenParser::demo();