#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "debug.h"
#include "compiler.h"
#include "object.h"
#include "memory.h"
#include "vm.h"

VM vm;

static void
reset_stack()
{
  vm.stack_top = vm.stack;
}

static void
runtime_error(const char *format, ...)
{
  va_list args;

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.chunk->code - 1;

  int line = vm.chunk->lines[instruction];

  fprintf(stderr, "[line %d] in script\n", line);

  reset_stack();
}

void
init_vm()
{
  reset_stack();
  vm.objects = NULL;
  init_table(&vm.strings);
}

void
free_vm()
{
  free_table(&vm.strings);
  free_objects();
}

void
push(Value value)
{
  *vm.stack_top = value;
  vm.stack_top++;
}

Value
pop()
{
  vm.stack_top--;
  return *vm.stack_top;
}

static Value
peek(int distance)
{
  return vm.stack_top[-1 - distance];
}

static bool
is_falsey(Value value)
{
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void
concatenate()
{
  ObjString *b = AS_STRING(pop());
  ObjString *a = AS_STRING(pop());

  int length  = a->length + b->length;
  char *chars = ALLOCATE(char, length + 1);

  memcpy(chars, a->chars, a->length + 1);
  memcpy(chars + a->length, b->chars, b->length);

  chars[length] = '\0';

  ObjString *result = take_string(chars, length);

  push(OBJ_VAL(result));
}

static void
lolcatenate()
{
  double b = AS_NUMBER(pop());
  ObjString *a = AS_STRING(pop());

  int l = a->length;
  int begin = l - b;
  int len = b;

  if (begin + len > l) len = l - begin;

  memmove(a->chars + begin, a->chars + begin + len, l - len + 1);

  ObjString *result = take_string(a->chars, len);

  push(OBJ_VAL(result));
}

static InterpretResult
run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(value_type, op)                     \
  do {                                                \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
      runtime_error("Operands must be numbers.");     \
      return INTERPRET_RUNTIME_ERROR;                 \
    }                                                 \
    double b = AS_NUMBER(pop());                      \
    double a = AS_NUMBER(pop());                      \
    push(value_type(a op b));                         \
  } while (false)

  for(;;)
  {
  #ifdef DEBUG_TRACE_EXECUTION
    printf("          ");

    for (Value *slot = vm.stack; slot < vm.stack_top; slot++)
    {
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }

    printf("\n");
    dissasemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
  #endif
    uint8_t instruction;

    switch (instruction = READ_BYTE())
    {
      case OP_CONSTANT:
      {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_NIL: push(NIL_VAL); break;
      case OP_TRUE: push(BOOL_VAL(true)); break;
      case OP_FALSE: push(BOOL_VAL(false)); break;

      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();

        push(BOOL_VAL(values_equal(a, b)));
        break;
      }

      case OP_GREATER: BINARY_OP(BOOL_VAL, >); break;
      case OP_LESS: BINARY_OP(BOOL_VAL, <); break;
      case OP_ADD:
        {
          if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
          {
            concatenate();
          }
          else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
          {
            double b = AS_NUMBER(pop());
            double a = AS_NUMBER(pop());

            push(NUMBER_VAL(a + b));
          }
          else
          {
            runtime_error("Operands must be numbers or strings.");
            return INTERPRET_RUNTIME_ERROR;
          }
          break;
        }
      case OP_SUB:
        {
          if (IS_NUMBER(peek(0)) && IS_STRING(peek(1)))
            {
              lolcatenate();
            }
          else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
            {
              double b = AS_NUMBER(pop());
              double a = AS_NUMBER(pop());

              push(NUMBER_VAL(a - b));
            }
          else
            {
              runtime_error("Operands must be numbers or strings.");
              return INTERPRET_RUNTIME_ERROR;
            }
          break;
        }
      case OP_MUL: BINARY_OP(NUMBER_VAL, *); break;
      case OP_DIV: BINARY_OP(NUMBER_VAL, /); break;
      case OP_NOT:
        push(BOOL_VAL(is_falsey(pop())));
        break;
      case OP_NEGATE:
        if (!IS_NUMBER(peek(0)))
        {
          runtime_error("Can't negate non-number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        push(NUMBER_VAL(-AS_NUMBER(pop())));
        break;
      case OP_RETURN:
      {
        print_value(pop());
        printf("\n");

        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP

}

InterpretResult
interpret(const char *source)
{
  Chunk chunk;

  init_chunk(&chunk);

  if (!compile(source, &chunk))
  {
    free_chunk(&chunk);
    return INTERPRET_RUNTIME_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  free_chunk(&chunk);

  return result;
}
