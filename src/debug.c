#include <stdio.h>
#include "debug.h"
#include "value.h"

void
dissasemble_chunk(Chunk *chunk, const char *name)
{
  printf("=== %s ===\n", name);

  for (int offset = 0; offset < chunk->count;)
  {
    offset = dissasemble_instruction(chunk, offset);
  }
}

static int
constant_instruction(const char *name, Chunk *chunk, int offset)
{
  uint8_t constant = chunk->code[offset + 1];

  printf("%-16s %4d '", name, constant);
  print_value(chunk->constants.values[constant]);
  printf("'\n");

  return offset + 2;
}

static int
simple_instruction(const char *name, int offset)
{
  printf("%s\n", name);
  return offset + 1;
}

int
dissasemble_instruction(Chunk *chunk, int offset)
{
  printf("%04d ", offset);

  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
  {
    printf("   | ");
  }
  else
  {
    printf("%04d ", chunk->lines[offset]);
  }

  uint8_t instruction = chunk->code[offset];

  switch (instruction)
  {
  case OP_RETURN:
    return simple_instruction("RETURN", offset);
  case OP_CONSTANT:
    return constant_instruction("CONSTANT", chunk, offset);
  default:
    printf("Unknown op %d", instruction);
    return offset + 1;
  }
}
