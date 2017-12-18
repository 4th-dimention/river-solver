@echo off

SET OPTS=
SET CODE_HOME=%cd%
pushd .
cl %OPTS% %CODE_HOME%\main.cpp -Fesolve
popd
