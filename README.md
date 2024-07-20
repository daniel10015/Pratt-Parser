# Pratt Parser (C++)
A very simple parser that only handles operator precedence for mathematical expressions. I made this to be used as an API in a variety of contexts whenever mathematical expressions are required to be parsed. Currently it only generates an AST, so I'm considering making another repo to compute these ASTs focusing on numerical methods. 

## Why a [pratt parser](https://en.m.wikipedia.org/w/index.php?title=Operator-precedence_parser&diffonly=true#Pratt_parsing) instead of a [bottom-up parser](https://en.wikipedia.org/wiki/Shunting_yard_algorithm)
It's much easier to extend the the rules for the parser, such as adding new operators, and the implementation is much more concise so there's fewer moving parts. There's no reduction in a bottom-up fashion, only shifting nodes directly from the input. 
