
#ifndef GUIGrid_h
#define GUIGrid_h

#include <vector>
#include <utils/geom/Boundery.h>

class GUINet;
class GUIEdge;
class GUILaneWrapper;
class GUIJunctionWrapper;
class GUIDetectorWrapper;
class GUIEmitterWrapper;

class GUIGrid {
public:
    class GridReader;

    GUIGrid(GUINet &net, int noXCells, int noYCells);
    ~GUIGrid();
    void init();
//    void get(GridReader &reader, int what);
    const Boundery &getBoundery() const;

    void get(/*GridReader &reader, */int what,
        double x, double y, double xoff, double yoff,
        size_t *setEdges, size_t *setJunctions, size_t *setDetectors,
        size_t *setEmitter);

    int getNoXCells() const;
    int getNoYCells() const;
    double getXCellSize() const;
    double getYCellSize() const;

private:
    struct SubSet {
        size_t   myIndex;
        size_t   mySet;
    };

    class Set {
    public:
        Set();
        ~Set();
        void add(size_t no);
        void remove(size_t no);
        void removeIfIn(const Set &other);
        void setInto(size_t *into) const;

        friend class GUIGrid; // !!!
    private:
        typedef std::vector<SubSet> Cont;
        Cont _cont;
    };

    class GridCell {
    public:
	    GridCell();
        ~GridCell();
        void addJunction(size_t no);
        void addEdge(size_t no);
        void addDetector(size_t no);
        void addEmitter(size_t no);

        void setJunctions(size_t *into) const;
        void setEdges(size_t *into) const;
        void setDetectors(size_t *into) const;
        void setEmitters(size_t *into) const;

        void removeIfIn(const GridCell &other);
    public:

        Set myJunctions;
        Set myEdges;
        Set myDetectors;
        Set myEmitter;
    };

    class index_finder {
    private:
        size_t myIndex;

    public:
        /// constructor
        explicit index_finder(size_t index)
            : myIndex(index) { }

        bool operator() (const SubSet &s) const
        {
            return s.myIndex==myIndex;
        }
    };

private:

    Boundery computeBoundery();
    void divideOnGrid();
    void computeEdgeCells(size_t index, GUIEdge *edge);
    void computeLaneCells(size_t index, GUILaneWrapper &lane);
    void setJunction(size_t index, GUIJunctionWrapper *junction);
    void setDetector(size_t index, GUIDetectorWrapper *detector);
    void setEmitter(size_t index, GUIEmitterWrapper *emitter);
    std::vector<size_t> getCellsContaining(Boundery boundery);
    void buildRelationships();
    void removeFrom(GridCell &cont, int x, int y);

private:

    double _xcellsize, _ycellsize;
    Boundery _boundery;
    GridCell *_grid;
    GridCell *_relations[3];
    int _xsize, _ysize;
    GUINet &_net;
};

#endif

