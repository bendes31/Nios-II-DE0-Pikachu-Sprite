/*===================================================================

   Template single/combined C file for ELEC 371 laboratory activity

   October 2021  Dr. N. Manjikian

   C programs for Nios II systems can be prepared with multiple
   separate files in project defined with the Monitor Program.

   A single C file, however, is necessary for the Web-based simulator
   because the current version of the Web-based tool handles a single
   file in its editor window.

   This single file merges together several .h header files and
   the exception_handler.c file with the assembly-language code
   for saving/restoring registers and handling the ea adjustment.

   The .h header file contents are positioned at the beginning of
   this template file, and the exception_handler.c file contents
   are positioned at the end of this template file. There are
   comments with "----" dividers and original-file identification
   to aid in understanding the different portions.

   In the middle of this template file is the portion that can be
   customized for a particular application. For interrupt support,
   there is an empty interrupt_handler() function. There is an
   empty Init() function and a simple main() function. Other
   functions can be added as necessary in front of main().

==================================================================*/



/*-----------------------------------------------------------------*/

#ifndef _NIOS2_CONTROL_H_
#define _NIOS2_CONTROL_H_


/* 
   This file defines macros that utilize the built-in functions
   in Altera's adaption of the GCC compiler to generate the
   control-register-access instructions for the Nios II processor.

   For ELEC 371 laboratory work, it has been modified by N. Manjikian.

   It should be noted that the manner in which the built-in functions
   are used below _differs_ from the manner in which they are shown in
   the DE0 Basic Computer reference document from Altera. The key
   difference is for _read_ operations. The conventional method of
   defining macros that read a value is to have the value returned,
   like a function. The method used in the original Altera code is
   not conventional. The second main difference is a simplification
   that avoids the use of a do..while loop found in the Altera version.
   The do..while loops may have been introduced for a specific reason (e.g.,
   to perhaps prevent the compiler from optimizing away the calls to
   the built-in functions, or to allow the assignment of the return
   value to a named variable). For the purposes of laboratory work,
   the revision of the macros by the instructor is appropriate, and
   the compiler-generated code is correct.

   The official names for the processor control register are ctl0, ctl1, ...
   These registers are more conveniently identified by their aliases,
   such as status, estatus, ...  Documentation on the Nios II processor
   provides the association between the official names and the aliases.

   Not all of the possible control-register accesses are covered by
   the macros below. The ones most relevant for ELEC 371 laboratory work
   are included. If access to the other control registers is required,
   additional macros could easily be introduced, based on the existing ones.

   The number on the right side of each macro definition refers to the
   corresponding 'ctl_' processor control register. The __builtin_wrctl()
   function passes the ctl register number and a value to the compiler for
   use in generating the appropriate assembly-language instruction.
   The __builtin_rdctl() function passes only the register number, and
   it _returns_ a value. The instructor-revised read macros then return
   this result as the return value of the macro for use by the C program.
*/


#define NIOS2_WRITE_STATUS(value)  (__builtin_wrctl (0, value))

#define NIOS2_READ_IENABLE()	   (__builtin_rdctl (3))

#define NIOS2_WRITE_IENABLE(value) (__builtin_wrctl (3, value))

#define NIOS2_READ_IPENDING()	   (__builtin_rdctl (4))


#endif /* _NIOS2_CONTROL_H_ */


/*-----------------------------------------------------------------*/
/*-----------------------------------------------------------------*/
/*             start of application-specific code                  */
/*-----------------------------------------------------------------*/


#define PIXEL_BUFFER ((volatile char *) 0x8000000)
#define Y  0xfc
#define B  0x00
#define W  0xff
#define R  0xe0
#define D  0xf1
#define Br 0xac
	
#define TIMER_STATUS        ((volatile int *) 0x10002000)
#define TIMER_CONTROL       ((volatile int *) 0x10002004)
#define TIMER_START_LOW    ((volatile int *) 0x10002008)
#define TIMER_START_HIGH     ((volatile int *) 0x1000200C)


#define PUSHBUTTON_DATA ((volatile int *) 0x10000050)
#define LEDS (volatile unsigned int *) 0x10000010

#define JTAG_UART_DATA    ((volatile unsigned *) 0x10001000)
#define JTAG_UART_CONTROL ((volatile unsigned *) 0x10001004)


#define VGA_WIDTH  80
#define VGA_HEIGHT 60

