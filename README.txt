MyMalloc - CS214 P1



[ MEMBERS ]
Karen Pedri - klp203
Richard Li - rl902



[ MAJOR DESIGN NOTES ]
Both mymalloc() and myfree() operate by iterating through the entire static heap
until finding either a valid chunk to allocate, or the pointer asked the test client requested
to deallocate. 

myfree() additionally coalesces adjacent free chunks. Non-adjacent free chunks are not worried about
as if myfree() functions properly, any adjacent free chunks will be immediately coalesced.

If the test client calls malloc(0), a proper pointer is returned pointing to 0-byte chunk, with
a fully in-tact header. I chose this rather than just returning null to avoid any errors when deallocating
the pointer or, more importantly, errors related to the heap being misrepresented as uninitialized.

Due to the previous design choice, the program will check if malloc() has been previously called by first
checking if the first byte of the heap is equal to 0. Since the client can call malloc(0), if this is true
it will also loop through the entire heap using an integer pointer to verify that the heap is truly uninitialized
and there are no headers later on.

If the client attempts to allocate a negative number of bytes, the conversion to size_t will convert it to a 
very large positive unsigned integer. Therefore, the client will recieve the "not enough heap space" error
and the program will return NULL.



[ TEST PROGRAMS ]
memgrind.c - there are five stress tests in memgrind.c, none of which focus on correctness checking but rather performance checking

memtest.c - this test program ensures that bytes allocated by the client are not overwritten with new calls to malloc, and also
checks that the full heap could be properly allocated (64 objects * (56  payload size + 8 header size) = 4096 bytes)

testclient.c - this test program utilizes a switch case to take several different command line arguments (integers) to test
the errors of the library, edge cases, and general correctness of the library



[ STRESS TESTS ]
The first three stress tests of memgrind.c are the ones that were provided.
For every stress test, the average time of 50 trials are returned in ms.

Stress test 1: allocates then immediately deallocates one byte 120 times

Stress test 2: allocated a 120 byte array, then allocates 120 one-byte objects
into said array, then deallocated the objects of the array and the array itself.

Stress test 3: randomly allocates or deallocates one-byte objects in an allocated
120 byte array. Objects are only deallocated if there currently exists an object
in the array. At the end, all remaining objects of the array are freed followed
by the array itself

OUR STRESS TESTS:

Stress test 4: allocated 1000 bytes to three pointers then deallocates the three
pointers (starting from the first one allocated) 120 times.

Stress test 5: allocates 1000 bytes to three different pointers in random order,
then deallocates the three pointers 120 times.



[ CORRECTNESS TESTS ]
I've separated testing of the library into three separate categories: general 
correctness, errors, and edge cases.


GENERAL CORRECTNESS
General correctness is handled by both memtest.c and testclient.c.
As stated previously, memtest.c ensures that client data is not overwritten by separate mallocs
and that the full heap can be successfully allocated.

testclient.c cases 6 through 8 handle general correctness testing.
CASE 6 tests that client data is never corrupted, even after malloc and free fail and both functions are called several times.
It accomplishes this by first allocating the integers 1 and 2 to a 8 byte payload, then calling malloc and free on the same pointer 
50 times and purposely failing malloc and free once. Afterwards, the integers 1 and 2 should still be at their original pointers.

CASE 7 is a very basic test of both malloc and free. It calls malloc on two separate pointers then free on the same pointers.

CASE 8 tests that if a block is deallocated, it can still be reallocated properly even if it is surrounded by two
allocated blocks. It does this by allocating three different sized pointers, freeing the middle one, then re-allocating it.


ERROR TESTS
Error testing is exclusively handled by testclient.c cases 1 through 5.

CASE 1 tests the free on a pointer not obtained from malloc error.

CASE 2 tests the double free on a single pointer error.

CASE 3 tests the free on a pointer not at the beginning of a block error.

CASE 4 tests the not enough contiguous space to malloc error.

CASE 5 tests the not enough total heap space to malloc error.


EDGE CASES
The two major edge cases I came up with were malloc(0) and malloc used on a negative number.
This is handled by testclient.c cases 9 and 10.

CASE 9 tests to make sure mallocing a negative number prints an error and returns NULL.

CASE 10 tests to make sure malloc(0) returns a pointer to a 0 byte block which can be properly
freed and reallocated.
