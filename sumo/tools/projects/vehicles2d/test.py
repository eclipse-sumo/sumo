#coding: utf-8
import sys
import unittest
import numpy, math
import main

from commons import StarNodeC, ANList
import test_dijkstra

def notes():
    """this functions contains only comments

    continue developing the step
    
    make a test where there are no points left in the 
    open list but the destinaition is not found
    
    """

VISUAL = False
VISUAL = True

# skip short cuts
lost = True
finished = False # don't skip tests 
afinished = True
unfinished = True # tests that are more conceptal should not be run unless they are under developement
devel_skip = True # things aren't ready jet for beeing tested
devel_run = False # this is what I am workin on, hence run this test and let it fail
broken = True # did work somewhen, but due to developent this test was broken, fix after devel
visual = False # finished tests that draw things

def skip_all_but_selected():
    global lost, finished, afinished, unfinished, devel_skip, devel_run, broken, visual 
    
    lost       = True
    finished   = True  
    afinished  = True
    unfinished = True 
    devel_skip = True 
    devel_run  = True
    broken     = True 
    visual     = True 

#skip_all_but_selected()    

def assertAlmostEqualTupleList(not_self, result, expected_result):
            if (len(result) == 0 or len(expected_result) == 0
                or len(result) != len(expected_result)):
                assert False, "lists have differing length or length of zero"
                
            for ii in range(len(result)):
                not_self.assertAlmostEqual(result [ii][0], expected_result [ii][0])
                not_self.assertAlmostEqual(result [ii][1], expected_result [ii][1])


    
