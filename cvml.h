#ifndef CVML_HEADER
#define CVML_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


/**
 * A structure representing a function cell.
 * The cell has three fields, in, out and op.
 * in is the incoming value, 
 * out is the result value.
 * op is the operation to perform from in to out
 */
struct Cell
{
	int _in;
	int _op;
	int _out;
};

/**
 * Extern wrapper of local static function to
 * clean all cells, reset state to initial for
 * vm
 */
extern void
cells_clean(void);

extern void
cells_core_dump(int depth);

/**
 * Top level function for executing byte code
 * on cells vm
 */
extern unsigned
cells_run_code(unsigned char* code);

#endif // CVML_HEADER
