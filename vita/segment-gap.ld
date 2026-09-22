/* Reserve space for vita-elf-create SCE metadata after the RX segment. */
SECTIONS { . = ALIGN(0x1000) + 0x1000; }
INSERT BEFORE .init_array;
