#ifndef GUIEdgeGrid_h
#define GUIEdgeGrid_h

#include <utility>
#include <utils/geom/Boundery.h>
#include <vector>
//#include <utils/geom/Bresenham.h>
#include "GUIEdgeCont.h"

class GUIEdge;

class GUIEdgeGrid {
private:
	size_t _xsize, _ysize;
	double _xcellsize, _ycellsize;
	// the grid of edge lists (two dimensional)
	GUIEdgeCont *_grid;
	GUIEdgeCont _empty;
	Boundery _boundery;
    typedef std::pair<int, int> CellPos;
    typedef std::vector<CellPos> CellPosVector;
public:
	GUIEdgeGrid(size_t xsize, size_t ysize);
	~GUIEdgeGrid();
	void init();
    GUIEdgeCont &get(double x, double y);
    const Boundery &getBoundery() const;
private:
	Boundery computeBoundery();
	void divideOnGrid(double xcellsize, double ycellsize, size_t xsize);
	void computeEdgeCells(double xcellsize, double ycellsize,
        GUIEdge *edge, size_t xsize);
    CellPos getInitCellPos(double x, double y);
    void set(GUIEdge *edge, CellPos pos);
    CellPosVector getNextCells(GUIEdge *edge, CellPos pos);
    bool checkPossible(GUIEdge *edge, int xpos, int ypos);
    bool checkIntersection(GUIEdge *edge,
        double cx1, double cy1, double cx2, double cy2);


        /*
    class EdgeAssigner : public Bresenham::BresenhamCallBack {
	private:
		double _xcellsize, _ycellsize;
		GUIEdgeCont *_grid;
		GUIEdge *_edge;
		double _xmin, _ymin;
		size_t _xsize;
	public:
		EdgeAssigner(double xcellsize, double ycellsize,
            GUIEdgeCont *grid, GUIEdge *edge, size_t xsize);
		~EdgeAssigner();
		void execute(double x, double y);
	};*/
};

#endif

