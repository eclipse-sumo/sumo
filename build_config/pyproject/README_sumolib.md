# sumolib

sumolib is a set of python modules for working with SUMO networks, simulation output and other simulation artifacts.

## Installation

Install sumolib by simply executing:
```pip install sumolib```

A [daily version](https://test.pypi.org/project/sumolib/) is also available in TestPyPI:
```pip install -i https://test.pypi.org/simple/ sumolib```

## Getting Started

To use sumolib in your Python code, import the `sumolib` module. The following code snippet shows a basic example
of how to load a network file and retrieve the coordinate of a node:

```python
import sumolib

# Parse the network
net = sumolib.net.readNet("myNet.net.xml")

# Retrieve the coordinate of a node based on its ID
print(net.getNode("myNodeID").getCoord())
```

## Documentation

The sumolib documentation is available online at
[https://sumo.dlr.de/docs/Tools/Sumolib.html](https://sumo.dlr.de/docs/Tools/Sumolib.html). For a list of available
functions take a look at the [pydoc generated documentation](http://sumo.dlr.de/pydoc/sumolib.html)
or simply browse the [source code here](https://github.com/eclipse-sumo/sumo/tree/main/tools/sumolib).


## Contributing

If you find a bug in sumolib or have a suggestion for a new feature, please report it on the SUMO issue tracker at
[https://github.com/eclipse-sumo/sumo/issues](https://github.com/eclipse-sumo/sumo/issues).
If you would like to contribute code to sumolib, please submit a pull request to the SUMO repository at
[https://github.com/eclipse-sumo/sumo](https://github.com/eclipse-sumo/sumo).

## License

sumolib is released under the Eclipse Public License 2.0 (EPL-2.0).
