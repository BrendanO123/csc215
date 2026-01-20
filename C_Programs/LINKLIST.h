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
void dellst(struct node* list);