char pikachu1[20][20];
char pikachu0[20][20];
int current;

void interrupt_handler(void){
	unsigned int ipending = NIOS2_READ_IPENDING();
	NIOS2_WRITE_IENABLE(0);
	if (ipending & 0x1){
		*TIMER_STATUS = 0;
		if (current) drawPikachu(20, 30, --current);
		else drawPikachu(20, 30, ++current); 
	}
	NIOS2_WRITE_IENABLE(1);
}

void draw_pixel(int color, int x, int y) {
    PIXEL_BUFFER[(y << 7) | x] = (char) color;
}

void draw_rect(int color, int x1, int y1, int x2, int y2) {
    for (int y = y1; y < y2; y++)
        for (int x = x1; x < x2; x++)
            draw_pixel(color, x, y);
}
void colourPalette(void){
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			draw_pixel ((16*x)+y, 2*x, 2*y);
			draw_pixel ((16*x)+y, 2*x, 2*y+1);
			draw_pixel ((16*x)+y, 2*x+1, 2*y);
			draw_pixel ((16*x)+y, 2*x+1, 2*y+1);
		}
	}
}

void drawPikachu(int x_start, int y_start, int sprite){
	for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 20; y++) {
			if (sprite) draw_pixel((int)pikachu1[x][y], y+y_start, x+x_start);
			else draw_pixel((int)pikachu0[x][y], y+y_start, x+x_start);
		}
	}
}

void Init (void)
{
	/* initialize software variables */
	
	/* set up each hardware interface */
	*TIMER_START_HIGH = 0x00BE;
	*TIMER_START_LOW = 0xBC20;
	*TIMER_STATUS = 0;
	*TIMER_CONTROL = 0x7;

	/* set up ienable */
	NIOS2_WRITE_IENABLE(1);
	/* enable global recognition of interrupts in procr. status reg. */
	NIOS2_WRITE_STATUS(1);
}

int main(void){
	Init();
	draw_rect(W, 0, 0, VGA_WIDTH, VGA_HEIGHT);
	drawPikachu(20, 30, 1);
}
	
/* place additional functions here */


/*-----------------------------------------------------------------*/
/*              end of application-specific code                   */
/*-----------------------------------------------------------------*/
char pikachu1[20][20] = {
	{W, W, W, W, W, W, B, B, W, W, W, W, W, W, W, W, W, B, W, W},
	{W, W, W, W, W, B, B, B, W, W, W, W, W, W, W, W, B, Y, B, W},
	{W, W, W, W, W, B, Y, B, W, W, W, W, W, W, W, B, Y, D, D, B},
	{W, W, W, W, B, Y, Y, B, W, W, W, W, B, B, B, Y, D, D, D, B},
	{W, W, W, W, B, Y, D, B, W, W, B, B, Br, Br, B, D, D, D, B, W},
	{W, W, W, B, Y, Y, Y, Y, B, B, Y, Y, Br, B, D, D, D, B, W, W},
	{W, W, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, B, D, D, B, W, W, W},
	{W, B, W, Y, Y, Y, Y, Y, Y, Y, Y, D, B, W, B, D, D, B, W, W},
	{W, B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W, W, B, D, B, W, W},
	{W, B, Y, Y, Y, Y, W, B, Y, Y, Y, Y, D, B, Br, D, B, W, W, W},
	{W, W, B, Y, Y, Y, B, B, R, R, Y, D, D, B, Br, Br, W, W, W, W},
	{W, W, W, B, D, Y, Y, Y, R, D, D, D, Br, Br, B, W, W, W, W, W},
	{W, W, B, Y, D, D, D, D, D, D, D, D, D, D, B, W, W, W, W, W},
	{W, W, W, B, B, Y, Y, Y, Y, Y, B, D, D, Br, B, W, W, W, W, W},
	{W, W, W, W, B, D, Y, Y, Y, B, Y, D, D, D, B, W, W, W, W, W},
	{W, W, W, B, D, B, D, D, D, D, B, D, D, D, B, W, W, W, W, W},
	{W, W, W, B, B, B, B, B, D, D, D, D, D, B, W, W, W, W, W, W},
	{W, W, W, W, W, W, W, W, B, B, B, D, B, B, W, W, W, W, W, W},
	{W, W, W, W, W, W, W, W, W, B, Y, Y, D, B, W, W, W, W, W, W},
	{W, W, W, W, W, W, W, W, W, W, B, B, B, W, W, W, W, W, W, W}};
	