class someTestcase(unittest.TestCase):
    
    @unittest.skipIf(devel_run, 'done')
    def test_hull(self):
        vessel_shape =  [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), ( 0, 0)]        
            
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=1)
        vessel = main.Vessel(myFlaeche,vessel_shape)

        ## segments
        
        self.assertEqual(vessel.get_num_hull_segments(), 5) 
        self.assertEqual(vessel.get_hull_segment(0), ((0, 0), (1, 1)))

        self.assertRaisesRegexp(AssertionError, "hull segment out of range",
                                vessel.get_hull_segment, 60)

        self.assertEqual(vessel.get_hull_segments(),
                         [((0, 0),  (1, 1)),
                          ((1, 1),  (1, 3)), 
                          ((1, 3),  (-1, 3)),
                          ((-1, 3), (-1, 1)),
                          ((-1, 1), ( 0, 0))])


        ## inclination
        
        self.assertRaisesRegexp(StandardError, "points are identical",
                                vessel.get_inclination, (0, 0), (0, 0))
        self.assertRaisesRegexp(StandardError, "points are identical",
                                vessel.get_inclination,(1.0, 0), (1, 0))
        self.assertEqual(vessel.get_inclination((0, 0), (1, 1)), 1)
        self.assertEqual(vessel.get_inclination((0, 0), (1, -1)), -1)
        self.assertEqual(vessel.get_inclination((0, 0), (-1, -1)), 1)
        self.assertEqual(vessel.get_inclination((0, 0), (1, 2)), 2)
        self.assertEqual(vessel.get_inclination((1, 1), (2, 3)), 2)
        self.assertEqual(vessel.get_inclination((1, 1), (3, 4)), 1.5)
        self.assertEqual(vessel.get_inclination((5, 2), (1, 1)), 0.25)
        self.assertEqual(vessel.get_inclination((1, 1), (1, 2)), numpy.float('inf'))
        self.assertEqual(vessel.get_inclination((1, 2), (1, 1)), numpy.float('-inf'))


        
    @unittest.skipIf(finished, 'done')
    def test_vessel_intersection_points(self):
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=1)
        vessel_shape =  [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), ( 0, 0)]        
        vessel = main.Vessel(myFlaeche,vessel_shape)
        
        ## intersects

        self.assertEqual(vessel.get_intersection_points((4.5, 4.5), (8.2, 8.2)),
                         ( [(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)],
                           [(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)]
                         )
                        )

        self.assertEqual(vessel.get_intersection_points((5, 5), (8, 8)),
                         ( [(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)],
                           [(5.0, 5.0), (6.0, 6.0), (7.0, 7.0), (8.0, 8.0)]
                         )
                        )

        
        result = vessel.get_intersection_points((4.5, 3.5), (8.2, 7.2))[0]
        expected_result = [(5.0, 4.0), (6.0, 5.0), (7.0, 6.0), (8.0, 7.0)] 
        assertAlmostEqualTupleList(self, result, expected_result)
        
        self.assertEqual(vessel.get_intersection_points((4.5, 5), (8.2, 5)),
                         ([(5.0, 5.0), (6.0, 5.0), (7.0, 5.0), (8.0, 5.0)], []) )

        self.assertEqual(vessel.get_intersection_points((4.5, 5), (4.5, 7.1)),
                         ([], [(4.5, 5), (4.5, 6), (4.5, 7)]) )

        self.assertEqual(vessel.get_intersection_points((4.5, 7.1), (4.5, 5)),
                         ([], [(4.5, 5), (4.5, 6.0), (4.5, 7.0)]) )

        myFlaeche = main.Flaeche(xdim=50,ydim=50,scale=0.5)
        vessel = main.Vessel(myFlaeche,vessel_shape)
        
        self.assertEqual(vessel.get_intersection_points((4.6, 4.6), (6.2, 6.2)),
                         ([(5.0, 5.0), (5.5, 5.5), (6.0, 6.0)],
                          [(5.0, 5.0), (5.5, 5.5), (6.0, 6.0)]
                         )
                        )

        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10)
        vessel = main.Vessel(myFlaeche,vessel_shape)


        self.assertEqual(vessel.get_intersection_points((140.0, 285.0),
                                                        (120, 285.0)),
                         ([(120.0, 285.0), (130.0, 285.0), (140.0, 285.0) ],
                          []
                         )
                        )


        self.assertEqual(vessel.get_intersection_points((60.0, 285.0),
                                                        (60.0, 255.0)),
                         ([],
                          [(60.0, 260.0), (60.0, 270.0), (60.0, 280.0) ],
                         )
                        )

        self.assertEqual(vessel.get_intersection_points((59.999999999999986, 285.0),
                                                        (60.0, 125.0)),
                         ([],
                          [(59.999999999999986, 200.0),
                           (59.999999999999986, 210.0),
                           (59.999999999999986, 220.0),
                           (59.999999999999986, 230.0),
                           (59.999999999999986, 240.0),
                           (59.999999999999986, 250.0),
                           (59.999999999999986, 260.0),
                           (59.999999999999986, 270.0),
                           (59.999999999999986, 280.0),
                           (60.0, 130.0),
                           (60.0, 140.0),
                           (60.0, 150.0),
                           (60.0, 160.0),
                           (60.0, 170.0),
                           (60.0, 180.0),
                           (60.0, 190.0)])
                         )
                        

        
    @unittest.skipIf(finished, 'done')
    def test_vessel_tranform_points(self):
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10)
        vessel_shape =  [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), ( 0, 0)]        
        vessel = main.Vessel(myFlaeche,vessel_shape)
        
        ## transform points in global coord to ego coord
        ## and back to global coords

        #####################
        # ego sits on 10,10 has no inclination and want's to know where in
        # global coordinates is 0, 0 and vice versa

        offset       = (10,10)
        omega        = 0
        ego_point    = (0,0)
        global_point = (10,10)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        ######################
        # ego sits on 9,9 has no inclination and want's to know where in
        # global coordinates is 1, 1 and vice versa

        offset       = (9, 9)
        omega        = 0
        ego_point    = (1, 1)
        global_point = (10, 10)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])


        #########################
        # ego sits on 1, 1  has an incliantion of 60 degrees
        # and want's to know where in global coordinates is 2, 0

        offset       = (1, 1)
        omega        = math.pi/3
        ego_point    = (2, 0)
        global_point = (2, 1+2*0.866025403784)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])
        
        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        
        ####################
        # ego sits on 0, 0 has an incliantion of 90 degrees
        # and want's to know where in global coordinates is ego(0, 1)

        offset       = (0, 0)
        omega        = math.pi/2
        ego_point    = (1, 0)
        global_point = (0, 1)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])

        
        #####################
        # ego sits on 0, 0 has an incliantion of 45 degrees
        # and want's to know where in global coordinates is ego(1.4142..., 0)
        # and vice versa
        
        offset       = (0, 0)
        omega        = math.pi/4
        ego_point    = (math.sqrt(2), 0)
        global_point = (1,1)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])
        
        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])


        #####################
        # ego sits on 1, 1 has an incliantion of 45 degrees
        # and want's to know where in global coordinates is ego(1.4142..., 0)
        # and vice versa

        offset       = (1, 1)
        omega        = math.pi/4
        ego_point    = (math.sqrt(2), 0)
        global_point = (2,2)

        result = vessel.transform_coord(ego_point, omega, offset)
        expected_result = global_point
        assertAlmostEqualTupleList(self, [result], [expected_result])
        
        result = vessel.transform_coord_to_ego(global_point, omega, offset)
        expected_result = ego_point
        assertAlmostEqualTupleList(self, [result], [expected_result])


    @unittest.skipIf(finished, 'done')
    def test_vessel_tranform_hull(self):
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10)
        vessel_shape =  [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), ( 0, 0)]        
        vessel = main.Vessel(myFlaeche,vessel_shape)

        ##################
        # get transformed the coords of the vehicle
        expected_result = [(15, 10), (16.0, 9.0), (18.0, 9.0),
                           (18.0, 11.0), (16.0, 11.0), (15, 10)]
        result = vessel.transform_hull_points(-math.pi/2, (15,10) )
        self.assertEqual(result, expected_result)

        expected_result = [(15, 10), (15.0, 11.414213562373096),
                           (13.585786437626904, 12.82842712474619),
                           (12.17157287525381, 11.414213562373096),
                           (13.585786437626904, 10.0), (15, 10)]


        [(15, 10), (16.0, 9.0), (18.0, 9.0),
                           (18.0, 11.0), (16.0, 11.0), (15, 10)]
        result = vessel.transform_hull_points(math.pi/4, (15,10) )
        self.assertEqual(result, expected_result)


    @unittest.skipIf(finished, 'done')
    def test_get_grey_and_black_shade(self):
        """testing which (visual) nodes are shadowed by the vehicle"""
        visual = VISUAL
        #visual = True
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10,
                                 output='result_grey_rotation_0_45_90')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.transform_hull_points(-math.pi/2, (100, 85) )

        self.assertEqual(vessel.get_grey_shade(),
        [(9, 8), (10, 8), (10, 7), (11, 6), (11, 7),
         (12, 5), (12, 6), (13, 4), (13, 5), (14, 4),
         (15, 4), (16, 4), (17, 4), (18, 4), (19, 4),
         (20, 4), (21, 4), (22, 4), (23, 4), (24, 4),
         (25, 4), (26, 4), (27, 4), (28, 4), (29, 4),
         (30, 4), (29, 5), (30, 5), (29, 6), (30, 6),
         (29, 7), (30, 7), (29, 8), (30, 8), (29, 9),
         (30, 9), (29, 10), (30, 10), (29, 11), (30, 11),
         (29, 12), (30, 12), (13, 12), (14, 12), (15, 12),
         (16, 12), (17, 12), (18, 12), (19, 12), (20, 12),
         (21, 12), (22, 12), (23, 12), (24, 12), (25, 12),
         (26, 12), (27, 12), (28, 12), (10, 9), (11, 9),
         (11, 10), (12, 10), (12, 11), (13, 11)])

        vessel.transform_hull_points(-math.pi/4, (100, 85) )
        self.assertEqual(vessel.get_grey_shade(),
        [(9, 8), (10, 8), (11, 8), (12, 8), (13, 8), (14, 8),
         (15, 8), (16, 8), (16, 9), (17, 9), (17, 10), (18, 10),
         (18, 11), (19, 11), (19, 12), (20, 12), (20, 13), (21, 13),
         (21, 14), (22, 14), (22, 15), (23, 15), (23, 16), (24, 16),
         (24, 17), (25, 17), (25, 18), (26, 18), (26, 19), (21, 24),
         (21, 25), (22, 23), (22, 24), (23, 22), (23, 23), (24, 21),
         (24, 22), (25, 20), (25, 21), (26, 20), (9, 14), (10, 14),
         (10, 15), (11, 15), (11, 16), (12, 16), (12, 17), (13, 17),
         (13, 18), (14, 18), (14, 19), (15, 19), (15, 20), (16, 20),
         (16, 21), (17, 21), (17, 22), (18, 22), (18, 23), (19, 23),
         (19, 24), (20, 24), (20, 25), (9, 9), (10, 9), (9, 10), (10, 10),
         (9, 11), (10, 11), (9, 12), (10, 12), (9, 13), (10, 13)])

        vessel.transform_hull_points(0, (100, 85) )
        self.assertEqual(vessel.get_grey_shade(),
        [(9, 8), (10, 8), (10, 9), (11, 9), (11, 10), (12, 10),
         (12, 11), (13, 11), (13, 12), (14, 12), (13, 13), (14, 13),
         (13, 14), (14, 14), (13, 15), (14, 15), (13, 16), (14, 16),
         (13, 17), (14, 17), (13, 18), (14, 18), (13, 19), (14, 19),
         (13, 20), (14, 20), (13, 21), (14, 21), (13, 22), (14, 22),
         (13, 23), (14, 23), (13, 24), (14, 24), (13, 25), (14, 25),
         (13, 26), (14, 26), (13, 27), (14, 27), (13, 28), (14, 28),
         (5, 28), (6, 28), (7, 28), (8, 28), (9, 28), (10, 28), (11, 28),
         (12, 28), (5, 19), (5, 20), (5, 21), (5, 22), (5, 23), (5, 24),
         (5, 25), (5, 26), (5, 27), (5, 12), (5, 13), (6, 12), (6, 13),
         (5, 14), (6, 14), (5, 15), (6, 15), (5, 16), (6, 16), (5, 17),
         (6, 17), (5, 18), (6, 18), (6, 19), (6, 11), (7, 10), (7, 11),
         (8, 9), (8, 10), (9, 9)])

        if visual:
            myFlaeche.vis_show(vessel.transformed_hull_points)
        
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10,
                                 output='result_black_rotation_90')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
   
        myFlaeche.vis_reset()
        vessel.transform_hull_points(-math.pi/2, (100, 85) )
        self.assertEqual(vessel.get_black_shade(),
        [(9, 8),
         (10, 7), (10, 8), (10, 9),
         (11, 6), (11, 7), (11, 8), (11, 9), (11, 10),
         (12, 5), (12, 6), (12, 7), (12, 8), (12, 9), (12, 10), (12, 11),
         (13, 4), (13, 5), (13, 6), (13, 7), (13, 8), (13, 9),(13, 10), (13, 11), (13, 12),
         (14, 4), (14, 5), (14, 6), (14, 7), (14, 8), (14, 9), (14, 10), (14, 11), (14, 12),
         (15, 4), (15, 5), (15, 6), (15, 7), (15, 8), (15, 9), (15, 10), (15, 11), (15, 12),
         (16, 4), (16, 5), (16, 6), (16, 7), (16, 8), (16, 9), (16, 10), (16, 11), (16, 12),
         (17, 4), (17, 5), (17, 6), (17, 7), (17, 8), (17, 9), (17, 10), (17, 11), (17, 12),
         (18, 4), (18, 5), (18, 6), (18, 7), (18, 8), (18, 9), (18, 10), (18, 11), (18, 12),
         (19, 4), (19, 5), (19, 6), (19, 7), (19, 8), (19, 9), (19, 10), (19, 11), (19, 12),
         (20, 4), (20, 5), (20, 6), (20, 7), (20, 8), (20, 9), (20, 10), (20, 11), (20, 12),
         (21, 4), (21, 5), (21, 6), (21, 7), (21, 8), (21, 9), (21, 10), (21, 11), (21, 12),
         (22, 4), (22, 5), (22, 6), (22, 7), (22, 8), (22, 9), (22, 10), (22, 11), (22, 12),
         (23, 4), (23, 5), (23, 6), (23, 7), (23, 8), (23, 9), (23, 10), (23, 11), (23, 12),
         (24, 4), (24, 5), (24, 6), (24, 7), (24, 8), (24, 9), (24, 10), (24, 11), (24, 12),
         (25, 4), (25, 5), (25, 6), (25, 7), (25, 8), (25, 9), (25, 10), (25, 11), (25, 12),
         (26, 4), (26, 5), (26, 6), (26, 7), (26, 8), (26, 9), (26, 10), (26, 11), (26, 12),
         (27, 4), (27, 5), (27, 6), (27, 7), (27, 8), (27, 9), (27, 10), (27, 11), (27, 12),
         (28, 4), (28, 5), (28, 6), (28, 7), (28, 8), (28, 9), (28, 10), (28, 11), (28, 12),
         (29, 4), (29, 5), (29, 6), (29, 7), (29, 8), (29, 9), (29, 10), (29, 11), (29, 12),
         (30, 4), (30, 5), (30, 6), (30, 7), (30, 8), (30, 9), (30, 10), (30, 11), (30, 12)])
        if visual:
            myFlaeche.vis_show(vessel.transformed_hull_points)

            
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10,
                                 output='result_black_rotation_45')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        myFlaeche.vis_reset()
        vessel.transform_hull_points(-math.pi/4, (100, 85) )
        self.assertEqual(vessel.get_black_shade(),
        [(9, 8), (9, 9), (9, 10), (9, 11), (9, 12), (9, 13), (9, 14),
         (10, 8), (10, 9), (10, 10), (10, 11), (10, 12), (10, 13), (10, 14), (10, 15),
         (11, 8), (11, 9), (11, 10), (11, 11), (11, 12), (11, 13), (11, 14), (11, 15), (11, 16),
         (12, 8), (12, 9), (12, 10), (12, 11), (12, 12), (12, 13),
         (12, 14), (12, 15), (12, 16), (12, 17),
         (13, 8), (13, 9), (13, 10), (13, 11), (13, 12), (13, 13),
         (13, 14), (13, 15), (13, 16), (13, 17), (13, 18),
         (14, 8), (14, 9), (14, 10), (14, 11), (14, 12), (14, 13),
         (14, 14), (14, 15), (14, 16), (14, 17), (14, 18), (14, 19),
         (15, 8), (15, 9), (15, 10), (15, 11), (15, 12), (15, 13),
         (15, 14), (15, 15), (15, 16), (15, 17), (15, 18), (15, 19), (15, 20),
         (16, 8), (16, 9), (16, 10), (16, 11), (16, 12), (16, 13),
         (16, 14), (16, 15), (16, 16), (16, 17), (16, 18), (16, 19), (16, 20), (16, 21),
         (17, 9), (17, 10), (17, 11), (17, 12), (17, 13), (17, 14),
         (17, 15), (17, 16), (17, 17), (17, 18), (17, 19), (17, 20), (17, 21), (17, 22),
         (18, 10), (18, 11), (18, 12), (18, 13), (18, 14), (18, 15),
         (18, 16), (18, 17), (18, 18), (18, 19), (18, 20), (18, 21), (18, 22), (18, 23),
         (19, 11), (19, 12), (19, 13), (19, 14), (19, 15), (19, 16),
         (19, 17), (19, 18), (19, 19), (19, 20), (19, 21), (19, 22), (19, 23), (19, 24),
         (20, 12), (20, 13), (20, 14), (20, 15), (20, 16), (20, 17),
         (20, 18), (20, 19), (20, 20), (20, 21), (20, 22), (20, 23), (20, 24), (20, 25),
         (21, 13), (21, 14), (21, 15), (21, 16), (21, 17), (21, 18),
         (21, 19), (21, 20), (21, 21), (21, 22), (21, 23), (21, 24), (21, 25),
         (22, 14), (22, 15), (22, 16), (22, 17), (22, 18), (22, 19),
         (22, 20), (22, 21), (22, 22), (22, 23), (22, 24),
         (23, 15), (23, 16), (23, 17), (23, 18), (23, 19),
         (23, 20), (23, 21), (23, 22), (23, 23), (24, 16),
         (24, 17), (24, 18), (24, 19), (24, 20), (24, 21),
         (24, 22), (25, 17), (25, 18), (25, 19), (25, 20), (25, 21),
         (26, 18), (26, 19), (26, 20)])
        if visual:
            myFlaeche.vis_show(vessel.transformed_hull_points)


    @unittest.skipIf(finished, 'done')
    def test_get_radius_to_point_ego(self):
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        # right-turn        
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        
        R = 20
        gamma_minuscle = math.pi/12              # 15°
        gamma_majuscle = 2 * gamma_minuscle      # 30°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], gamma_majuscle)

        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)
        
        R = 20
        gamma_minuscle = math.pi/8              # 22.5°
        gamma_majuscle = 2 * gamma_minuscle     # 45°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1- math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], gamma_majuscle)

        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)

        R = 20
        gamma_minuscle = math.pi/6              # 30°
        gamma_majuscle = 2 * gamma_minuscle     # 60°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)

        R = 20
        gamma_minuscle = math.pi/4              # 45°
        gamma_majuscle = 2 * gamma_minuscle     # 90°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)

        R = 20
        gamma_minuscle = math.pi/6              #  60°
        gamma_majuscle = 2 * gamma_minuscle     # 120°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)

        R = 20
        gamma_minuscle = 5 * math.pi/6          #  75°
        gamma_majuscle = 2 * gamma_minuscle     # 150°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is positive
        # (Case A)

        R = 20
        gamma_minuscle = math.pi/2              #  90°
        gamma_majuscle = 2 * gamma_minuscle     # 160°
        xx_r  = R * math.sin(gamma_majuscle)
        yy_r  = R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)



        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is negativ but still yy is positive
        # (Case B)
        
        R = 20
        beta = math.pi / 6
        expected_angle = 2 * math.pi - beta  
        xx_r  = -R * math.sin(beta)
        yy_r  =  R * ( 1 - math.cos(beta))
        
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], expected_angle )

        # right-turn
        #############################################
        # find radius and angle to a point when
        # xx is negativ but still yy is positive
        # (Case B)
        

        R = 20
        beta = math.pi / 4
        expected_angle = 2 * math.pi - beta  
        xx_r  = -R * math.sin(beta)
        yy_r  = R * ( 1 - math.cos(beta))
        
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], expected_angle )

        # left-turn
        #############################################
        # find radius and angle to a point when
        
        R = 20
        gamma_minuscle = 15. / 360 * 2 * math.pi         # 15°
        gamma_majuscle = 2. * gamma_minuscle             # 30°
        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1 - math.cos(gamma_majuscle))
        
        self.assertAlmostEqual(gamma_minuscle, 0.2617993877991494)
        self.assertAlmostEqual(gamma_majuscle, 0.5235987755982988)
        self.assertAlmostEqual(xx_r , 10)
        self.assertAlmostEqual(yy_r , -2.679491924311226)
        
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], gamma_majuscle)
        
        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is negativ
        
        R = 20
        gamma_minuscle = math.pi/8              # 22.5°
        gamma_majuscle = 2 * gamma_minuscle     # 45°
        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1- math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], gamma_majuscle)

        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is negativ

        R = 20
        gamma_minuscle = math.pi/6              # 30°
        gamma_majuscle = 2 * gamma_minuscle     # 60°
        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is negativ

        R = 20
        gamma_minuscle = math.pi/4              # 45°
        gamma_majuscle = 2 * gamma_minuscle     # 90°
        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is negativ

        R = 20
        gamma_minuscle = math.pi/6              #  60°
        gamma_majuscle = 2 * gamma_minuscle     # 120°
        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)

        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is negativ and yy is positive

        R = 20
        gamma_minuscle = 5 * math.pi/6          #  75°
        gamma_majuscle = 2 * gamma_minuscle     # 150°
        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1 - math.cos(gamma_majuscle))

        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1],  gamma_majuscle)


        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is negativ

        R = 20
        gamma_minuscle = 90. / 360 * 2 * math.pi         # 90°
        gamma_majuscle = 2. * gamma_minuscle             # 180°

        xx_r  =  R * math.sin(gamma_majuscle)
        yy_r  = -R * ( 1 - math.cos(gamma_majuscle))

        
        self.assertAlmostEqual(gamma_minuscle, 1.5707963267948966)
        self.assertAlmostEqual(gamma_majuscle, 3.141592653589793)
        self.assertAlmostEqual(xx_r , 0)
        self.assertAlmostEqual(yy_r , -40)
        
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], gamma_majuscle)


        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx is positiv and yy is negativ

        R = 20
        beta = math.pi / 6
        expected_angle = 2 * math.pi - beta  
        xx_r  = -R * math.sin(beta)
        yy_r  = -R * ( 1 - math.cos(beta))
        
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], expected_angle )

        # left-turn
        #############################################
        # find radius and angle to a point when
        # xx and yy are negativ 

        R = 20
        beta = math.pi / 4
        expected_angle = 2 * math.pi - beta  
        xx_r  = -R * math.sin(beta)
        yy_r  = -R * ( 1 - math.cos(beta))
        
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], expected_angle )


        #######
        # make sure, that you don't wish to get the radius between
        # two same points

        xx_r  = 0.00000000001
        yy_r  = 0.00000000001
        self.assertRaisesRegexp(StandardError, "points must be different",
                                vessel.get_radius_to_point_ego, (xx_r, yy_r))

        xx_r  = -0.00000000001
        yy_r  = -0.00000000001
        self.assertRaisesRegexp(StandardError, "points must be different",
                                vessel.get_radius_to_point_ego, (xx_r, yy_r))
        
        xx_r  = 0
        yy_r  = 0
        self.assertRaisesRegexp(StandardError, "points must be different",
                                vessel.get_radius_to_point_ego, (xx_r, yy_r))


        #########
        # check what happens in case of a 180 degree left/right Turn
        
        R = 20
        AA = math.pi
        xx_r  = 0 
        yy_r  = 40
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        R = 20
        AA = math.pi
        xx_r  = 0.00000000001 
        yy_r  = 40
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        R = 10
        AA = math.pi
        xx_r  = 0 
        yy_r  = -20
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        R = 10
        AA = math.pi
        xx_r  = 0.00000000001 
        yy_r  = -20
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)


        #######
        # check what happens if the point is straight ahead or streight behind
        
        R = float('inf')
        AA = 0
        xx_r  = 10 
        yy_r  = 0
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)

        R = float('-inf')
        AA = 0
        xx_r  = -100 
        yy_r  = 0
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[0], R)
        self.assertAlmostEqual(vessel.get_radius_to_point_ego((xx_r, yy_r))[1], AA)
        
