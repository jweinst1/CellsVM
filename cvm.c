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

/**
 * Byte code instruction labels
 */
#define CELLINS_X_STOP       0
#define CELLINS_X_INC_PTR    1
#define CELLINS_X_DEC_PTR    2
#define CELLINS_X_PUT_IN     3
#define CELLINS_X_PUT_OUT    4
#define CELLINS_X_PUT_OP     5
#define CELLINS_X_PRINT_IN   6
#define CELLINS_X_PRINT_OUT  7
#define CELLINS_X_PUSH_BACK  8
#define CELLINS_X_PUSH_NEXT  9
#define CELLINS_X_CALL_OP    10
/**
 * Byte Value Type labels
 */
#define CELLINS_T_INT  1
#define CELLINS_T_PLUS 2
#define CELLINS_T_SUB  3



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

extern int
cells_run_code(unsigned char* code)
{
	while(*code != CELLINS_X_STOP)
	{
		switch(*code)
		{
			case CELLINS_X_STOP:
			   return 1;
			case CELLINS_X_INC_PTR:
			   assert((++CELL_PTR) != CELL_END);
			   break;
			case CELLINS_X_DEC_PTR:
			   assert((CELL_PTR--) != CELL_BEGIN);
			   break;
			default:
			   fprintf(stderr, "INS ERROR: Unkown instruction code byte %u\n", *code);
			   cells_core_dump(50);
			   exit(2);
		}
	}
}

int main(int argc, char const *argv[])
{
	cells_clean();
	cells_core_dump(10);
	return 0;
}
