# `wosp` - Word-oriented search and print

by Andrew Trettel


## Usage

    wosp "QUERY" file1.txt file2.txt ...

For example,

    wosp "((assent OR process) ADJ5 (law\$1 OR act\$1)) OR (right\$1 AMONG (people OR person\$1))" declaration.txt constitution.txt


## Query language

The query language is based on the query language used at the
[USPTO](https://www.uspto.gov/patents/search/patent-public-search/operators).
However, `wosp` extends the language with two additional proximity operators.

Currently supported operators:

- Boolean operators: `AND`, `OR`, `NOT`, and `XOR`.

- Proximity operators: `ADJ`, `NEAR`, `AMONG`, `ALONG`, `WITH`, `SAME`, `NOT
  ADJ`, `NOT NEAR`, `NOT AMONG`, `NOT ALONG`, `NOT WITH`, and `NOT SAME`.

Negated proximity operators work with and without space between `NOT` and the
proximity operator (that is, `NOTADJ` is valid).


## How does it work?

1. `wosp` reads each word of a source document and puts them into a
doubly-linked list.

2. `wosp` builds a trie data structure to index the locations of all words.
The trie allows for fast queries of all of locations of a given word, and also
allows for the handling of wildcard characters in words.

3. Users then can query the trie and linked list for words, creating "matches".
A match is a continuous series of words.  Each match keeps track of the
previously searched for words.  Using matches, users can create complicated
queries to search for specific concepts in a text document.

-------------------------------------------------------------------------------

Copyright (C) 2025 Andrew Trettel
