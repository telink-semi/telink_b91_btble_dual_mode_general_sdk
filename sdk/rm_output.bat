@Rem        �ű����ܣ��ݹ������ɾ��ָ�����ļ���
echo off & color 0A
::ָ����ʼ�ļ���
set DIR="%cd%"
set DEL_NAME=output
echo DIR=%DIR%


for /d /R %DIR% %%f in (*) do ( 
	@echo %%f
	@echo %DEL_NAME%
	@echo %%f | findstr %DEL_NAME% >nul && rd /s /q %%f || echo no
)
@echo success all
pause