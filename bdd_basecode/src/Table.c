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
    int cmp2 = strcmp(nodeKey, self->key2);

    int res = 0;
    switch (self->requestOp)
    {
    case OP_EQ:
        if (cmp1 == 0) res |= FILTER_FOUND;
        if (cmp1 <= 0) res |= FILTER_SEARCH_RIGHT;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        break;

    case OP_LT:
        if (cmp1 < 0) res |= FILTER_FOUND;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        break;
        
    case OP_GT:
        if (cmp1 > 0) res |= FILTER_FOUND;
        if (cmp1 <= 0) res |= FILTER_SEARCH_RIGHT;
        break;

    case OP_LEQ:
        if (cmp1 <= 0) res |= FILTER_FOUND;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        break;

    case OP_GEQ:
        if (cmp1 >= 0) res |= FILTER_FOUND;
        if (cmp1 <= 0) res |= FILTER_SEARCH_RIGHT;
        break;

    case OP_BETW:
        if (cmp1 >= 0 && cmp2 <= 0) res |= FILTER_FOUND;
        if (cmp1 <= 0) res |= FILTER_SEARCH_RIGHT;
        if (cmp1 >= 0) res |= FILTER_SEARCH_LEFT;
        break;

    default:
        assert(false);
        break;
    }

    return res;
}

Table *Table_createFromCSV(char *csvPath, char *folderPath)
{
    //Creation et initialisation d'une structure Table
    Table *table = NULL;
    table = (Table*)calloc(1, sizeof(Table));
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
    char* toIndex = NULL;
    toIndex = (char*)calloc(table->attributeCount, sizeof(char));
    assert(toIndex);
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;
        attribute->id = i;

        fscanf(csv, "%63[^;];%lu;%c;\n", attribute->name, &attribute->size, toIndex + i);

        table->entrySize += attribute->size;
    }

    
    fscanf(csv, "%lu;\n", &table->entryCount);

    char path[512];
    snprintf(path, 512, "%s/%s.tbl", folderPath, table->name);



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

    fclose(csv);

    //creation de l'index
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;

        if (toIndex[i])
        {
            attribute->index = Index_create(table, i, table->folderPath);
            printf("%s\n", attribute->name);

            assert(attribute->index);
        }
    }
    free(toIndex);

    //Ecriture du header du fichier tbl
    Table_writeHeader(table);

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
        fwrite(&attribute->index->rootPtr, sizeof(uint64_t), 1, tbl);
        fwrite(&(uint64_t) { -1 }, sizeof(uint64_t), 1, tbl);

    }


    fwrite(&self->entryCount, sizeof(uint64_t), 1, tbl);
    fwrite(&(uint64_t) { -1 }, sizeof(uint64_t), 1, tbl);
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
    for (int i = 0; i < table->attributeCount; i++)
    {
        Attribute* attribute = table->attributes + i;

        fread(&attribute->name, MAX_NAME_SIZE, 1, tbl);
        fread(&attribute->size, sizeof(uint64_t), 1, tbl);
        fread(indexRoot + i, sizeof(uint64_t), 1, tbl);
        fread(tmp, sizeof(uint64_t), 1, tbl); //skip la partie concernant les entrees libres  
        attribute->id = i;
        table->entrySize += attribute->size;
    }
    fread(&table->entryCount, sizeof(uint64_t), 1, tbl);
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
            printf("Loading Index\n");
            attribute->index = Index_load(table, i, table->folderPath, indexRoot[i], -1);
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
    char nodeKey[512];
    uint64_t size = self->attributes[filter->attributeIndex].size;
    int dataLength = 8; //huit premiers octets (pointeur)
    uint64_t start = 8;
    for (int i = 0; i < self->attributeCount; i++)
    {
        if (i < filter->attributeIndex)
            start += self->attributes[i].size;

        dataLength += self->attributes[i].size;
    }
    dataLength -= size;

    FILE* dat = NULL;
    dat = self->dataFile;
    assert(dat && "dat missing");

    FSeek(dat, start, SEEK_SET);
   
    int check;

    while ((check = fread(nodeKey, 1, size, dat)) > 0)
    {
        if (Filter_test(filter, nodeKey) & FILTER_FOUND)
        {
            EntryPointer pos = ftell(dat) - start - size;
            SetEntry_insert(resultSet, pos);
        }
        FSeek(dat, dataLength, SEEK_CUR);
    }
}

void Table_printSearchResult(SetEntry* self, Table* table)
{
    Entry* newEntry = Entry_create(table);
    SetEntryIter* iter = SetEntryIter_create(self);
    assert(iter && newEntry);

    while (SetEntryIter_isValid(iter))
    {
        Table_readEntry(table, newEntry, iter->curr->data);
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

    if (self->nextFreePtr == INVALID_POINTER)
    {
        EntryPointer newEntryPtr = self->entrySize * self->entryCount;
        Table_writeEntry(self, entry, newEntryPtr);
        self->entryCount++;
    }
    else
        Table_writeEntry(self, entry, self->nextFreePtr);


    Table_writeHeader(self);
    return;
}

void Table_removeEntry(Table* self, EntryPointer entryPtr)
{
    assert(self && entryPtr != INVALID_POINTER);
    FILE* dat = self->dataFile;
    FSeek(dat, entryPtr, SEEK_SET);
    fwrite(&(uint64_t) { -2 }, sizeof(uint64_t), 1, dat);
    fwrite(&(uint64_t) { 0 }, sizeof(uint64_t), 1, dat);
    if ((self->entrySize - sizeof(uint64_t)) > 0)
        fwrite(&(char) {0}, 1, (self->entrySize - sizeof(uint64_t)), dat);

    EntryPointer* tmp = &self->nextFreePtr;
    if (*tmp != -1)
    {
        while (*tmp != -2)
        {
            FSeek(dat, *tmp, SEEK_SET);
            fread(tmp, sizeof(EntryPointer), 1, dat);
        }
        FSeek(dat, -sizeof(EntryPointer), SEEK_CUR);
        fwrite(&entryPtr, sizeof(EntryPointer), 1, dat);
    }
    else
        self->nextFreePtr = entryPtr;

    self->entryCount--;

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
    newEntry->nextFreePtr = table->nextFreePtr;
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
    if (!self)
        return;
    for (int i = 0; i < self->attributeCount; i++)
        printf("%s\t", self->values[i]);
    printf("\n");
}
