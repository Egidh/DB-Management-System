/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "Table.h"
#include "Index.h"

int Filter_test(Filter *self, char *nodeKey)
{
    int cmp1 = strcmp(nodeKey, self->key1);
    int cmp2;
    if (self->key2 && self->requestOp == OP_BETW) cmp2 = strcmp(nodeKey, self->key2);
    else if (self->requestOp == OP_BETW) return -1;
    if (self->requestOp == OP_BETW && (strcmp(self->key1, self->key2) > 0)) {
        int tmp = cmp1;
        cmp1 = cmp2;
        cmp2 = tmp;
    }

    int res = 0;
    switch (self->requestOp)
    {
    case OP_EQ:
        if (cmp1 == 0) res |= FILTER_FOUND;
        if (cmp1 <= 0) res |= FILTER_SEARCH_RIGHT;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        break;

    case OP_LT:
        if (cmp1 < 0) res = 7;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        break;
        
    case OP_GT:
        if (cmp1 > 0) res = 7;
        if (cmp1 <= 0) res |= FILTER_SEARCH_RIGHT;
        break;

    case OP_LEQ:
        if (cmp1 <= 0) res = 7;
        if (cmp1 > 0) res |= FILTER_SEARCH_LEFT;
        break;

    case OP_GEQ:
        if (cmp1 >= 0) res = 7;
        if (cmp1 < 0) res |= FILTER_SEARCH_RIGHT;
        break;

    case OP_BETW:
        if (cmp1 >= 0 && cmp2 <= 0 ) res = 7;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        if (cmp2 <= 0) res |= FILTER_SEARCH_RIGHT;
        break;

    default:
        assert(false);
        break;
    }

    return res;
}

Table *Table_createFromCSV(char *csvPath, char *folderPath)
{
    printf("Creation de la table\n");
    //Creation et initialisation d'une structure Table
    Table *table = (Table*)calloc(1, sizeof(Table));
    if (!table)
        printf("pas de table\n");
    assert(table);
    sprintf(table->folderPath, "%s", folderPath);

    //ouverture du fichier csv
    FILE* csv = fopen(csvPath, "r");
    if (!csv)
        printf("pas de csv\n");
    assert(csv);

    //Lecture du header du fichier csv et creation du header de la table
    fscanf(csv, "%[^;];%d;\n", table->name, &table->attributeCount);
    table->attributes = (Attribute*)calloc(table->attributeCount, sizeof(Attribute));
    table->entrySize = sizeof(uint64_t);
    int* toIndex = NULL;
    toIndex = (int*)calloc(table->attributeCount, sizeof(int));
    assert(toIndex);
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;
        attribute->id = i;

        fscanf(csv, "%63[^;];%lu;%d;\n", attribute->name, &attribute->size, toIndex + i);

        table->entrySize += attribute->size;
    }

    
    fscanf(csv, "%lu;\n", &table->entryCount);

    char path[512];
    snprintf(path, 512, "%s/%s.tbl", folderPath, table->name);


    printf("Creation du fichier dat\n");
    ///ouverture du fichier dat
    snprintf(path, 512, "%s/%s.dat", folderPath, table->name);
    FILE* dat = fopen(path, "wb+");
    if (!dat)
        printf("pas de dat\n");
    assert(dat);

    table->dataFile = dat;

    //Ecriture du fichier dat
    Entry* newEntry = Entry_create(table);
    newEntry->attributeCount = table->attributeCount;
    for (int i = 0; i < table->entryCount; i++)
    {
        for (int j = 0; j < newEntry->attributeCount; j++)
        {
            memset(newEntry->values[j], 0, table->attributes[j].size);
            fscanf(csv, "%[^;];", newEntry->values[j]);
        }
        
        newEntry->nextFreePtr = -2;
        Table_writeEntry(table, newEntry, i * table->entrySize);
        fscanf(csv, "\n");
    }
    Entry_destroy(newEntry);

    table->nextFreePtr = INVALID_POINTER;
    table->validEntryCount = table->entryCount;

    fclose(csv);


    printf("Creation des index\n");
    //creation de l'index
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;

        if (toIndex[i])
        {
            attribute->index = Index_create(table, i, table->folderPath);
            //printf("%s\n", attribute->name);

            assert(attribute->index);
        }
    }
    free(toIndex);

    //Ecriture du header du fichier tbl
    Table_writeHeader(table);

    printf("Table creee\n");

    return table;
}

