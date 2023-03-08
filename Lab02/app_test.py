import unittest
import app
from unittest.mock import Mock

class ApplicationTest(unittest.TestCase):

    def setUp(self):

        fake_get_names = Mock()
        fake_get_names.return_value = (["William", "Oliver", "Henry", "Liam"], ["William", "Oliver", "Henry"])
        app.Application.get_names = fake_get_names
    
    def test_app(self):

        obj = app.Application()

        fake_get_random = Mock()
        fake_get_random.return_value = "Liam"
        app.Application.get_random_person = fake_get_random

        n_person = obj.select_next_person()
        self.assertEqual(n_person, "Liam")
        print(n_person, "selected")

        obj.notify_selected()



if __name__ == "__main__":
    unittest.main()