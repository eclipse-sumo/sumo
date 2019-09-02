---
title: Purgatory/Traffic Lights
permalink: /Purgatory/Traffic_Lights/
---

# Traffic Lights

## Command 0x41: Get Traffic Light Status

|    integer     |       integer        |      integer       |
| :------------: | :------------------: | :----------------: |
| TrafficLightId | IntervalStart \[ms\] | IntervalEnd \[ms\] |

The traffic lights of the junction specified by *TrafficLight-Id* are
asked for their state transitions that will occur within a given time
interval defined by *IntervalStart* and *IntervallEnd*. All traffic
light switches within this interval are then reported as a series of
*Traffic Light Switch* responses (s. below).

### Response 0x91: Traffic Light Switch

|      integer      |    string     |          float          |     string     |  ubyte   |      integer      |
| :---------------: | :-----------: | :---------------------: | :------------: | :------: | :---------------: |
| SwitchTime \[ms\] | PrecedingEdge | PositionOnPrecedingEdge | SucceedingEdge | NewPhase | YellowTime \[ms\] |

This response is sent for each switch in a traffic lights phase that
occurs within the time interval given by the command *Get Traffic Light
Status*. The *NewPhase* that the traffic light will switch to is
reported along with the time of the switch (*SwitchTime*) and the
duration of the yellow phase that precedes the phase switch
(*YellowTime*).
A switch is reported for each pair of *PrecedingEdge* and
*SucceedingEdge* that is affected by it. Additionally, the traffic
light's *PositionOnPrecedingEdge* is given.