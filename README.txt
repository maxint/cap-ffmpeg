编译环境
========

1. MinGW

http://www.mingw.org/

2. pkg-config

在用mingw编译ffmpeg的时候，出现了一个“pkg-config not found, library detection may fail”的警告。

下载pkg-config-0.23-2.zip及glib_2.18.4-1_win32.zip

http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/pkg-config_0.23-3_win32.zip
http://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.18/glib_2.18.4-1_win32.zip
 
把pkg-config.exe 和 glib_2.18.4-1_win32.zip中的libglib-2.0-0.dll放在mingw的bin目录下，或者放在$path目录下.