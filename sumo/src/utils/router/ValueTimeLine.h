#ifndef ValueTimeLine_h
#define ValueTimeLine_h

class ValueTimeLine {
public:
    typedef std::pair<long, long> TimeRange;
    typedef std::pair<TimeRange, double> ValuedTimeRange;
    typedef std::vector<ValuedTimeRange> TimedValueVector;
private:
    TimedValueVector _values;
public:
    ValueTimeLine();
    ~ValueTimeLine();
    void addValue(long begin, long end, float value);
    void addValue(TimeRange range, float value);
    float getValue(long time) const;
    size_t noDefinitions() const;
    const TimeRange &getRangeAtPosition(size_t pos) const;
private:
    class range_finder {
    private:
        long _time;
    public:
        /** constructor */
        explicit range_finder(long time) : _time(time) { }
        /** the comparing function */
        bool operator() (ValuedTimeRange vrange) {
            TimeRange range = vrange.first;
            return range.first<=_time && range.second>=_time;
        }
    };
};

#endif
