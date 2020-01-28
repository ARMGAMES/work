#ifndef __DALI_TIME_MSG_H__
#define __DALI_TIME_MSG_H__

#include "ProtocolMsg.h"

constexpr int64_t MAX_QUERY_RANGE_USEC = 730 * USEC_IN_DAY;	// 730 days
constexpr int64_t MAX_QUERY_MARGIN_USEC = 1 * USEC_IN_DAY;	// 1 day
constexpr int64_t MIN_QUERY_DATE = -6342883200000000;		// 1700-01-01
constexpr int64_t MAX_QUERY_DATE = 34681132800000000;		// 3000-01-01

#define TIME_MSG_HEADER "Save time"

struct TimeMsg
	: public IMsg
{

	int64_t save_time;				// savet

	TimeMsg()
		: save_time( svclib::NULL_I64 )
	{}

	TimeMsg( const IMsg::Key & key )
		: IMsg( key )
		, save_time( svclib::NULL_I64 )
	{}

	TimeMsg( int64_t save_time_ )
		: save_time( save_time_ )
	{}

	virtual void format() override
	{}

	virtual dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		return parser.parseInt64( save_time );
	}

	virtual dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return composer.composeInt64( save_time );
	}

	virtual string toString() const override
	{
		return string( "saved=" ).append( dali::toString( save_time ) );
	}

	virtual string toLine() const
	{
		return dali::toString( save_time );
	}

	static std::string header()
	{
		return TIME_MSG_HEADER;
	}

	template< typename K >
	struct Query
		: public IMsg::Query< K >
	{

		int64_t save_time_from, save_time_to;

		Query( int64_t time_from = MIN_QUERY_DATE, int64_t time_to = MAX_QUERY_DATE )
			: IMsg::Query< K >()
			, save_time_from( time_from )
			, save_time_to( time_to )
		{}

		Query( const Query& that )
			: IMsg::Query< K >( that )
			, save_time_from( that.save_time_from )
			, save_time_to( that.save_time_to )
		{}

		Query( uint16_t asz, uint16_t psz, uint16_t pix, int64_t time_from, int64_t time_to )
			: IMsg::Query< K >( asz, psz, pix )
			, save_time_from( time_from )
			, save_time_to( time_to )
		{}

		Query& operator=( const Query& that )
		{
			IMsg::Query< K >::operator=( that );
			if ( this != &that )
			{
				save_time_from = that.save_time_from;
				save_time_to = that.save_time_to;
			}
			return *this;
		}

		virtual void format()
		{
			IMsg::Query< K >::format();
			auto max_val = svclib::getTime( true ) + MAX_QUERY_MARGIN_USEC;
			if ( save_time_to < save_time_from || save_time_to > max_val )
				save_time_to = max_val;
			auto min_val = save_time_to - MAX_QUERY_RANGE_USEC;
			if ( save_time_from < min_val )
				save_time_from = min_val;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return IMsg::Query< K >::parse( parser )
				.parseInt64( save_time_from )
				.parseInt64( save_time_to )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return IMsg::Query< K >::compose( composer )
				.composeInt64( save_time_from )
				.composeInt64( save_time_to )
				;
		}

		string toString() const override
		{
			return IMsg::Query< K >::toString()
				.append( " save=[" ).append( svclib::toString( save_time_from ) ).append(",").append( svclib::toString( save_time_to)).append( "]" )
				;
		}

		virtual void setRollingMonth()
		{
			auto now = svclib::getTime();
			save_time_from = now - 30 * USEC_IN_DAY;
			save_time_to = now + USEC_IN_SEC;
		}
	};

	template < typename K >
	struct DeltaQuery : public TimeMsg::Query< K >
	{

		int64_t diff_time_from, diff_time_to;

		DeltaQuery(
			int64_t stime_from = MIN_QUERY_DATE,
			int64_t stime_to = MAX_QUERY_DATE,
			int64_t dtime_from = MIN_QUERY_DATE,
			int64_t dtime_to = MAX_QUERY_DATE
		)
			: TimeMsg::Query< K >( stime_from, stime_to )
			, diff_time_from( dtime_from )
			, diff_time_to( dtime_to )
		{}

		DeltaQuery( const DeltaQuery& that )
			: TimeMsg::Query< K >( that )
			, diff_time_from( that.diff_time_from )
			, diff_time_to( that.diff_time_to )
		{}

		DeltaQuery( uint16_t asz, uint16_t psz, uint16_t pix, int64_t time_from, int64_t time_to, int64_t test_time_from, int64_t test_time_to )
			: TimeMsg::Query< K >( asz, psz, pix, time_from, time_to )
			, diff_time_from( test_time_from )
			, diff_time_to( test_time_to )
		{}

		DeltaQuery& operator=( const DeltaQuery& that )
		{
			TimeMsg::Query< K >::operator=( that );
			if ( this != &that )
			{
				diff_time_from = that.save_time_from;
				diff_time_to = that.save_time_to;
			}
			return *this;
		}

		void format() override
		{
			TimeMsg::Query< K >::format();

			auto max_val = svclib::getTime( true ) + MAX_QUERY_MARGIN_USEC;
			if ( diff_time_to < diff_time_from || diff_time_to > max_val )
				diff_time_to = max_val;
			auto min_val = diff_time_to - MAX_QUERY_RANGE_USEC;
			if ( diff_time_from < min_val )
				diff_time_from = min_val;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return TimeMsg::Query< K >::parse( parser )
				.parseInt64( diff_time_from )
				.parseInt64( diff_time_to )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return TimeMsg::Query< K >::compose( composer )
				.composeInt64( diff_time_from )
				.composeInt64( diff_time_to )
				;
		}

		string toString() const override
		{
			return TimeMsg::Query< K >::toString()
				.append( " time_diff=[" ).append( svclib::toString( diff_time_from ) ).append( "," ).append( svclib::toString( diff_time_to ) ).append( "]" )
				;
		}

		void setRollingMonth() override
		{
			TimeMsg::Query< K >::setRollingMonth();
			auto now = svclib::getTime();
			diff_time_from = now - 30 * USEC_IN_DAY;
			diff_time_to = now + USEC_IN_SEC;
		}
	};
};

#endif
