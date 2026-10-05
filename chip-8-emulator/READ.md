chip 8 emulator


========memory structure===========
    0x000-0x1FF: Originally reserved for the CHIP-8 interpreter, but in our modern 
    emulator we will just never write to or read from that area. Except for…

    0x050-0x0A0: Storage space for the 16 built-in characters (0 through F), which 
    we will need to manually put into our memory because ROMs will be looking for those 
    characters.

    0x200-0xFFF: Instructions from the ROM will be stored starting at 0x200, and 
    anything left after the ROM’s space is free to use.

