:: PYR-40346 DFS: Extend Wallet API for Daily Fantasy Sports
::
:: for "atf\genprot.pl" script issues talk to Larry Cheng 
:: invoke with -thin 

cd %~dp0..\..
C:\Perl64\bin\perl5.22.1.exe   atf\genprot.pl -thin -genMoveConstructor -dontGenValidationFuncs protocols\PoolBetting\PB_Server_WAPI.txt
pause
