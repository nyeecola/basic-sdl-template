heap_t *create_heap(int initial_size, bool (*compare)(void *, void *, void *))
{
    assert(initial_size > 0);

    heap_t *heap = (heap_t *) malloc(sizeof(*heap));
    assert(heap);

    heap->size = initial_size;
    heap->count = 0;
    heap->data = (void **) calloc(initial_size, sizeof(*(heap->data)));
    heap->compare = compare;

    return heap;
}

void push_heap(heap_t *heap, void *item, void *cmp_data)
{
/*
    if (heap->count == heap->size)
    {
        heap->size *= 2;
        heap->data = (void **) realloc(heap->data, sizeof(*(heap->data)) * heap->size);
        assert(heap->data);
    }
*/

    int i = heap->count;
    heap->count++;
    int parent;
    while (i)
    {
        parent = (i - 1) / 2;

        if (heap->compare(cmp_data, heap->data[parent], item))
        {
            break;
        }

        heap->data[i] = heap->data[parent];

        i = parent;
    }
    heap->data[i] = item;
}

void *pop_heap(heap_t *heap, void *cmp_data)
{
    void *result = heap->data[0];

    printf("result %p\n", result);
    heap->count--;
    int i = 0;
    int i_to_swap;
    int i_child;
    while(true)
    {
        // first child
        i_to_swap = (i * 2) + 1;
        if (i_to_swap >= heap->count)
        {
            break;
        }

        // second child
        i_child = i_to_swap + 1;
        if ((i_child < heap->count) && heap->compare(cmp_data, heap->data[i_child], heap->data[i_to_swap]))
        {
            i_to_swap = i_child;
        }

        if (heap->compare(cmp_data, heap->data[heap->count], heap->data[i_to_swap]))
        {
            break;
        }

        heap->data[i] = heap->data[i_to_swap];
        i = i_to_swap;
    }
    printf("i %d\n", i);
    heap->data[i] = heap->data[heap->count];

    printf("result %p\n", result);
    return result;
}

void destroy_heap(heap_t **heap)
{
    assert(heap);
    assert(*heap);

    free((*heap)->data);
    free(*heap);
    *heap = 0;
}
