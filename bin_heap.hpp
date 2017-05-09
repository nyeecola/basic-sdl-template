typedef struct
{
    int size; // total space allocated / size of a single item
    int count; // num items in heap currently
    void **data;
    bool (*compare)(void *, void *, void *);
} heap_t;

