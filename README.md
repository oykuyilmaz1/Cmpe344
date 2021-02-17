# Cmpe344

### About
This project takes an assembly code (see Input File Below) and calculates the number of stalls, CPI and the total clock cycle.
### How to run
> g++ main.cpp -o main.o
> ./main.o <input-path> <output-path>
### Input File
  * You are allowed to use only the add, and, or, sub, ld, sd, and beq instructions.
  * If you want to simulate a branch taken, your rs1 and rs2 must be the same. You do not provide a label, but provide how many lines should the simulator jump, in
the label location. Example:
  > x31,x31,3 ---- branch taken, jump 3 lines
  > x2,x3,7   ---- branch not taken
### Assumptions
  * We have a forwarding unit, thus add,and,or,sub instructions do not cause a stall.
  * We have a forwarding unit, thus ld instruction causes one stall
  * We calculate the result of the beq instruction in the ALU state, so it causes 2 stalls.
  * Even if the branch is takenor not taken, pipeline stalls until its result is found.
