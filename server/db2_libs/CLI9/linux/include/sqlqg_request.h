/******************************************************************************
**
** Source File Name: sqlqg_request.h
**
** (C) COPYRIGHT International Business Machines Corp. 1987, 2006
** All Rights Reserved
** Licensed Materials - Property of IBM
**
** US Government Users Restricted Rights - Use, duplication or
** disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
**
*******************************************************************************/
#ifndef SQL_H_SQLQG_REQUEST
#define SQL_H_SQLQG_REQUEST

#define _GENERIC_SERVER_DEBUG_

/**
   \file sqlqg_request.h

   \brief Types, classes, methods and functions for query planning

   This file defines the types, classes, methods and functions that
   are used by wrappers during query planning time

   \author Jons-Tobias Wamhoff
*/

#include "sql.h" 
#include "sqlqg_misc.h"

// sqlqg
class Nickname;
class Unfenced_Generic_Nickname;   
class Unfenced_Generic_Server;

// request
class Parsed_Query_Fragment;
class Request_Exp;
class Request_Constant;
class Request;
class Request_Exp_Type;

// other
struct sqlno_globals;
struct sqlno_qtb;
struct sqlno_iset;
struct sqlno_qgmP_array;
struct sqlno_apcb;
union  sqlno_set;
class  sqlnq_pid;

/**
   \class Request_Exp
   \brief Represents node in an expression tree

   The Request_Exp class represents a node in an expression tree.
   This node can be a column reference, a constant value, a host
   parameter, or an operator.

   \note This class is never instantiated by the wrapper.
*/
class Request_Exp : public Sqlqg_Base_Class {
public:
  /**
    \enum Request_Exp::kind
    \brief Kind of Request_Exp node
  */
  enum kind
  {
    badkind,  /**< Invalid node */
    column,   /**< Node represents a column reference */
    unbound,  /**< Node represents a host parameter */
    constant, /**< Node represents a constant value */
    oper      /**< Node represents an operator and has child Request_Exp nodes */
  };

  //**** start of the interface used by the wrapper writer ****

  /**
    \brief Retrieve kind of a Request_Exp node

    \warning This function is deprecated.

    \param a_request [in]
      Request object the Request_Exp tree belongs to. Can be null.

    \param a_kind [out]
      Request_Exp node kind

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_kind (Request*   a_request,
                                     kind*      a_kind);

  /**
    \brief Retrieve Kind of a Request_Exp node

    \retval Request_Exp node kind
  */
  kind                     get_kind (void);

  /**
    \brief Retrieve data type
    
      Returns a structure to describe the data type associated with
      the Request_Exp.

    \param a_type [out]
      Pointer to the type of descriptor

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_data_type (Request_Exp_Type** a_type);

  /**
    \brief Retrieve parent Request_Exp
    
      Returns a pointer to the parent Request_Exp node of the current node.

    \return Pointer to the parent node. The value is null if this is a top-level node.
  */
  Request_Exp*             get_parent (void);

  /**
    \brief Retrieve sibling Request_Exp
    
      Returns a pointer to the next child of the same parent
      (that is, the next sibling of the current node).

    \return Pointer to the next child. The value is null if there are no more siblings.
  */
  Request_Exp*             get_next_child (void);

  /**
    \brief Retrieve global handle
    
      Given a Request_Exp node returns a integer handle
      identifying the expression. These handles can be used to find
      the position of the parameter in the array returned by
      Reply::get_parameter_order, etc.

    \param a_handle [out]
      Integer handle identifying the expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_handle (int* a_handle);

  /**
    \brief Retrieve PID pointer
    
      Returns a pointer to the original PID node the current
      Request_Exp node was extracted from.

    \return Pointer to PID node
  */
  sqlnq_pid*               get_pid (void);
  
  /**
    \brief Retrieve column name
    
      Returns a name of a column node in a Request_Exp.

    \warning Valid only for nodes of kind Request_Exp::column

    \param a_col_name [out]
      Pointer to the column name (not null-terminated)
    
    \param a_name_length [out]
      Length of the column name

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_column_name (char** a_col_name,
                                            int*   a_name_length);

  /**
    \brief Retrieve column global quantifier handle
    
      Returns an integer handle representing the column quantifier
      (the table or table expression).

    \warning Valid only for nodes of kind Request_Exp::column

    \param a_quant_handle [out]
      Integer handle number (that is the global index of the qun)

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_column_quantifier_handle (int* a_quant_handle);

  /**
    \brief Retrieve constant value
    
      Returns the value of a constant node in a Request_Exp tree.

    \warning Valid only for nodes of kind Request_Exp::constant

    \param a_constant_value [out]
      Pointer to Request_Constant structure - same interface as Runtime_Data

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_value (Request_Constant** a_constant_value);
  
  /**
    \brief Retrieve operator first child Request_Exp
    
      Returns a pointer to the first child of an operator Request_Exp node.

    \warning Valid only for nodes of kind Request_Exp::oper

    \return Pointer to the first child. The value is null if there are no children.
  */
  Request_Exp*             get_first_child (void);

