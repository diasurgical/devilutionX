/*****************************************************************************/
/* huffman.h                              Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Description :                                                             */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* xx.xx.xx  1.00  Lad  The first version of huffman.h                       */
/* 03.05.03  2.00  Lad  Added compression                                    */
/* 08.12.03  2.01  Dan  High-memory handling (> 0x80000000)                  */
/*****************************************************************************/
 
#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

//-----------------------------------------------------------------------------
// Defines
 
#define HUFF_ITEM_COUNT    0x203        // Number of items in the item pool
#define LINK_ITEM_COUNT    0x80         // Maximum number of quick-link items

//-----------------------------------------------------------------------------
// Structures and classes

// Input stream for Huffmann decompression
class TInputStream
{
    public:

    TInputStream(void * pvInBuffer, size_t cbInBuffer);
    unsigned int Get1Bit();
    unsigned int Peek7Bits();
    unsigned int Get8Bits();
    void SkipBits(unsigned int BitCount);
 
    unsigned char * pbInBufferEnd;      // End position in the the input buffer
    unsigned char * pbInBuffer;         // Current position in the the input buffer
    unsigned int BitBuffer;             // Input bit buffer
    unsigned int BitCount;              // Number of bits remaining in 'dwBitBuff'
};
 

// Output stream for Huffmann compression
class TOutputStream
{
    public:
 
    TOutputStream(void * pvOutBuffer, size_t cbOutLength);
    void PutBits(unsigned int dwValue, unsigned int nBitCount);
    void Flush();
 
    unsigned char * pbOutBufferEnd;     // End position in the output buffer
    unsigned char * pbOutBuffer;        // Current position in the output buffer
    unsigned int BitBuffer;             // Bit buffer
    unsigned int BitCount;              // Number of bits in the bit buffer
};

// A virtual tree item that represents the head of the item list
#define LIST_HEAD()  ((THTreeItem *)(&pFirst))

enum TInsertPoint
{
    InsertAfter = 1,
    InsertBefore = 2
};

// Huffmann tree item
struct THTreeItem
{
    THTreeItem()    { pPrev = pNext = NULL; DecompressedValue = 0; Weight = 0; pParent = pChildLo = NULL; }
//  ~THTreeItem()   { RemoveItem(); }

    void         RemoveItem();
//  void         RemoveEntry();
 
    THTreeItem  * pNext;                // Pointer to lower-weight tree item
    THTreeItem  * pPrev;                // Pointer to higher-weight item
    unsigned int  DecompressedValue;    // 08 - Decompressed byte value (also index in the array)
    unsigned int  Weight;               // 0C - Weight
    THTreeItem  * pParent;              // 10 - Pointer to parent item (NULL if none)
    THTreeItem  * pChildLo;             // 14 - Pointer to the child with lower-weight child ("left child")
};


// Structure used for quick navigating in the huffmann tree.
// Allows skipping up to 7 bits in the compressed stream, thus
// decompressing a bit faster. Sometimes it can even get the decompressed
// byte directly.
struct TQuickLink
{      
    unsigned int ValidValue;            // If greater than THuffmannTree::MinValidValue, the entry is valid
    unsigned int ValidBits;             // Number of bits that are valid for this item link
    union
    {
        THTreeItem  * pItem;            // Pointer to the item within the Huffmann tree
        unsigned int DecompressedValue; // Value for direct decompression
    };
};
                                           

// Structure for Huffman tree (Size 0x3674 bytes). Because I'm not expert
// for the decompression, I do not know actually if the class is really a Hufmann
// tree. If someone knows the decompression details, please let me know
class THuffmannTree
{
    public:
    
    THuffmannTree(bool bCompression);
    ~THuffmannTree();

    void  LinkTwoItems(THTreeItem * pItem1, THTreeItem * pItem2);
    void  InsertItem(THTreeItem * item, TInsertPoint InsertPoint, THTreeItem * item2);

    THTreeItem * FindHigherOrEqualItem(THTreeItem * pItem, unsigned int Weight);
    THTreeItem * CreateNewItem(unsigned int DecompressedValue, unsigned int Weight, TInsertPoint InsertPoint);

    unsigned int FixupItemPosByWeight(THTreeItem * pItem, unsigned int MaxWeight);
    bool  BuildTree(unsigned int CompressionType);

    void  IncWeightsAndRebalance(THTreeItem * pItem);
    void  InsertNewBranchAndRebalance(unsigned int Value1, unsigned int Value2);

    void  EncodeOneByte(TOutputStream * os, THTreeItem * pItem);
    unsigned int DecodeOneByte(TInputStream * is);

    unsigned int Compress(TOutputStream * os, void * pvInBuffer, int cbInBuffer, int nCmpType);
    unsigned int Decompress(void * pvOutBuffer, unsigned int cbOutLength, TInputStream * is);
 
    THTreeItem   ItemBuffer[HUFF_ITEM_COUNT];   // Buffer for tree items. No memory allocation is needed
    unsigned int ItemsUsed;                     // Number of tree items used from ItemBuffer
 
    // Head of the linear item list
    THTreeItem * pFirst;                        // Pointer to the highest weight item
    THTreeItem * pLast;                         // Pointer to the lowest weight item

    THTreeItem * ItemsByByte[0x102];            // Array of item pointers, one for each possible byte value
    TQuickLink   QuickLinks[LINK_ITEM_COUNT];   // Array of quick-link items
    
    unsigned int MinValidValue;                 // A minimum value of TQDecompress::ValidValue to be considered valid
    unsigned int bIsCmp0;                       // 1 if compression type 0
};

#endif // __HUFFMAN_H__
