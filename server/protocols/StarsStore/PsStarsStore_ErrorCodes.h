// When you add entries to this table, make sure to send the updated file to StarsStore or talk to GiuseppeG.

ERR(Pending,                         46, "Pending Transaction.") // this is internal to STS, it will not be sent to outside Co
ERR(NoSuchUser,                       2, "No such user or authentication token failed.")
ERR(Internal,                         7, "Internal logical error.") // this is usually sent if some field has an illegal value, or if the server fails an internal test

ERR(NoSuchTicket,                 10701, "This ticketId does not exist.")
ERR(TicketBelongsToAnother,       10702, "The ticketId belongs to another user.")
ERR(OperationNotAllowed,          10703, "Cannot perform this operation when the ticket is in this state.") // the state and operation are also appended
ERR(DuplicateMissmatch,           10704, "Duplicate request from same source,refId and non-matching content.") // the non matching fields are also appended

ERR(AccountClosed,                10705, "The user's account is closed.")
ERR(SelfExcluded,                 10706, "User self excluded.")
ERR(NoPlayPrivilege,              10707, "User has no play privilege.")
ERR(PSRelatedNotAllowed,          10708, "PS employees accounts are not allowed.") 
ERR(AdminNotAllowed,              10709, "Users with admin privileges are not allowed.")
ERR(Blocked,                      10710, "User is blocked from spending tickets.") // not checked when tickets are issued
ERR(AccountNotEmailVerified,      10711, "Account is not email verified, user must send a confirmation email.")
ERR(LimitedMode,                  10712, "User has Limited Mode access, spending tickets is not allowed.") // not checked when tickets are issued
ERR(BannedByRegulator,            10713, "User is banned by regulator.") // added on 2018-03-13, enforced only for DK license

ERR(PersonalLimitExceeded,        10717, "This operation would exceed the personal limit.") // the exact numbers are also appended
ERR(GlobalLimitExceeded,          10718, "This operation would exceed the global limit.") // the exact numbers are also appended

ERR(NoSportsPlay,                 10721, "User is not allowed to place Sports bets.") // PYR-68925 added on 2018-04-04
ERR(NoSportsComms,                10722, "No communications allowed from Sports to this user.") // PYR-68925 added on 2018-04-04

// When you add entries to this table, make sure to send the updated file to StarsStore or talk to GiuseppeG.