  /**
    \brief Retrieve operator number of children
    
      Returns the number of children of an operator Request_Exp node.

    \warning Valid only for nodes of kind Request_Exp::oper

    \return Number of children
  */
  int                      get_number_of_children (void);

  /**
    \brief Retrieve operator token value
    
      Returns the token value (i.e. the function name) for oper nodes
      in a Request_Exp node.

    \warning Valid only for nodes of kind Request_Exp::oper

    \param a_operator_token [out]
      Pointer to the token (not null-terminated)
    
    \param a_token_length [out]
      Length of the token

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_token (char**  a_operator_token,
                                      int*    a_token_length);

  /**
    \brief Retrieve operator signature
    
      Returns a full function signature for an operator Request_Exp node.

    \warning Valid only for nodes of kind Request_Exp::oper

    \param a_signature [out]
      Pointer to the null-terminated signature

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_signature (char** a_signature);

  //**** end of the interface used by the wrapper writer ****

  /**
    \brief Create and return a new Request_Exp tree
    
      Given a root PID tree node a Request_Exp tree is created with
      valid values for its data member.

    \param a_pid [in]
      Pointer to a root PID tree node

    \param a_result_rexp [out]
      Pointer to root Request_Exp node of the newly created tree

    \param a_qun_set [in]
      Optional set of quantifiers used to distinguish unbound and column kinds

    \param a_parent_rexp [in]
      Optional pointer to parent Request_Exp node of currently processed node.
      Do not specify this parameter, used internally for recursive calls only.

    \param a_like_pattern [in]
      Optional flag to tell that the parent is a like pattern. 

    \param a_num_sibling [in]
      optional parameter to tell the number of actual sibling for this node.

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code create_request_exp_tree (sqlnq_pid*    a_pid,
                                                    Request_Exp** a_result_rexp,
                                                    sqlno_set*    a_qun_set     = NULL,
                                                    Request_Exp*  a_parent_rexp = NULL,
                                                    bool          a_like_pattern = FALSE,
                                                    sqlint32      a_num_sibling = 1);

  /**
    \brief Copy and return a Request_Exp tree
    
      Starting from the current node this function creates and returns
      a copy of the node and all its children (subtree).

    \param a_result_rexp [out]
      Pointer to root Request_Exp node of the newly created copy of the tree

    \param a_pool [in]
      Optional pointer to a pool to allocate the memory from

    \param a_parent_rexp [in]
      Optional pointer to parent Request_Exp node of currently processed node.
      Do not specify this parameter, used internally for recursive calls only.

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        copy_request_exp_tree (Request_Exp** a_result_rexp,
                                                  sqlno_apcb*   a_pool            = NULL,
                                                  Request_Exp*  a_parent_rexp     = NULL);

protected:
  /** \brief Pointer to original node in pid tree */
  sqlnq_pid*   m_pid;

  /** \brief Kind of this node */
  kind         m_kind;

  /** \brief Pointer to parent node */
  Request_Exp* m_parent;

  /** \brief Pointer to child node */
  Request_Exp* m_child;

  /** \brief Pointer to sibling node */
  Request_Exp* m_next_child;

private:
  /**
    \brief Constructor
    
      Construct a root Request_Exp node.

    \param a_pid [in]
      Pointer to original node in pid tree

    \param a_kind [in]
      Kind of this node
  */
  Request_Exp (sqlnq_pid*   a_pid,
               kind         a_kind);

  /**
    \brief Constructor
    
      Construct a child Request_Exp node.

    \param a_pid [in]
      Pointer to original node in pid tree

    \param a_kind [in]
      Kind of this node

    \param a_parent [in]
      Pointer to parent node
  */
  Request_Exp (sqlnq_pid*   a_pid,
               kind         a_kind,
               Request_Exp* a_parent);

  /**
    \brief Constructor
    
      Not implemented.
  */
  Request_Exp (void);

  /**
    \brief Destructor
    
      Destruct a subtree beginning with the current node as root.
  */
  ~Request_Exp (void);

  /**
    \brief Operator new

      Allocation in the optimizer's memory pools. Structures allocated by
      this operator do not need to be explicitly deallocated.
      This operator accepts an optional pointer to a memory pool to allow
      allocating Request_Exp trees in optimizer memory.

    \param a_size [in]
      Size of requested memory block

    \param a_rc [out]
      Return code (0 indicates Success; SQLQG_ERROR for Error)

    \param a_poolP [in]
      Optional pointer to memory pool. If NULL use QG pool.

    \return Pointer to new memory block
  */
  static void*              operator new (size_t             a_size,
                                          Sqlqg_Return_Code* a_rc,
                                          sqlno_apcb*        a_poolP = NULL);

