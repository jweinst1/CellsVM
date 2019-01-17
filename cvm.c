#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Prototype for tiny functional VM
// Uses cell based approach, different than
// stack or register

#ifndef CELL_AMNT
#define CELL_AMNT 1000
#endif
// Conveience macro that prints file and line
#define PRINT_FILE_LINE printf("At line %u in '%s':\n", __LINE__, __FILE__)
#define PRINT_FILE_LINE_T(title) printf("<%s> At line %u in '%s':\n", title, __LINE__, __FILE__)

/**
 * A structure representing a function cell.
 * The cell has two fields, in and out.
 * in is the incoming value, out is the result value.
 * op is the operation to perform from in to out
 */
struct Cell
{
	int _in;
	int _op;
	int _out;
};
// This is the cell board
// Multiple cells in a row represent nested values, that should be evaluated first.
static struct Cell CELLS[CELL_AMNT];
// easy reference to size of all cells.
static const size_t CELL_MEM_SIZE = sizeof(CELLS);
// The current cell pointer 
static struct Cell* CELL_PTR = CELLS;
// Boundary pointers
static const struct Cell* const CELL_BEGIN = CELLS;
static const struct Cell* const CELL_END = CELLS + CELL_AMNT;
/* This copies the in field to the out field in order
 * to prepare for running the operation.
 * Note: the in field is not preserved if a nested call is present.
 */
#define CELL_COPY_IN_OUT(cell) (cell->_out = cell->_in)

/**
 * Byte code instruction labels
 * uses -----
 * - Stop: Terminates the running of instructions, aka exit.
 * - inc ptr: Increments the cell pointer by one. Cannot increment pointer 
 *            to end cell
 * - dec ptr: decrements the cell pointer by one. Cannot decrement before begin
 *            pointer.
 */
#define CELLINS_X_STOP       0
#define CELLINS_X_INC_PTR    1
#define CELLINS_X_DEC_PTR    2
#define CELLINS_X_PUT_IN     3
#define CELLINS_X_PUT_OUT    4
#define CELLINS_X_PUT_OP     5
#define CELLINS_X_PUT_IO     6
#define CELLINS_X_PRINT_IN   7
#define CELLINS_X_PRINT_OUT  8
#define CELLINS_X_PUSH_BACK  9
#define CELLINS_X_PUSH_NEXT  10
#define CELLINS_X_JUMP       11
#define CELLINS_X_END        12
#define CELLINS_X_INT        13
#define CELLINS_X_IN_FIELD   14
#define CELLINS_X_PLUS       15
#define CELLINS_x_SUB        16



static void 
cells_clean(void)
{
	memset(CELLS, 0, CELL_MEM_SIZE);
}

static void
cells_core_dump(int depth)
{
	int i;
	assert(depth <= CELL_AMNT);
	for(i = 0  ; i < depth ; i++)
	{
		printf("|- Cell %i: (in: %d, out: %d, op: %d) -|\n", 
			    i, 
			    CELLS[i]._in,
			    CELLS[i]._out,
			    CELLS[i]._op);
	}
}

extern unsigned
cells_run_code(unsigned char* code)
{
	 unsigned char * code_start = code;
	 int jump_c = 0;
	 goto STATE_BASE;
STATE_BASE:
     switch(*code++)
     {
     	case CELLINS_X_STOP: 
     	     goto STATE_STOP;
     	case CELLINS_X_INC_PTR:
     	     assert((++CELL_PTR) != CELL_END);
     	     goto STATE_BASE;
     	case CELLINS_X_DEC_PTR:
     	     assert((CELL_PTR--) != CELL_BEGIN);
     	     goto STATE_BASE;
		case CELLINS_X_PUT_IN:
			 CELL_PTR->_in = *(int*)(code);
			 code += sizeof(int);
			 goto STATE_BASE;
		case CELLINS_X_PUT_OUT:
			 CELL_PTR->_out = *(int*)(code);
			 code += sizeof(int);
			 goto STATE_BASE;
	    case CELLINS_X_PUT_IO:
	         // copies in value to out field
	         CELL_PTR->_out = CELL_PTR->in;
	         goto STATE_BASE;
		case CELLINS_X_PUT_OP:
			 CELL_PTR->_op = *(int*)(code);
			 code += sizeof(int);
			 goto STATE_BASE;
		case CELLINS_X_PRINT_IN:
			 printf("%d\n", CELL_PTR->_in);
			 goto STATE_BASE;
		case CELLINS_X_PRINT_OUT:
			 printf("%d\n", CELL_PTR->_out);
			 goto STATE_BASE;
		case CELLINS_X_PUSH_BACK:
			 assert(CELL_PTR != CELL_BEGIN);
			 (CELL_PTR - 1)->_in = CELL_PTR->_out;
			 goto STATE_BASE;
		case CELLINS_X_PUSH_NEXT:
			 assert((CELL_PTR + 1) != CELL_END);
			 (CELL_PTR + 1)->_in = CELL_PTR->_out;
			 goto STATE_BASE;
		case CELLINS_X_JUMP:
			 jump_c = (*(int*)(code));
			 code = code_start + jump_c;
		     goto STATE_BASE;
		case CELLINS_X_INT:
		     goto STATE_HANDLE_INT;
		case CELLINS_X_IN_FIELD:
		     goto STATE_HANDLE_IN_FIELD;
     	default:
     	     PRINT_FILE_LINE_T("base state");
     	     goto STATE_UNKNOWN_BYTE;
     }
STATE_HANDLE_IN_FIELD:
     // The handling here makes the machine look at
     // the in field as the other operand, as opposed to the bytecode stream
     // usually after a CELLINS_X_PUSH_BACK or 
     // CELLINS_X_PUSH_NEXT occurs.
     switch(CELL_PTR->op)
     {
     	case CELLINS_X_PLUS:
     	    CELL_PTR->_out += CELL_PTR->_in;
     	    goto STATE_BASE;
     	case CELLINS_x_SUB:
     	    CELL_PTR->_out -= CELL_PTR->_in;
     	    goto STATE_BASE;
     	default:
     	    PRINT_FILE_LINE_T("invalid op for in field");
     	    goto STATE_EXIT;
     }
STATE_HANDLE_INT:
     switch(CELL_PTR->_op)
     {
     	case CELLINS_X_PLUS:
     	    CELL_PTR->_out += *(int*)(code);
     	    code += sizeof(int);
     	    goto STATE_BASE;
     	case CELLINS_x_SUB:
     	    CELL_PTR->_out -= *(int*)(code);
     	    code += sizeof(int);
     	    goto STATE_BASE;
     	default:
     	    PRINT_FILE_LINE_T("invalid op for int");
     	    goto STATE_EXIT;
     }
STATE_UNKNOWN_BYTE:
     fprintf(stderr, "Error: Unknown byte: '%u' found in byte code.\n", *(code - 1));
     cells_core_dump(10);
     exit(2);
STATE_EXIT:
     cells_core_dump(30);
     exit(2);
STATE_STOP:
	 return code - code_start;
}

static unsigned char test_code[] = {
	CELLINS_X_PUT_OUT, 9, 0, 0, 0,
	CELLINS_X_PUT_OP, CELLINS_X_PLUS, 0, 0, 0,
	CELLINS_X_PRINT_OUT
};

int main(int argc, char const *argv[])
{
	cells_clean();
	cells_core_dump(10);
	cells_run_code(test_code);
	cells_core_dump(10);
	return 0;
}
