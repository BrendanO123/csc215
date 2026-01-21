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

    TEST_CASE("Test dellst"){
        struct node* list;
        list = mknode(42);
        struct node* mynode1; mynode1 = mknode(12);
        list->next = mynode1;

        ASSERT(list->num == 42);
        ASSERT(list->next == mynode1);
        ASSERT(mynode1->num == 12);
        ASSERT(mynode1->next == NULL);

        endListAtNode(list);
        ASSERT(list->next == NULL);

        dellst(&list);
        ASSERT(list == NULL);
    }

    END_TESTING();
}