  /**
    \brief Create and return a new Request_Exp tree
    
      Given a root PID tree node a Request_Exp tree is created with
      valid values for its data member.

    \param a_pid [in]
      Pointer to a PID tree node.

    \param a_qun_set [in]
      Set of quantifiers used to distinguish unbound and column kinds.
      A NULL pointer is accepted resulting in no distinction between bound and
      unbound columns.
    
    \param a_result_kind [out]
      Determinded kind for given PID node.

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code find_request_exp_kind (sqlnq_pid* a_pid,
                                        sqlno_set* a_qun_set,
                              kind*      a_result_kind);

  friend class Request;
};

/**
   \class Parsed_Query_Fragment
   \brief Encapsulation of a query fragment

     The virtual Parsed_Query_Fragment class the the parent of classes
     Request and Reply.

   \note This class is never instantiated by the wrapper.
*/
class Parsed_Query_Fragment : public Sqlqg_Base_Class {
public:
  /**
    \brief Constructor
    
      Sets query to no distinct.
  */
  Parsed_Query_Fragment (void);
  
  /**
    \brief Retrieve number of quantifiers
    
      Retrieve the number of quantifiers (nicknames and table expressions) for this query.

    \return Number of quantifiers
  */
  int                       get_number_of_quantifiers (void);

  /**
    \brief Retrieve number of predicates
    
      Retrieve the number of predicates for this query.

    \return Number of predicates
  */
  int                       get_number_of_predicates (void);

  /**
    \brief Retrieve number of head expressions
    
      Retrieve the number of head (select list) expressions for this query.

    \return Number of head expressions
  */
  int                       get_number_of_head_exp (void);

  /**
    \brief Retrieve number of GROUP BY expressions
    
      Retrieve the number of GROUP BY expressions for this query.

    \return Number of GROUP BY expressions
  */
  int                       get_number_of_gb_exp (void);

  /**
    \brief Retrieve number of global predicates
    
      Retrieve the number of global predicates for this query.

    \return Number of global predicates
  */
  int                       get_number_of_global_predicates(void); 

  /**
    \brief Retrieve quantifier handle
    
      Returns the handle for the nth quantifier for this query.

    \param a_quant_pos [in]
      Position of the handle in list (starting at 1)

    \param a_quant_handle [out]
      Pointer to integer handle for quantifier

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_quantifier_handle (int  a_quant_pos, 
                                                   int* a_quant_handle);

  /**
    \brief Retrieve predicate handle
    
      Returns the handle for the nth predicate for this query.

    \param a_pred_pos [in]
      Position of the handle in list (starting at 1)

    \param a_pred_handle [out]
      Pointer to integer handle for predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_predicate_handle (int  a_pred_pos,
                                                  int* a_pred_handle);

  /**
    \brief Retrieve global predicate handle
    
      Returns the handle for the nth global predicate for this query.

    \param a_gb_pred_pos [in]
      Position of the handle in list (starting at 1)

    \param a_gb_pred_handle [out]
      Pointer to integer handle for global predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_global_predicate_handle (int  a_gb_pred_pos,
                                                         int* a_gb_pred_handle);

  /**
    \brief Retrieve head expression handle
    
      Returns the handle for the nth head expression for this query.

    \param a_head_exp_pos [in]
      Position of the handle in list (starting at 1)

    \param a_head_exp_handle [out]
      Pointer to integer handle for head expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_head_exp_handle (int  a_head_exp_pos,
                                                 int* a_head_exp_handle);

  /**
    \brief Retrieve GROUP BY expression handle
    
      Returns the handle for the nth GROUP BY expression for this query.

    \param a_gb_exp_pos [in]
      Position of the handle in list (starting at 1)

    \param a_gb_exp_handle [out]
      Pointer to integer handle for GROUP BY expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_gb_exp_handle (int  a_gb_exp_pos,
                                               int* a_gb_exp_handle);

  /**
    \brief Retrieve nickname object
    
      Returns a nickname object for a quantifier at a given position in the Parsed_Query_Fragment.

    \param a_quant_handle [in]
      Integer handle for the nickname

    \param a_nickname [out]
      Pointer to the Unfenced_Generic_Nickname object

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code  get_nickname (int                         a_quant_handle,
                                          Unfenced_Generic_Nickname** a_nickname);

  /**
    \brief Retrieve head expression
    
      Returns a Request_Exp object that represents the head expression which
      is associated with the specified handle.

    \param a_head_exp_handle [in]
      Integer handle for the head expression

    \param a_head_exp [out]
      Pointer to the Request_Exp object representing the head expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code get_head_exp (int           a_head_exp_handle,
                                          Request_Exp** a_head_exp);

  /**
    \brief Retrieve predicate
    
      Returns a Request_Exp object that represents the predicate which
      is associated with the specified handle.

    \param a_pred_handle [in]
      Integer handle for the predicate

    \param a_pred_exp [out]
      Pointer to the Request_Exp object representing the predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code get_predicate (int           a_pred_handle,
                                           Request_Exp** a_pred_exp);

  /**
    \brief Retrieve table function expression
    
      Returns a Request_Exp object that represents the table function expression which
      is associated with the specified handle.

    \param a_quant_handle [in]
      Integer handle for the quantifier

    \param a_table_exp [out]
      Pointer to the Request_Exp object representing the table function expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code get_table_function_exp (int           a_quant_handle,
                                                    Request_Exp** a_table_exp);

  /**
    \brief Set the DISTINCT flag for a query

    \param a_distinct [in]
      Integer distinct flag (0 for no distinct)
  */
  void                      set_distinct (int a_distinct);

