import unittest
import random
from server import dist, p1_eat_p2
#import mock
#from server import dist


class TestServerModule(unittest.TestCase):
    def test_dist(self):
        for x in range(100):
            x1 =  random.randint(-1000, 1000)
            x2 =  random.randint(-1000, 1000)
            y1 =  random.randint(-1000, 1000)
            y2 =  random.randint(-1000, 1000)
        self.assertTrue(dist(x1, y1, x2, y2) >= 0)

if __name__ == '__main__':
    unittest.main()
