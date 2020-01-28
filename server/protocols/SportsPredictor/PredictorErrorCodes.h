// When you add entries to this table, make sure to send the updated file to Victiv or talk to GiuseppeG.

ERR(Pending,                         46, "Pending Transaction.") // this is internal to PredictorServer, it will not be sent to Victiv
ERR(NoSuchUser,                       2, "No such user or authentication token failed.")
ERR(Internal,                         7, "Internal logical error.") // this is usually sent if some field has an illegal value, or if the server fails an internal test

ERR(NoSuchTicket,                 10901, "This ticketId does not exist.")
ERR(TicketBelongsToAnother,       10902, "The ticketId belongs to another user.")
ERR(OperationNotAllowed,          10903, "Cannot perform this operation when the ticket is in this state.") // the state and operation are also appended
ERR(DuplicateMissmatch,           10904, "Duplicate request from same source,refId and non-matching content.") // the non matching fields are also appended

ERR(AccountClosed,                10905, "The user's account is closed.")
ERR(SelfExcluded,                 10906, "User self excluded.")
ERR(NoPlayPrivilege,              10907, "User has no play privilege.")
ERR(PSRelatedNotAllowed,          10908, "PS employees accounts are not allowed.") 
ERR(AdminNotAllowed,              10909, "Users with admin privileges are not allowed.")
ERR(Blocked,                      10910, "User is blocked from spending tickets.") // not checked when tickets are issued
ERR(AccountNotEmailVerified,      10911, "Account is not email verified, user must send a confirmation email.")
ERR(LimitedMode,                  10912, "User has Limited Mode access, spending tickets is not allowed.") // not checked when tickets are issued
ERR(BannedByRegulator,            10913, "User is banned by regulator.") // added on 2018-03-13, enforced only for DK license

ERR(PersonalLimitExceeded,        10917, "This operation would exceed the personal limit.") // the exact numbers are also appended
ERR(GlobalLimitExceeded,          10918, "This operation would exceed the global limit.") // the exact numbers are also appended

ERR(NoSportsPlay,                 10921, "User is not allowed to place Sports bets.") // PYR-68925 added on 2018-04-04
ERR(NoSportsComms,                10922, "No communications allowed from Sports to this user.") // PYR-68925 added on 2018-04-04

// When you add entries to this table, make sure to send the updated file to Victiv or talk to GiuseppeG.
