/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "Index.h"
#include "Table.h"

// Prototypes
int64_t Index_getNodeHeight(Index *self, NodePointer nodePtr);
int Index_getNodeBalance(Index *self, NodePointer nodePtr);
void Index_updateNode(Index *self, NodePointer nodePtr);
void Index_setLeftNode(Index *self, NodePointer nodePtr, NodePointer leftPtr);
void Index_setRightNode(Index *self, NodePointer nodePtr, NodePointer rightPtr);
NodePointer Index_getSubtreeMaximum(Index *self, NodePointer nodePtr);
void Index_replaceChild(
    Index *self, NodePointer parentPtr,
    NodePointer currChild, NodePointer newChild);
void Index_rotateLeft(Index *self, NodePointer nodePtr);
void Index_rotateRight(Index *self, NodePointer nodePtr);
void Index_balance(Index *self, NodePointer nodePtr);

void Index_readNode(Index *self, IndexNode *indexNode, NodePointer nodePtr)
{
    // NE PAS MODIFIER
    assert(self->indexFile && nodePtr != INVALID_POINTER);

    FSeek(self->indexFile, nodePtr, SEEK_SET);
    size_t size = sizeof(IndexNode) - MAX_INDEX_ATTRIBUTE_SIZE + self->attributeSize;
    fread(indexNode, size, 1, self->indexFile);
    indexNode->key[MAX_INDEX_ATTRIBUTE_SIZE - 1] = '\0';
}

void Index_writeNode(Index *self, IndexNode *indexNode, NodePointer nodePtr)
{
    // NE PAS MODIFIER
    assert(self->indexFile && nodePtr != INVALID_POINTER);

    FSeek(self->indexFile, nodePtr, SEEK_SET);
    size_t size = sizeof(IndexNode) - MAX_INDEX_ATTRIBUTE_SIZE + self->attributeSize;
    fwrite(indexNode, size, 1, self->indexFile);
}

NodePointer Index_createNode(Index *self, char *key, EntryPointer entryPtr)
{
    // NE PAS MODIFIER
    IndexNode node = { 0 };
    node.nextFreePtr = INVALID_POINTER;
    node.parentPtr = INVALID_POINTER;
    node.leftPtr = INVALID_POINTER;
    node.rightPtr = INVALID_POINTER;

    node.height = 0;
    strncpy(node.key, key, MAX_INDEX_ATTRIBUTE_SIZE);
    node.key[MAX_INDEX_ATTRIBUTE_SIZE - 1] = '\0';
    node.entryPtr = entryPtr;

    NodePointer nodePtr = self->nextFreePtr;
    if (nodePtr != INVALID_POINTER)
    {
        IndexNode freeNode = { 0 };
        Index_readNode(self, &freeNode, nodePtr);
        self->nextFreePtr = freeNode.nextFreePtr;

        FSeek(self->indexFile, nodePtr, SEEK_SET);
    }
    else
    {
        FSeek(self->indexFile, 0, SEEK_END);
        nodePtr = FTell(self->indexFile);
    }

    Index_writeNode(self, &node, nodePtr);

    return nodePtr;
}

void Index_destroyNode(Index *self, NodePointer nodePtr)
{
    IndexNode node = { 0 };
    node.nextFreePtr = self->nextFreePtr;
    self->nextFreePtr = nodePtr;
    Index_writeNode(self, &node, nodePtr);
}

Index *Index_create(Table *table, int attributeIndex, char *folderPath)
{
    assert(table);
    Index* newIndex = (Index*)calloc(1, sizeof(Index));
    assert(newIndex);

    newIndex->table = table;
    newIndex->attributeIndex = attributeIndex;
    newIndex->attributeSize = table->attributes[attributeIndex].size;
    newIndex->rootPtr = INVALID_POINTER;
    newIndex->nextFreePtr = INVALID_POINTER;

    char path[512];
    sprintf(path, "%s/%s_%d.idx", folderPath, table->name, attributeIndex);
    FILE *idx = fopen(path, "wb+");
    assert(idx);

    newIndex->indexFile = idx;
    Entry* newEntry = Entry_create(table);
    uint64_t length = table->validEntryCount;
    for (int i = 0; i < length; i++)
    {
        EntryPointer entryPointer = i * table->entrySize;
        Table_readEntry(table, newEntry, entryPointer);
        if(newEntry->nextFreePtr != VALID_ENTRY)
        {
            printf("skipped\n");
            length++;
            continue;
        }
        Index_insertEntry(newIndex, newEntry->values[attributeIndex], entryPointer);
    }

    Entry_destroy(newEntry);
    
    return newIndex;
}

