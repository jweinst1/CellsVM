#include "cvml.h"
#include <assert.h>

// Core Library Functionality for Cell Virtual Machine
// Uses cells as design for a virtual machine as 
// opposed to a stack or registers.

#ifndef CELL_AMNT
#define CELL_AMNT 1000
#endif
// Conveience macro that prints file and line
#define PRINT_FILE_LINE printf("At line %u in '%s':\n", __LINE__, __FILE__)
#define PRINT_FILE_LINE_T(title) printf("<%s> At line %u in '%s':\n", title, __LINE__, __FILE__)


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



static void 
_cells_clean(void)
{
	memset(CELLS, 0, CELL_MEM_SIZE);
}

void cells_clean(void)
{
	_cells_clean();
}

static void
_cells_core_dump(int depth)
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

void cells_core_dump(int depth)
{
	_cells_core_dump(depth);
}

unsigned
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
	         CELL_PTR->_out = CELL_PTR->_in;
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
     switch(CELL_PTR->_op)
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
     _cells_core_dump(10);
     exit(2);
STATE_EXIT:
     _cells_core_dump(30);
     exit(2);
STATE_STOP:
	 return code - code_start;
}