#    def test_get_neighbours(self):
#        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10)
#        vessel = main.Vessel(myFlaeche,
#                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
#        vessel.
        

    @unittest.skipIf(finished, 'done')
    def test_get_reachable_center_points(self):
        visual = True
        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,output='reachables_center')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        vessel.x = 300; vessel.y = 305; vessel.rotation = 0; vessel.r = 20

        result = sorted(vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                            vessel.rotation,
                                                     test_result='get_inner_ego_bounding_box'))

        expected_result = sorted([(300.0, 285.0), (300.0, 325.0),
                                  (320.0, 285.0), (320.0, 325.0)])
        self.assertEqual(result, expected_result)
        
        #######################
        # rotate by 45°
        vessel.x = 300; vessel.y = 305; vessel.rotation = math.pi/4 ; vessel.r = 20
        
        
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_inner_ego_bounding_box')
        sorted_result = sorted(result)

        rr =  vessel.r / math.sqrt(2)  # (route radius)
        expected_result = sorted([(300.0 -   rr,   305.0 +   rr ),   #1
                                  (300.0       ,   305.0 + 2*rr ),   #2
                                  (300.0 +   rr,   305.0 -   rr ),   #3
                                  (300.0 + 2*rr,   305.0        )])  #4

        visual_result = result
        visual_result += result[0]
        myFlaeche.vis_add_poly(visual_result, 'green')
        
        self.assertEqual(sorted_result, expected_result)

        #######################
        # get outer ego bounding box
        
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_outer_ego_bounding_box')
        sorted_result = sorted(result)

        rr =  vessel.r / math.sqrt(2)
        RR =  3* vessel.r / math.sqrt(2)
        
        expected_result = sorted([(300.0 -   RR,   305.0 +   rr ),   #1
                                  (300.0 -   rr,   305.0 +   RR ),   #2
                                  (300.0 +   RR,   305.0 -   rr ),   #3
                                  (300.0 +   rr,   305.0 -   RR )])  #4

        visual_result = result
        visual_result += result[0]
        myFlaeche.vis_add_poly(visual_result, 'green')
        
        self.assertEqual(sorted_result, expected_result)

        #######################
        # get global bounding box which is too big
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_global_bounding_box')
        sorted_result = sorted(result)
        
        expected_result = sorted([(300.0 -   RR,   305.0 -   RR ),   #A1
                                  (300.0 -   RR,   305.0 +   RR ),   #A2
                                  (300.0 +   RR,   305.0 -   RR ),   #A3
                                  (300.0 +   RR,   305.0 +   RR )])  #A4

        visual_result = result
        visual_result += result[0]
        myFlaeche.vis_add_poly(visual_result, 'purple')
        
        self.assertEqual(sorted_result, expected_result)


        #########################
        # get cell center points 
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_all_center_points')

