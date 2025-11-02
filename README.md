# Wosp: full-text search on the command line

by Andrew Trettel

<https://www.andrewtrettel.com/>


## Summary

Wosp is a command-line program that performs full-text search on text
documents.  Wosp stands for *word-oriented search* and *print*. I created Wosp
to give me more control over how I search for information locally.  It works
differently than line-oriented search tools like grep, so it can search for
matches spanning multiple lines.  Wosp supports an expressive query language
that contains both Boolean and proximity operators.  It also supports nested
queries, trunctation, wildcard characters, and fuzzy searching.


## Installation

To install Wosp, simply run `make` and move the binary accordingly.


## Usage

The basic format of a Wosp command is as follows:

    $ wosp "QUERY" file1.txt file2.txt ...

Queries are how searchers find information using Wosp's query language.  The
query language is influenced by the [query language used at the United States
Patent and Trademark
Office](https://www.uspto.gov/patents/search/patent-public-search/operators).
The USPTO's query language is an implementation of the query language used in
[BRS/Search](https://en.wikipedia.org/wiki/BRS/Search), which was a copy of the
query language used in [IBM STAIRS](https://en.wikipedia.org/wiki/IBM_STAIRS).
That said, Wosp's query language includes many unique features not found in
previous systems.

To search for a keyword in some documents, a searcher just needs to use the
word:

    $ wosp "detective" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:816:am the only one in the world. I'm a consulting detective, if you can understand
    ...

The wildcard character is `?`.  That character will match any character in the
text.  Wosp also supports trunctuation using either the dollar sign `$` or the
number sign `#` followed optionally by a number for the maximum number of
characters to include.  This allows for powerful queries that can match many
different words with one wildcard given:

    $ wosp "#2m?n" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:286:free as an income of eleven shillings and sixpence a day will permit a man to
    ...

`#2m?n` matches "man", "men", "woman", "women", and "human" all in a single
term.

Wosp supports different case sensitivity options and fuzzy search with a given
edit distance.  The edit distance is the number of errors that Wosp will
correct.  It widens the search for keywords and can find typos, but too much
edit distance may find too many results to be useful.

The default is for case insensitive search with an edit distance of zero.
Searchers control the case sensitivity and edit distance using the search
option commands `ICASE` (insensitive case), `SCASE` (sensitive case), `LCASE`
(lowercase), `UCASE` (uppercase), and `TCASE` (title case or more specifically
start case, where the initial letter is capitalized).  These commands must
precede either a wildcard term or parentheses.

    $ wosp "TCASE holmes" A_Study_in_Scarlet.txt

    ...
    A_Study_in_Scarlet.txt:425:"Dr. Watson, Mr. Sherlock Holmes," said Stamford, introducing us.
    ...

To specify the edit distance with one of these commands, add the edit distance
number to the end of the command:

    $ wosp "ICASE1 detective" A_Study_in_Scarlet.txt

    ...
    A_Study_in_Scarlet.txt:878:months or so. It might be made a text-book for detectives to teach them what to
    ...

In this case, Wosp also returned the word "detectives" since it is one error
way from "detectives".  A better way to return plurals is to use trunctuation
like with `detective#1`, since it is much more explicit about where the extra
character should go.

Wosp support Boolean operations.  Boolean operations focus on the files and
whether they contain different terms or not.  The four Boolean operations are
`OR`, `AND`, `NOT`, and `XOR`.

`OR` returns all matches for any term given.  Like all Boolean operations, it
takes two arguments and searchers can chain together multiple `OR` statements
to produce long lists of terms to search all at once.

    $ wosp "detective#1 OR officer#1" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:260:however, with many other officers who were in the same situation as myself, and
    A_Study_in_Scarlet.txt:816:am the only one in the world. I'm a consulting detective, if you can understand what that is. Here in London we have lots of Government detectives and lots of
    ...

Searchers can string together many `OR` operations in a row.

    $ wosp "detective#1 OR officer#1 OR police OR policem?n OR constable#1" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:260:however, with many other officers who were in the same situation as myself, and
    A_Study_in_Scarlet.txt:487:might start a paper on those lines. Call it the Police News of the Past."
    ...

`AND` returns results from files that contain both terms searched for.  It is
good for limiting the number of files to be searched.  `NOT` returns results
from files that contain the first term but not the second.  `XOR` words
similarly to `OR` but only returns documents with one of the terms given in the
sequence of `XOR` operations.

Wosp supports using parentheses to nest operations.  Parentheses are on the
highest precedence level, so they are evaluated first, just like in arithmetic
operations.  Parentheses allow for searchers to create complicated queries that
join many different ideas into a single query.  This feature saves searchers
much time by preventing the needless repetition of queries with slightly
different variations.

Wosp supports many different proximity operations.  [Proximity
search](https://en.wikipedia.org/wiki/Proximity_search_(text)) is a search
technique that allows searchers to find terms that are close to each other in
the text.  Closeness typically implies that the terms are related in some
manner, so proximity search ensures that searchers find relevant terms in the
same context rather than in different parts of a long document.  In that sense,
proximity search works like a more limiting form of the Boolean `AND`.

The most basic form of proximity search is phrase search in quotes, which is
typically the only form of proximity search most search engines support today.

    $ wosp "'police constable'" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:1070:and against this wall was leaning a stalwart police constable, surrounded by a

This query is the same as `police ADJ constable`.  `ADJ` is the adjacency
operator.  It finds neighboring words in the order given.  One advantage of
using `ADJ` over quotes is that queries can support Boolean operations as
needed.  For example, using parentheses to group terms, a searcher can search
for `police ADJ (constable#1 OR officer#1 OR detective#1 OR m?n)` to broaden
their search without having to type `police` multiple times.

The other proximity operators do not have a given order like `ADJ`.  Instead,
they search in both directions within a given language element.  `NEAR`
searches neighboring words, `AMONG` searches in the same clause, `WITH`
searches in the same sentence, `ALONG` searches in the same line, and `SAME`
searches in the same paragraph.

    $ wosp "detective#1 WITH (case#1 OR evidence)" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:1360:thing which may help you in the case," he continued, turning to the two detectives. "There has been murder done, and the murderer was a man. He was
    A_Study_in_Scarlet.txt:2217:"When Mrs. Charpentier paused," the detective continued, " I saw that the whole case hung upon one point. Fixing her with my eye in a way which I always found
    ...

Like the search option operators, all proximity operators also support added
numbers at the end, and those numbers specify how many neighboring language
elements to search.  The default is one language element.  For example, the
following example uses a proximity search to within 5 words of each other:

    $ wosp "(private OR police OR government) NEAR5 (constable#1 OR officer#1 OR detective#1)" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:817:what that is. Here in London we have lots of Government detectives and lots of private ones. When these fellows are at fault they come to me, and I manage to
    A_Study_in_Scarlet.txt:1070:and against this wall was leaning a stalwart police constable, surrounded by a
    A_Study_in_Scarlet.txt:2006:"It's the Baker Street division of the detective police force," said my
    ...

Some systems allow for proximity search, but only between neighboring words.
In these systems, it is impossible to nest proximity searches using
parentheses.  Wosp allows nested proximity search without limitation.

For example, searchers can search for proximity matches in proximity to other
terms or other proximity matches.

    $ wosp "night#1 WITH (bod#3 NEAR4 victim#1)" A_Study_in_Scarlet.txt

    A_Study_in_Scarlet.txt:2264:killed him without leaving any mark. The night was so wet that no one was about, so Charpentier dragged the body of his victim into the empty house. As

Wosp's query language is designed to be expressive like this.  It is designed
for advanced users who want power and the ability to express complicated
searches with precision.

Wosp also supports negated proximity operators (`NOT WITH`, etc.).  These are
useful on occassion but in general are mostly useful for limiting overly broad
searches.

-------------------------------------------------------------------------------

Copyright (C) 2025 Andrew Trettel

See `LICENSE.md` for license details.

SPDX-License-Identifier: GPL-3.0-or-later
