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
#define CELLINS_X_PRINT_IN   6
#define CELLINS_X_PRINT_OUT  7
#define CELLINS_X_PUSH_BACK  8
#define CELLINS_X_PUSH_NEXT  9
#define CELLINS_X_CALL_OP    10
#define CELLINS_X_JUMP       11
#define CELLINS_X_END_OP     12
#define CELLINS_X_INT        13
#define CELLINS_X_PLUS       14
#define CELLINS_x_SUB        15



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
	 const unsigned char * const code_start = code;

	 return code - code_start;
}

int main(int argc, char const *argv[])
{
	cells_clean();
	cells_core_dump(10);
	return 0;
}
