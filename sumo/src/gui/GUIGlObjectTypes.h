#ifndef GUIGlObjectType_h
#define GUIGlObjectType_h


/**
 * ChooseableArtifact
 * Enumeration to differ to show the list of which atrifact
 * By now, one can display edges, junctions and vehicles
 */
enum GUIGlObjectType {
    /// a vehicles
    GLO_VEHICLE = 0,
    /// a detector
    GLO_DETECTOR = 1,
    /// an emitter
    GLO_EMITTER = 2,
    /// a lane
    GLO_LANE = 3,
    /// an edges
    GLO_EDGE = 4,
    /// a junctions
    GLO_JUNCTION = 5
};

#endif
