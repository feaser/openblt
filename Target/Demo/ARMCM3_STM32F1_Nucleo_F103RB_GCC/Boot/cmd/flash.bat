call env.bat
openocd.exe -f interface/stlink-v2-1.cfg -f target/stm32f1x_stlink.cfg -f flash.cfg