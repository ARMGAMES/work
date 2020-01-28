#ifndef __DALI_I_MSG_H__
#define __DALI_I_MSG_H__

#include "protocol.h"

struct IMsg // interface for messages
{

	struct Key
	{

		Key(){}
		Key( const Key& that ){}
		Key& operator = ( const Key& msg ) { return *this; }
		virtual ~Key() {}
		virtual void format() = 0;
		virtual dali::MsgComposer& compose( dali::MsgComposer& composer ) const = 0;
		virtual dali::MsgParser& parse( dali::MsgParser& parser ) = 0;
		virtual std::string toString() const = 0;
	};

	IMsg() {}
	IMsg( const IMsg::Key & key ) {}
	IMsg& operator = ( const IMsg& msg ) { return *this; }
	virtual ~IMsg() {}
	virtual void format() = 0;
	virtual dali::MsgComposer& compose( dali::MsgComposer& composer ) const = 0;
	virtual dali::MsgParser& parse( dali::MsgParser& parser ) = 0;
	virtual std::string toString() const = 0;

	template< typename K >
	struct Query
	{
		vector< K > keys;
		uint16_t agg_sz, page_sz, page_ix;
		bool is_count;

		Query( uint16_t asz = 0, uint16_t psz = 0, uint16_t pix = 0 )
			: agg_sz( asz )
			, page_sz( psz )
			, page_ix( pix )
			, is_count( false )
		{}

		Query( const Query& that )
			: keys( that.keys )
			, agg_sz( that.agg_sz )
			, page_sz( that.page_sz )
			, page_ix( that.page_ix )
			, is_count( that.is_count )
		{}

		virtual ~Query() {}

		Query& operator=( const Query& that )
		{
			if ( this != &that )
			{
				keys = that.keys;
				agg_sz = that.agg_sz;
				page_sz = that.page_sz;
				page_ix = that.page_ix;
				is_count = that.is_count;
			}
			return *this;
		}

		virtual void format()
		{
			for ( auto & key : keys )
				key.format();
		}

		virtual inline dali::MsgParser& parse( dali::MsgParser& parser )
		{
			return parser
				.parseIterable( keys )
				.parseUint16( agg_sz )
				.parseUint16( page_sz )
				.parseUint16( page_ix )
				;
		}

		virtual inline dali::MsgComposer& compose( dali::MsgComposer& composer ) const
		{
			return composer
				.composeIterable( keys )
				.composeUint16( agg_sz )
				.composeUint16( page_sz )
				.composeUint16( page_ix );
		}

		virtual inline string toString() const
		{
			string str = string().append( " keys={" );
			for ( size_t i = 0; i < keys.size(); ++i )
				str.append( i ? ",(" : "(" ).append( keys[i].toString() ).append( ")" );
			return str
				.append( "}" )
				.append( " asz=" ).append( dali::intString( agg_sz ) )
				.append( " psz=" ).append( svclib::intString( page_sz ) )
				.append( " pix=" ).append( svclib::intString( page_ix ) )
				.append( " is_count=" ).append( is_count ? "Y" : "N" )
				;
		}

		inline bool isAggregation() const { return agg_sz > 0; }

		inline bool isPage() const { return page_sz > 0; }

		inline bool isCount() const { return is_count; }

		template < typename I >
		inline vector< I > getPage( const vector< I > & vec ) const
		{
			int sz = static_cast< int >( vec.size() );
			int begin_ix = page_sz * page_ix;
			if ( begin_ix >= sz )
				return vector< I >();
			int end_ix = begin_ix + page_sz;
			if ( end_ix >= sz )
				end_ix = sz;
			auto cbegin = next( vec.cbegin(), begin_ix );
			auto cend = next( cbegin, end_ix - begin_ix );
			return vector< I >( cbegin, cend );
		}

	};

};

#endif