/******************************************************************************
**
** Source File Name: sqlqg_fenced_server.h
**
** (C) COPYRIGHT International Business Machines Corp. 1987, 2003
** All Rights Reserved
** Licensed Materials - Property of IBM
**
** US Government Users Restricted Rights - Use, duplication or
** disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
** Function = Include File defining:
**              Query Gateway fenced-side interfaces for
**              server objects.
**
** Operating System: All
**
*******************************************************************************/
#ifndef SQL_H_SQLQG_FENCED_SERVER
#define SQL_H_SQLQG_FENCED_SERVER

#include "sqlqg_server.h"
#include "sqlxa.h"

// Forward declarations.
class FencedWrapper;
class FencedRemote_User;
class Remote_Connection;
class sqlqg_FMP_Request;
class sqlqg_FMP_Reply;
struct sqlo_xlatch;
class FencedNickname;
class Nickname_Info;
class User_Info;
class Sqlqg_Array;
////////////////////////////////////////////////////////////////////////////////
// FencedServer class
////////////////////////////////////////////////////////////////////////////////
class FencedServer : public Server {
public:

  // Constructors
  FencedServer (sqluint8*       a_server_name,
                server_kind     a_server_kind,
                FencedWrapper*  a_server_wrapper,
                sqlint32*       a_rc);

  virtual sqlint32 initialize_server (Server_Info*  a_server_info,
                                      sqlint32      a_init_subclass=TRUE);

  // responsible for fetching remote datasource dbcfg attributes
  // it will be called when in 'slqqgRemoteServerInit' 
  sqlint32 verify_register_server_info(Server_Info * a_server_info, 
					       Server_Info ** delta_info);

  virtual sqlint32 commit (void);
  virtual sqlint32 rollback (void);

  virtual Sqlqg_Return_Code get_nickname_list(
                            const char *a_remote_auth_id,
                            const char *a_remote_password,
                            Sqlqg_Array *a_nickname_list);

  sqlint32 drop_remote_user (Remote_User* a_user,
                             Sqlqg_DeleteType a_flag = SQLQG_DELETE_ALL);

  sqlint32 connect_current_user (Remote_Connection** a_connection);

  Sqlqg_Return_Code get_outbound_conn_originator(sqluint8** a_outbound_orig_userID); 

  sqlint32 find_connection (Remote_Connection** a_connection);

  sqlint32 connect (FencedRemote_User*   a_user,
                    Remote_Connection**  a_connection,
                    bool                 a_switch_user = false);

  virtual Sqlqg_Return_Code switch_connected_user(FencedRemote_User*   a_new_user, 
                                                  Remote_Connection**  a_connection);

  Sqlqg_Return_Code switch_user_attributes(FencedRemote_User* a_old_user, 
                                           Remote_Connection* a_connection);

  sqlint32 disconnect (Remote_Connection* a_connection);

  sqlint32 update_unref_counts (void);

  sqlint32 isExternalDataSource (void);

  sqlqg_FMP_Reply* getReply (void);

  void setReply (sqlqg_FMP_Reply* a_reply);

  friend sqlint32 sqlqg_FMP_FindServer (sqluint8*          a_server_name,
                                        sqlqg_FMP_Request* a_req,
                                        FencedServer**     a_server);

  sqlint32 reset_unreferenced_count (const sqlint32 a_remote_operation_rc);

  void has_connected (void);
  void has_disconnected (void);

  sqluint8 get_mon_connect (void);
  sqluint8 get_mon_disconnect (void);
  sqluint32 get_inbound_connect_trust_type (void) const;

  sqlint32 block_one_req_iud (void);

  void Latch (void);

  void UnLatch (void);

  virtual const char *get_licensing_data_source_name(void) const;

  virtual const char *get_licensing_server_name(void) const;

  void set_rmid (sqluint32 a_rmid); // Functions for setting and
  sqluint32 get_rmid (void) const;  // retrieving the RM ID

  void set_xid (SQLXA_XID a_xid);
  const SQLXA_XID* get_xid (void) const;
  void reset_xid (void);

protected:

  //////////////
  // Data     //
  //////////////
  struct sqlo_xlatch* xlatchP;

  sqlqg_FMP_Reply*    m_reply;

  FencedRemote_User*  connected_user;

  Hash_Table          connections;

  Remote_Connection*  mru_connection;

  // Bitmask indicating if an inbound trusted connection has been requested
  // and/or established. Please see the values in sqlrr_trusted_contexts.h
  // SQLRR_xxxx_TRUSTED_CONNECTION_XXXX
  sqluint32  m_inbound_connect_trust_type; 

  sqluint8 mon_connect;    // flag to indicate whether server has connected
  sqluint8 mon_disconnect; // flag to indicate whether server has disconnected

  sqluint32 m_rmid;
  SQLXA_XID m_xid;

  //////////////
  // Methods  //
  //////////////
  // Destructor.
  // Do not destroy directly; use wrapper->drop_server ()
  virtual ~FencedServer (void);

  virtual sqlint32 create_remote_connection (FencedRemote_User*   a_user,
                                             Remote_Connection**  a_connection);

  // get remote server attributes.
  virtual sqlint32 verify_my_register_server_info(Server_Info  * a_server_info, 
						  Server_Info  ** delta_info);
  
  // Methods to save and restore the pConHandle and pAppData ptrs from original
  // trusted connection. Only needed for case where trusted drda wrapper fails
  // to get an explicit trusted connection and a switch-user is equivalent to a 
  // disconnect followed by a reconnect using the original ConHandle
  virtual void save_orig_conn_attrs();
  virtual void restore_orig_conn_attrs() const;
};

// Inline methods

// save_orig_conn_attrs and restore_orig_conn_attrs are no-ops except for wrappers
// that override these methods
inline void FencedServer::save_orig_conn_attrs() {}
inline void FencedServer::restore_orig_conn_attrs() const {}

inline sqlqg_FMP_Reply*
FencedServer::getReply (void)
{
  return m_reply;
}

inline void
FencedServer::setReply (sqlqg_FMP_Reply* a_reply)
{
  m_reply = a_reply;
}

inline void
FencedServer::has_connected (void)
{
  mon_connect = TRUE;
}

inline void
FencedServer::has_disconnected (void)
{
  mon_disconnect = TRUE;
}

inline sqluint32
FencedServer::get_inbound_connect_trust_type(void) const
{
  return m_inbound_connect_trust_type;
}

inline void
FencedServer::set_rmid (sqluint32 a_rmid)
{
  m_rmid = a_rmid;
}

inline sqluint32
FencedServer::get_rmid(void) const
{
  return m_rmid;
}

inline void
FencedServer::set_xid(SQLXA_XID a_xid)
{
  XIDCPY(m_xid, a_xid);
}

inline const SQLXA_XID*
FencedServer::get_xid(void) const
{
  return (&m_xid);
}

inline void
FencedServer::reset_xid(void)
{
  SQLXA_NULLXID(m_xid);
}
#endif
