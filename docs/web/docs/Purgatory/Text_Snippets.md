---
title: Purgatory/Text Snippets
permalink: /Purgatory/Text_Snippets/
---

# Networks

[NETGENERATE](../NETGENERATE.md) allows to create networks in a
very comfortable way. For some small-sized tests of rerouting
strategies, tls-signals etc., this is probably the best - read "fastest"
- solution to get a network one can run some simulations at. The clear
naming of the streets also eases defining own routes. But networks
generated using [NETGENERATE](../NETGENERATE.md) are of course
useless as soon as you want to simulate a real-world network.

Using [NETCONVERT](../NETCONVERT.md) one can import road networks
from several sources, among them VISUM, shape files, and OSM databases.
This is of course the tool to use for setting up real-world networks.

Still, most examples within the data-section were written as XML files
by hand and converted using [NETCONVERT](../NETCONVERT.md) for
several reasons. At first, the examples are small enough and one may
steer the output better than when using
[NETGENERATE](../NETGENERATE.md). Furthermore, defining own
networks using XML-data is more flexible.