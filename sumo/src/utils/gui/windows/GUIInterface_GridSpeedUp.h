#ifndef GUIInterface_GridSpeedUp_h
#define GUIInterface_GridSpeedUp_h

class GUIInterface_GridSpeedUp {
public:
    GUIInterface_GridSpeedUp() { }

    virtual ~GUIInterface_GridSpeedUp() { }

    virtual void get(int what,
        double x, double y, double xoff, double yoff,
        size_t *setEdges, size_t *setJunctions, size_t *setAdditional) { }

    /// returns the number of cells in x-direction
    int getNoXCells() const;

    /// returns the number of cells in y-direction
    int getNoYCells() const;

    /// returns the size of each cell in x-direction
    double getXCellSize() const;

    /// returns the size of each cell in y-direction
    double getYCellSize() const;


};