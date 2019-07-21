/* Intrusive double-linked list implementation,
 * based on https://github.com/webcoyote/coho/blob/master/Base/List.h
 */

#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

#define OBJECT_NAME(obj) (((const char *)&typeid(obj)) + 8)

/******************************************************************************
*
*   List definition macros
*
***/

// Define a field within a structure that will be used to link it into a list
#define LIST_LINK(T) TLink<T>

template <class T>
class TLink;

/******************************************************************************
*
*   TList
*
***/

//=============================================================================
template <class T>
class TList {
public:
	TList();
	~TList();

	void UnlinkAll();
	void DeleteAll();

	T *Head();

	enum InsertPos {
		NONE = 0,
		AFTER,
		BEFORE
	};

	void Insert(T *node, InsertPos pos, T *ref);
	T *Remove(T *node);
	T *Create(InsertPos pos = BEFORE, size_t extra = 0, int memflags = 0);

private:
	size_t m_offset;
	TLink<T> m_link;

	TLink<T> *GetLinkFromNode(T *node) const;

	// Hide copy-constructor and assignment operator
	TList(const TList &);
	TList &operator=(const TList &);

	static inline void SDelete(T *node)
	{
		SMemFree(node, (char *)OBJECT_NAME(T), SLOG_OBJECT, 0);
	}
};

//=============================================================================
template <class T>
TList<T>::~TList()
{
	// BUGFIX: Unlinking does not free memory, should use DeleteAll()
	UnlinkAll();
}

//=============================================================================
template <class T>
TList<T>::TList()
{
	size_t offset = offsetof(T, m_Link);
	// Mark this node as the end of the list, with the link offset set
	m_link.m_prevLink = &m_link;
	m_offset = offset;
	m_link.m_nextNode = (T *)~((size_t)&m_link - offset);
}

//=============================================================================
template <class T>
void TList<T>::DeleteAll()
{
	while (T *node = m_link.Next()) {
		node->Delete(0x0);
		SDelete(node);
	}
}

//=============================================================================
template <class T>
inline T *TList<T>::Head()
{
	return m_link.Next();
}

//=============================================================================
template <class T>
inline TLink<T> *TList<T>::GetLinkFromNode(T *node) const
{
	//    assert(m_offset != (size_t) -1);
	//    return (TLink<T> *) ((size_t) node + m_offset);
	return &node->m_Link;
}

template <class T>
T *TList<T>::Remove(T *node)
{
	TLink<T> *link = node ? &node->m_Link : &m_link;
	T *next = link->Next();
	node->Delete(0x0);
	SDelete(node);
	return next;
}

template <class T>
T *TList<T>::Create(InsertPos pos, size_t extra, int memflags)
{
	T *node = new (extra, memflags) T;
	if (pos != NONE)
		Insert(node, pos, NULL);
	return node;
}

template <class T>
void TList<T>::Insert(T *node, InsertPos pos, T *ref)
{
	TLink<T> *reflink;
	TLink<T> *i = node ? GetLinkFromNode(node) : &m_link;
	if (i->IsLinked())
		i->Unlink();

	reflink = ref ? GetLinkFromNode(ref) : &m_link;

	switch (pos) {
	case AFTER:
		i->InsertAfter(node, reflink, m_offset);
		break;
	case BEFORE:
		i->InsertBefore(node, reflink);
		break;
	}
}

//=============================================================================
template <class T>
void TList<T>::UnlinkAll()
{
	for (;;) {
		T *node = m_link.Next();
		if ((int)node <= 0)
			break;
		node->m_Link.Unlink();
	}
}

/******************************************************************************
*
*   TLink
*
***/

//=============================================================================
template <class T>
class TLink {
public:
	TLink()
	    : m_prevLink(NULL)
	    , m_nextNode(NULL)
	{
	}
	~TLink()
	{
		Unlink();
	}

	bool IsLinked() const
	{
		return m_prevLink != NULL;
	}
	void Unlink();

	T *Next()
	{
		if ((ptrdiff_t)m_nextNode <= 0)
			return NULL;
		return m_nextNode;
	}

	TLink<T> *NextLink(size_t offset = -1)
	{
		if ((ptrdiff_t)m_nextNode <= 0)
			return (TLink<T> *)~((size_t)m_nextNode);

		if ((int)offset < 0) {
			// Calculate the offset from a node pointer to a link structure
			offset = (size_t)this - (size_t)m_prevLink->m_nextNode;
		}

		// Get the link field for the next node
		return (TLink<T> *)((size_t)m_nextNode + offset);
	}

	void InsertBefore(T *node, TLink<T> *nextLink)
	{
		TLink<T> *p = nextLink->m_prevLink;
		m_prevLink = p;
		m_nextNode = p->m_nextNode;

		p->m_nextNode = node;
		nextLink->m_prevLink = this;
	}

	inline void InsertAfter(T *node, TLink<T> *prevLink, const size_t &offset)
	{
		m_prevLink = prevLink;
		m_nextNode = prevLink->m_nextNode;

		prevLink->NextLink(offset)->m_prevLink = this;
		prevLink->m_nextNode = node;
	}

private:
	TLink<T> *m_prevLink; // pointer to the previous >link field<
	T *m_nextNode;        // pointer to the next >object<

	// Hide copy-constructor and assignment operator
	TLink(const TLink &);
	TLink &operator=(const TLink &);

	friend class TList<T>;
};

//=============================================================================
template <class T>
void TLink<T>::Unlink()
{
	if (IsLinked()) {
		NextLink()->m_prevLink = m_prevLink;
		m_prevLink->m_nextNode = m_nextNode;

		m_prevLink = NULL;
		m_nextNode = NULL;
	}
}

DEVILUTION_END_NAMESPACE
