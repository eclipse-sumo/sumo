#ifndef MSMSPhaseDefinition_h
#define MSMSPhaseDefinition_h


/**
 * The definition of a single phase */
class MSPhaseDefinition {
public:
    /// the duration of the phase
    size_t          duration;

    /// constructor
    MSPhaseDefinition(size_t durationArg, const std::bitset<64> &driveMaskArg,
            const std::bitset<64> &breakMaskArg,
            const std::bitset<64> &yellowMaskArg)
        : duration(durationArg), driveMask(driveMaskArg),
            breakMask(breakMaskArg), yellowMask(yellowMaskArg)
    {
    }

    /// destructor
    virtual ~MSPhaseDefinition() { }

    const std::bitset<64> &getDriveMask() const {
        return driveMask;
    }

    const std::bitset<64> &getBreakMask() const {
        return breakMask;
    }

    const std::bitset<64> &getYellowMask() const {
        return yellowMask;
    }

private:
    /// invalidated standard constructor
    MSPhaseDefinition();

    /// the mask which links are allowed to drive within this phase (green light)
    std::bitset<64>  driveMask;

    /// the mask which links must not drive within this phase (red light)
    std::bitset<64>  breakMask;

    /// the mask which links have to decelerate(yellow light)
    std::bitset<64>  yellowMask;


};

#endif

