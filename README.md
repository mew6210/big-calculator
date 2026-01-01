Arbitrary-precision Integer Calculator

# Features:

## Interpreter-like token processing
- [x] Lexer
- [x] Pratt Parser
- [x] Tree-walk interpreter

## Basic arithmetic operations
- [x] Addition +
- [x] Subtraction -
- [x] Multiplication *
- [x] Division /
- [x] Powers (exp function)

## Supported syntax
- [x] Expressions
	```
	2 + 3 * 6 - (2 + 4)
	//returns 14
	```
- [x] Variables
	```
	a = 2 + 3 * 6
	//a is now saved as 20
	```
- [x] Function definitions and invocations 
	```
	myFunction1(x,y,thirdVariable) = x * y + thirdVariable
	myFunction1(1,2,3)
	//returns 5
	```


## Standard library
### Program Information & Debugging
- **`help()`** - Display quick introduction and usage guide
- **`showVars()`** - List all currently declared variables and their values
- **`showFunctions()`** - Display all available standard library functions with documentation
- **`showMyFunctions()`** - List all user-defined functions
- **`lexerOutput(0|1)`** - Toggle lexer tokenization debug output
- **`parserOutput(0|1)`** - Toggle parser AST debug output

### File Operations
- **`execFile(filename)`** - Execute another file line by line
- **`save()`** - Persist current variables and functions to disk (loaded on next startup)
- **`clearSave()`** - Delete the persistent save file

### Mathematical Operations
- **`abs(x)`** - Return absolute value of `x`
- **`max(a, b)`**/**`min(a, b)`** - Return maximum/minimum of two values
- **`cmp(a, b)`** - Compare two values with descriptive output (e.g., "5 is bigger")
- **`sum(x, y, ...)`** - Calculate sum of any number of arguments
- **`prod(x, y, ...)`** - Calculate product of any number of arguments
- **`mod(a, b)`** - Return remainder of division `a / b`
- **`exp(base, exponent)`** - Exponentiation (alternative to `^` operator)
- **`cntDigits(x)`** - Count number of decimal digits in `x`

### Inspection & Analysis
- **`inspect(value, [base])`** - Display internal chunk-level representation of numbers (base 10 or 16)
  - Example: `inspect(5000000)` shows decimal chunk representation from default
  - Example: `inspect(300, 16)` shows hexadecimal chunk representation
 

## Other features
- [x] Stl grammar correction, using levenshtein distance
	```
	mx(1,2)
	//Output will be:
	Evaluator error: "mx()" function not found
	Note: Did you mean max()?
	```
- [x] Function info lookup
	```
	?max()
	//displays info about max function
	//also works for user defined functions like f(x) = 2*x
	```