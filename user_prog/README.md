###### Explanation

The example relation code is the product of
deep thinking about the best way to embed
relational (logic) programming in C++.  The
design principles are:  relations are coroutines,
logic variables only store their own identity, 
values are returned packed in structs, and the 
caller controls when and how computations 
that require extra storage are handled.

From Castor, I borrowed the idea of using switch
statements to simulate coroutines as a tool
to implement relations.  Thus from the
standpoint of control flow and resolution
strategy (depth first search), they are similar.

From miniKanren, I
borrowed the idea that logic variables are
just numbers and do not store values directly
(instead, a substitution maps the number to
a value in an environment).

Traditionally a substitution is a pair, a variable
(number) and a value.  Relation results are
communicated via extending the environment 
with additional substitution pairs.

However
we take a different approach of storing
logic variables separately from result values.
Logic variables are stored in expression
objects, but values are returned packed together in 
structs, without the logic variable numbers.

The key insight is that, given a structure of
(C++, plain ol' data) values, the fact that
certain locations within that struct correspond
to which logic variables is a form of metadata.
In the same way that reflection can be used to
map (human readable) names of fields to
their values in an object, so too can mapping
logic variables to their substitution be 
considered _a kind of_ reflection operation.

The implementation is similar to the _mirror_
design pattern described by Gilaad Bracha.
The mechanism for reflection is in a separate 
object than what is being reflected upon.  In
this case, the relation expression objects
know the logic variable identities they were
constructed with and can identify these
positionally within their own result structs.

Relations in the source language transpile to
C++ classes.  They receive identities of logic
variables (the arguments to the relation) in
the constructor of the C++ class.  The definition
of the relation (calls to other relations)
corresponds to private member variables,
except where the definition is recursive.

Because the relation expression objects do
not contain any data, only identities of logic
variables and subexpressions, they can be
passed by value and are relocatable in RAM.
However they are not purely functional: the
control flow state is a mutable variable.

When the relation expression object is
constructed, it is not yet activated and must
be advanced by calling the number function. This
jumps to the corresponding case label in the
"coroutine" switch and may change the value
of the control flow state for next time.  A tree
of relation expression objects may be pictured
as being like the click wheels in a mechanical
counter.