#        print vessel.x, vessel.y
        
        expected_result = [
            (255.0, 265.0), (255.0, 275.0), (255.0, 285.0), (255.0, 295.0),
            (255.0, 305.0), (255.0, 315.0), (255.0, 325.0), (255.0, 335.0), (255.0, 345.0),

            (265.0, 265.0), (265.0, 275.0), (265.0, 285.0), (265.0, 295.0),
            (265.0, 305.0), (265.0, 315.0), (265.0, 325.0), (265.0, 335.0), (265.0, 345.0),

            (275.0, 265.0), (275.0, 275.0), (275.0, 285.0), (275.0, 295.0),
            (275.0, 305.0), (275.0, 315.0), (275.0, 325.0), (275.0, 335.0), (275.0, 345.0),

            (285.0, 265.0), (285.0, 275.0), (285.0, 285.0), (285.0, 295.0),
            (285.0, 305.0), (285.0, 315.0), (285.0, 325.0), (285.0, 335.0), (285.0, 345.0),

            (295.0, 265.0), (295.0, 275.0), (295.0, 285.0), (295.0, 295.0),
            (295.0, 305.0), (295.0, 315.0), (295.0, 325.0), (295.0, 335.0), (295.0, 345.0),

            (305.0, 265.0), (305.0, 275.0), (305.0, 285.0), (305.0, 295.0),
#            (305.0, 305.0),
                            (305.0, 315.0), (305.0, 325.0), (305.0, 335.0), (305.0, 345.0),

            (315.0, 265.0), (315.0, 275.0), (315.0, 285.0), (315.0, 295.0),
            (315.0, 305.0), (315.0, 315.0), (315.0, 325.0), (315.0, 335.0), (315.0, 345.0),

            (325.0, 265.0), (325.0, 275.0), (325.0, 285.0), (325.0, 295.0),
            (325.0, 305.0), (325.0, 315.0), (325.0, 325.0), (325.0, 335.0), (325.0, 345.0),

            (335.0, 265.0), (335.0, 275.0), (335.0, 285.0), (335.0, 295.0),
            (335.0, 305.0), (335.0, 315.0), (335.0, 325.0), (335.0, 335.0), (335.0, 345.0),

            (345.0, 265.0), (345.0, 275.0), (345.0, 285.0), (345.0, 295.0),
            (345.0, 305.0), (345.0, 315.0), (345.0, 325.0), (345.0, 335.0), (345.0, 345.0)]

        self.assertEqual(sorted(result), sorted(expected_result))
        
        for pp in result:
            myFlaeche.vis_add_single_point(pp, 'grey')


        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_zone_zero_center_points')

        expected_result =[
            (275.0, 305.0), (275.0, 315.0), (275.0, 325.0), (275.0, 335.0),
            (285.0, 305.0), (285.0, 315.0), (285.0, 325.0), (285.0, 335.0),
            (295.0, 285.0), (295.0, 295.0), (295.0, 305.0), (295.0, 315.0),
            (295.0, 325.0), (295.0, 335.0),
            (305.0, 275.0), (305.0, 285.0), (305.0, 295.0),
            #(305.0, 305.0),
            (305.0, 315.0), (305.0, 325.0),
            (315.0, 275.0), (315.0, 285.0), (315.0, 295.0), (315.0, 305.0),
            (325.0, 275.0), (325.0, 285.0), (325.0, 295.0), (325.0, 305.0)]

        self.assertEqual(sorted(result), sorted(expected_result))
        
        for pp in result:
            myFlaeche.vis_add_single_point(pp, 'orange')


        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_zone_one_center_points')

        expected_result = [(315.0, 315.0)]
        self.assertEqual(sorted(result), sorted(expected_result))
        
        for pp in result:
            myFlaeche.vis_add_single_point(pp, 'green')

            
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_zone_two_center_points')

        expected_result = [
            (265.0, 345.0),(275.0, 345.0), (285.0, 345.0), (295.0, 345.0),
            (305.0, 335.0), (305.0, 345.0),
            (315.0, 325.0), (315.0, 335.0), (315.0, 345.0),
            (325.0, 315.0), (325.0, 325.0), (325.0, 335.0), (325.0, 345.0),
            (335.0, 275.0), (335.0, 285.0), (335.0, 295.0), (335.0, 305.0),
            (335.0, 315.0), (335.0, 325.0), (335.0, 335.0), (335.0, 345.0),
            (345.0, 265.0), (345.0, 275.0), (345.0, 285.0), (345.0, 295.0),
            (345.0, 305.0), (345.0, 315.0), (345.0, 325.0), (345.0, 335.0), (345.0, 345.0)]

        self.assertEqual(sorted(result), sorted(expected_result))
        for pp in result:
            myFlaeche.vis_add_single_point(pp, 'blue')

            
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_zone_three_center_points')

        expected_result = [
            (255.0, 265.0), (255.0, 275.0), (255.0, 285.0), (255.0, 295.0),
            (255.0, 305.0), (255.0, 315.0), (255.0, 325.0), (255.0, 335.0), (255.0, 345.0),
            (265.0, 265.0), (265.0, 275.0), (265.0, 285.0), (265.0, 295.0), (265.0, 305.0),
            (265.0, 315.0), (265.0, 325.0), (265.0, 335.0),
            (275.0, 265.0), (275.0, 275.0), (275.0, 285.0), (275.0, 295.0),
            (285.0, 265.0), (285.0, 275.0), (285.0, 285.0), (285.0, 295.0),
            (295.0, 265.0), (295.0, 275.0),
            (305.0, 265.0), (315.0, 265.0), (325.0, 265.0), (335.0, 265.0)]

        for pp in result:
            myFlaeche.vis_add_single_point(pp, 'pink')

        self.assertEqual(sorted(result), sorted(expected_result))


        # get extention points
        
        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation,
                                                     test_result='get_extention_center_points')

        expected_result = [
            (215.0, 225.0), (215.0, 235.0), (215.0, 245.0), (215.0, 255.0), (215.0, 265.0),
            (215.0, 275.0), (215.0, 285.0), (215.0, 295.0), (215.0, 305.0), (215.0, 315.0),
            (215.0, 325.0), (215.0, 335.0), (215.0, 345.0), (215.0, 355.0), (215.0, 365.0),
            (215.0, 375.0), (215.0, 385.0),
            (225.0, 225.0), (225.0, 235.0), (225.0, 245.0), (225.0, 255.0), (225.0, 265.0),
            (225.0, 275.0), (225.0, 285.0), (225.0, 295.0), (225.0, 305.0), (225.0, 315.0),
            (225.0, 325.0), (225.0, 335.0), (225.0, 345.0), (225.0, 355.0), (225.0, 365.0),
            (225.0, 375.0), (225.0, 385.0),
            (235.0, 225.0), (235.0, 235.0), (235.0, 245.0), (235.0, 255.0), (235.0, 265.0),
            (235.0, 275.0), (235.0, 285.0), (235.0, 295.0), (235.0, 305.0), (235.0, 315.0),
            (235.0, 325.0), (235.0, 335.0), (235.0, 345.0), (235.0, 355.0), (235.0, 365.0),
            (235.0, 375.0), (235.0, 385.0),
            (245.0, 225.0), (245.0, 235.0), (245.0, 245.0), (245.0, 255.0), (245.0, 265.0),
            (245.0, 275.0), (245.0, 285.0), (245.0, 295.0), (245.0, 305.0), (245.0, 315.0),
            (245.0, 325.0), (245.0, 335.0), (245.0, 345.0), (245.0, 355.0), (245.0, 365.0),
            (245.0, 375.0), (245.0, 385.0),
            (255.0, 225.0), (255.0, 235.0), (255.0, 245.0), (255.0, 255.0), (255.0, 355.0),
            (255.0, 365.0), (255.0, 375.0), (255.0, 385.0),
            (265.0, 225.0), (265.0, 235.0), (265.0, 245.0), (265.0, 255.0), (265.0, 355.0),
            (265.0, 365.0), (265.0, 375.0), (265.0, 385.0),
            (275.0, 225.0), (275.0, 235.0), (275.0, 245.0), (275.0, 255.0), (275.0, 355.0),
            (275.0, 365.0), (275.0, 375.0), (275.0, 385.0),
            (285.0, 225.0), (285.0, 235.0), (285.0, 245.0), (285.0, 255.0), (285.0, 355.0),
            (285.0, 365.0), (285.0, 375.0), (285.0, 385.0),
            (295.0, 225.0), (295.0, 235.0), (295.0, 245.0), (295.0, 255.0), (295.0, 355.0),
            (295.0, 365.0), (295.0, 375.0), (295.0, 385.0),
            (305.0, 225.0), (305.0, 235.0), (305.0, 245.0), (305.0, 255.0), (305.0, 355.0),
            (305.0, 365.0), (305.0, 375.0), (305.0, 385.0),
            (315.0, 225.0), (315.0, 235.0), (315.0, 245.0), (315.0, 255.0), (315.0, 355.0),
            (315.0, 365.0), (315.0, 375.0), (315.0, 385.0),
            (325.0, 225.0), (325.0, 235.0), (325.0, 245.0), (325.0, 255.0), (325.0, 355.0),
            (325.0, 365.0), (325.0, 375.0), (325.0, 385.0),
            (335.0, 225.0), (335.0, 235.0), (335.0, 245.0), (335.0, 255.0), (335.0, 355.0),
            (335.0, 365.0), (335.0, 375.0), (335.0, 385.0),
            (345.0, 225.0), (345.0, 235.0), (345.0, 245.0), (345.0, 255.0), (345.0, 355.0),
            (345.0, 365.0), (345.0, 375.0), (345.0, 385.0),
            (355.0, 225.0), (355.0, 235.0), (355.0, 245.0), (355.0, 255.0), (355.0, 265.0),
            (355.0, 275.0), (355.0, 285.0), (355.0, 295.0), (355.0, 305.0), (355.0, 315.0),
            (355.0, 325.0), (355.0, 335.0), (355.0, 345.0), (355.0, 355.0), (355.0, 365.0),
            (355.0, 375.0), (355.0, 385.0), (365.0, 225.0), (365.0, 235.0), (365.0, 245.0),
            (365.0, 255.0), (365.0, 265.0), (365.0, 275.0), (365.0, 285.0), (365.0, 295.0),
            (365.0, 305.0), (365.0, 315.0), (365.0, 325.0), (365.0, 335.0), (365.0, 345.0),
            (365.0, 355.0), (365.0, 365.0), (365.0, 375.0), (365.0, 385.0),
            (375.0, 225.0), (375.0, 235.0), (375.0, 245.0), (375.0, 255.0), (375.0, 265.0),
            (375.0, 275.0), (375.0, 285.0), (375.0, 295.0), (375.0, 305.0), (375.0, 315.0),
            (375.0, 325.0), (375.0, 335.0), (375.0, 345.0), (375.0, 355.0), (375.0, 365.0),
            (375.0, 375.0), (375.0, 385.0), (385.0, 225.0), (385.0, 235.0), (385.0, 245.0),
            (385.0, 255.0), (385.0, 265.0), (385.0, 275.0), (385.0, 285.0), (385.0, 295.0),
            (385.0, 305.0), (385.0, 315.0), (385.0, 325.0), (385.0, 335.0), (385.0, 345.0),
            (385.0, 355.0), (385.0, 365.0), (385.0, 375.0), (385.0, 385.0)]
        
        self.assertEqual(sorted(result), sorted(expected_result))
        
        for pp in result:
            myFlaeche.vis_add_single_point(pp, 'darkseagreen')
        
        myFlaeche.vis_add_current(myFlaeche.get_cell((vessel.x, vessel.y)))

        myFlaeche.draw_course(vessel, vessel.r, 2 * math.pi * vessel.r )
        if visual:
            myFlaeche.vis_show()

        result = vessel.get_reachable_center_points( (vessel.x, vessel.y),
                                                     vessel.rotation)
        expected_result = [
            (315.0, 315.0, 17, -3, 49.66666666666667, 0.3493443980164794, 0.40268456375838924),
            (265.0, 345.0, 3, 53, 26.584905660377355, 3.0285057797001587, 0.752306600425834),
            (275.0, 345.0, 10, 45, 23.611111111111114, 2.7042547618419093, 0.8470588235294116),
            (285.0, 345.0, 17, 38, 22.80263157894737, 2.3002659553237232, 0.8770917484131563),
            (295.0, 345.0, 24, 31, 24.79032258064516, 1.8239805813548409, 0.8067664281067014),
            (305.0, 335.0, 24, 17, 25.441176470588232, 1.2325938747921426, 0.7861271676300579),
            (305.0, 345.0, 31, 24, 32.020833333333336, 1.3176120722349525, 0.6245933636955107),
            (315.0, 325.0, 24, 3, 97.5, 0.24870998909352288, 0.20512820512820512),
            (315.0, 335.0, 31, 10, 53.04999999999999, 0.6240842431250666, 0.3770028275212065),
            (315.0, 345.0, 38, 17, 50.970588235294116, 0.84132669826607, 0.39238315060588574),
            (325.0, 315.0, 24, -10, 33.800000000000004, 0.789582239399523, 0.5917159763313609),
            (325.0, 325.0, 31, -3, 161.66666666666669, 0.1929475503651738, 0.12371134020618556),
            (325.0, 335.0, 38, 3, 242.16666666666669, 0.1575679219782876, 0.08258774948382656),
            (325.0, 345.0, 45, 10, 106.25000000000001, 0.4373378917478839, 0.18823529411764703),
            (335.0, 275.0, 3, -45, 22.599999999999998, 3.0084563260381456, 0.8849557522123894),
            (335.0, 285.0, 10, -38, 20.31578947368421, 2.626945223647616, 0.9844559585492229),
            (335.0, 295.0, 17, -31, 20.161290322580644, 2.138384545211552, 0.9920000000000001),
            (335.0, 305.0, 24, -24, 24.0, 1.5707963267948966, 0.8333333333333334),
            (335.0, 315.0, 31, -17, 36.76470588235294, 1.003208108378241, 0.544),
            (335.0, 325.0, 38, -10, 77.2, 0.5146474299421773, 0.25906735751295334),
            (335.0, 335.0, 45, -3, 339.0, 0.13313632755164762, 0.058997050147492625),
            (335.0, 345.0, 53, 3, 469.66666666666663, 0.11308687388963476, 0.042583392476934),
            (345.0, 265.0, 3, -60, 30.075000000000003, 3.0416758621459077, 0.6650041562759766),
            (345.0, 275.0, 10, -53, 27.443396226415093, 2.7686188502553595, 0.7287727741491922),
            (345.0, 285.0, 17, -45, 25.711111111111112, 2.41918514260243, 0.7778738115816767),
            (345.0, 295.0, 24, -38, 26.57894736842105, 2.014960130605857, 0.7524752475247525),
            (345.0, 305.0, 31, -31, 31.000000000000004, 1.5707963267948966, 0.6451612903225805),
            (345.0, 315.0, 38, -24, 42.083333333333336, 1.1266325229839362, 0.4752475247524752),
            (345.0, 325.0, 45, -17, 68.05882352941177, 0.7224075109873633, 0.2938634399308557),
            (345.0, 335.0, 53, -10, 145.45, 0.37297380333443353, 0.13750429700928155),
            (345.0, 345.0, 60, -3, 601.5, 0.09991679144388552, 0.03325020781379884)
        ]
        
        self.assertEqual( result, expected_result)
            
        
    @unittest.skipIf(finished, 'done')
    def test_get_reachables(self):
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10,output='reachables')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        vessel.x = 100; vessel.y = 105; vessel.rotation = 0; vessel.r = 20
#        vessel.radiussdfa=20

        expected_result = sorted([(11, 10), (11, 9), (11,11)])
        result = vessel.get_reachables()
        self.assertEqual(result, expected_result)

        
        vessel.x = 100; vessel.y = 105; vessel.rotation = 0; vessel.r = 80
        expected_result = sorted([(11.0, 10.0), (12.0, 9.0), (12.0, 10.0), (12.0, 11.0)])
        result = vessel.get_reachables()
        self.assertEqual(result, expected_result)

        
        vessel.x = 105; vessel.y = 105; vessel.rotation = math.pi/4;
        vessel.r = 180
        result =  vessel.get_reachables()

        myFlaeche.vis_add_current(myFlaeche.get_cell((vessel.x, vessel.y)))
        myFlaeche.vis_add_reachable(result)

        myFlaeche.draw_course(vessel, vessel.r, 45)
        myFlaeche.vis_show()
        
        #                 [(11.0, 10.0), (12.0, 9.0), (12.0, 10.0), (12.0, 11.0)])


        
    @unittest.skipIf(finished, 'done')
    def test_vehicle_pyhsics(self):
        """return a hypthetic (future) postition depending on the driving incliantion

        of the stearing wheal """
        
        myFlaeche = main.Flaeche(xdim=450,ydim=450,scale=10)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        

        """test sreight"""
        
        vessel.x = 100; vessel.y = 100; vessel.rotation = 0;  vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=0, delta=10), (110, 100, 0)) 
            
        vessel.x = 100; vessel.y = 100; vessel.rotation = math.pi; vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=0, delta=10), (90, 100, math.pi)) 

        vessel.x = 100; vessel.y = 100; vessel.rotation = math.pi/2; vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=0, delta=10), (100, 110, math.pi/2)) 

        vessel.x = 100; vessel.y = 100; vessel.rotation = -math.pi/2; vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=0, delta=10), (100.0, 90.0, -math.pi/2)) 

        vessel.x = 100; vessel.y = 100; vessel.rotation = math.pi/4; vessel.r = 20
        self.assertAlmostEqual(vessel.physics_stearing(driving=0, delta=14.14213562)[0], 110, 4) 
        self.assertAlmostEqual(vessel.physics_stearing(driving=0, delta=14.14213562)[1], 110, 4) 

        # driving hard right
        vessel.x = 100; vessel.y = 100; vessel.rotation = 0; vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=1, delta=math.pi/2 * 20),
                        (120, 120, math.pi/2)) 

        
        # driving hard left
        vessel.x = 100; vessel.y = 100; vessel.rotation = 0; vessel.r = 20
        self.assertEqual(vessel.physics_stearing(driving=-1, delta=math.pi/2 * 20),
                         (120.0, 80.0, -math.pi/2)) 

        # driving right, but not hard
        vessel.x = 100; vessel.y = 100; vessel.rotation = 0; vessel.r = 10
        self.assertEqual(vessel.physics_stearing(driving=0.5, delta=math.pi/2 * 20),
                         (120, 120, math.pi/2)) 

        
    @unittest.skipIf(finished, 'done')
    def test_flaeche(self):
        """constuctor test"""
        
        myFlaeche = main.Flaeche(xdim=20,ydim=10,scale=1)
        self.assertEqual(myFlaeche.cluster_length_x, 20)
        self.assertEqual(myFlaeche.cluster_length_y, 10)
        # return myId = 1


    @unittest.skipIf(finished, 'done')
    def test_flaeche_round(self):
        """rounding down dimention
        
        when Flaechen length is not a multipple of scale  """
        
        myFlaeche = main.Flaeche(xdim=20.5,ydim=20.5,scale=1)
        self.assertEqual(myFlaeche.cluster_length_x, 20)
        self.assertEqual(myFlaeche.cluster_length_y, 20)
        # return myId = 2

        
    @unittest.skipIf(finished, 'done')    
    def test_flaeche_clustersize(self):
        """create Flaeche with scale not 1"""
        
        myId = 3
        myFlaeche = main.Flaeche(xdim=20.5,ydim=20.5,scale=0.5)
        self.assertEqual(myFlaeche.cluster_length_x, 41)
        self.assertEqual(myFlaeche.cluster_length_x, 41)
    
    @unittest.skipIf(devel_run, '')
    def test_flaeche_coord_is_legal(self):
        """test to vertify that a  given position is within
           the limits of the flaeche"""
        myFlaeche = main.Flaeche(xdim=30, ydim=30, scale=0.5)
        self.assertFalse(myFlaeche.is_valid_coord_pos((-1,  0)))
        self.assertFalse(myFlaeche.is_valid_coord_pos(( 0, -1)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((-1, -1)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((31,  0)))
        self.assertFalse(myFlaeche.is_valid_coord_pos(( 8, 31)))
        self.assertFalse(myFlaeche.is_valid_coord_pos((31, 31)))
        self.assertTrue(myFlaeche.is_valid_coord_pos(( 7, 7)))
        
            
    @unittest.skipIf(finished, 'done')
    def test_flaeche_node_is_legal(self):
        """test to vertify if a node is inside a Flaeche"""

        myFlaeche = main.Flaeche(xdim=100,ydim=100,scale=1)
        self.assertFalse(myFlaeche.is_valid_node_pos((200,200))) 
        self.assertTrue(myFlaeche.is_valid_node_pos((0,0)))

    @unittest.skipIf(finished, 'done')
    def test_flaeche_get_cell_and_cells(self):
        myFlaeche = main.Flaeche(xdim=100,ydim=100,scale=0.7)
        self.assertEqual(myFlaeche.get_cell((21.5, 21.5)), (30, 30))
        
        myFlaeche = main.Flaeche(xdim=100,ydim=100,scale=1)

        self.assertEqual(myFlaeche.get_cell((15.5, 15.5)), (15, 15))
        self.assertEqual(myFlaeche.get_cell((15, 15)), (15, 15))

        ### cells
        self.assertEqual(myFlaeche.get_cells((15.5, 15.5)), [(15, 15)])
        self.assertEqual(myFlaeche.get_cells((15, 15.5)), [(14, 15), (15, 15)])
        self.assertEqual(myFlaeche.get_cells((0, 15.5)), [(0, 15)])

        self.assertEqual(myFlaeche.get_cells((15.5, 15)), [(15, 14), (15, 15)])
        self.assertEqual(myFlaeche.get_cells((15.5, 0)), [(15, 0)])

        self.assertEqual(myFlaeche.get_cells((15, 15)), [(14, 14), (14, 15), (15, 14), (15, 15)])
        self.assertEqual(myFlaeche.get_cells((15, 0)), [(14, 0), (15, 0)])
        self.assertEqual(myFlaeche.get_cells((0, 15)), [(0, 14), (0, 15)])
        self.assertEqual(myFlaeche.get_cells((0.5, 0.5)), [(0, 0)])

    def test_get_cell_and_sector(self):
        """test to vertify if a node is inside a Flaeche"""

        myFlaeche = main.Flaeche(xdim=100,ydim=100,scale=1)
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 0 )[1]              ,  0 )
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 0.0001 )[1]         ,  0 )
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 2*math.pi/16/2 )[1] ,  0 )
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 2*math.pi/16/2 +0.001)[1] , 1)
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 2*math.pi-0.001)[1] ,  0 )
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), math.pi-0.001)[1]   ,  8 )

        # multi_turn
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 2*math.pi )[1]      ,  0 )
        self.assertEqual(myFlaeche.get_cell_and_sector( (1.5,1.5), 4*math.pi/16/2 +0.001)[1] , 1)
        
     
        

