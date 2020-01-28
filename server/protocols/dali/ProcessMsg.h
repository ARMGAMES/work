#ifndef __DALI_PROCESS_MSG_H__
#define __DALI_PROCESS_MSG_H__

#include "SampleMsg.h"

#define PROCESS_MSG_HEADER SAMPLE_MSG_HEADER "|Hash|Seq1|Seq2|File Name|File Size|File Dir"

struct ProcessMsg // dali process data
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		vector<uint8_t> file_hash;
		string file_name;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, file_hash( svclib::NULL_BIN )
			, file_name( svclib::NULL_STR )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, file_hash( that.file_hash )
			, file_name( that.file_name )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				file_hash = that.file_hash;
				file_name = that.file_name;
			}
			return *this;
		}

		void format() override
		{
			SampleMsg::Key::format();
			dali::cut( file_hash, FILE_HASH_SIZE );
			dali::cut( file_name, FILE_NAME_SIZE );
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			SampleMsg::Key::parse( parser )
				.parseBinary( file_hash, FILE_HASH_SIZE, "FILE_HASH_SIZE" )
				.parseString( file_name, 2 * FILE_NAME_SIZE, "2 * FILE_NAME_SIZE" )
				;
			return parser;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeBinary( file_hash )
				.composeString( file_name )
				;
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( " hash=" ).append( dali::toBase16( file_hash ) )
				.append( " name=" ).append( file_name )
				;
		}

	};

	vector<uint8_t> file_hash;				// fh - MD5 hash
	vector<uint8_t> seq1_hash;				// s1h - file sequence 1 is MD5 hash of [0..64KiB ) if file is greater than or equal to 64KiB empty otherwise
	vector<uint8_t> seq2_hash;				// s2h - file sequence 2 is MD5 hash of [256KiB..512KiB ) if file is greater than or equal to 512KiB empty otherwise
	string file_name;				// fn
	uint64_t file_size;				// fs
	string file_dir;				// fd

	ProcessMsg()
		: file_size( 0 )
	{}

	ProcessMsg( const Key & key )
		: SampleMsg( key )
		, file_hash( key.file_hash )
		, file_name( key.file_name )
		, file_size( 0 )
	{}

	void format() override
	{
		SampleMsg::format();
		dali::cut( file_hash, FILE_HASH_SIZE );
		dali::cut( seq1_hash, FILE_HASH_SIZE );
		dali::cut( seq2_hash, FILE_HASH_SIZE );
		dali::cut( file_name, FILE_NAME_SIZE );
		dali::cut( file_dir, FILE_DIR_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseBinary( file_hash, FILE_HASH_SIZE, "FILE_HASH_SIZE" )
			.parseBinary( seq1_hash, FILE_HASH_SIZE, "FILE_HASH_SIZE:SEQ1" )
			.parseBinary( seq2_hash, FILE_HASH_SIZE, "FILE_HASH_SIZE:SEQ2" )
			.parseString( file_name, 2 * FILE_NAME_SIZE, "2 * FILE_NAME_SIZE" )
			.parseUint64( file_size )
			.parseString( file_dir, 2 * FILE_DIR_SIZE, "2 * FILE_DIR_SIZE" )
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeBinary( file_hash )
			.composeBinary( seq1_hash )
			.composeBinary( seq2_hash )
			.composeString( file_name )
			.composeUint64( file_size )
			.composeString( file_dir )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " hash=" ).append( dali::toBase16( file_hash ) )
			.append( " seq1=" ).append( dali::toBase16( seq1_hash ) )
			.append( " seq2=" ).append( dali::toBase16( seq2_hash ) )
			.append( " name=" ).append( file_name )
			.append( " size=" ).append( dali::intString( file_size ) )
			.append( " dir=" ).append( file_dir )
			;
	}

	string toLine() const override
	{
		return SampleMsg::toLine()
			.append( "|" ).append( dali::toBase16( file_hash ) )
			.append( "|" ).append( dali::toBase16( seq1_hash ) )
			.append( "|" ).append( dali::toBase16( seq2_hash ) )
			.append( "|" ).append( dali::quoteString( file_name ) )
			.append( "|" ).append( dali::intString( file_size ) )
			.append( "|" ).append( dali::quoteString( file_dir ) )
			;
	}

	static std::string header()
	{
		return PROCESS_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;
	typedef BaseMsg::Key DeltaKey;
	typedef TimeMsg::DeltaQuery< DeltaKey > DeltaQuery;

};

#endif