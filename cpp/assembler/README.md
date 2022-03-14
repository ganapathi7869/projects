# Assembler

**Introduction:**  A single pass Assembly compiler used in process of compilation takes low level assembly language as input and converts it into relocatable machine code and generating along information for the loader.
<p align="center">
  <img src="https://github.com/ganapathi7869/projects/blob/main/cpp/assembler/resources/stages.png" alt=""/>
</p>
<br/>
<img src="https://github.com/ganapathi7869/projects/blob/main/cpp/assembler/resources/assembler.png" alt=""/>
<p align="center">
  <img src="https://github.com/ganapathi7869/projects/blob/main/cpp/assembler/resources/assembler.png" alt=""/>
</p>

Generates instructions by evaluating the mnemonics (symbols) in operation field and find the value of symbol and literals to produce machine code.

 - Generates 'Symbol table' for user defined variables and arrays and their addresses in the memory.
 - Generates 'Block table' to store all the blocks in the code.
 - Handles nested conditional and jump statements in the code.