void Table_writeHeader(Table *self)
{
    //Initialisation du chemin d'ecriture
    char path[580];
    snprintf(path, sizeof(path), "%s/%s.tbl", self->folderPath, self->name);


    //ouverture du fichier
    FILE* tbl = fopen(path, "wb");
    if (!tbl)
        printf("pas de tbl\n");
    assert(tbl);

    //Ecriture du header du fichier tbl
    fwrite(self->name, MAX_NAME_SIZE, 1, tbl);
    fwrite(&self->attributeCount, sizeof(int), 1, tbl);

    //Ecriture du corps du fichier tbl
    for (int i = 0; i < self->attributeCount; i++)
    {
        Attribute* attribute = self->attributes + i;

        fwrite(attribute->name, MAX_NAME_SIZE, 1, tbl);
        fwrite(&attribute->size, sizeof(uint64_t), 1, tbl);
        if (attribute->index)
        {
            fwrite(&attribute->index->rootPtr, sizeof(uint64_t), 1, tbl);
            fwrite(&attribute->index->nextFreePtr, sizeof(uint64_t), 1, tbl);
        }
        else
        {
            fwrite(&(uint64_t) { -1 }, sizeof(uint64_t), 1, tbl);
            fwrite(&(uint64_t) { -1 }, sizeof(uint64_t), 1, tbl);
        }
        

    }


    fwrite(&self->entryCount, sizeof(uint64_t), 1, tbl);
    fwrite(&self->validEntryCount, sizeof(uint64_t), 1, tbl);
    fwrite(&self->nextFreePtr, sizeof(uint64_t), 1, tbl);
    fclose(tbl);

}

Table *Table_load(char *tblFilename, char *folderPath)
{
    //Initialisation du chemin de lecture
    char path[512];
    strncpy(path, folderPath, 512 - sizeof(tblFilename) - 1); //assure de ne pas avoir de buffer overflow lors des concatenations
    strcat(path, "/");
    strcat(path, tblFilename);

    //ouverture du fichier
    FILE* tbl = fopen(path, "rb");
    if (!tbl)
        printf("pas de .tbl\n");
    assert(tbl);


    //Creation et initialisation d'une structure Table
    Table* table = NULL;
    table = (Table*)calloc(1, sizeof(Table));
    if (!table)
        printf("pas de table\n");
    assert(table);
    sprintf(table->folderPath, "%s", folderPath);

    //Remplissage des donnees
    fread(table->name, MAX_NAME_SIZE, 1, tbl);
    fread(&table->attributeCount, sizeof(int), 1, tbl);
    table->attributes = (Attribute*)calloc(table->attributeCount, sizeof(Attribute));
    void* tmp = calloc(1, sizeof(uint64_t));
    table->entrySize = sizeof(EntryPointer);

    uint64_t* indexRoot = NULL;
    indexRoot = (uint64_t*)calloc(table->attributeCount, sizeof(uint64_t));
    uint64_t* indexFreePtr = NULL;
    indexFreePtr = (uint64_t*)calloc(table->attributeCount, sizeof(uint64_t));

    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;

        fread(&attribute->name, MAX_NAME_SIZE, 1, tbl);
        fread(&attribute->size, sizeof(uint64_t), 1, tbl);
        fread(indexRoot + i, sizeof(uint64_t), 1, tbl);
        fread(indexFreePtr + i, sizeof(uint64_t), 1, tbl); //skip la partie concernant les entrees libres  
        attribute->id = i;
        table->entrySize += attribute->size;
    }
    fread(&table->entryCount, sizeof(uint64_t), 1, tbl);
    fread(&table->validEntryCount, sizeof(uint64_t), 1, tbl);
    fread(&table->nextFreePtr, sizeof(uint64_t), 1, tbl);
    free(tmp);
    fclose(tbl);

    //ouverture du data file
    strcpy(path, folderPath);
    strcat(path, "/");
    strncat(path, tblFilename, strlen(tblFilename) - 4);
    strcat(path, ".dat");

    table->dataFile = fopen(path, "rb+");
    assert(table->dataFile);

    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;
        if (indexRoot[i] != INVALID_POINTER)
        {
            //printf("Loading Index\n");
            attribute->index = Index_load(table, i, table->folderPath, indexRoot[i],indexFreePtr[i]);
        }
    }
    free(indexRoot);
    return table;
}