  /**
    \brief Retrieve the DISTINCT flag for a query

    \return Integer distinct flag (0 for no distinct)
  */
  int                       get_distinct (void);

  /**
    \brief Retrieve flag if PID node was generated

    \param a_hexp_pos [in]
      Position in m_pid_array
      
    \retval 1 for true
    \retval 0 for false
  */
  int                       is_generated_pid (int a_hexp_pos);
   
protected:
  /** \brief Pointer to set of predicates of query clause */
  sqlno_iset*         m_preds;

  /** \brief Pointer to set of GROUP BY expressions of query clause */
  sqlno_iset*         m_gb_exps;

  /** \brief Pointer to set of head expressions of query clause */
  sqlno_iset*         m_hexps;

  /** \brief Pointer to set of quantifiers of query clause */
  sqlno_iset*         m_quns;

  /** \brief Pointer to set of global predicates of query clause */
  sqlno_iset*         m_global_preds;

  /** \brief Pointer to array of pointers to PID nodes */
  class sqlnq_pid**   m_pid_array;

  /** \brief Count of pointers to PID nodes in m_pid_array */
  int                 m_pid_array_size;

  /** \brief Indicates if this is a distinct query */
  int                 m_distinct;

  /** \brief Pointer to optqtb pointer */
  sqlno_qtb*          m_optqtbP;

  /** \brief Pointer to array of Request_Exp trees representing predicates. 
             Array uses same index as corresponding entry in m_preds */
  class Request_Exp** m_pred_rexps;

  /** \brief Count of Request_Exp trees in m_pred_rexps. 
             Does not reflect count of entries in m_pred_rexps, equals number of m_preds */
  int                 m_pred_rexps_size;

  /** \brief Pointer to array of Request_Exp trees representing head expressions. 
             Array uses same index as corresponding entry in m_hexps */
  class Request_Exp** m_head_exp_rexps;

  /** \brief Count of Request_Exp trees in m_head_exp_rexps. 
             Does not reflect count of entries in m_head_exp_rexps, equals number of m_hexps */
  int                 m_head_exp_rexps_size;

  /** \brief Pointer to array of Request_Exp trees representing table funciton expressions. 
             Array uses same index as corresponding entry in m_quns */
  class Request_Exp** m_tab_func_rexps;

  /** \brief Count of Request_Exp trees in m_tab_func_rexps. 
             Does not reflect count of entries in m_tab_func_rexps, equals number of m_quns */
  int                 m_tab_func_rexps_size;
  
  /**
    \brief Add integer element to a set
    
      Adds an integer element to the end of a set represented by an integer array.

    \param a_set [in]
      Set the element is to be added to

    \param a_element [in]
      Integer element that is to be added to the set

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code  add_to_set (sqlno_iset* a_set,
                                        int         a_element);

  /**
    \brief Get integer element from a set
    
      Gets an integer elment (index) from a set represented by an integer array.

    \param a_set [in]
      Set the element is to be gathered from

    \param a_index [in]
      Index of the element in the set, starts at 1

    \param a_element [out]
      Integer element from the set

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code  get_set_element (sqlno_iset* a_set,
                                             int         a_index,
                                             int*        a_element);

  /**
    \brief Get integer element from a set
    
      Gets an integer elment (index) from a set represented by an integer array.

    \param a_set [in]
      Set the element is to be gathered from

    \param a_element [in]
      Integer element to be found in the set

    \param a_index [out]
      Index of the element in the set, starts at 0, -1 indicates not found

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code  get_set_element_pos (sqlno_iset* a_set,
                                                 int      a_element,
                                                 int*     a_index);

  /**
    \brief Copy a set
    
      memcpy an iset.

    \warning Both isets must be already allocated

    \param a_target [out]
      Set a_source will be copied to

    \param a_source [in]
      Set that to be copied to a_target
  */
  static void               copy_iset (sqlno_iset* a_target,
                                       sqlno_iset* a_source);
  
