# Nios-II-DE0-Pikachu-Sprite
A 2-Sprite moving Pikachu using the VGA Buffer on the Nios II DE0 board. Uses a C program with hardware clock interrupts to switch sprites.

Instructions:
1. Open the Nios II DE0 Simulator using the link below
2. Press Ctrl+M to access the Memory Tab
3. At memory address 0x00000200, change the opcode from 0x06c10034 to 0x06c02034
4. In Debugging Checks on the Settings tab, uncheck "Memory acces to unallocated stack space" and "Instruction fetch: Modified opcode"
5. Click "Continue" on the top bar to run the program. The Sprite should appear in the VGA Pixel Buffer window in the Devices tab on the right.

Link to Nios II DE0 Simulator: https://cpulator.01xz.net/?sys=nios-de0&loadasm=share/sMCydU3.s&lang=c

This program uses starter code provided by Dr. Nariag Manjikian for use in ELEC 371 at Queen's University in Kingston, ON, Canada.
