typedef struct {
    char **items;
    size_t size;
} array;
array custom_types;

void init_array(array *arr);
void append(array *arr, const char *value);
void free_array(array *arr);

void init_array(array *arr) {
    arr->items=NULL;
    arr->size=0;
}

void append(array *arr, const char *value) {
    //arr->items=realloc(arr->items, arr->size * sizeof(char *));
    //arr->items[arr->size-1] = strdup(value); 
    arr->items = realloc(arr->items, (arr->size + 1) * sizeof(char *));
    arr->items[arr->size] = strdup(value);
    arr->size=arr->size+1;
}

void free_array(array *arr) {
    for (int i=0; i<arr->size; i++) {
        free(arr->items[i]);
    }
    free(arr->items);
}