::
:: for "atf\genprot.pl" script issues talk to Larry Cheng 
:: invoke with -thin 

cd %~dp0..\..
C:\Perl64\bin\perl5.26.3.exe   atf\genprot.pl -thin -genMoveConstructor -dontGenValidationFuncs protocols\StarsStore\PsStarsStoreAPI.txt
pause
