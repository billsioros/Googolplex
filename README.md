
# Googolplex - A C++ document search engine

* Kept the trie's nodes' siblings lexicographically sorted in order to reduce
the insertion and look up complexity of the trie

* Used a heap in order to determine the top K (maxResults) relevant documents
in order to achieve a retrieval time of (worst case) O(log(info->lines))

* Defined and used an (File) Info struct in order to avoid redundant procedures
like determining the length of a specific document

* Locally defined and used a Util, list like, struct in order to avoid reallocing memory
in the file validation step

* For further documentation please refer to the source files

COMPILE & RUN:
In order to run the program:
* mkdir bin
* make
* cd /bin
* ./minisearch -i relevant/path/to/docfile -k <maxResults>

~ billsioros ~
