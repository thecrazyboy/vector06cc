# Introduction #
Floppies are good (only) when emulated.
Хороший флоп -- эмулированный флоп.

# Как воспользоваться дисководом #
Надо взять SD или MMC карточку. Она должна быть отформатирована в FAT16, совсем необязательно только что, можно вчера или год назад. На ней надо создать каталог под названием `VECTOR06` (без суффикса C!) и в этот каталог положить мнооого файлов с расширением .fdd. При начальном запуске будет выбран первый попавшийся образ из этого каталога. Чтобы выбрать образ другого диска, вызовите экранное меню (ScrollLock) и выберите нужный образ диска. После выбора образа, с него можно загрузиться (RESET/F11, затем RESTART/F12), а можно просто продолжать работать как будто бы была вынута одна дискета и вставлена другая.

Если что-то не так, можно посмотреть в консоль на RS-232:115200-8-N-1. Правда, вряд ли там можно увидеть чего-нибудь полезное.

Образы дискет годятся традиционные .fdd 1024x5x2xNN, .mst вроде бы то же самое, но .mst нужно сначала переименовать в .fdd. Если диск не загрузочный, можно загрузить образ квазидиска -- тамошний МикроДОС будет видеть и дискету. Обмениваться файлами между диском и образом дискеты можно с помощью плагина [MI\_V004](http://vector06cc.googlecode.com/files/MI_V004.ZIP%20) к Far-у. Для запуска игры в формате .rom с диска, проще всего предаврительно переименовать файл в .com, тогда он будет запускаться как обычная дисковая программа [_последнее верно для большинства, но не для всех программ_].

Для загрузки с дискеты при загруженном квазидиске нужно крепко удерживать F1+F2 при сбросе через F11 или KEY0.

# How to use a floppy image #
You need a FAT16-formatted SD or MMC card and a floppy image in .fdd or .mst format. Create a folder called `VECTOR06` in the root directory of your SD card and put the .fdd file in it. There can be as many files as you wish: the first .fdd found will be used after initial boot, later you can select other floppy images via the OSD (press ScrollLock). After the image is selected you can either reboot from that disk (F11, then F12; or, alternatively, RESET then RESTART via OSD menu), or just work as if you're juggling the floppies, like in the good olé days.

If something doesn't work as expected, you can check out debug output on the serial port. If the disk is detected but it isn't bootable, you can still load ramdisk image as described in [GettingStarted](GettingStarted.md) and it will be visible as drive A:

If ramdisk is loaded with a bootable image, but you need to reboot from a floppy, hold F1+F2 keys while resetting by F11 or KEY0.

Files can be transferred to and fro floppies with the help of file exchange plugin for Far Manager. The (freeware) plugin can be found in the downloads section. To run a .rom file from disk, it must be renamed into .com before it is transferred to the .fdd image [_the latter is true for most, but not all programs_].

# See also #
[Getting Started](GettingStarted.md)