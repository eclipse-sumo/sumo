#ifndef MSLink_h
#define MSLink_h

/** MSLinks represent the connnection between lanes. */
class MSLink
{
public:
    friend class MSLane;

    /// Constructor
    MSLink( MSLane* succLane, bool yield );

    /// Some Junctions need to switch the Priority
    void setPriority( bool prio );

    /// MSLink's destination lane.
    MSLane* myLane;

    /// MSLinks's default right of way, true for right of way MSLinks.
    bool myPrio;

    /** Indicator if a vehicle wants to take this MSLink. Set only
        if it's velocity is sufficient. */
    bool myDriveRequest;

public:
    /** Function object in order to find the requested MSLink out
        of myMSLinks, if there is a used one. */
    class LinkRequest
    {
    public:

        typedef const MSLink* first_argument_type;
        typedef bool result_type;

        result_type operator() ( first_argument_type MSLink ) const;
    };


private:
    MSLink();
};

#endif