#    @unittest.skipIf(finished, 'done')
    def test_nodeData(self):
        """test to vertify if a node is inside a Flaeche"""

        myFlaeche = main.Flaeche(xdim=100,ydim=100,scale=1)
        self.assertEqual(main.NodeDataHandler(myFlaeche.cluster[1][1]).center_x, 1.5)
        self.assertEqual(main.NodeDataHandler(myFlaeche.cluster[1][1]).center_y, 1.5)
        
        self.assertEqual(main.NodeDataHandler(myFlaeche.cluster[0][0]).center_x, 0.5)
        self.assertEqual(main.NodeDataHandler(myFlaeche.cluster[0][0]).center_y, 0.5)

        
class someTestcase_ada_star(unittest.TestCase):
########### adastar #######################################        
        
    @unittest.skipIf(finished, '')    
    def test_adastar_nodes_id_creation(self):
        """test to check the id string and get_coords of a main.AdAStarNode"""

        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        myAdA     = main.AdAStar(start_node=(0, 0, 0), end_node=(20, 10, 0), vessel=vessel)

        dn_2   = myFlaeche.get_node_data((3, 3)) 
        mySN   = StarNodeC(dn_2, sector=0, tt=4, dd=5, lastNode=None)
        
 #       mySN      = StarNodeC(xx=3 ,yy=3, sector=0 ,tt=4, dd=5, lastNode=None)
        self.assertEqual(mySN.id, '3_3_0')
        self.assertEqual(mySN.get_coords(), (3, 3, 0))


    @unittest.skipIf(devel_skip, '')    
    def test_adastar_nodes_basic_funtionality(self):
        """testing init, get_min,  the home brewn node list """    

        """test the ways of initialiation of DNLists"""
        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])

        
        myD = main.AdAStar(myFlaeche, start_node=(0, 0), end_node=(20,10))

        myDN_1 = main.StarNode(xx=3  ,yy=3, sector=0 ,tt=4, dd=5, lastNode=None)
        myDN_2 = main.StarNode(xx=4  ,yy=3, sector=0 ,tt=4, dd=5, lastNode=None)
        myDN_3 = main.StarNode(xx=5  ,yy=3, sector=0 ,tt=4, dd=5, lastNode=None)
        myDN_4 = main.StarNode(xx=20 ,yy=9, sector=0 ,tt=2, dd=1, lastNode=None)


        myDD_1 = DNList([myDN_1, myDN_2, myDN_3])
    
        """insert a few nodes into the list, and iter over them by id"""
        myDNL_id_1 = DNList([myDN_1, myDN_2, myDN_3], 'id')
        self.assertEqual([ii for ii in myDNL_id_1], ['3_3_0', '4_3_0', '5_3_0'])

        """alter the order of the few nodes , and iter over them by id"""
        myDNL_id_2 = DNList([myDN_3, myDN_2, myDN_1], 'id')
        self.assertEqual([ii for ii in myDNL_id_2], ['5_3_0', '4_3_0', '3_3_0'])
        
        """insert a few nodes into the list, and iter over them by tuple"""
        myDNL_tuple = DNList([myDN_1, myDN_2, myDN_3], 'tuple')
        self.assertEqual([ii for ii in myDNL_tuple], [(3, 3, 0), (4, 3, 0), (5, 3, 0)])
        
        """return a node by its id, if not exists return none """
        myDNL_ret_id = DNList([myDN_1, myDN_2, myDN_3], 'id')
        self.assertEqual(myDNL_ret_id.get_by_id('3_3_0'), myDN_1)
        self.assertIsNone(myDNL_ret_id.get_by_id('3_3_1'))

        """return a node by its tuple, if not exists return none """
        myDNL_ret_tup = DNList([myDN_1, myDN_2, myDN_3], 'tuple')
        self.assertEqual(myDNL_ret_id.get_by_tuple((3,3,0)), myDN_1)
        self.assertIsNone(myDNL_ret_id.get_by_tuple((3,3,1)))

        """return a node by its tuple, even if the DNodeList iters on ids """
        myDNL_ret_tup = DNList([myDN_1, myDN_2, myDN_3], 'id')
        self.assertEqual(myDNL_ret_id.get_by_tuple((3,3,0)), myDN_1)


