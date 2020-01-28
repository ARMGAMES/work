#ifndef ppstream_h_included
#define ppstream_h_included

#include "ppincludebase.h"
#include "ppblock.h"

/* abstract */ class PInputStream
{
public:
	virtual size_t read( unsigned char* buf, size_t sz ) = 0;
	virtual ~PInputStream()
	{}
};

/* abstract */ class POutputStream
{
public:
	virtual void write( const unsigned char* buf, size_t sz ) = 0;
	virtual bool supportSeekTell() const { return false; }
	virtual void seek( int offset, int origin ) { PASSERT4( 0 ); }//origin - SEEK_CUR, SEEK_SET, SEEK_END
	virtual unsigned long tell() const { return 0; }
	virtual ~POutputStream()
	{}

};

class PMemOutputStream : public POutputStream
{
private:
	PBlock& out;

public:
	PMemOutputStream( PBlock& out_ );
	void write( const unsigned char* buf, size_t sz ) override;

private: // prohibit default assignment operator
	PMemOutputStream& operator=( const PMemOutputStream& other );
};

class PMemInputStream : public PInputStream
{
private:
	PBlock& in;
	size_t pos;

public:
	PMemInputStream( PBlock& in_ );
	size_t read( unsigned char* buf, size_t sz ) override;

private: // prohibit default assignment operator
	PMemInputStream& operator=( const PMemInputStream& other );
};

class PFileOutputStream : public POutputStream
{
private:
	FILE* f;

public:
	PFileOutputStream( FILE* f_ );
	void write( const unsigned char* buf, size_t sz ) override;
	bool supportSeekTell() const override;
	void seek( int offset, int origin ) override;//origin - SEEK_CUR, SEEK_SET, SEEK_END
	unsigned long tell() const override;
};

class PFileInputStream : public PInputStream
{
private:
	FILE* f;

public:
	PFileInputStream( FILE* f_ );
	size_t read( unsigned char* buf, size_t sz ) override;
};

#endif