  friend class Predicate_List;
};

/**
   \class Request
   \brief Encapsulatiuon of a query fragment

   The Request class encapsulates a query fragment to be analyzed
   and processed by the wrapper.

   \note This class is never instantiated by the wrapper.
*/
class Request : public Parsed_Query_Fragment  {
public:
  /**
    \brief Constructor
    
      Constructs a request from local optmizer sets. All Request_Exp trees that
      are needed to represent predicates, head eaxpressions and table
      function expressions are created at this time.

    \param a_optqtbP [in]
      Pointer to optqtb

    \param a_quns [in]
      Optional pointer to set of quantifier handles

    \param a_cols [in]
      Optional pointer to set of columns handles

    \param a_preds [in]
      Optional pointer to set of quantifier handles

    \param a_pred_context [in]
      Optional pointer to set of predicate contexts
  */
  Request (sqlno_qtb* a_optqtbP,
           sqlno_set* a_quns         = NULL,
           sqlno_set* a_cols         = NULL,
           sqlno_set* a_preds        = NULL,
           sqlno_set* a_pred_context = NULL);

  /**
    \brief Destructor
    
      Deletes all Request_Exp trees that belong to this query and m_quns_bit_set.
  */
  virtual ~Request (void);
  
protected:
  /** \brief Pointer to quantifier set, used for unbound column determination */
  sqlno_set*          m_quns_bit_set;

  /**
    \brief Constructor
    
      Not implemented.
  */
  Request(void);

  /**
    \brief Generate PID nodes for multiple QNCs

    \param a_global_index [in]
      Integer handle for head expression

    \param a_pid [out]
      Pointer to generated PID node

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        make_pid (int         a_global_index,
                                     sqlnq_pid** a_pid);

  friend class Reply;
  friend class Predicate_List;
};

/**
   \class Reply
   \brief Represents portion of a query that a wrapper can process

   The Reply class represents a portion of a query that the wrapper
   can process. This class can be subclassed if the wrapper uses a
   cost model other than the default model.

   \note This class is instantiated by the create_reply() method of
   the Request Unfenced_Generic_Server class. If the wrapper implements
   a subclass of the Reply class, the create_reply() method is
   overridden in the wrapper-specific subclass of Unfenced_Generic_Server.
*/
class Reply : public Parsed_Query_Fragment {
public:
  /**
    \enum Reply::order_direction
    \brief Direction of an order
  */
  enum order_direction
  {
    ASC, /**< Ascending order */
    DSC  /**< Descending order */
  };
  
  //**** start of the interface used by the wrapper writer ****

  /**
    \brief Operator new

      Allocation in the optimizer's memory pools. Structures allocated by
      this operator do not need to be explicitly deallocated.
      Called from create_empty_reply.

    \param a_size [in]
      Size of requested memory block

    \param a_rc [out]
      Return code (0 indicates Success; SQLQG_ERROR for Error)

    \param a_poolP [in]
      Optional pointer to memory pool

    \return Pointer to new memory block
  */
  static void*              operator new (size_t             a_size,
                                          Sqlqg_Return_Code* a_rc,
                                          sqlno_apcb*        a_poolP = NULL);

  // create a Reply of the same size as the Request
  /**
    \brief Constructor

      Create a Reply of the same size as the Request.

    \param a_rq [in]
      Pointer to Request from which this reply is derived

    \param a_srv [in]
      Pointer to server for a request or reply protocol

    \param a_rc [out]
      Return code (0 indicates Success; SQLQG_ERROR for Error)

    \param a_poolP [in]
      Optional pointer to memory pool. Used only by the engine.
  */
  Reply (Request*                 a_rq,
         Unfenced_Generic_Server* a_srv,
         Sqlqg_Return_Code*       a_rc,
         sqlno_apcb*              a_poolP = NULL);

  /**
    \brief Destructor
    
      Not implemented.
  */
  virtual ~Reply(void);

  /**
    \brief Add head expression

      Add a head expression handle and its corresponding Request_Exp to the Reply.

    \param a_head_exp_handle [in]
      Integer handle for the head expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         add_head_exp (int a_head_exp_handle);

  /**
    \brief Add predicate

      Add a predicate handle and its corresponding Request_Exp to the Reply.

    \param a_pred_handle [in]
      Integer handle for the predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         add_predicate (int a_pred_handle);

  /**
    \brief Add quantifier

      Add a quantifier handle and its corresponding Request_Exp to the Reply.

    \param a_quant_handle [in]
      Integer handle for the quantifier

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         add_quantifier (int a_quant_handle);

  /**
    \brief Add GROUP BY expression

      Add a GROUP BY expression to the Reply.

    \param a_gb_exp_handle [in]
      Integer handle for the GROUP BY expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         add_gb_exp (int a_gb_exp_handle);

  /**
    \brief Add order by expression

      Add an ORDER BY specification to the reply.

    \param a_gindex [in]
      Head expression index for ordering (not a handle)

    \param a_direction [in]
      Direction (ascending or descending) for ordering

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         add_order_entry (int                    a_gindex,
                                             Reply::order_direction a_direction);

  /**
    \brief Retrieve number of order entries

      Retrieve the number of ORDER BY entries for the reply.

    \return Number of ORDER BY entries
  */
  int                       get_number_of_order_entries (void);

