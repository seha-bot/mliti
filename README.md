# mliti

Boolean logic simplifier. It only simplifies until [DNF](https://en.wikipedia.org/wiki/Disjunctive_normal_form). If multiple equivalent DNF forms exist for a given input, any one could be returned, but consistently.

This project is not meant to showcase anything, it was for educational purposes only.

The original goal was to help my classmates with their logics classes with this program by generating a series of steps to turn their input into DNF, but I couldn't make it.
My block was inputs with more than 3 variables because the rules I had for simplifications just didn't apply for some cases.
This may change sometimes in the future when I get smarter.

## Syntax

You may only use single uppercase characters for variable names.

Valid constructs:

- Variable: `A`, `B`, `C`, ..., `Z`;
- Parentheses: `(anything here)`;
- Conjunction: both `AB` and `A and B` mean the same thing;
- Disjunction: `A or B`;
- Negation: `nA` (only a single `n` is allowed per expression. `nnA` is invalid, use `n(nA)` instead).

Precedence (highest to lowest):

1. Parentheses
2. Negation
3. Conjunction
4. Disjunction

## Usage

For more options use `mliti -h`.

By simply calling `mliti` with an expression, it will print the simplified version:

```sh
$ mliti 'AnA'
f
$ mliti 'A or nA'
t
$ mliti 'AnA or (B or C)'
C or B
```