void Table_writeEntry(Table *table, Entry *entry, EntryPointer entryPointer)
{
    // NE PAS MODIFIER
    assert(table->dataFile && entryPointer != INVALID_POINTER);

    FSeek(table->dataFile, entryPointer, SEEK_SET);
    fwrite(&(entry->nextFreePtr), sizeof(EntryPointer), 1, table->dataFile);
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;
        fwrite(entry->values[i], attribute->size, 1, table->dataFile);
    }
}

void Table_readEntry(Table *table, Entry *entry, EntryPointer entryPointer)
{
    // NE PAS MODIFIER
    assert(table->dataFile && entryPointer != INVALID_POINTER);

    FSeek(table->dataFile, entryPointer, SEEK_SET);
    fread(&(entry->nextFreePtr), sizeof(EntryPointer), 1, table->dataFile);
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;
        fread(entry->values[i], attribute->size, 1, table->dataFile);
    }
}

void Table_destroy(Table *self)
{
    if (!self) return;
    if(self->dataFile)
        fclose(self->dataFile);
    
    for (int i = 0; i < self->attributeCount; i++)
    {
        if (self->attributes[i].index) Index_destroy(self->attributes[i].index);
    }
    free(self->attributes);
    free(self);
}

void Table_search(Table *self, Filter *filter, SetEntry *resultSet)
{
    assert(self && filter);

    if (!resultSet) resultSet = SetEntry_create();

    Entry *testedEntry = Entry_create(self);
    
    Index* index = self->attributes[filter->attributeIndex].index;

    if (index)
    {
        Index_searchRec(index, index->rootPtr, filter, resultSet);
    }
    else
    {
        uint64_t length = self->validEntryCount;
        for (int i = 0; i < length; i++)
        {
            EntryPointer entryPtr = i * self->entrySize;
            Table_readEntry(self, testedEntry, entryPtr);
            if (testedEntry->nextFreePtr != VALID_ENTRY)
            {
                length++;
                continue;
            }

            if (Filter_test(filter, testedEntry->values[filter->attributeIndex]) & FILTER_FOUND)
                SetEntry_insert(resultSet, entryPtr);
        }
    }
    Entry_destroy(testedEntry);
}

void Table_combinationSearch(Table *self, Filter* filterA, Filter* filterB, Combination comb, SetEntry *resultSet)
{
    assert(self && filterA && filterB);

    if (!resultSet) resultSet = SetEntry_create();

    SetEntry* tempResA = SetEntry_create();
    SetEntry* tempResB = SetEntry_create();
    SetEntryIter* iter;
    SetEntryNode** node = (SetEntryNode**)calloc(1, sizeof(SetEntryNode*));

    switch (comb)
    {
    case OR:
        Table_search(self, filterA, resultSet);
        Table_search(self, filterB, resultSet);
        break;

    case AND:
        Table_search(self, filterA, tempResA);
        Table_search(self, filterB, tempResB);
        Table_printSearchResult(tempResA, self);
        Table_printSearchResult(tempResB, self);
        iter = SetEntryIter_create(tempResB);
        while (SetEntryIter_isValid(iter))
        {
            if (SetEntry_find(tempResA, SetEntryIter_getValue(iter), node))
                SetEntry_insert(resultSet, SetEntryIter_getValue(iter));
            
            SetEntryIter_next(iter);
        }
        SetEntryIter_destroy(iter);

        break;

    case WITHOUT:
        Table_search(self, filterA, resultSet);
        Table_search(self, filterB, tempResB);

        iter = SetEntryIter_create(tempResB);
        while (SetEntryIter_isValid(iter))
        {
            if (SetEntry_find(resultSet, SetEntryIter_getValue(iter), node))
                SetEntry_remove(resultSet, SetEntryIter_getValue(iter));

            SetEntryIter_next(iter);
        }
        SetEntryIter_destroy(iter);

        break;

    default :
        break;
    }
    SetEntry_destroy(tempResA);
    SetEntry_destroy(tempResB);
    free(node);
}

