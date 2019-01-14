# CellsVM
*A tiny virtual machine that uses a cell based approach*

## Intro

Traditionally, virtual machines use either a stack based approach, or a register based approach. Most native CPU's are register based, while only a few virtual machines are. Stack machines are easier to implment, but register machines offer faster performance.

### Stack Mechanics

The stack machine involves *pushing* and *popping* elements off the stack to control the contents. This operation is used for loading but can incur performance costs due to the constant resizing of the stack's memory. Additionally, evaluating nested expressions still requires new frames.
