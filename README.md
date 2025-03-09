# `wosp` - Word-oriented search and print

by Andrew Trettel


## Usage

    ./wosp < example.txt


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
