@Rem        脚本功能：递归遍历，删除指定子文件夹
echo off & color 0A
::指定起始文件夹
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