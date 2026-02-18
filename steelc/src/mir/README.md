# MIR (Middle Intermediate Representation)

## Overview

The MIR module defines the Steel compiler's middle-level intermediate representation. It sits between the AST and the backend.

MIR is mostly designed to provide a stable interface for the backend, ensuring the backend only has to worry about translating instructions and not about additional complexities like variable scoping, type checking, or other high-level concerns. This allows the backend to focus on generating efficient machine code without wasting time rewriting the same logic for handling these concerns.

## Structure
### mir/
Contains all of the core MIR data structures and logic.
### mir/building/
Mostly just a utility interface for building MIR easily without having to worry about each specific instruction.


