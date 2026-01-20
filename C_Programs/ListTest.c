#include "LINKLIST.H"
#include "BDSCTEST.H"
#define NULL 0

main() {
    START_TESTING("LINKLIST.C");

    TEST_CASE("Test can create node") {
        struct node* mynode;
        mynode = mknode(42);
        ASSERT(mynode->num == 42);
        ASSERT(mynode->next == NULL);
    }

    END_TESTING();
}