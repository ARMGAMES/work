# -*- coding: utf-8 -*-
#############################################################################################################################
## Copyright (C) 2017 PYR Software Ltd. All rights reserved.
#############################################################################################################################

from __future__ import print_function, absolute_import, division, unicode_literals
from builtins import *

import sys
import os
from getopt import getopt
from future.utils import raise_with_traceback

from pyr.atfverify.atfverify import gen_atf_def, verify_atf_def

__version__ = '1.0'
__license__ = 'PYR'
__author__ = 'Vadim Panin'
__copyright__ = 'Copyright (C) 2017 PYR Software Ltd. All rights reserved.'
__all__ = ['main', 'gen_action', 'verify_action']

def _get_create_update_opts(opts):

	verbose = False
	list_path = None
	gen_path = None
	perl_path = None

	for o, v in opts:
		if '-l' == o or '--list' == o:
			list_path = v.replace('\\', '/')
		elif '-G' == o or '--genprot' == o:
			gen_path = v
		elif '-p' == o or '--perl' == o:
			perl_path = v
		elif '-v' == o or '--verbose' == o:
			verbose = True
	
	#print("args: {} {} {} {}".format(list_path, gen_path, perl_path, verbose))

	return list_path, gen_path, perl_path, verbose


def gen_action(opts):
	list_path, gen_path, perl_path, verbose = _get_create_update_opts(opts)
	return gen_atf_def(list_path, gen_path, perl_path, verbose)


def verify_action(opts):
	list_path, gen_path, perl_path, verbose = _get_create_update_opts(opts)
	return verify_atf_def(list_path, gen_path, perl_path, verbose)

_ACTIONS = {
	'gen':		gen_action,
	'generate':	gen_action,
	'vrf':		verify_action,
	'verify':	verify_action,
}


def _usage():
	params = {
		'copyright': __copyright__.strip(),
		'version': __version__,
		'app_name': os.path.basename(sys.argv[0])
	}

	print('''{copyright}
{app_name} utility version {version}

usage {app_name} <verify|generate> [--list <atf_files_list>] [options]

Options:
    -l, --list <path>      path to ATF files list, 'ATFVerify.ini' by default, see below;
    -G, --genprot <path>   path to Generator script '../../atf/genprot.pl' by default;
    -p, --perl <path>      path to Perl executable, 'perl' by default;
    -v, --verbose          turn on verbose output.

ATF files list should be in the following format:
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
'''
	.format(**params), file=sys.stderr)

def main():
	try:
		if len(sys.argv) < 2:
			_usage()
			return 10 #InvalidUsage

		action = sys.argv[1]
		if 'help' == action:
			_usage()
			return 10 #InvalidUsage

		ropts = []
		opts, _ = getopt(sys.argv[2:], 'l:G:p:v', ['list=','genprot=', 'perl=', 'verbose'])
		for o, v in opts:
			ropts.append((o, v))

		return _ACTIONS[action](ropts)
	except Exception as e:
		print("Failed by exception '{}'".format(e))
		return 100 #UnknownError

	return OK
