#ifndef ReferencedItem_h
#define ReferencedItem_h

class ReferencedItem {
private:
    bool _wasSaved;
public:
    ReferencedItem() : _wasSaved(false) { }
    virtual ~ReferencedItem() { }
    bool isSaved() { return _wasSaved; }
    void markSaved() { _wasSaved = true; }
};

#endif
