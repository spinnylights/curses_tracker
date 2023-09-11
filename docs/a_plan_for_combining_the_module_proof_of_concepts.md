# A plan for combining the module proofs-of-concept
9/02/23

So, I have two proofs-of-concept that together sketch out a full
design for the module system, except for the interpreter. (The
interpreter will probably follow the same tack as the curve
interpreter, and maybe will provide guidance together with that
interpreter for making a more general and reusable interpreter
interface. In a way, you can look at the existing curves
interpreter as a proof of concept for that part, is I guess what
I'm saying.)

Anyway, one of the proofs-of-concept is for the
actual graph traversal algorithm, which computes the right order
to update all the modules in based on their connections (even if
there are cycles). I based my work on:

* _Introduction to Algorithms_, 3rd Ed., Tomas H. Cormen et.
  al., MIT Press, 2009, Chapter 22 "Elementary Graph Algorithms,"
  pgs. 589–623 (especially Section 22.3 "Depth-first search,"
  pgs. 603–612, and Section 22.5 "Strongly connected components,"
  pgs. 615–621)
* _Data Structures and Network Algorithms_, Robert Endre Tarjan,
  Society for Industrial and Applied Mathematics, 1983, Chapter
  1 "Foundations," Section 1.5 "Trees and Graphs," pgs. 14–19
  (especially depth-first search on pg. 17)
* _The Art of Computer Programming_, Vol. 1, Donald Knuth,
  Addison-Wesley, 1997, Chapter 2 "Information Structures,"
  Section 2.2 "Linear Lists," Subsection 2.2.3 "Linked
  Allocation," pgs. 254–273

The other proof of concept expresses the module and port
implementations and interfaces as C++ types. It does this in a
way that provides for type-safe connections between ports with
arbitrarily-typed data, has low overhead in runtime and space
terms, and lays the groundwork for modules delivered in user
plug-ins that are loaded dynamically. For that, I relied heavily
on "Part III: Abstraction Mechanisms" (pgs. 449–827) of _The C++
Programming Language_, 4th. Ed., Bjarne Stroustrup, Pearson
Education, 2013.

Now that I have these, the puzzle is how to bring them together
in the main program. Mercifully I doubt this will be as
challenging as it was to write the proofs-of-concept. The main
question, I think, is where to incorporate the graph traversal
data and behavior in the module and port types, and what
modifications need to be made to the graph traversal behavior to
actually fit the main program.

It seems fine to me, at least at this point, to keep the d/f/seen
data in `Modular` and expose some sort of interface to it in
`Module` (or the like). `Module`/`Modular` should probably also
include some way to report upstream and downstream modules based
on the port connections, and this might require some adjustment
to the port connection logic in the types proof of concept. Other
than that, I think the main difference from the behavior in the
traversal proof of concept will be that `Modules` (which I guess
might take the place of `Synth`, at least to some extent) will
need to store an ordered list of all the modules in update order,
probably by pointer, instead of just writing their names to
stdout.

Just for an easy summary:

* d/f/seen in `Modular`, expose interface in `Module`
* Include upstream/downstream tracking in port connection logic
* `Modules` stores an ordered list of all the modules in "update
  order," probably by pointer
* `Modules` will probably take on at least part of the
  functionality in `Synth`; I'll have to think more on that
