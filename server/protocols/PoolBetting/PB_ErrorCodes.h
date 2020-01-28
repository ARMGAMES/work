// When you add entries to this table, make sure to send the updated file to ColossusSports or talk to GiuseppeG.

ERR(NoSuchUser,                       2, "No such user or authentication token failed.")
ERR(InsufficientFunds,                5, "Insufficient funds.")
ERR(Internal,                         7, "Internal logical error.")
ERR(Pending,                         46, "Pending transaction has not completed yet.")

ERR(AccountClosed,                10401, "The user's account is closed.")
// ERR(CashBanned,                   10402, "Cash based user bets are banned.")
ERR(SelfExcluded,                 10403, "User self excluded.")
ERR(NoPlayPrivilege,              10404, "User has no play privilege.")
ERR(NoSportsPlayPrivilege,        10405, "User has no sports play privilege.")
ERR(AccountNotEmailVerified,      10406, "Account is not email verified, user must send a confirmation email.")
// ERR(NotAvailInCountry,         10407, "Not available in player’s country.")
ERR(AccountNoRealMoney,           10408, "Account is not allowed to use real money.")
ERR(Blocked,                      10409, "User is blocked from placing bets.")  
ERR(SuspendRM,                    10410, "SuspendRM is set.")
ERR(Accumulated7DayLimit,         10411, "Bet exceeds the user's 7 day accumulated limit.") // PYR-34388
ERR(AdminNotAllowed,              10412, "Users with admin privileges are not allowed.") // PYR-33913
ERR(PSRelatedNotAllowed,          10415, "PS employees accounts are not allowed.") // PYR-33913
ERR(BannedByRegulator,            10416, "User bets are banned by regulator.") // PYR-33913
// ERR(NotAuthVerified,              10417, "User has not yet been verified by 3rd party system.") // PYR-33913 PYR-44987 PYR-49043
ERR(PGAD_Down,                    10418, "User is in PGAD_Down limited mode, no bets allowed.") // PYR-35175
ERR(LimitedMode,                  10419, "User has Limited Mode access, placing bets is not allowed.") // PYR-45294 
ERR(StakeLimit,                   10420, "Ticket exceeds the user's stake limit.")

ERR(TranWAPI_AlreadyUsed,         10440, "This transaction ID has already been used.")
ERR(UnknownTicketId,              10441, "Unknown Ticket ID.")
ERR(TicketWasRejected,            10442, "Ticket is known but was rejected.")
ERR(TicketAlreadyClosed,          10443, "Ticket was already closed.") 
ERR(TicketAlreadyExists,          10444, "Ticket was already created by different transaction.") 
ERR(TicketCurrencyMissmatch,      10445, "The payout has different currency than the ticket.")  // PYR-49337
ERR(PrevTran4SameTktStillPending, 10446, "The previous transaction for the same ticket is still pending. Retry later.") // PYR-49337

// When you add entries to this table, make sure to send the updated file to ColossusSports or talk to GiuseppeG.
