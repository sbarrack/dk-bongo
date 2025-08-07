#include "gcb.h"

uint8_t gcSendGet(const uint8_t pin, uint8_t* command, const uint8_t commandLen, uint8_t* report, const uint8_t reportLen) {
	uint8_t bitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);

	volatile uint8_t* modePort = portModeRegister(port);
	volatile uint8_t* outPort = portOutputRegister(port);
	volatile uint8_t* inPort = portInputRegister(port);

	uint8_t oldSREG = SREG;
	cli();

	gcSend(command, commandLen, modePort, outPort, bitMask);
	uint8_t receivedBytes = gcGet(report, reportLen, modePort, outPort, inPort, bitMask);

	SREG = oldSREG;

	return receivedBytes;
}

void gcSend(const uint8_t* buff, uint8_t len, volatile uint8_t* modePort, volatile uint8_t* outPort, uint8_t bitMask) {
	*outPort |= bitMask;
	*modePort |= bitMask;

	register uint8_t bitCount;
	register uint8_t data;

	__asm__ volatile (
		"bytes:\n\t"
		"ldr %[data],%a[buff],1\n\t"//(2) data -> buffer++
		"mov %[bitCount],8\n\t"		//(1) bitCount = 8
		"nop\n\tnop\n"				//(2)
		
		"bits:\n\t"
		"nop\n\tnop\n\t"			//(2)
		"ldr %[outPort],%a[low]\n\t"//(2) outPort -> low
		//1us - 4
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"//(116)
		"lsls %[data],1\n\t"						//(1) data << 1
		"bcc zero\n\t"								//(1|1+p) if C == 0, goto
		"ldr %[outPort],%a[high]\n"					//(2) outPort -> high
		
		"zero:\n\t"
		//2us - 5
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"

		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"

		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"	//(235)
		"ldr %[outPort],%a[high]\n\t"			//(2) outPort -> high
		//1us - 8
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\t"		//(112)
		"subs %[bitCount],1\n\t"//(1) bitCount--
		"beq next_byte\n\t"		//(1|1+p) if bitCount == 0, goto
		"b bits\n"				//(1+p) goto
		
		"next_byte:\n\t"
		"subs %[len],1\n\t"	//(1) len--
		"beq stop_bit\n\t"	//(1|1+p) if len == 0, goto
		"b bytes\n"			//(1+p) goto
		
		"stop_bit:\n\t"
		/*	I've seen several different opinions about what the proper stop bit is:
			1) Both console and controller send a 1 bit (Nico and James)
			2) Controller sends a 2us low - 2us high, console sends a 1us low - 2us high (n64squid)
			3) According to other N64 documentation both send a 1us low - 2us high except if
				the console sends the 0x02/0x03 command (accessing N64 controller pak)
			I'll just stick with a 1 bit here.
		*/
		"ldr %[outPort],%a[low]\n\t"//(2) outPort -> low
		//1us - 2
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"

		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"	//(118)
		"ldr %[outPort],%a[high]\n"									//(2) outPort -> high
		
		://outs
		//read and write
		[buff] "+r" (buff),
		[outPort] "+r" (outPort),
		//out only, write only
		[bitCount] "=&r" (bitCount),
		[data] "=&r" (data)
		://ins
		[len] "r" (len),
		[high] "r" (*outPort | bitMask),
		[low] "r" (*outPort & ~bitMask)
	);
}