void Table_printSearchResult(SetEntry* self, Table* table)
{
    Entry* newEntry = Entry_create(table);
    SetEntryIter* iter = SetEntryIter_create(self);
    assert(iter && newEntry);

    while (SetEntryIter_isValid(iter))
    {
        EntryPointer entryPtr = SetEntryIter_getValue(iter);
        Table_readEntry(table, newEntry, entryPtr);
        Entry_print(newEntry);

        SetEntryIter_next(iter);
    }
    printf("\n");

    Entry_destroy(newEntry);
    SetEntryIter_destroy(iter);
}


void Table_insertEntry(Table *self, Entry *entry)
{
    assert(self && entry);
    
    FILE* dat = self->dataFile;
    assert(dat);
    EntryPointer newEntryPtr;
    Entry *nextFreeEntry = Entry_create(self);

    if (self->nextFreePtr == INVALID_POINTER)
    {
        newEntryPtr = self->entrySize * self->entryCount;
        Table_writeEntry(self, entry, newEntryPtr);
        self->entryCount++;
    }
    else
    {
        newEntryPtr = self->nextFreePtr;
        Table_readEntry(self, nextFreeEntry, newEntryPtr);
        Table_writeEntry(self, entry, self->nextFreePtr);
        self->nextFreePtr = nextFreeEntry->nextFreePtr;
    }
    self->validEntryCount++;


    for (int i = 0; i < self->attributeCount; i++)
    {
        Index* index = self->attributes[i].index;
        if (index)
        {
            Index_insertEntry(index, entry->values[i], newEntryPtr);

        }
    }

    Table_writeHeader(self);

    return;
}

void Table_removeEntry(Table* self, EntryPointer entryPtr)
{
    if (!self || entryPtr == INVALID_POINTER) return;
    Entry* remove = Entry_create(self);
    Table_readEntry(self, remove, entryPtr);

    for (int i = 0; i < self->attributeCount; i++)
    {
        Attribute* attribute = self->attributes + i;

        if (attribute->index) Index_removeEntry(attribute->index, remove->values[i], entryPtr);
    }
    remove->nextFreePtr = self->nextFreePtr;

    Entry_destroy(remove);
    remove = Entry_create(self);

    remove->nextFreePtr = self->nextFreePtr;
    self->nextFreePtr = entryPtr;

    Table_writeEntry(self, remove, entryPtr);
    self->validEntryCount--;

    Table_writeHeader(self);

    return;
}

void Table_debugPrint(Table *self)
{
    printf("Nom : %s\n", self->name);
    printf("Chemin du dossier : %s\n", self->folderPath);
    printf("Nombre d'attributs : %d\n", self->attributeCount);
    for (int i = 0; i < self->attributeCount; i++)
    {
        Attribute* attribute = &self->attributes[i];
        printf("Attribut %d :\n nom : %s\n", attribute->id, attribute->name);
        printf(" taille : %lu\n", attribute->size);
    }
    printf("\nNombre d'entrees : %lu\n", self->entryCount);
    printf("\nNombre d'entrees valides : %lu\n", self->validEntryCount);
    printf("\nTaille des entrees : %lu\n", self->entrySize);
    printf("Prochain emplacement libre : %lu\n", self->nextFreePtr);

    return;
}

Entry *Entry_create(Table *table)
{
    char** newValues = (char**)calloc(table->attributeCount, sizeof(char*));

    for (int i = 0; i < table->attributeCount; i++)
        newValues[i] = (char*)calloc(1, table->attributes[i].size);

    Entry* newEntry = (Entry*)calloc(1, sizeof(Entry));
    newEntry->nextFreePtr = VALID_ENTRY;
    newEntry->values = newValues;
    newEntry->attributeCount = table->attributeCount;
    

    return newEntry;
}

void Entry_destroy(Entry *self)
{
    if (!self) return;
    for (int i = 0; i < self->attributeCount; i++)
        free(self->values[i]);
    free(self->values);
    free(self);
}

void Entry_print(Entry *self)
{
    if (!self && self->nextFreePtr == VALID_ENTRY)
        return;
    for (int i = 0; i < self->attributeCount; i++)
        printf("%s\t", self->values[i]);
    printf("\n");
}

void Table_modifyEntry(Table* self, Entry* newEntry, EntryPointer entryPtr)
{
    if (!self || !newEntry || entryPtr == INVALID_POINTER)
    {
        printf("Entree non valide\n");
        return;
    }
    Table_removeEntry(self, entryPtr);
    Table_insertEntry(self, newEntry);
}
