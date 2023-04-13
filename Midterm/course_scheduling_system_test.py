import unittest
import course_scheduling_system as css
from unittest.mock import Mock

class CSSTest(unittest.TestCase):

    def setUp(self):

        self.fake_check_exist = Mock()
        self.fake_check_exist.return_value = True
        css.CSS.check_course_exist = self.fake_check_exist
        
        self.obj = css.CSS()
    
    def test1(self):
        self.assertEqual(self.obj.add_course(('Algorithms', 'Monday', 3, 4)), True)
        self.assertEqual(self.obj.get_course_list(), [('Algorithms', 'Monday', 3, 4)])

    def test2(self):
        self.assertEqual(self.obj.add_course(('DataStructures', 'Tuesday', 3, 4)), True)
        self.assertEqual(self.obj.add_course(('OperatingSystem', 'Tuesday', 4, 5)), False)
        self.assertEqual(self.obj.get_course_list(), [('DataStructures', 'Tuesday', 3, 4)])

    def test3(self):

        self.fake_check_exist.return_value = False
        self.assertEqual(self.obj.add_course(("ComputerArchitecture", "Friday", 3, 4)), False)

    def test4(self):
        
        with self.assertRaises(TypeError):
            self.obj.add_course("fuck")

    def test5(self):

        self.obj.add_course(('Algorithms', 'Monday', 3, 4))
        self.obj.add_course(('DataStructures', 'Tuesday', 3, 4))
        self.obj.add_course(("ComputerArchitecture", "Friday", 3, 4))
        self.obj.remove_course(('DataStructures', 'Tuesday', 3, 4))

        self.assertEqual(self.obj.get_course_list(), [('Algorithms', 'Monday', 3, 4), ("ComputerArchitecture", "Friday", 3, 4)])
        self.assertEqual(self.obj.check_course_exist.call_count, 4)
        print("\n", self.obj)
        
    def test6(self):

        with self.assertRaises(TypeError):
            self.obj.add_course((1, 2, 3, 4))

        with self.assertRaises(TypeError):
            self.obj.add_course(("fuck", 2, 3, 4))
        
        with self.assertRaises(TypeError):
            self.obj.add_course(("fuck", "Friday", 3, 9))

        self.obj.remove_course(('OperatingSystem', 'Tuesday', 4, 5))

        self.fake_check_exist.return_value = False
        self.obj.remove_course(('OperatingSystem', 'Tuesday', 4, 5))

if __name__ == "__main__":
    unittest.main()