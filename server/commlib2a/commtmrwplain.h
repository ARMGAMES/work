#ifndef commtmrwplain_h_included
#define commtmrwplain_h_included

template< class _MultiSocket >
class _CommRWPlain
{
private:
//	_CommCompleteCompressedRWCommunicator commBase;
	_CommCompleteCommunicator< _CommCompressedCommunicator< _CommRWCommunicator<_MultiSocket> > > commBase;
	_CommStream0ProtocolHandler< _CommCompleteCommunicator<_CommCompressedCommunicator< _CommRWCommunicator<_MultiSocket>>>, _CommRWCommunicator<_MultiSocket> > handler;

public:
	static const char* stringId() { return "TCP"; }
	void construct( _MultiSocket& rw, int slot, CommRWThreadManagerBase& mgr, _CommProtocolLog * logger_  )
	{
		commBase.construct( rw, slot, mgr.routingTable );
		handler.construct( commBase, commBase , logger_ );
#ifndef COMM_CLIENT_ONLY
		handler.setMinimumProtocol(mgr.minimumProtocol);
		handler.setParametersEnforced(mgr.minEnforcedBlock,mgr.maxEnforcedBlock,mgr.randomProtectionEnforced);
#endif
		handler.setDefaultProtocol(mgr.DefaultProtocol);
		handler.setDefaultFrameSize(mgr.MaxBlockSize);
		handler.setDefaultLargeFrameSize(mgr.MaxBlockSizeVer5);
		handler.setDefaultCompressSize(mgr.CompressBitsSize);
#ifndef COMM_CLIENT_ONLY
		handler.setMaximumMessageSize(mgr.maximumMessageSize,mgr.maxMessageSizeDebug);
		handler.setMaximumMessageRate(mgr.maximumMessageRate,mgr.maximumMessageRatePeriod,mgr.maxMessageRateDebug);
		if (mgr.enableServerEncryption)
		{
			handler.setServerEncryption(mgr.publicKey,mgr.privateKey);
		}
        if (mgr.getNetworkStatsLogThreshold())
        {
            handler.setNetworkStatsLogThreshold(mgr.getNetworkStatsLogThreshold());
        }
#endif
		handler.setClientEncryption(mgr.enableClientEncryption);
	}
	void enableMux()
	{
		handler.enableMux();
	}
	void setKeepAlive( int msecPoll, int msecWrite, int msecRead, bool keepAliveIgnoreDataPackets )
	{
		handler.setKeepAlive( msecPoll, msecWrite, msecRead, keepAliveIgnoreDataPackets );
	}
	void setDefaultFrameSize(UINT32 FrameSize)
	{
		handler.setDefaultFrameSize(FrameSize);
	}
	void setDefaultLargeFrameSize(UINT32 FrameSize)
	{
		handler.setDefaultLargeFrameSize(FrameSize);
	}
	void setDefaultCompressSize(UINT32 CompressBitsSize)
	{
		handler.setDefaultCompressSize(CompressBitsSize);
	}
	void setDefaultProtocol(UINT32 DefaultProtocol)
	{
		handler.setDefaultProtocol(DefaultProtocol);
	}
#ifndef COMM_CLIENT_ONLY
	void setMinimumProtocol(UINT32 minimumProtocol)
	{
		handler.setMinimumProtocol(minimumProtocol);
	}
	void setParametersEnforced(UINT32 minEnforcedBlock,UINT32 maxEnforcedBlock,UINT32 randomProtectionEnforced)
	{
		handler.setParametersEnforced(minEnforcedBlock,maxEnforcedBlock, randomProtectionEnforced);
	}
	void setMaximumMessageSize(UINT32 maximumMessageSize, bool maxMessageSizeDebug)
	{
		handler.setMaximumMessageSize(maximumMessageSize,maxMessageSizeDebug);
	}
	void setMaximumMessageRate(UINT32 maximumMessageRate, UINT32 maximumMessageRatePeriod, bool maxMessageRateDebug)
	{
		handler.setMaximumMessageRate(maximumMessageRate,maximumMessageRatePeriod,maxMessageRateDebug);
	}
	void setServerEncryption(UINT32 * publicKey, UINT32 * privateKey)
	{
		handler.setServerEncryption(publicKey,privateKey);
	}
#endif
	void setClientEncryption(bool enable)
	{
		handler.setClientEncryption(enable);
	}

	void _log (const char * fmt, ...)
	{
		va_list args;
		va_start( args, fmt );
		handler._vlog(fmt, args);
		va_end(args);
	}

	void init( _CommPhysicalConnection* physConn )
	{
		commBase._CommRWCommunicator<_MultiSocket>::init( physConn );
		commBase._CommCompressedCommunicator< _CommRWCommunicator<_MultiSocket> >::init();
		commBase.init();//init CompleteRWCommunicator
		handler.init( physConn->inbound );
	}
	void deinit()
	{
		handler.deinit();
	}

	bool isConnected() const
	{
		return handler.isConnected();
	}

	void forceSlotFailure( const char* errMsg, int errCode = 0 )
	{
		handler._log(errMsg);
		commBase.forceSlotFailure( errMsg, errCode );
	}
	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		return handler.serviceCallTransport(id,msg_body );
	}

	void writeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		handler.writeMsg( mux, muxChannelId, msg );
	}
	void asyncWriteOk()
	{
		handler.asyncWriteOk();
	}
	void asyncReadOk()
	{
		handler.asyncReadOk();
	}
	void setFirstRead()
	{
		handler.setFirstRead();
	}
	void storeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		handler.storeMsg( mux, muxChannelId, msg );
	}
	void flushStoredMsgs()
	{
		handler.flushStoredMsgs();
	}


};

#endif