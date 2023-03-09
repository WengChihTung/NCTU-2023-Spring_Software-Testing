import unittest
from calculator import Calculator
import math

addlist = [(1, 1), (2.0, 2.5), (3, 5), (1234, 86574), (-5324, 583)]
divlist = [(1, 1), (2, -5), (3.7, 8), (5213513, 480192803), (-13512323, 5985)]
sqrtlist = [9, 1523456, 5, 3, 0]
explist = [-42, 134, 0, -124, 5.354]


class ApplicationTest(unittest.TestCase):

    def test_add(self):
        for x, y in addlist:
            with self.subTest(x=x, y=y):
                self.assertEqual(Calculator.add(x, y), x + y)
        with self.assertRaises(TypeError):
            Calculator.add(2, "2.0")

    def test_divide(self):
        for x, y in divlist:
            with self.subTest(x=x, y=y):
                self.assertEqual(Calculator.divide(x, y), x / y)
        with self.assertRaises(ZeroDivisionError):
            Calculator.divide(2, 0)

    def test_sqrt(self):
        for x in sqrtlist:
            with self.subTest(x=x):
                self.assertEqual(Calculator.sqrt(x), math.sqrt(x))
        with self.assertRaises(ValueError):
            Calculator.sqrt(-2)

    def test_exp(self):
        for x in explist:
            with self.subTest(x=x):
                self.assertEqual(Calculator.exp(x), math.exp(x))
        with self.assertRaises(AttributeError):
            Calculator.expp(123412341324)


if __name__ == '__main__':
    unittest.main()