######## redunant to test_dijksta # doesn't do any diffrent there
#        keep as comment, in case the DNList becomes a subclass
#
#        """test to retrive the minimal node from List"""
#        myDNL_get_min = DNList([myDN_1, myDN_2, myDN_3, myDN_4])
#        self.assertEqual(myDNL_get_min.get_min_node(), myDN_4)
#        
#        """test to retrive the minimal node and remove it from the list"""
#        myDNL_pop_min = DNList([myDN_1, myDN_2, myDN_3, myDN_4])
#        self.assertEqual(myDNL_pop_min.get_min_node(pop=True), myDN_4)
#        self.assertNotEqual(myDNL_pop_min.get_min_node(), myDN_4)


        """test to get back the tuples of all nodes in the list"""
        myDNL_tup_list = main.AdAStar.DNList([myDN_1, myDN_2, myDN_3])
        self.assertEqual(myDNL_tup_list.get_tuples(), [(3, 3, 0), (4, 3, 0), (5, 3, 0)])
        
        """test to see what happens if the node list is empty"""
        

        """make sure that only each node id is only once in the list
           should be already managed in the initalization
           keep a 'global'/class list with all nodes
           
           * handling alternation after creation - maybe function_closurures
           * overwriting the append function
           
           """
        
        """make sure that only nodes from the same AdAStar are added"""
        # currently there is only one AdAStar at a time        

        """test to make sure, the list only takes adastar nodes"""
        # this does not work - not too urgent now        
        # self.assertRaises(AssertionError, main.AdAStar.DNList, [myDN_1, myDN_2, 1] )
        
        """insert a few nodes into the list, and iter over them returning 
           the distance traveled"""  # to be done when needed

        """insert a few nodes into the list, and iter over them returning 
           the total distance """   # to be done when needed

    
    @unittest.skipIf(devel_run, 'done')
    def test_dikstra_get_nodes_list(self):
        """test the getter of the nodes lists"""
        
        start = (10, 10, 0)
        end   = (20, 10, 0)
        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)

        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        
        myD = main.AdAStar(start_node=start, end_node=end, vessel=vessel)
        self.assertEqual(myD.get_open_nodes(), [])

        dn_1   = myFlaeche.get_node_data((3, 3)) 
        myDN_1 = StarNodeC(dn_1, sector=0, tt=4, dd=5, lastNode=None)

        dn_2   = myFlaeche.get_node_data((4, 3)) 
        myDN_2 = StarNodeC(dn_2, sector=0, tt=4, dd=5, lastNode=None)

        dn_3   = myFlaeche.get_node_data((5, 5)) 
        myDN_3 = StarNodeC(dn_3, sector=0, tt=4, dd=5, lastNode=None)

        dn_4   = myFlaeche.get_node_data((5, 6)) 
        myDN_4 = StarNodeC(dn_4, sector=0, tt=4, dd=5, lastNode=None)

        
