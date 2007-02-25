upgrade:	
	rm -rf code.bin
	../router/mipsel-uclibc/merge ram_zimage.bin target.squshfs 1
	chmod 644 code.bin
	../router/mipsel-uclibc/addpattern -i code.bin -o upgrade_code.bin -p WAG2
	../router/mipsel-uclibc/tichksum upgrade_code.bin

rom:	
	rm -rf rom_code.bin
	../router/mipsel-uclibc/merge ram_zimage.bin target.squshfs 0
	chmod 644 rom_code.bin
