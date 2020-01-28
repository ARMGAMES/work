from setuptools import setup, find_packages

setup(
	name				= 'atfverify',
	namespace_packages	= ['pyr'],
	version				= '1.0',
	packages			= find_packages(),
	entry_points		= {'console_scripts': ['atfverify = pyr.atfverify.cmdline:main']},
	install_requires	= ['future>=0.16.0'],
	author				= 'PYR Software Ltd.',
	author_email		= 'vadimp@starstechnologies.com',
	description			= 'ATF Verification Tool',
	license				= 'PYR'
)