void Index_destroy(Index* self)
{
    if (!self) return;
    fclose(self->indexFile);
    free(self);
}

Index *Index_load(
    Table* table, int attributeIndex, char* folderPath,
    NodePointer rootPtr, NodePointer nextFreePtr)
{
    if(rootPtr == INVALID_POINTER)
    {
        printf("Invalid pointer to root !\n");
        return NULL;
    }
    char path[512];
    sprintf(path, "%s/%s_%d.idx", folderPath, table->name, attributeIndex);
    FILE* idx = fopen(path, "rb+");
    assert(idx);

    Index* newIndex = (Index*)calloc(1, sizeof(Index));
    newIndex->attributeIndex = attributeIndex;
    newIndex->attributeSize = table->attributes[attributeIndex].size;
    newIndex->indexFile = idx;
    newIndex->nextFreePtr = nextFreePtr;
    newIndex->rootPtr = rootPtr;
    newIndex->table = table;

    return newIndex;
}



void Index_insertEntry(Index* self, char* key, EntryPointer entryPtr)
{
    NodePointer nodePtr; 
    IndexNode node;

    if (self->nextFreePtr != INVALID_POINTER)
    {
        nodePtr = self->nextFreePtr;
        Index_readNode(self, &node, nodePtr);
        nodePtr = Index_createNode(self, key, entryPtr);
        self->nextFreePtr = node.nextFreePtr;
    }
    else
        nodePtr = Index_createNode(self, key, entryPtr);

    

    if (self->rootPtr == INVALID_POINTER)
    {
        self->rootPtr = nodePtr;
        Index_updateNode(self, self->rootPtr);
        return;
    }

    // Recherche le parent parmi les feuilles
    NodePointer parentPtr = self->rootPtr;
    IndexNode parent;
    while (true)
    {
        Index_readNode(self, &parent, parentPtr);
        NodePointer childPtr =
            strcmp(key, parent.key) <= 0 ?
            parent.leftPtr : parent.rightPtr;

        if (childPtr == INVALID_POINTER) break;

        parentPtr = childPtr;
    }

    // Ajoute le nouveau noeud
    if (strcmp(key, parent.key) <= 0)
    {
        Index_setLeftNode(self, parentPtr, nodePtr);
    }
    else
    {
        Index_setRightNode(self, parentPtr, nodePtr);
    }

    // Rééquilibre l'arbre
    Index_balance(self, nodePtr);
}

int64_t Index_getNodeHeight(Index *self, NodePointer nodePtr)
{
    if (nodePtr == INVALID_POINTER) return -1;
    IndexNode node;
    Index_readNode(self, &node, nodePtr);
    return node.height;
}

int Index_getNodeBalance(Index *self, NodePointer nodePtr)
{
    if (nodePtr == INVALID_POINTER) return 0;
    IndexNode node;
    Index_readNode(self, &node, nodePtr);


    int hr = Index_getNodeHeight(self, node.rightPtr);
    int hl = Index_getNodeHeight(self, node.leftPtr);
    return hr - hl;
}

void Index_updateNode(Index *self, NodePointer nodePtr)
{
    if (nodePtr == INVALID_POINTER) return;
    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    int hr = Index_getNodeHeight(self, node.rightPtr);
    int hl = Index_getNodeHeight(self, node.leftPtr);
    node.height = 1 + (hr > hl ? hr : hl);

    Index_writeNode(self, &node, nodePtr);
}

void Index_setLeftNode(Index *self, NodePointer nodePtr, NodePointer leftPtr)
{
    // Fonction d'exemple

    assert(nodePtr != INVALID_POINTER);

    IndexNode node;
    Index_readNode(self, &node, nodePtr);
    node.leftPtr = leftPtr;
    Index_writeNode(self, &node, nodePtr);

    if (leftPtr != INVALID_POINTER)
    {
        IndexNode left;
        Index_readNode(self, &left, leftPtr);
        left.parentPtr = nodePtr;
        Index_writeNode(self, &left, leftPtr);
    }
}

