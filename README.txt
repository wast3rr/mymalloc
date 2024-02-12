MyMalloc - CS214 P1

Members:
Karen Pedri - klp203
Richard Li - rl902

Test Plans
-----------


Test Programs Description
--------------------------
memtest.c tests whether or not malloc allows overlapping objects, which would be detrimental to the client as information can be overwritten
if malloc is called again. If this program returns 0 incorrect bytes, we can assume that mymalloc() and myfree() does not allow block payloads to overlap.

errortest.c goes through every possible error for mymalloc() and myfree(), and tests whether the errors are properly found and reported. 
Specifically, it tests the malloc contiguous space error, free on an address not at the beginning of an object error, malloc not enough 
total heap space error, double free error, and free called on an address not obtained through malloc error.
