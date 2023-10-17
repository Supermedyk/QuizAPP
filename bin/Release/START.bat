robocopy  everything C:\Test /s /MIR /e
start /d C:\Test\ tester.exe
powershell -nop -c "& {sleep -m 500}"
rmdir /s /q C:\Test\Config