void Index_setRightNode(Index *self, NodePointer nodePtr, NodePointer rightPtr)
{
    assert(nodePtr != INVALID_POINTER);

    IndexNode node;
    Index_readNode(self, &node, nodePtr);
    node.rightPtr = rightPtr;
    Index_writeNode(self, &node, nodePtr);

    if (rightPtr != INVALID_POINTER)
    {
        IndexNode right;
        Index_readNode(self, &right, rightPtr);
        right.parentPtr = nodePtr;
        Index_writeNode(self, &right, rightPtr);
    }
}


NodePointer Index_getSubtreeMaximum(Index *self, NodePointer nodePtr)
{
    return INVALID_POINTER;
}

void Index_replaceChild(
    Index *self, NodePointer parentPtr,
    NodePointer currChildPtr, NodePointer newChildPtr)
{

    if (parentPtr == INVALID_POINTER)
    {
        self->rootPtr = newChildPtr;
        Index_updateNode(self, self->rootPtr);
    }
    else
    {
        IndexNode parent;
        Index_readNode(self, &parent, parentPtr);

        if (parent.leftPtr == currChildPtr)
        {
            parent.leftPtr = newChildPtr;
            Index_writeNode(self, &parent, parentPtr);
        }
            
        else if (parent.rightPtr == currChildPtr)
        {
            parent.rightPtr = newChildPtr;
            Index_writeNode(self, &parent, parentPtr);
        }
        else
            assert(false);

        Index_writeNode(self, &parent, parentPtr);
    }

    if (newChildPtr != INVALID_POINTER)
    {
        IndexNode newChild;
        Index_readNode(self, &newChild, newChildPtr);
        newChild.parentPtr = parentPtr;
        Index_writeNode(self, &newChild, newChildPtr);
    }
}

void Index_rotateLeft(Index *self, NodePointer nodePtr)
{
    assert(nodePtr != INVALID_POINTER);

    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    NodePointer parentPtr = node.parentPtr;
    NodePointer rightPtr = node.rightPtr;

    IndexNode right;
    Index_readNode(self, &right, rightPtr);

    Index_setRightNode(self, nodePtr, right.leftPtr);
    Index_setLeftNode(self, rightPtr, nodePtr);
    Index_replaceChild(self, parentPtr, nodePtr, rightPtr);

    // Met à jour les hauteurs
    Index_updateNode(self, nodePtr);
    Index_updateNode(self, rightPtr);
}

void Index_rotateRight(Index *self, NodePointer nodePtr)
{
    assert(nodePtr != INVALID_POINTER);

    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    NodePointer parentPtr = node.parentPtr;
    NodePointer leftPtr = node.leftPtr;

    IndexNode left;
    Index_readNode(self, &left, leftPtr);

    Index_setLeftNode(self, nodePtr, left.rightPtr);
    Index_setRightNode(self, leftPtr, nodePtr);
    Index_replaceChild(self, parentPtr, nodePtr, leftPtr);

    // Met à jour les hauteurs
    Index_updateNode(self, nodePtr);
    Index_updateNode(self, leftPtr);
}

void Index_balance(Index *self, NodePointer nodePtr)
{
    assert(self && nodePtr != INVALID_POINTER);

    // Remonte éventuellement jusqu'à la racine pour rééquilibrer l'arbre
    while (nodePtr != INVALID_POINTER)
    {
        Index_updateNode(self, nodePtr);
        int balance = Index_getNodeBalance(self, nodePtr);

        IndexNode node;
        Index_readNode(self, &node, nodePtr);

        if (balance == 2)
        {
            int rightBalance = Index_getNodeBalance(self, node.rightPtr);
            if (rightBalance == -1)
            {
                Index_rotateRight(self, node.rightPtr);
            }
            Index_rotateLeft(self, nodePtr);
        }
        else if (balance == -2)
        {
            int leftBalance = Index_getNodeBalance(self, node.leftPtr);
            if (leftBalance == 1)
            {
                Index_rotateLeft(self, node.leftPtr);
            }
            Index_rotateRight(self, nodePtr);
        }

        nodePtr = node.parentPtr;
    }
}

NodePointer Index_maximum(Index *self, NodePointer nodePtr)
{
    assert(self);

    if (nodePtr == INVALID_POINTER) return INVALID_POINTER;

    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    while (node.rightPtr != INVALID_POINTER)
    {
        nodePtr = node.rightPtr;
        Index_readNode(self, &node, nodePtr);
    }
    return nodePtr;
}