#        myDN_1 = StarNodeC(xx=3 ,yy=3, sector=0 ,tt=4, dd=5, lastNode=None)
#        myDN_2 = StarNodeC(xx=4 ,yy=3, sector=0 ,tt=4, dd=5, lastNode=None)
#        myDN_3 = StarNodeC(xx=5 ,yy=5, sector=0 ,tt=4, dd=5, lastNode=None)
#        myDN_4 = StarNodeC(xx=5 ,yy=6, sector=0 ,tt=4, dd=5, lastNode=None)

        """get the nodes tuples"""
        myD.open_nodes_list [0:0] = [myDN_1, myDN_2] 
        self.assertEqual(sorted(myD.get_open_nodes('tuples')), sorted([(3, 3, 0), (4, 3, 0)]))

######## redunant to test_dijksta # doesn't do any diffrent there
#        keep as comment, in case the DNList becomes a subclass
#
#        """get the node them self """
#        self.assertEqual(sorted(myD.get_open_nodes()), sorted([myDN_1, myDN_2]))

        """get the node tuples of closed list """
        myD.closed_nodes_list [0:0] = [myDN_3, myDN_4] 
        self.assertEqual(sorted(myD.get_closed_nodes('tuples')), sorted([(5, 5, 0), (5, 6, 0)]))
        

    @unittest.skipIf(devel_skip, 'done')    
    def test_get_neighbours(self):
        """test to receive any neigbour, regardless wheather legal or not"""

        start = ( 0,  0)
        end   = (20, 10)
        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)
        myD = main.AdAStar(myFlaeche, start, end)
        self.assertEqual(myFlaeche.get_neighbours((2,2)), [(1,1), (1,2), (1,3), (2,1), (2,3), (3,1), (3,2), (3,3)])
        self.assertEqual(myFlaeche.get_neighbours((0,2)), [(0,1), (0,3), (1,1), (1,2), (1,3)])
        #return myId = 5


    @unittest.skipIf(devel_skip, '')    
    def test_neighbours(self):
        """test to vertify two cells are neibourghs """

        start = ( 0,  0)
        end   = (20, 10)
        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)
        myD = main.AdAStar(myFlaeche, start, end)
        self.assertTrue(myFlaeche.neighbours((2,2), (1,1)))
        self.assertTrue(myFlaeche.neighbours((2,2), (2,3)))
        self.assertFalse(myFlaeche.neighbours((2,2), (2,4)))
        self.assertRaises(AssertionError, myFlaeche.neighbours, (2,2), (2,2))
  

##############
#     this test is not realy usefull now, because it catches an error for 
#     both: not giving the coorrect coordinates or giving coordinates without angle
#     Maybe make the teste better later or remove in 2016 :-)        
        
#    @unittest.skipIf(devel_run, 'done')
#    def test_adastar_to_do(self):
#        """test for illegal start or end possitions"""
#        
#        start = (0, 0, 0)
#        end = (20, 10, 0)
#        myFlaeche = main.Flaeche(xdim=10,ydim=10,scale=1)
        #    self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), ( 1,  1) )

        
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1,  1), ( 1, 11) )

#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, ( 1, 11), ( 1, 11) )
        
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), (11, 11) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11,  1), ( 1, 11) )
        
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), (11, 11) )
        1#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), (11,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 1,  1) )
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 1, 11) )
        
#        self.assertRaises(StandardError, main.AdAStar, myFlaeche, (11, 11), ( 0, 0) )
 
    
    # is visual
    @unittest.skipIf(devel_run, '')    
    def test_dijsktra_step_internals(self):  
        """ Test for adastar's results after the first step        """
        
        """test if the start point is added to the open node list"""

        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,output='result_ada_star')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.r = 20        
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;

        
        start = (30, 10, 0)  # cell coordinates
        end   = (40, 10, 0)

        myD_step_zero = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        myD_step_zero.step()        
        self.assertEqual(myD_step_zero.get_open_nodes('tuples'), [( 30, 10, 0) ])
        self.assertEqual(myD_step_zero.get_closed_nodes('tuples'), [])

        expected_result = [
            (30, 6, 8), (30, 14, 8),

            (31, 6, 7),  (31, 10, 0), (31, 14, 7),
            (32, 6, 6),  (32, 7, 5),  (32, 8, 4),  (32, 9, 2),  (32, 10, 0),
            (32, 11, 2), (32, 12, 4), (32, 13, 5), (32, 14, 6)
        ]

        myD_step_zero.step()
        result = myD_step_zero.get_open_nodes('tuples')
        self.assertEqual(sorted(myD_step_zero.get_open_nodes('tuples')),
                         sorted(expected_result))

        myD_step_zero.step()
#        for oo in myD_step_zero.get_closed_nodes():
#            print oo.__dict__


        """test that nodes in the closed nodes list are omitted"""
        myD_fake_closed = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        # make a first step
        myD_fake_closed.step()    

        # this is the second one to choose
        dn_2d  = myFlaeche.get_node_data((31, 10)) 
        myDN_2 = StarNodeC(dn_2d, sector=0, tt=4, dd=5, lastNode=None)

        myD_fake_closed.closed_nodes_list.append(myDN_2)
        
        # should find myDN_2 as suspicious node
        # and reject as it already is in closed node list
        
        myD_fake_closed.step()
        self.assertTrue((31, 10, 0) not in myD_fake_closed.get_open_nodes('tuples'))


    @unittest.skipIf(devel_run, '')    
    def test_dijsktra_step_internals_update_open_list(self):  
        """ Test for adastar's results after the first step        """

        """ test if the start point is added to the open node list"""
        
        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,output='result_ada_star')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.r = 20        
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;
        
        start = (30, 10, 0)  # cell coordinates
        end   = (40, 10, 0)

        myD_fake_open = main.AdAStar(vessel=vessel, start_node=start, end_node=end)
        

        
        # make a first step
        myD_fake_open.step()    

        # put a fake dummy of a node which will be found into the open list
        # put bad data for the dummy so the real one will be better and
        #the node data has to be  updated

        test_node_tuple = (31, 10, 0)
        
        dn_2d  = myFlaeche.get_node_data((test_node_tuple[0], test_node_tuple[1])) 
        myDN_2 = StarNodeC(dn_2d, sector=test_node_tuple[2], tt=4, dd=180, lastNode=None)
        myD_fake_open.open_nodes_list.append(myDN_2)
 
        # make sure that the fake one is realy in the open list
        self.assertTrue( (31, 10, 0) in myD_fake_open.get_open_nodes('tuples'))
        faked_node_data = ANList(myD_fake_open.open_nodes_list).get_by_tuple(test_node_tuple)
        self.assertEqual(faked_node_data.full,184)

        # make a step  
        myD_fake_open.step()

        # make sure the node data has been updated
        self.assertTrue( (31, 10, 0) in myD_fake_open.get_open_nodes('tuples'))
        faked_node_data = ANList(myD_fake_open.open_nodes_list).get_by_tuple(test_node_tuple)
        self.assertEqual(faked_node_data.full,10)


    @unittest.skipIf(devel_run, '')    
    def test_dijsktra_step_internals_find_final(self):
        """ Test for adastar's results after the first step        """

        """test that the algo stops if the end is reached - manipulated lists"""
        
        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,output='result_ada_star')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.r = 20        
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;
        
        start = (30, 10, 0)  # cell coordinates
        end   = (40, 10, 0)

        ###
        myD_fake_open = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD_fake_open.step()    
        myD_fake_open.step()    
        
        test_node_tuple = (40, 10, 0)
        
        dn_2d  = myFlaeche.get_node_data((test_node_tuple[0], test_node_tuple[1])) 
        myDN_2 = StarNodeC(dn_2d, sector=test_node_tuple[2], tt=1, dd=1, lastNode=None)
        myD_fake_open.open_nodes_list.append(myDN_2)
        self.assertTrue(myD_fake_open.step())
        

    @unittest.skipIf(devel_run, 'done')
    def test_dijsktra_run_and_rebuild (self): 
        """run the algorithm on a simple example"""
        visual = VISUAL
        visual = True