  /**
    \brief Retrieve a specific ORDER BY entry

    \param a_pos [in]
      Position of order entry (starting at 1)

    \param a_gindexP [out]
      Head expression index for the ORDER BY expression

    \param a_direction [out]
      Direction (ascending or descending) for ordering

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_order_entry (int                     a_pos,
                                             int*                    a_gindexP,
                                             Reply::order_direction* a_direction);

  /**
    \brief Retrieve the execution descriptor

      Returns the execution descriptor that is associated with the Reply.

    \param a_exec_desc [out]
      Pointer to the execution descriptor

    \param a_exec_desc_size [out]
      Size of the execution descriptor
  */
  void                      get_exec_desc (void** a_exec_desc,
                                           int*   a_exec_desc_size);

  /**
    \brief Store an execution descriptor in the reply.

      Storage for the execution descriptor must be allocated with
      Wrapper_Utillities::allocate.

    \param a_exec_desc [in]
      Pointer to the execution descriptor

    \param a_exec_desc_size [in]
      Size of the execution descriptor
  */
  void                      set_exec_desc (void* a_exec_desc,
                                           int   a_exec_desc_size);

  /**
    \brief Retrieve m_no_block token
    
    \return Value of m_no_block
  */
  short                     get_no_block(void);

  /**
    \brief Set Reply::m_no_block token to 1
  */
  void                      set_no_block(void);
  
  /**
    \brief Retrieve a list of parameter handles

      Returns the ordering of the parameters in a form of integer handle
      array. A handle for a parameter can be obtained by get_parameter_handle.
      The order of the list corresponds to the order of the parameter values
      in the Remote_Operation object.

    \param a_number_of_params [out]
      Pointer to number of the parameter handles

    \param a_param_handle_array [out]
      Pointer to array of the parameter handles

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         get_parameter_order (int*  a_number_of_params,
                                                 int** a_param_handle_array);
  
  /**
    \brief Retrieve the cardinality for the query fragment

      Virtual function, can be overriden by the wrapper writer.

    \param a_cardinality [out]
      Cardinality

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code cardinality (float* a_cardinality);

  /**
    \brief Retrieve the total cost for a query fragment

      Virtual function, can be overriden by the wrapper writer.

    \param a_total_cost [out]
      Total cost

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code total_cost (float* a_total_cost);

  /**
    \brief Retrieve cost to run a query fragment again

      Virtual function, can be overriden by the wrapper writer.

    \param a_re_exec_cost [out]
      Re-execution cost

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code re_exec_cost (float* a_re_exec_cost);

  /**
    \brief Retrieve the cost to fetch the first tuple

      Virtual function, can be overriden by the wrapper writer.

    \param a_first_tuple_cost [out]
      Cost to fetch first tuple

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code first_tuple_cost (float* a_first_tuple_cost);

  /**
    \brief Retrieve all costing values at one time

      Virtual function, can be overriden by the wrapper writer.

    \param a_total_cost [out]
      Total cost

    \param a_first_tuple_cost [out]
      Cost to fetch first tuple

    \param a_re_exec_cost [out]
      Re-execution cost

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  virtual Sqlqg_Return_Code all_costs (float* a_total_cost,
                                       float* a_first_tuple_cost,
                                       float* a_re_exec_cost);

  /**
    \brief Retrieve next Reply

      Retrieve a pointer to the next reply in a chain of replies.
    
    \return Pointer to the next reply. The value is null if at the end of the chain.
  */
  Reply*                    next (void);

  /**
    \brief Link a new reply to the current reply

      The wrapper verifies that it adds the reply to the end of the chain.
      Otherwise, memory leaks can occur.

    \param a_next_reply [in]
      Pointer to Reply object to be added to chain
  */
  void                      set_next_reply (Reply* a_next_reply);

  /**
    \brief Retrieve a pointer to the server that owns this reply
    
    \return Pointer to the server object
  */
  Unfenced_Generic_Server*  get_server (void);

  //**** end of the interface used by the wrapper writer ****

  /**
    \brief Copy a Reply object

      Actual copy construction - to be used only by the optimizer
      when copying the reply into a new memory pool (the original goes away).
      Used to save the Reply of the final plan in the engine along with the
      accepted Request_Exp trees.

    \param a_new_reply [out]
      Pointer to newly created Reply

    \param a_optP [in]
      Pointer to globals

    \param a_poolP [in]
      Pointer to memory pool

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         copy_reply (Reply**        a_new_reply,
                                        sqlno_globals* a_optP,
                                        sqlno_apcb*    a_poolP);

protected:
  /** \brief Pointer do set of order description */
  sqlno_iset*               m_order_info;