void Index_removeEntry(Index *self, char *key, EntryPointer entryPtr)
{
    assert(self);

    //if (self->rootPtr == INVALID_POINTER)
    //{
    //    printf("Racine de l'index invalide\n");
    //    return;
    //}

    NodePointer nodePtr = INVALID_POINTER;
    nodePtr = Index_searchEntry(self, key, entryPtr);
    if ( nodePtr == INVALID_POINTER)
    {
        printf("Entree non trouvée\n");
        return;
    }

    IndexNode node;
    Index_readNode(self, &node, nodePtr);
    assert(nodePtr != INVALID_POINTER);

    NodePointer start = INVALID_POINTER;
    if (node.leftPtr == INVALID_POINTER)
    {
        // Remplacement par le fils droit
        Index_replaceChild(self, node.parentPtr, nodePtr, node.rightPtr);
        start = node.parentPtr;
        Index_destroyNode(self, nodePtr);
    }
    else if (node.rightPtr == INVALID_POINTER)
    {
        // Remplacement par le fils gauche
        Index_replaceChild(self, node.parentPtr, nodePtr, node.leftPtr);
        start = node.parentPtr;
        Index_destroyNode(self, nodePtr);
    }
    else
    {
        // Le noeud a deux fils
        // On l'échange avec sa valeur immédiatement inférieure (qui n'a plus de fils droit)
        NodePointer maxLeftPtr = Index_maximum(self, node.leftPtr);
        IndexNode maxLeft;
        Index_readNode(self, &maxLeft, maxLeftPtr);

        //on transfere les données de maxleft à node
        node.entryPtr = maxLeft.entryPtr;
        strncpy(node.key, maxLeft.key, self->attributeSize);
        Index_writeNode(self, &node, nodePtr);

        // Puis on le supprime comme précédemment
        Index_replaceChild(self, maxLeft.parentPtr, maxLeftPtr, maxLeft.leftPtr);
        start = maxLeft.parentPtr;
        Index_destroyNode(self, maxLeftPtr);
    }

    // Equilibre l'arbre à partir du parent du noeud supprimé
    if(start != INVALID_POINTER)
        Index_balance(self, start);


    return;
}

void Index_debugPrintRec(Index *self, NodePointer nodePtr, int depth, int isLeft)
{
    if (nodePtr == INVALID_POINTER) return;

    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    Index_debugPrintRec(self, node.rightPtr, depth + 1, false);

    for (int i = 0; i < depth - 1; i++) printf("          ");
    if (depth > 0)
    {
        if (isLeft) printf("  \\-");
        else printf("  /-");
    }
    printf("|%s : %ld|\n", (node.key), node.height);

    Index_debugPrintRec(self, node.leftPtr, depth + 1, true);
}

void Index_debugPrint(Index *self, int depth, NodePointer nodePtr)
{
    if (nodePtr == INVALID_POINTER) return;
    Index_debugPrintRec(self, nodePtr, 0, true);
}

void Index_searchRec(Index *self, NodePointer nodePtr, Filter *filter, SetEntry *resultSet)
{
    if (nodePtr == INVALID_POINTER) return;
    
    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    int res = Filter_test(filter, node.key);
    if (res & FILTER_FOUND) SetEntry_insert(resultSet, node.entryPtr);
    if (res & FILTER_SEARCH_LEFT) Index_searchRec(self, node.leftPtr, filter, resultSet);
    if (res & FILTER_SEARCH_RIGHT) Index_searchRec(self, node.rightPtr, filter, resultSet);
}

NodePointer Index_searchEntryRec(Index *self, char *key, EntryPointer entryPtr, NodePointer nodePtr)
{
    if (nodePtr == INVALID_POINTER) return INVALID_POINTER;
    
    IndexNode node;
    Index_readNode(self, &node, nodePtr);

    int comp = strcmp(node.key, key);
    if (comp == 0 && node.entryPtr == entryPtr) return nodePtr;
    
    NodePointer left = Index_searchEntryRec(self, key, entryPtr, node.leftPtr);
    NodePointer right = Index_searchEntryRec(self, key, entryPtr, node.rightPtr);

    return (left != INVALID_POINTER) ? left: right;
}

NodePointer Index_searchEntry(Index *self, char *key, EntryPointer entryPtr)
{
    return Index_searchEntryRec(self, key, entryPtr, self->rootPtr);
}