char pikachu0[20][20] = {
	{W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
	{W, W, W, W, W, B, B, W, W, W, W, W, W, W, W, W, W, B, W, W},
	{W, W, W, W, B, Y, B, W, W, W, W, W, W, W, W, W, B, Y, B, W},
	{W, W, W, W, B, Y, B, W, W, W, W, W, W, W, W, B, Y, D, D, B},
	{W, W, W, B, Y, Y, B, W, W, W, W, B, B, B, B, Y, D, D, D, B},
	{W, W, W, B, Y, D, B, W, W, B, B, Br,Br,B, B, D, D, D, B, W},
	{W, W, B, Y, Y, Y, Y, B, B, Y, Y, Br,B, B, D, D, D, B, W, W},
	{W, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W, B, D, D, B, W, W, W},
	{B, W, Y, Y, Y, Y, Y, Y, Y, Y, D, B, W, W, B, D, D, B, W, W},
	{B, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W, W, B, D, B, W, W},
	{B, Y, Y, Y, Y, Y, W, B, Y, Y, Y, Y, D, B, Br,D, B, W, W, W},
	{W, B, Y, Y, Y, Y, B, B, R, R, Y, D, D, B, Br,Br,W, W, W, W},
	{W, W, B, D, Y, Y, Y, Y, R, D, D, D, Br,Br,B, W, W, W, W, W},
	{W, W, B, Y, D, D, D, D, D, B, D, D, D, D, B, W, W, W, W, W},
	{W, W, W, B, B, D, Y, Y, B, Y, D, D, D, Br,B, W, W, W, W, W},
	{W, W, W, B, D, B, D, D, D, B, D, D, D, D, B, W, W, W, W, W},
	{W, W, W, B, B, B, B, B, D, D, D, D, D, B, W, W, W, W, W, W},
	{W, W, W, W, W, W, W, W, B, B, B, D, B, B, W, W, W, W, W, W},
	{W, W, W, W, W, W, W, W, W, B, Y, Y, D, B, W, W, W, W, W, W},
	{W, W, W, W, W, W, W, W, W, W, B, B, B, W, W, W, W, W, W, W}};
/*-----------------------------------------------------------------*/


/* 
   exception_handler.c

   This file is a portion of the original code supplied by Altera.

   It has been adapted by N. Manjikian for use in ELEC 371 laboratory work.

   Various unnecessary or extraneous elements have been excluded. For
   example, declarations in C for external functions called from asm()
   instructions are not required because any reference to external names
   in asm() instructions is embedded directly in the output written to
   the assembly-language .s file without any other checks by the C compiler.

   There is one particularly important change: on _reset_, the jump must be
   to the >> _start << location in order to properly initialize the stack
   pointer and to perform other crucial initialization tasks that ensure
   proper C semantics for variable initialization are enforced. The Altera
   version of the code jumped to main(), which will _not_ perform these
   crucial initialization tasks correctly.

   Finally, a reference to control register 'ctl4' in the asm() sequence
   has been replaced with the more meaningful alias 'ipending' for clarity.

   Other than the changes described above, the file contents have also been
   reformatted to fit in 80 columns of text, and comments have been edited.
*/


/* The assembly language code below handles processor reset */
void the_reset (void) __attribute__ ((section (".reset")));

/*****************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" *
 * we allow the linker program to locate this code at the proper reset vector*
 * address. This code jumps to _startup_ code for C program, _not_ main().   *
 *****************************************************************************/

void the_reset (void)
{
  asm (".set noat");         /* the .set commands are included to prevent */
  asm (".set nobreak");      /* warning messages from the assembler */
  asm ("movia r2, _start");  /* jump to the C language _startup_ code */
  asm ("jmp r2");            /* (_not_ main, as in the original Altera file) */
}

/* The assembly language code below handles exception processing. This
 * code should not be modified; instead, the C language code in the normal
 * function interrupt_handler() [which is called from the code below]
 * can be modified as needed for a given application.
 */

void the_exception (void) __attribute__ ((section (".exceptions")));

/*****************************************************************************
 * Exceptions code. By giving the code a section attribute with the name     *
 * ".exceptions" we allow the linker program to locate this code at the      *
 * proper exceptions vector address. This code calls the interrupt handler   *
 * and later returns from the exception to the main program.                 *
 *****************************************************************************/

void the_exception (void)
{
  asm (".set noat");         /* the .set commands are included to prevent */
  asm (".set nobreak");      /* warning messages from the assembler */
  asm ("subi sp, sp, 128");
  asm ("stw  et, 96(sp)");
  asm ("rdctl et, ipending"); /* changed 'ctl4' to 'ipending' for clarity */
  asm ("beq  et, r0, SKIP_EA_DEC");   /* Not a hardware interrupt, */
  asm ("subi ea, ea, 4");             /* so decrement ea by one instruction */ 
  asm ("SKIP_EA_DEC:");
  asm ("stw	r1,  4(sp)"); /* Save all registers */
  asm ("stw	r2,  8(sp)");
  asm ("stw	r3,  12(sp)");
  asm ("stw	r4,  16(sp)");
  asm ("stw	r5,  20(sp)");
  asm ("stw	r6,  24(sp)");
  asm ("stw	r7,  28(sp)");
  asm ("stw	r8,  32(sp)");
  asm ("stw	r9,  36(sp)");
  asm ("stw	r10, 40(sp)");
  asm ("stw	r11, 44(sp)");
  asm ("stw	r12, 48(sp)");
  asm ("stw	r13, 52(sp)");
  asm ("stw	r14, 56(sp)");
  asm ("stw	r15, 60(sp)");
  asm ("stw	r16, 64(sp)");
  asm ("stw	r17, 68(sp)");
  asm ("stw	r18, 72(sp)");
  asm ("stw	r19, 76(sp)");
  asm ("stw	r20, 80(sp)");
  asm ("stw	r21, 84(sp)");
  asm ("stw	r22, 88(sp)");
  asm ("stw	r23, 92(sp)");
  asm ("stw	r25, 100(sp)"); /* r25 = bt (r24 = et, saved above) */
  asm ("stw	r26, 104(sp)"); /* r26 = gp */
  /* skip saving r27 because it is sp, and there is no point in saving sp */
  asm ("stw	r28, 112(sp)"); /* r28 = fp */
  asm ("stw	r29, 116(sp)"); /* r29 = ea */
  asm ("stw	r30, 120(sp)"); /* r30 = ba */
  asm ("stw	r31, 124(sp)"); /* r31 = ra */
  asm ("addi	fp,  sp, 128"); /* frame pointer adjustment */

  asm ("call	interrupt_handler"); /* call normal function */

  asm ("ldw	r1,  4(sp)"); /* Restore all registers */
  asm ("ldw	r2,  8(sp)");
  asm ("ldw	r3,  12(sp)");
  asm ("ldw	r4,  16(sp)");
  asm ("ldw	r5,  20(sp)");
  asm ("ldw	r6,  24(sp)");
  asm ("ldw	r7,  28(sp)");
  asm ("ldw	r8,  32(sp)");
  asm ("ldw	r9,  36(sp)");
  asm ("ldw	r10, 40(sp)");
  asm ("ldw	r11, 44(sp)");
  asm ("ldw	r12, 48(sp)");
  asm ("ldw	r13, 52(sp)");
  asm ("ldw	r14, 56(sp)");
  asm ("ldw	r15, 60(sp)");
  asm ("ldw	r16, 64(sp)");
  asm ("ldw	r17, 68(sp)");
  asm ("ldw	r18, 72(sp)");
  asm ("ldw	r19, 76(sp)");
  asm ("ldw	r20, 80(sp)");
  asm ("ldw	r21, 84(sp)");
  asm ("ldw	r22, 88(sp)");
  asm ("ldw	r23, 92(sp)");
  asm ("ldw	r24, 96(sp)");
  asm ("ldw	r25, 100(sp)");
  asm ("ldw	r26, 104(sp)");
  /* skip r27 because it is sp, and we did not save this on the stack */
  asm ("ldw	r28, 112(sp)");
  asm ("ldw	r29, 116(sp)");
  asm ("ldw	r30, 120(sp)");
  asm ("ldw	r31, 124(sp)");

  asm ("addi	sp,  sp, 128");

  asm ("eret"); /* return from exception */

  /* Note that the C compiler will still generate the 'standard'
     end-of-normal-function code with a normal return-from-subroutine
     instruction. But with the above eret instruction embedded
     in the final output from the compiler, that end-of-function code
     will never be executed.
   */ 
}
