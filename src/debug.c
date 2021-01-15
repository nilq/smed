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
    case OP_CONSTANT:
      return constant_instruction("CONSTANT", chunk, offset);
    case OP_NIL:
      return simple_instruction("NIL", offset);
    case OP_TRUE:
      return simple_instruction("TRUE", offset);
    case OP_FALSE:
      return simple_instruction("FALSE", offset);
    case OP_EQUAL:
      return simple_instruction("OP_EQUAL", offset);
    case OP_GREATER:
      return simple_instruction("OP_GREATER", offset);
    case OP_LESS:
      return simple_instruction("OP_LESS", offset);
    case OP_ADD:
      return simple_instruction("ADD", offset);
    case OP_SUB:
      return simple_instruction("SUB", offset);
    case OP_MUL:
      return simple_instruction("MUL", offset);
    case OP_DIV:
      return simple_instruction("DIV", offset);
    case OP_NOT:
      return simple_instruction("NOT", offset);
    case OP_NEGATE:
      return simple_instruction("NEGATE", offset);
    case OP_RETURN:
      return simple_instruction("RETURN", offset);
    default:
      printf("Unknown op %d", instruction);
      return offset + 1;
  }
}