  /** \brief Pointer to execution descriptor - the 'black box' with the wrapper's execution info */
  void*                     m_exec_desc;
  
  /** \brief Size of execution descriptor */
  int                       m_exec_desc_size;

  /** \brief Chaining of replies when the wrapper returns more than one */
  Reply*                    m_next_reply;

  /** \brief Pointer to Request from which this Reply was generated */
  Request*                  m_orig_req;

  /** \brief Pointer to server for which this reply was generated */
  Unfenced_Generic_Server*  m_srv;

  /** \brief Indicates if we want to disable block fetch, 0 by default */
  short                     m_no_block;

  /**
    \brief Constructor
    
      Not implemented.
  */
  Reply (void);
  
  /**
    \brief Allocate memory for set members

      Allocate the integer arrays for each clause given the pool and their sizes.
      Update the object's protected data memebers - no return.

    \param a_poolP [in]
      Pointer to memory pool

    \param a_num_quantifiers [in]
      Number of quantifiers

    \param a_num_head_exp [in]
      Number of head expressions

    \param a_num_predicates [in]
      Number of predicates

    \param a_num_gb_exp [in]
      Number of GROUP BY expressions

    \param a_num_ob_exp [in]
      Number of ORDER BY expressions

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code         allocate_isets (struct sqlno_apcb* a_poolP,
                                            int                a_num_quantifiers,
                                            int                a_num_head_exp,
                                            int                a_num_predicates,
                                            int                a_num_gb_exp,
                                            int                a_num_ob_exp);
  
  friend sqlint32 sqlnt_build_rpd_incol_list (sqlno_globals*     a_optP,
                                              Reply*             a_reply,
                                              sqlno_qgmP_array** a_qgm_arrayP);
  friend class Predicate_List;
};

/**
   \class Predicate_List
   \brief Encapsulates a list of predicates

   The Predicate_List class encapsulates a list of predicates and is
   used by the RRC protocol.
*/
class Predicate_List : public Sqlqg_Base_Class  {
public:
  /**
    \brief Creates a empty Predicate_List

      To be used by the wrapper to instantiate an empty predicate list.

    \param a_reply [in]
      Pointer to Reply object this Predicate_List is created for. Reply
      has a pointer to Request, the actual representation of the query.

    \param a_pred_list [out]
      Pointer to newly created Predicate_List

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code create_empty_predicate_list (Reply*           a_reply,
                                                        Predicate_List** a_pred_list);

  /**
    \brief Create a predicate list and copy the predicates from the Request

      Creates a predicate list that contains all of the predicates for a specific reply.

    \param a_reply [in]
      Pointer to Reply object that owns the predicates. Reply
      has a pointer to Request, the actual representation of the query.

    \param a_pred_list [out]
      Pointer to newly created Predicate_List

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  static Sqlqg_Return_Code create_and_copy_predicate_list (Reply*           a_reply,
                                                           Predicate_List** a_pred_list);
  
  /**
    \brief Retrieve the number of predicates in the list
    
    \return Number of predicates
  */
  int                      get_number_of_predicates (void);

  /**
    \brief Retrieve a predicate expression for an expression handle
    
      Returns a Request_Exp object that represents the predicate which
      is associated with the specified handle.

    \param a_pred_handle [in]
      Integer handle for the predicate

    \param a_pred_exp [out]
      Pointer to the Request_Exp object representing the predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_predicate (int           a_pred_handle,
                                          Request_Exp** a_pred_exp);

  /**
    \brief Retrieve predicate handle
    
      Returns the handle for the nth predicate for this list.

    \param a_pred_pos [in]
      Position of the handle in list (starting at 1)

    \param a_pred_handle [out]
      Pointer to integer handle for predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_predicate_handle (int  a_pred_pos,
                                                 int* a_pred_handle);

  /**
    \brief Retrieve the number of predicates that are already applied
    
    \return Number of predicates
  */
  int                      get_number_of_applied_predicates (void);

  /**
    \brief Retrieve an applied predicate expression for an expression handle
    
      Returns a Request_Exp object that represents the predicate which
      is associated with the specified handle.

    \param a_applied_pred_handle [in]
      Integer handle for the predicate

    \param a_applied_pred_exp [out]
      Pointer to the Request_Exp object representing the predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_applied_predicate (int           a_applied_pred_handle,
                                                  Request_Exp** a_applied_pred_exp);

  /**
    \brief Retrieve applied predicate handle
    
      Returns the handle for the nth applied predicate for this list.

    \param a_applied_pred_pos [in]
      Position of the handle in list (starting at 1)

    \param a_applied_pred_handle [out]
      Pointer to integer handle for applied predicate

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        get_applied_predicate_handle (int  a_applied_pred_pos,
                                                         int* a_applied_pred_handle);

  /**
    \brief Add a predicate to the list

    \param a_pred_handle [in]
      Pointer to integer handle for the predicate expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        add_predicate (int a_pred_handle);

  /**
    \brief Add a applied predicate to the list

    \param a_applied_pred_handle [in]
      Pointer to integer handle for the applied predicate expression

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        add_applied_predicate (int a_applied_pred_handle);

  /**
    \brief Destructor
    
      Not implemented.
  */
  ~Predicate_List (void);
  