####

        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,
                                 output='result_ada_star_full_run_straight')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.r = 20        
#       vessel.x = 300; vessel.y = 305; vessel.rotation = 0;
        
        start = (30, 10, 0)  # cell coordinates
        end   = (40, 10, 0)

        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)

        myD.run()
        myD.rebuild_path()
        myD.patch_path()

        path = ANList(myD.path, 'tuples').get_tuples()
        self.assertEqual(path, [
            (30, 10, 0), (31, 10, 0), (32, 10, 0), (33, 10, 0),
            (34, 10, 0), (35, 10, 0), (36, 10, 0), (37, 10, 0),
            (38, 10, 0), (39, 10, 0), (40, 10, 0)
        ])

        if visual:
            myD.draw_path()
        del(myD)

    @unittest.skipIf(devel_run, '')    
    def test_dijsktra_step_internals_rebuild_error(self):  
        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,output='result_ada_star')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.r = 20        

        myD = main.AdAStar((0, 0, 0), (10, 10, 0),vessel=vessel)
        self.assertRaisesRegexp(StandardError, "algorithm must be run first successfully",
                                myD.rebuild_path)


    @unittest.skipIf(devel_run, '')    
    def test_dijsktra_step_internals_find_final__a(self):
        visual = VISUAL
        visual = True
        myFlaeche = main.Flaeche(xdim=500,ydim=500,scale=10,
                                 output='result_hindrance_punctual_ada')
        vessel = main.Vessel(myFlaeche,
                             [(0, 0), (40, 40), (40, 200), (-40, 200), (-40, 40), ( 0, 0)])
        vessel.r = 20        

        blocked_nodes = [(xx, 15) for xx in range(5, 25)]
        myFlaeche.load_node_data(blocked_nodes)

        start = (30, 10, 0)  # cell coordinates
        end   = (40, 10, 0)

        start = (3,  11, 8)  # cell coordinates
        end   = (16, 19, 0)


        
        myD = main.AdAStar(vessel=vessel, start_node=start, end_node=end)


        myD.run(verbose=True, visual=visual)
        myD.rebuild_path()
        print 
        print
        
        for bla in  myD.path:
            print bla.id
#        main.make_movie(myFlaeche.output)

#        self.assertEqual(myD.DNList(myD.path, 'tuples').get_tuples(),
#                         [(3, 11), (4, 12), (4, 13), (4, 14), (4, 15),
#                          (5, 16), (6, 16), (7, 16), (8, 16), (9, 16),
#                          (10, 16), (11, 17), (12, 17), (13, 18),
#                          (14, 18), (15, 18), (16, 19)])
                         
        if visual:
            myD.draw_path()


    @unittest.skipIf(devel_skip, 'done')
    def test_wiki (self):
        visual = VISUAL
        """run the aglo like in the wikipedia example"""
        myFlaeche = main.Flaeche(xdim=300,ydim=300,scale=10, output='result_mediawiki_example')
        myD = main.AdAStar(myFlaeche, (17, 3), (3, 17))

        blocked_nodes = [(xx, 7) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 8) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 9) for xx in range(4, 16)]
        blocked_nodes[0:0] = [(xx, 10) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 11) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 12) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 13) for xx in range(13, 16)]
        blocked_nodes[0:0] = [(xx, 14) for xx in range(13, 16)]


        myFlaeche.load_node_data(blocked_nodes)
        myD = main.AdAStar(myFlaeche, (3, 19), (18, 3))
        myD.run(visual=visual)
        myD.rebuild_path()
        self.assertEqual(myD.DNList(myD.path, 'tuples').get_tuples(),
                         [(3, 19), (4, 18), (5, 17), (6, 16), (7, 15), (8, 15),
                          (9, 15), (10, 15), (11, 15), (12, 15), (13, 15), (14, 15),
                          (15, 15), (16, 14), (16, 13), (16, 12), (16, 11), (16, 10),
                          (16, 9), (16, 8), (16, 7), (17, 6), (17, 5), (17, 4), (18, 3)])

        if visual:
            myD.draw_path(final=True)
            main.make_movie(myFlaeche.output)


    @unittest.skipIf(devel_skip, 'done')
    def test_get_distance_to_end(self):
        """ttel the distance heurisitics from the point to dest."""
        
        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)
        myD = main.AdAStar(myFlaeche, (10, 10), (20, 10))
        self.assertEqual(myD.get_distance_to_end((15, 10)), 5)
        #self.assertEqual(myD.get_distance_to_end((15, 15)), 1.41421 * 5)
        self.assertEqual(myD.get_distance_to_end((20, 10)), 0)   # reach the end
        self.assertEqual(myD.get_distance_to_end((20, 20)), 10)  # negaive root
        self.assertRaises(StandardError, myD.get_distance_to_end, (40, 40)) # illegal poi
    
        # return myId = 6
        
    # very long - hence at the end
    @unittest.skipIf(devel_skip, 'done')
    def test_get_distance_between(self):
        """test the correct disstance between two points"""
        
        myId = 12
        myFlaeche = main.Flaeche(xdim=30,ydim=30,scale=1)
        myD = main.AdAStar(myFlaeche, (10, 10), (20, 10))
        # points are equal
        self.assertEqual(myD.get_distance_between_points((2, 2), (2, 2)), 0)
        # point a outside
        self.assertRaises(StandardError, myD.get_distance_between_points, (-3, -3), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, (-3,  8), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, ( 8, -3), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, ( 8, 40), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, (40,  8), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, (40, 40), (10, 10))

        # point b outside
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (-3, -3))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (-3,  8))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), ( 8, -3))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), ( 8, 40))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (40,  8))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (40, 40))

        # both points outside - check if both are different illegal values and check 
        # that even if twice the same illegal point is submitted it will not be accepted
        illegal_points = [(-3,-3), (-3, 8), (8, -3), (40, 40), (40, 8), (8, 40)]
        for ilpt_1 in illegal_points:
            for ilpt_2 in illegal_points:
                #print ilpt_1, ilpt_2
                self.assertRaises(StandardError, myD.get_distance_between_points, ilpt_1, ilpt_2)
                
        # both points are next to each other / all around
        self.assertEqual(myD.get_distance_between_points((10, 10), (10,  9)), myFlaeche.scale)
        self.assertEqual(myD.get_distance_between_points((10, 10), (10, 11)), myFlaeche.scale)
        self.assertEqual(myD.get_distance_between_points((10, 10), ( 9, 10)), myFlaeche.scale)
        self.assertEqual(myD.get_distance_between_points((10, 10), (11, 10)), myFlaeche.scale)
        
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), ( 9,  9)), myFlaeche.scale*1.41421356)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), ( 9, 11)), myFlaeche.scale*1.41421356)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (11,  9)), myFlaeche.scale*1.41421356)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (11, 11)), myFlaeche.scale*1.41421356)

        # 2 points a bit off
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (20, 10)), 10)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (21, 15)), 12, 0)
        # 121 + 25 = 145  appox 12 * 12


        ## same again with a driffent scale
        myFlaeche = main.Flaeche(xdim=9,ydim=9,scale=0.25)
        myD = main.AdAStar(myFlaeche, (10, 10), (20, 10))
        # points are equal
        self.assertEqual(myD.get_distance_between_points((2, 2), (2, 2)), 0)
        # point a outside
        self.assertRaises(StandardError, myD.get_distance_between_points, (-3, -3), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, (-3,  8), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, ( 8, -3), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, ( 8, 40), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, (40,  8), (10, 10))
        self.assertRaises(StandardError, myD.get_distance_between_points, (40, 40), (10, 10))

        # point b outside
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (-3, -3))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (-3,  8))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), ( 8, -3))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), ( 8, 40))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (40,  8))
        self.assertRaises(StandardError, myD.get_distance_between_points, (10, 10), (40, 40))

        # both points outside - check if both are different illegal values and check 
        # that even if twice the same illegal point is submitted it will not be accepted
        illegal_points = [(-3,-3), (-3, 8), (8, -3), (40, 40), (40, 8), (8, 40)]
        for ilpt_1 in illegal_points:
            for ilpt_2 in illegal_points:
                #print ilpt_1, ilpt_2
                self.assertRaises(StandardError, myD.get_distance_between_points, ilpt_1, ilpt_2)
                
        # both points are next to each other all around
        self.assertEqual(myD.get_distance_between_points((10, 10), (10,  9)), myFlaeche.scale)
        self.assertEqual(myD.get_distance_between_points((10, 10), (10, 11)), myFlaeche.scale)
        self.assertEqual(myD.get_distance_between_points((10, 10), ( 9, 10)), myFlaeche.scale)
        self.assertEqual(myD.get_distance_between_points((10, 10), (11, 10)), myFlaeche.scale)
        
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), ( 9,  9)), myFlaeche.scale*1.41421356)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), ( 9, 11)), myFlaeche.scale*1.41421356)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (11,  9)), myFlaeche.scale*1.41421356)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (11, 11)), myFlaeche.scale*1.41421356)

        # 2 points a bit off
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (20, 10)), 2.5)
        self.assertAlmostEqual(myD.get_distance_between_points((10, 10), (21, 15)), 3, 0)
        # 121 + 25 = 145  appox 12 * 12


 
if __name__ == '__main__':
 #   unittest.main()
    suite  = unittest.TestLoader().loadTestsFromTestCase(someTestcase)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(someTestcase_ada_star))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(test_dijkstra.someTestcaseDD))

    if True:#False:
        suite   = unittest.TestSuite();
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_update_open_list"))
#        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final"))
        suite.addTest(someTestcase_ada_star("test_dijsktra_step_internals_find_final__a"))
    
    unittest.TextTestRunner().run(suite)