uint8_t gcGet(uint8_t* buff, uint8_t len, volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask) {
	*modePort &= ~bitMask;
	*outPort |= bitMask;

	register uint8_t timeoutCount;
	register uint8_t bitCount;
	register uint8_t inputVal;
	register uint8_t data;
	register uint8_t receivedBytes;
	register uint8_t initialTimeoutCount;

	__asm__ volatile (
		"\tmov %[bitCount],8\n\t"		//(1) bitCount = 8
		"mov %[receivedBytes],0\n\t"	//(1) receivedBytes = 0
		"mov %[initialTimeoutCount],0\n"//(1) initialTimeoutCount = 0

		"wait_for_low_extra:\n\t"
		"mov %[timeoutCount],0\n"//(1) timeoutCount = 0

		"wait_for_low_loop_extra1:\n\t"
		"ldr %[inputVal], %a[inPort]\n\t"	//(2) inputVal -> inPort
		"ands %[inputVal], %[bitMask]\n\t"	//(1) to compare
		"beq wait_for_measure\n\t"			//(1|1+p) if inputVal = 0, goto
		"subs %[timeoutCount],1\n\t"		//(1) timeoutCount--
		"bne wait_for_low_loop_extra1\n\t"	//(1|1+p) if timeoutCount != 0, goto
		"mov %[timeoutCount],0\n"			//(1) timeoutCount = 0

		"wait_for_low_loop_extra2:\n\t"
		"ldr %[inputVal], %a[inPort]\n\t"	//(2) inputVal -> inPort
		"ands %[inputVal], %[bitMask]\n\t"	//(1) to compare
		"beq wait_for_measure\n\t"			//(1|1+p) if inputVal = 0, goto
		"subs %[timeoutCount],1\n\t"		//(1) timeoutCount--
		"bne wait_for_low_loop_extra2\n"	//(1|1+p) if timeoutCount != 0, goto

		"wait_for_low:\n\t"
		"mov %[timeoutCount],128\n"//(1) timeoutCount = 128

		"wait_for_low_loop:\n"
		"ldr %[inputVal], %a[inPort]\n\t"	//(2) inputVal -> inPort
		"ands %[inputVal], %[bitMask]\n\t"	//(1) to compare
		"beq wait_for_measure\n\t"			//(1|1+p) if inputVal = 0, goto
		"subs %[timeoutCount],1\n\t"		//(1) timeoutCount--
		"bne wait_for_low_loop\n\t"			//(1|1+p) if timeoutCount != 0, goto
		"subs %[initialTimeoutCount],1\n\t"	//(1)
		"bne wait_for_low_extra\n\t"		//(1|1+p) if initialTimeoutCount != 0, goto
		"b exit\n"							//(1+p) goto

		"wait_for_measure:\n\t"
		/*	This brings me back to the proper stop bit talk. Normally, I'd be
			inclined to wait only 1us to measure the bit, HOWEVER, the controller
			could send a 2us low long stop bit instead of a 1 bit and cause
			trouble. The duty cycle for reading a bit is 2us long so I still have the
			opportunity to read the bit 1us later than normal and I'd have to wait
			anyways for the next bit.
		*/
		//2us - 9
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"

		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"

		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
		"nop\n\t"							//(231)
		"mov %[initialTimeoutCount],1\n\t"	//(1) initialTimeoutCount = 1
		"lsl %[data],1\n\t"					//(1) data << 1
		"ldr %[inputVal],%a[inPort]\n\t"	//(2) inputVal -> inPort
		"ands %[inputVal],%[bitMask]\n\t"	//(1) to compare
		"beq check_bit_count\n\t"			//(1|1+p) if inputVal = 0, goto
		"orr %[data],1\n"					//(1) data | 1

		"check_bit_count:\n\t"
		"subs %[bitCount],1\n\t"			//(1) bitCount--
		"bne wait_for_high\n\t"				//(1|1+p) if bitCount != 0, goto
		"ldr %[buff],%a[data]\n\t"			//(2) buff -> data
		"ldr %[data],%a[buff],1\n\t"		//(2) &buff++
		"add %[receivedBytes],1\n\t"		//(1) receivedBytes++
		"mov %[bitCount],8\n\t"				//(1) bitCount = 8
		"movs %[len],%[receivedBytes]\n\t"	//(1) to compare
		"beq exit\n"						//(1|1+p) if len == receivedBytes, goto

		"wait_for_high:\n\t"
		"mov %[timeoutCount],128\n"//(1) timeoutCount = timeout

		"wait_for_high_loop:\n\t"
		"ldr %[inputVal],%a[inPort]\n\t"	//(2) inputVal -> inPort
		"ands %[inputVal],%[bitMask]\n\t"	//(1) to compare
		"bne wait_for_low\n\t"				//(1|1+p) if inputVal != 0, goto
		"subs %[timeoutCount],1\n\t"		//(1) timeoutCount--
		"bne wait_for_high_loop\n"			//(1|1+p) if timeoutCount != 0, goto
		
		"exit:\n"

		://outs
		//read and write
		[buff] "+r" (buff),
		//out only, write only
		[receivedBytes] "=&r" (receivedBytes),
		[bitCount] "=&r" (bitCount),
		[timeoutCount] "=&r" (timeoutCount),
		[initialTimeoutCount] "=&r" (initialTimeoutCount),
		[inputVal] "=&r" (inputVal),
		[data] "=&r" (data)
		://ins
		[len] "r" (len),
		[inPort] "r" (inPort),
		[bitMask] "r" (bitMask)
	);
	return receivedBytes;
}
