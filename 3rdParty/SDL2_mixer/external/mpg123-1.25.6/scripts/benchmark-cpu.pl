#!/usr/bin/perl
#
# benchmark-cpu.pl: benchmark CPU optimizations of mpg123
#
# initially written by Nicholas J Humfrey <njh@aelius.com>, placed in the public domain
#

use strict;
#use Time::HiRes qw/time/;

my $MPG123_CMD = shift @ARGV;
my @TEST_FILES = @ARGV;

die "Please specify full path to mpg123 >= 1.7.0 and a test MP3 file to decode" if (scalar(@ARGV) < 1);
die "mpg123 command does not exist" unless (-e $MPG123_CMD);
die "mpg123 command is not executable" unless (-x $MPG123_CMD);
for(@TEST_FILES)
{
	die "test MP3 file does not exist" unless (-e $_);
}

# Force unbuffed output on STDOUT
#$|=1; # why?

# Check the CPUs available
my $cpulist = `$MPG123_CMD --test-cpu`;
chomp( $cpulist );
die "Failed to get list of available CPU optimizations" unless ($cpulist =~ s/^Supported decoders: //);

my @cpus = split( / /, $cpulist );
my @encs = qw(s16 f32);

printf STDERR ("Found %d CPU optimizations to test...\n\n", scalar(@cpus) );

print "#mpg123 benchmark (user CPU time in seconds for decoding)\n";
print "#decoder";
for(@encs){ print "	t_$_/s"; }
print "\n";

my $allret = 0;

foreach my $cpu (@cpus)
{
	print "$cpu";
	foreach my $e (@encs)
	{
		# using user CPU time
		my @start_time  = times();
		my $ret = system($MPG123_CMD, '-q', '--cpu', $cpu, '-e', $e, '-t', @TEST_FILES );
		my @end_time = times();
		my $runtime = $end_time[2] - $start_time[2];
		if($ret)
		{
			print STDERR "Execution of $MPG123_CMD failed with code $ret!\n";
			$runtime = 0;
			$allret = 1;
		}
		# third entry is child user time
		printf("	%4.2f", $runtime);
	}
	print("\n");
}

exit($allret);
