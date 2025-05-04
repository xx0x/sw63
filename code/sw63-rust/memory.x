MEMORY
{
  /* Leave 8k for the bootloader */
  FLASH (rx) : ORIGIN = 0x00000000 + 8K, LENGTH = 256K - 8K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

/* This is where the call stack will be allocated. */
/* The stack is of the full descending type. */
/* NOTE Do NOT modify `_stack_start` unless you know what you are doing */
_stack_start = ORIGIN(RAM) + LENGTH(RAM);