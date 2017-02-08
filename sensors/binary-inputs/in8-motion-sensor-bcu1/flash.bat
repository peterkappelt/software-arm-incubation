SET JLINK_PATH="C:\Program Files (x86)\SEGGER\JLink_V612f\JLink.exe"

@echo r > %TEMP%/temp.jlink
@echo loadbin %1,0x0 >> %TEMP%/temp.jlink
@echo r >> %TEMP%/temp.jlink
@echo q >> %TEMP%/temp.jlink

%JLINK_PATH% -device LPC1115/303 -if SWD -speed 4000 -autoconnect 1 -CommanderScript %TEMP%/temp.jlink