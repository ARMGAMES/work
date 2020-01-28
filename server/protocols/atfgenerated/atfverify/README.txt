Verifies that all entries in ATF file list have up-to-date generated header and source files.

usage {app_name} <verify|generate> [--list <atf_files_list>] [options]

Options:
    -l, --list <path>      path to ATF file list, 'ATFVerify.ini' by default, see below;
    -G, --genprot <path>   path to genprot.pl script '../../atf/genprot.pl' by default;
    -p, --perl <path>      path to Perl executable, 'perl' by default;
    -v, --verbose          turn on verbose output.

ATF file list should be in the following format:
    sample_atf.txt [generation options]
    sample2_atf.txt [generation options]

Error codes:
    0 - OK
    1 - generation failure
    2 - verification failure
    3 - genprot.pl script not found
    10 - invaid usage
    20 - unknown error (most likely IO exception)
    100 - Perl is not installed