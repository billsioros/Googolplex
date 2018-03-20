
(a) Kept the trie's nodes' siblings lexicographically sorted in order to reduce
the insertion and look up complexity of the trie

(b) Used a heap in order to determine the top K (maxResults) relevant documents
in order to achieve a retrieval time of (worst case) O(log(info->lines))

(c) Defined and used an (File) Info struct in order to avoid redundant procedures
like determining the length of a specific document

(d) Locally defined and used a Util, list like, struct in order to avoid reallocing memory
in the file validation step

(e) For further documentation please refer the source files

ATTENTION:
In order to run the program:
(1) make
(2) cd /bin
(3) ./minisearch -i <relevant/path/to/docfile> -k K

~ Vasileios Sioros - 1115201500144 ~
