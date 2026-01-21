struct node {
    int num;
    struct node* next;
};

struct node* mknode(int n);

char* prnlist(struct node* list, char* list_str);
int lsize(struct node* list);
int isempty(struct node* list);
void insert(struct node* list, int data);
int remove(struct node* list, int data);
void dellst(struct node** list); // deletes linked list and sets node* list to NULL (needs double pointer to set the pointer to NULL)
void endListAtNode(struct node* nodeObj); // deletes all nodes after the current node and sets nodeObj->next to NULL