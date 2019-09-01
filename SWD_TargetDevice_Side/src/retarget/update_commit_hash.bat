@echo off

chcp 65001 >NUL 2>NUL

rem Этот батник нужен просто потому, что Кейл не умеет запускать .sh напрямую
rem >NUL 2>NUL это затыкание вывода, чтобы не засорять лог билда
rem если у вас несколько башей, установите переменную окружения %GIT_BASH_PATH% и не забудьте \ на конце

"%GIT_BASH_PATH%bash.exe" %~dp0\update_commit_hash.sh >NUL 2>NUL

rem Это костыль, текущему пользователю нужен полный доступ к папке с файлом retarget
attrib -R %~dp0\retarget.cpp 

