use v5.14;
use strict;
use warnings;

use constant SAMPLING_RATE => 48_000;

my @prime = ();
if (0) {
    my $prime_numbers = gen_prime_numbers( int(SAMPLING_RATE / 2) );
    @prime = @{$prime_numbers};

    open(my $fh, '>', 'prime.txt')
        or die "Can't open prime.txt: $!";

    while ( my @numbers = splice(@prime, 0, 10) ) {
        print $fh join( ',', map sprintf("%5d", $_), @numbers );
        print $fh "\n";
    }

    close $fh;
    exit 0;
}
else {
    open(my $fh, '<', 'prime.txt')
        or die "Can't open prime.txt: $!";

    while ( my $line = <$fh> ) {
        chomp $line;
        next if not $line;
        push @prime, map int($_), split(/,/, $line);
    }

    close $fh;
}

if (0) {
    srand();
    my $base = 29.5 + rand(100) / 100;
    my $tmp = log2(45.5 / $base);
    my @delay_times = map {
        $base * (2 ** ($_ * ($tmp / 7)));
    } 0..7;

    while (my @items = splice(@delay_times , 0, 4)) {
        say join(", ", map {
            sprintf("%.1f", $_);
        } @items);
    }

    exit 0;
}

if (1) {
    my @delay_samples = map {
        find_prime_number( int(SAMPLING_RATE * $_ / 1_000), \@prime );
    } ( 30.2, 32.1, 34.0, 36.0,
        38.2, 40.5, 42.9, 45.5 );

    say join( ', ', map sprintf("%4d", $_), @delay_samples );

    my $n = 64;
    my @params = map {
        my $t = $_ / $n;
        my $time_sec = 2 ** (1 + (7 * $t));

        my $samples = int(SAMPLING_RATE * $time_sec);
        my @times_list = map {
            $samples / $_;
        } @delay_samples;

        # printf("[%2d] %5.2f\n", $i, $time_sec);

        my @gain_list = calc_gain( @times_list );

        +{
            time_sec => $time_sec,
            gain_list => \@gain_list
        };
    } 1..($n - 1);

    for (my $i=0; $i<scalar(@params); $i++) {
        my $j = $i + 1;
        my $param = $params[$i];
        my $gain_csv = join( ', ', map {
            my $gain = $_;
            # sprintf("0x%04X", int(($gain * 0x8000) + .5));
            sprintf("%.6f", $gain);
        } @{$param->{gain_list}} );

        printf("    { %s }, // [%2d] %.3f\n",
            $gain_csv, $j, $param->{time_sec});
    }
}

sub calc_gain {
    my @times_list = @_;
    return map {
        2 ** (log2(0.001) / $_);
    } @times_list;
}

sub calc_zmax {
    my $gain = shift;
    return 1.0 / (1.0 - $gain);
}

sub find_prime_number {
    my ( $num, $numbers ) = @_;

    my $n = scalar(@{$numbers});
    for (my $i=0; $i<$n; $i++) {
        if ( $num < $numbers->[$i] ) {
            my $diff_over = $numbers->[$i] - $num;
            my $diff_under = $num - $numbers->[$i-1];

            if ( $diff_over < $diff_under ) {
                return $numbers->[$i];
            }
            else {
                return $numbers->[$i-1];
            }
        }
    }

    die "near value not found: ", $num;
}

sub gen_prime_numbers {
    my $max_value = shift;
    die "max_value is too small" if $max_value <= 2;

    my @prime = 2..$max_value;
    my $num = $prime[0];
    while ( $num != $prime[-1] ) {
        @prime = grep {
            ($_ <= $num) or (($_ % $num) != 0);
        } @prime;

        $num = bigger_number( $num, \@prime );
    }

    return \@prime;
}

sub bigger_number {
    my ( $th, $numbers ) = @_;
    my @tmp = grep { $th < $_; } @{$numbers};
    return @tmp ? $tmp[0] : $th;
}

sub log2 {
    return log($_[0]) / log(2.0);
}
