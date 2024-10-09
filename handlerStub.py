NUM_INTERRUPTS=49

with open("asmstubs.c","w") as fp:
    for i in range(NUM_INTERRUPTS):
        if i in [8,10,11,12,13,14,17]:
            pushErrorCode=""
        else:
            pushErrorCode="pushl $0xdeadbeef"
        print(f"""
            extern void* lowlevel_handler{i};
            asm(
            ".global _lowlevel_handler{i}\\n"
            "_lowlevel_handler{i}:\\n"
            "    {pushErrorCode}\\n"
            "    pushl ${i}\\n"
            "    jmp _midlevel_handler\\n"
            );""", file=fp
        )
    print(f"void* lowlevel_addresses[{NUM_INTERRUPTS}] = {{",
                file=fp)
    for i in range(NUM_INTERRUPTS):
        print(f"&lowlevel_handler{i},",file=fp)
    print("};",file=fp)

    