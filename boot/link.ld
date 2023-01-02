























































OUTPUT_ARCH( "riscv" )
MEMORY
{
  boot : ORIGIN = 134217728, LENGTH = 1024
}
SECTIONS
{
  .text : { *.o(.text*) } > boot
}