  /**
    \brief Constructor
    
      Used by engine only.

    \param a_preds [in]
      Set of integer handles of predicates

    \param a_context [in]
      The context flag is a parameter passed originally to sqlno_ff_compute
      If the default implementation is invoked, it will pass it back,
      so we restore the same state in the function.

    \param a_rc [in]
      Return code (0 indicates Success; SQLQG_ERROR for Error)
  */
  Predicate_List (sqlno_iset*        a_preds,
                  void*              a_context,
                  Sqlqg_Return_Code* a_rc);
  
  friend class Unfenced_Generic_Server;
  
protected:
  /** \brief Pointer to context */
  void*       m_context;

  /** \brief Pointer to set of predicates */
  sqlno_iset* m_preds;

  /** \brief Pointer to set of applied predicates */
  sqlno_iset* m_applied_preds;

  /** \brief Pointer to set of head expressions */
  sqlno_iset* m_hexps;

  /** \brief Pointer to optqtb pointer */
  sqlno_qtb*  m_optqtbP;

  /** \brief Pointer to Request this Reply is created for */
  Reply*      m_orig_reply;

  /**
    \brief Constructor
    
      Not implemented.
  */
  Predicate_List (void);
  
  /**
    \brief Constructor
    
      Used by create_empty_predicate_list

    \param a_reply [in]
      Pointer to Reply object this Preicate_List belongs to

    \param a_rc [in]
      Return code (0 indicates Success; SQLQG_ERROR for Error)
  */
  Predicate_List (Reply*             a_reply,
                  Sqlqg_Return_Code* a_rc);

  /**
    \brief Initialization of predicate lists
    
      Called from the constructor.

    \param a_preds [in]
      Set of integer handles of predicates

    \retval 0 Success
    \retval SQLQG_ERROR Error
  */
  Sqlqg_Return_Code        init_predicate_list (sqlno_iset* a_preds);
};

////////////////////////////////////////////////////////////////////////////////
//
//                                inlines
//
////////////////////////////////////////////////////////////////////////////////
inline Request_Exp* Request_Exp::get_parent (void)
{
  return m_parent;
}

inline Request_Exp* Request_Exp::get_next_child (void)
{
  return m_next_child;
}

inline Request_Exp* Request_Exp::get_first_child (void)
{
  return m_child;
}

inline int Request_Exp::get_number_of_children (void)
{
  int count = 0;

  for(Request_Exp* cur_rexp  = m_child;
                   cur_rexp != NULL;
                   cur_rexp  = cur_rexp->m_next_child)
  {
    count++;
  }

  return count;
}

inline Request_Exp::kind Request_Exp::get_kind (void)
{
  return m_kind;
}

inline Sqlqg_Return_Code Request_Exp::get_kind(Request *rq, Request_Exp::kind *res)
{
  *res = m_kind;
  
  return 0;
}

inline sqlnq_pid* Request_Exp::get_pid(void)
{
  return m_pid;
}

inline int Parsed_Query_Fragment::is_generated_pid (int a_hexp_pos) 
{
  return (m_pid_array && m_pid_array[a_hexp_pos]) ? 1 : 0;
}

inline int Parsed_Query_Fragment::get_distinct (void)
{
  return m_distinct;
}

inline void Parsed_Query_Fragment::set_distinct (int a_distinct)
{
  m_distinct = a_distinct;
}

inline Unfenced_Generic_Server* Reply::get_server (void)
{
  return m_srv;
}

inline short Reply::get_no_block (void)
{
  return m_no_block;
}

inline void Reply::set_no_block (void)
{
  m_no_block = 1;
}

inline void Reply::get_exec_desc(void** a_exec_desc,
                                 int*   a_exec_desc_size)
{
  *a_exec_desc      = m_exec_desc;
  *a_exec_desc_size = m_exec_desc_size;
}

inline void Reply::set_exec_desc(void* a_exec_desc,
                                 int   a_exec_desc_size)
{
  m_exec_desc      = a_exec_desc;
  m_exec_desc_size = a_exec_desc_size;
}

inline void Reply::set_next_reply (Reply* a_next_reply)
{
  m_next_reply = a_next_reply;
}

inline Reply* Reply::next()
{
  return m_next_reply;
}

inline Predicate_List::~Predicate_List (void)
{}

#endif //  __SQLQG_REQUEST_H__ included
