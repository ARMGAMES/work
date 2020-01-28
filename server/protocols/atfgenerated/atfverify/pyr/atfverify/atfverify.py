# -*- coding: utf-8 -*-
#############################################################################################################################
## Copyright (C) 2017 PYR Software Ltd. All rights reserved.
#############################################################################################################################

from __future__ import print_function, absolute_import, division, unicode_literals
from distutils.spawn import find_executable
from builtins import *

__version__ = '1.0'
__license__ = 'PYR'
__author__ = 'Vadim Panin'
__copyright__ = 'Copyright (C) 2017 PYR Software Ltd. All rights reserved.'
__all__ = ['gen_atf_def', 'verify_atf_def']

# ERROR CODES:
OK = 0
GenerationFailure = 1
VerificationFailure = 2
GenprotNotFound = 3
InvalidUsage = 10
UnknownError = 20
PerlNotInstalled = 100

import os
import sys
import shutil
import argparse
import platform
import subprocess
import filecmp
import difflib

class Item:
	filename = ""
	filename_tmp = ""
	keys = []
	generated_ok = None

	def __init__(self, filename, keys, filename_tmp):
		self.filename = filename
		self.keys = keys
		self.filename_tmp = filename_tmp

class Worker:

	list_path = None
	gen_path = None
	perl_path = None
	verbose = False
	output_dir = None
	
	items = []
	items_failed = []
	items_different = []

	def __init__(self, list_path, gen_path, perl_path, verbose, output_dir):
		self.list_path = list_path
		self.gen_path = gen_path
		self.perl_path = perl_path
		self.verbose = verbose
		self.output_dir = output_dir
		
		if self.verbose:
			print("Verbose mode ON")
		
		if not self.list_path:
			self.list_path = 'ATFVerify.ini'
		
		if not self.perl_path:
			self.perl_path = 'perl'
			
		if not self.gen_path:
			# trying to guess genprot path
			self.gen_path = os.path.abspath("../../atf/genprot.pl")
			if not os.access(self.gen_path, os.X_OK):
				self.gen_path = os.path.abspath("../atf/genprot.pl")
			if not os.access(self.gen_path, os.X_OK):
				self.gen_path = os.path.abspath("../../../atf/genprot.pl")
			if not os.access(self.gen_path, os.X_OK):
				self.gen_path = os.path.abspath("../../../../atf/genprot.pl")
		else:
			self.gen_path = os.path.abspath(self.gen_path)
	
	
	def _check_for_perl(self):
		# testing if Perl is installed
		exec_path = find_executable(self.perl_path)
		return True if os.access(exec_path if exec_path else '', os.X_OK) else False
	
	
	def _check_for_genprot(self):
		# testing if genprot path is valid
		return True if os.access(self.gen_path, os.X_OK) else False
	
	
	def _read_file_list(self):
		# parsing ATF definitions list
		with open(self.list_path, 'rt') as inputfile:
			for line in inputfile.readlines():
				parts = line.split()
				filename = parts[0]
				keys = parts[1:]
				self.items.append(Item(filename,keys, os.path.join(self.output_dir, os.path.basename(filename))))
	
	
	def _print_items(self):
		# starting to process the files
		print("=== Found {} ATF definitions".format(len(self.items)))
		if verbose:
			for item in self.items:
				print("\t{} {}".format(item.filename, item.keys))
	
	
	def _generate_items(self):
		# generating h/cpp
		print("\n=== Generating h/cpp files")
		self.items_failed = []
		for item in self.items:
			#if verbose:
			print("\tExecuting Perl script for {} {}".format(item.filename, item.keys))

			atf_filename = item.filename
			old_cwd = os.getcwd()
			if len(self.output_dir) > 0:
				os.chdir(self.output_dir) # this is the only way to keep the path to TempDir out of comments in h/cpp
				atf_filename = os.path.basename(item.filename)

			params = [ self.perl_path, self.gen_path] + item.keys + [ atf_filename ]

			p = subprocess.Popen(params, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			stdout_data, stderr_data = p.communicate()
			os.chdir(old_cwd)
			if self.verbose:
				print(stdout_data)
			if p.returncode:
				item.generated_ok = False
				self.items_failed.append(item.filename)
				print(stderr_data)
				print("*** Error generating h/cpp from {} (return code {}) ***".format(item.filename, p.returncode))
			else:
				item.generated_ok = True
	
	
	def _copy_items_to_output_dir(self):
		# copying all atf files to temporary directory
		print("\n=== Copying {} atf files to a temporary directory".format(len(self.items)))
		for item in self.items:
			shutil.copyfile(item.filename, item.filename_tmp)
	
	
	def _compare_items(self):
		print("\n=== Comparing the existing and generated files")
		for item in self.items:
			if not item.generated_ok: # skip item if generation failed
				continue

			postfix = ""
			if "-thin" in item.keys:
				postfix = "_thin"

			for ext in [".h",".cpp"]:
				orig_fn = os.path.splitext(item.filename)[0] + postfix + ext
				temp_fn = os.path.splitext(item.filename_tmp)[0] + postfix + ext

				with open(orig_fn,'rt') as orig_fh, open(temp_fn,'rt') as temp_fh:
					orig_lines = orig_fh.read().splitlines()
					temp_lines = temp_fh.read().splitlines();

					# skipping the first 9 lines for each file
					# those lines contain comment blocks and ifdef guards, which depend on the file path
					del orig_lines[:9]
					del temp_lines[:9]

					d = difflib.Differ()
					diff = difflib.unified_diff(orig_lines, temp_lines)
					delta = '\n'.join(diff)

					if len(delta) > 0:
						self.items_different.append(orig_fn)
						if self.verbose:
							print("\n=== {} diff:".format(orig_fn))
							print(delta)
					else:
						if self.verbose:
							print("=== {} verified OK".format(orig_fn))
	
	
	def _print_failed_items(self):
		if len(self.items_failed):
			print("\n=== FAILED TO GENERATE:")
			for filename in self.items_failed:
				print("\t{}".format(filename))

		if len(self.items_different):
			print("\n=== VERIFICATION FAILED for:")
			for filename in self.items_different:
				print("\t{}".format(filename))
	
	
	def _calc_return_code(self):
		return_code = OK
		
		if len(self.items_failed):
			return_code = GenerationFailure
		elif len(self.items_different):
			return_code = VerificationFailure
		
		return return_code
	
	
	def do_generation(self):
		if not self._check_for_perl():
			print("=== Perl executable not found! '{}'".format(self.perl_path))
			return PerlNotInstalled
		if not self._check_for_genprot():
			print("genprot script not found! '{}', please specify it in arguments".format(self.gen_path))
			return GenprotNotFound
		self._read_file_list()
		self._generate_items()
		self._print_failed_items()
		return self._calc_return_code()
	
	
	def do_verification(self):
		if not self._check_for_perl():
			print("=== Perl executable not found! '{}'".format(self.perl_path))
			return PerlNotInstalled
		if not self._check_for_genprot():
			print("genprot script not found! '{}, please specify it in arguments'".format(self.gen_path))
			return GenprotNotFound
		self._read_file_list()
		self._copy_items_to_output_dir()
		self._generate_items()
		self._compare_items()
		self._print_failed_items()
		return self._calc_return_code()

def gen_atf_def(list_path, gen_path, perl_path, verbose):
	worker = Worker(list_path, gen_path, perl_path, verbose, "")
	return_code = worker.do_generation()
	
	if return_code == OK:
		print("\n=== Generation result: OK")
	else:
		print("\n=== Geenration result: FAIL")
	
	return return_code

def verify_atf_def(list_path, gen_path, perl_path, verbose):
	output_dir=".atfverifytmp"
	shutil.rmtree(output_dir, ignore_errors=True)
	os.makedirs(output_dir)
	
	worker = Worker(list_path, gen_path, perl_path, verbose, output_dir)
	return_code = worker.do_verification()
	
	shutil.rmtree(output_dir, ignore_errors=True)
	
	if return_code == OK:
		print("\n=== Verification result: OK")
	else:
		print("\n=== Verification result: FAIL")
	
	return return_code



