@echo off

chcp 65001 >NUL 2>NUL

rem ���� ������ ����� ������ ������, ��� ���� �� ����� ��������� .sh ��������
rem >NUL 2>NUL ��� ��������� ������, ����� �� �������� ��� �����
rem ���� � ��� ��������� �����, ���������� ���������� ��������� %GIT_BASH_PATH% � �� �������� \ �� �����

"%GIT_BASH_PATH%bash.exe" %~dp0\update_commit_hash.sh >NUL 2>NUL

rem ��� �������, �������� ������������ ����� ������ ������ � ����� � ������ retarget
attrib -R %~dp0\retarget.cpp 

