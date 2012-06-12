#ifndef HEADER_REFERENCECOUNTED_H
#define HEADER_REFERENCECOUNTED_H

/**
 * Base class for reference-counted objects.
 *
 * A reference counted object destroyed itself automatically as soon as its
 * reference counter hits 0. You must call grab() when you store a pointer to
 * a reference-counted object to ensure its survival, and release() it when it
 * you no longer need it.
 */
class ReferenceCounted {

private:
    unsigned int m_iRefs;

public:
    /**
     * Constructor.
     *
     * The reference counter is initialized with 1, so if you no longer need it
     * after its creation (whether you passed a pointer to it or not), you need
     * to call release(). Else, you must not call grab().
     */
    ReferenceCounted()
      : m_iRefs(1)
    {
    }

    virtual ~ReferenceCounted() {}

    /**
     * Indicate that we are keeping a reference to that object.
     */
    inline void grab()
    {
        m_iRefs++;
    }

    /**
     * We no longer keep a reference to that object.
     */
    inline bool release()
    {
        m_iRefs--;
        if(m_iRefs == 0)
        {
            delete this;
            return true;
        }
        else
            return false;
    }

};

#endif
