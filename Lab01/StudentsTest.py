import unittest
import Students

class Test(unittest.TestCase):
    students = Students.Students()

    user_name = ['John', 'Mary','Thomas','Jane']
    user_id = []

    # test case function to check the Students.set_name function
    def test_0_set_name(self):
        print("\nStart set_name test\n")
        for i in range(0, 4):
            k = self.students.set_name(self.user_name[i])
            self.user_id.append(i)
            self.assertEqual(k, self.user_id[i])

            print(k, self.user_name[i])
        print("\nFinish set_name test\n")

    # test case function to check the Students.get_name function
    def test_1_get_name(self):
        print("\nStart get_name test\n")
        print("user_id length = ", len(self.user_id))
        print("user_name length = ", len(self.user_name), "\n")
        for i in range(0, 5):
            str = self.students.get_name(i)
            if i not in self.user_id:
                self.user_name.append('There is no such user')
            self.assertEqual(str, self.user_name[i])

            print("id ", i, " : ", str)
        print("\nFinish get_name test\n")