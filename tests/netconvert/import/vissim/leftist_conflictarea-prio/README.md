
# files for testing conversion of counter intuitive turning prioritization 
(on a right-hand-traffic network)

## conversion command
	netcovert --vissim=CA_Test.inpx -o ca_test.net.xml

## files:
* ``CA_Test.inpx`` : VISSIM input 
* ``CA_Test.net.xml`` : expected netconvert result
  with the command given above, ``ca_test.net.xml`` should be identical to
  ``CA_Test.net.xml``
* ``ca.rou.xml`` : route definitions for simulation test
* ``ca_test.sumo.cfg`` : simulation configuration

