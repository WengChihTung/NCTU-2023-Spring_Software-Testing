import unittest
import app
from unittest.mock import Mock

class ApplicationTest(unittest.TestCase):

    def setUp(self):

        fake_get_names = Mock()
        fake_get_names.return_value = (["William", "Oliver", "Henry", "Liam"], ["William", "Oliver", "Henry"])
        app.Application.get_names = fake_get_names
    
    def fake_write(self, name):
        context = "Congrats, " + name + "!"
        return context

    def fake_send(self, name, context):
        print(context)

    def test_app(self):

        obj = app.Application()

        fake_get_random = Mock()
        fake_get_random.return_value = "Liam"
        app.Application.get_random_person = fake_get_random

        n_person = obj.select_next_person()
        self.assertEqual(n_person, "Liam")
        print(n_person, "selected")

        fake_mail_write = Mock(side_effect = self.fake_write)
        fake_mail_send = Mock(side_effect = self.fake_send)
        app.MailSystem.write = fake_mail_write
        app.MailSystem.send = fake_mail_send

        obj.notify_selected()

        print("\n\n")
        print(fake_mail_write.call_args_list)
        print(fake_mail_send.call_args_list)

        self.assertEqual(fake_mail_write.call_count, fake_mail_send.call_count)
        self.assertEqual(fake_mail_write.call_count, 4)
        self.assertEqual(4, fake_mail_send.call_count)

if __name__ == "__main__":
    unittest.main()