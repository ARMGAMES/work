#ifndef ppzstream_h_included
#define ppzstream_h_included

#include "ppzip.h"



class PGZipOutputStream : public POutputStream
{
private:
	PGZip zip;

public:
	PGZipOutputStream( POutputStream& stream, int compLevel = -1 )
	:zip(stream, compLevel)
	{}

	void write( const unsigned char* buf, size_t sz ) override
	{ zip.write(buf, sz); }

	void close()
	{ zip.close(); }
};


class PGZipInputStream : public PInputStream
{
private:
	PUnGZip zip;

public:
	PGZipInputStream( PInputStream& stream )
	:zip(stream)
	{}

	size_t read( unsigned char* buf, size_t sz ) override
	{ return zip.read(buf, sz); }

	void close()
	{ zip.close(); }
};


#endif