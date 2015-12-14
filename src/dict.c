
#include <string.h>
#include <stdlib.h>
#include <dict.h>

dict_t* dict()
{
    dict_t* d = malloc(sizeof(dict_t));
    d->size = 0;
    return d;
}

bool dict_in(dict_t* dict, char* key)
{
    for(int i = 1;i <= dict->size;i++)
        if(strcmp(key, dict->entry[i]->key) == 0)
            return true;
    return false;
}

bool dict_add(dict_t* dict, char* key, void* value)
{
    if(dict_in(dict, key)) return false;
    dict->size++;
    dict->entry[dict->size] = malloc(sizeof(dict_entry_t));
    dict->entry[dict->size]->key = key;
    dict->entry[dict->size]->value = value;
    return true;
}

void* dict_value(dict_t* dict, char* key)
{
    for(int i = 1;i <= dict->size;i++)
        if(strcmp(key, dict->entry[i]->key) == 0)
            return dict->entry[i]->value;
    return (void*) -1;
}

void dict_copy(dict_t* from, dict_t* to)
{
    for(int i = 1;i <= from->size;i++)
        dict_add(to, from->entry[i]->key, from->entry[i]->value);
}

void dict_free(dict_t* dict)
{
    for(int i = 1;i <= dict->size;i++)
        free(dict->entry[i]);
    free(dict);
}

void dict_remove(dict_t* d, char* key)
{
    for(int i = 0, c = 0;i <= d->size + 1;i++)
    {
        if(strcmp(d->entry[i]->key, key) == 0)
        {
            free(d->entry[i]);
            d->size--;
        }
        else
        {
            d->entry[c] = d->entry[i];
            c++;
        }
    }
}


