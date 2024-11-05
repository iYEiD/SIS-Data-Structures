#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

struct Student;

struct Course
{
    int courseID, coursecapacity;
    char courseName[50], instructorName[30];
    int startingHour, endingHour;
    string day;
    Course *next;
    Course *previous;
};

struct UniversityCourseList
{
    Course *head;
    Course *tail;
};

struct EnrolledStudent
{
    Student *studentReference;
    EnrolledStudent *next;
};

struct EnrolledCourse
{
    Course *courseReference;
    EnrolledCourse *next;
    EnrolledCourse *previous;
};

struct EnrolledCourseList
{
    EnrolledCourse *head;
    EnrolledCourse *tail;
};

struct Student
{
    int studentID;
    char firstName[30], lastName[30], emailAddress[50];
    EnrolledCourseList *enrolledCourses;
    Student *next;
};
/* ID (STUDENT ID)
data.csv is     for students accounts (ID,FNAME,LNAME,EMAIL)
enrollment.csv   for tracking enrollment actions (ID,COURSEID)
courselist.csv   for courses offered by uni (COURSEID,CAPACITY,COURSENAME,INSTRUCTORNAME,STARTINGHOUR,ENDINGHOUR,DAY)
Instructors.csv   for instructors (INSTRUCTORNAME, COURSEID)
attendace.csv     for attendance (ID,COURSEID,ATTENDANCE)
grades.csv        for grades (COURSEID,STUDENTID,GRADE)
*/


//-----------------------------------------------------------------------------------------------------------------------
// Extras :
struct InstructingCourse
{
    Course *c;
    InstructingCourse *next;
    InstructingCourse *previous;
};

struct InstructingCourseList
{
    InstructingCourse *head;
    InstructingCourse *tail;
};

struct Instructor
{
    string fname;
    string password;
    InstructingCourseList *thelist;
    Instructor *next;
};

Instructor *LinkInstructor(Instructor *instructor)
{
    ifstream file;
    string word;
    int i;
    Instructor *head = new Instructor;
    bool forhead = true;
    file.open("Instructors.csv");
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        i = 1;
        while (getline(ss, word, ','))
        {
            if (i == 1)
            {
                instructor->fname = word;
            }
            else if (i == 2)
            {
                instructor->password = word;
            }
            i++;
        }
        Instructor *instructor2 = new Instructor;
        instructor->next = instructor2;
        if (forhead)
        {
            head = instructor;
            forhead = false;
        }
        instructor = instructor2;
    } // linked the instructors , later we will link the logged in instructor to his courses in PrepareinstructorData
    return head;
}

bool checkadmin(Instructor *instructor)
{
    fstream file;
    string line;
    bool checkpas = false;
    file.open("Instructors.csv");
    while (getline(file, line))
    {
        // use stringstream
        stringstream ss(line);
        string word;
        bool checkpass = false;
        int i = 1;
        while (getline(ss, word, ','))
        {
            if (i == 1)
            {
                if (word == instructor->fname)
                {
                    checkpas = true;
                }
            }
            else if (i == 2)
            {
                if (checkpas)
                {
                    if (word == instructor->password)
                        return true;
                }
            }
            i++;
        }
    }
    return false;
}

void PrepareInstructorData(Instructor *instructor, UniversityCourseList *ListC)
{
    // this is to create a linked list for the Instructors courses(not sorted)
    InstructingCourseList *list = new InstructingCourseList;
    list->head = NULL;
    list->tail = NULL;

    InstructingCourse *temp = new InstructingCourse;
    temp->next = NULL;
    temp->previous = NULL;
    temp->c = NULL;

    Course *counter = ListC->head;
    bool him = false;
    bool thehead = true;
    while (counter != NULL)
    {

        if (counter->instructorName == instructor->fname)
            him = true;
        if (him)
        {
            if (thehead)
            {
                temp->c = counter;
                thehead = false;
                temp->previous = NULL;
                temp->next = new InstructingCourse;
                list->head = temp;
                temp->next->previous = temp;
                temp = temp->next;
                him = false;
            }
            else
            {
                temp->c = counter;
                temp->next = new InstructingCourse;
                him = false;
                temp->next->previous = temp;
                list->tail = temp;
                temp = temp->next;
            }
        }counter=counter->next;
    }

    if (list->head == NULL)
        return;
    if (list->tail != NULL)
        list->tail->next = NULL; // for instructors who teach 1 course only
    instructor->thelist = list;
    // Courses instructor teaches are now in the list
    // i used the same logic from GETSTUDENTDATA
}

void whatdoiinstruct(Instructor *instructor)
{
    InstructingCourse *counter = instructor->thelist->head;
    while (counter != NULL)
    {
        if (counter->c->instructorName == instructor->fname)
        {
            cout << counter->c->courseName << endl;
        }
        counter = counter->next;
    }
}

void attendance(Instructor *instructor)
{
    cout << "Please enter the course ID: ";
    int id;
    cin >> id;
    bool verified = false;
    InstructingCourse *counter = instructor->thelist->head;
    while (counter->next != NULL)
    {
        if (counter->c->courseID == id)
            if (instructor->fname == counter->c->instructorName)
                verified = true;
        counter = counter->next;
    }
    if (verified)
    {
        fstream file;
        fstream attendance;
        file.open("enrollment.csv");
        attendance.open("attendance.csv", ios::app);
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string word;
            string studentid;
            int i = 1;
            while (getline(ss, word, ','))
            {
                if (i == 1)
                {

                    studentid = word;
                }
                else if (i == 2)
                {
                    if (word == to_string(id))
                    {
                        cout << "Is student " << studentid << " present? (Y/N)";
                        char choice;
                        cin >> choice;
                        if (choice == 'Y')
                        {
                            attendance << id << "," << studentid << ","
                                       << "P" << endl;
                        }
                        else if (choice == 'N')
                        {
                            attendance << id << "," << studentid << ","
                                       << "A" << endl;
                        }
                    }
                }
                i++;
            }
        }

        file.close();
        attendance.close();
    }
    else
        cout << "You are not authorized to do this action,This isn't your course!" << endl;
}

void gradestudent(Instructor *instructor)
{
    cout << "Please enter the course ID: ";
    int id;
    cin >> id;
    bool verified = false;
    InstructingCourse *counter = instructor->thelist->head;
    while (counter->next != NULL)
    {
        if (counter->c->courseID == id)
            if (instructor->fname == counter->c->instructorName)
                verified = true;
        counter = counter->next;
    }
    if (verified)
    {
        fstream file;
        fstream grades;
        file.open("enrollment.csv");
        grades.open("grades.csv", ios::app);
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string word;

            string studentid;
            int i = 1;
            while (getline(ss, word, ','))
            {
                if (i == 1)
                {
                    studentid = word;
                }
                else if (i == 2)
                {
                    if (word == to_string(id))
                    {
                        cout << "Enter Student " << studentid << " grade: ";
                        int grade;
                        cin >> grade;
                        grades << id << "," << studentid << "," << grade << endl;
                    }
                }
                i++;
            }
        }

        file.close();
        grades.close();
    }
    else
        cout << "You are not authorized to do this action,This isn't your course!" << endl;
}

void adminloggedin(Instructor *instructor, UniversityCourseList *listc)
{   PrepareInstructorData(instructor, listc);
    cout << "Welcome Dr. " << instructor->fname << endl;
    int choice;
    cout << "1. List all courses I instruct" << endl;
    cout << "2. Do class attendance" << endl;
    cout << "3. Grade students" << endl;
    cout << "Your choice : ";
    cin >> choice;
    switch (choice)
    {
    case 1:
        whatdoiinstruct(instructor);
        break;
    case 2:

        attendance(instructor);
        break;

    case 3:
        gradestudent(instructor);
        break;

    default:
        break;
    }
}

void Instructorlogin(UniversityCourseList *listc)
{
    Instructor *instructor = new Instructor;
    cout << "Please verify your credentials to continue : " << endl;
    cout << "Dr. : ";
    cin >> instructor->fname;
    cout << "Please enter your password: ";
    cin >> instructor->password;
    if (checkadmin(instructor))
        adminloggedin(instructor, listc); // login;
    else
        cout << "Wrong credentials, please try again" << endl;
}

void viewmygrade(Student *s){
fstream grades;
grades.open("grades.csv");
string line;
 bool student=false ;
while(getline(grades,line)){
    stringstream ss(line);
    string word;
   
    string studentid ;
    int i = 1;
    string id;
    while(getline(ss,word,',')){
        if(i == 1){
          id=word;
        }
        else if (i==2){
            if(word == to_string(s->studentID)){
                student=true;
            }
        }
        else if(i==3){
            if(student){
                cout<<"Course ID: "<<id<<" Grade: "<<word<<endl;
            }
        }
        i++;
    }
}}
//-----------------------------------------------------------------------------------------------------------------------
string convertToString(char *a, int size);
void Showmycourses(Student *s);
void bubblesort(Student *s);
bool emailformat(char email[])
{
    string mail = convertToString(email, 50);
    if (!isalpha(mail[0]))
        return false;
    else
    {
        int posAT = -1, posdot = -1;
        for (int i = 0; i < mail.length(); i++)
        {
            if (mail[i] == '@')
                posAT = i;
            else if (mail[i] == '.')
                posdot = i;
        }

        if (posAT == -1 || posdot == -1)
            return false;
        if (posdot < posAT || posdot == mail.length() - 1)
            return false;
    }
    return true;
}

int IDGen(Student *s)
{
    ifstream data("data.csv");
    string line;
    int linecount = 0;
    if (data.is_open())
    {
        while (data.peek() != EOF)
        {
            getline(data, line);
            linecount++;
        }
        s->studentID = linecount + 1;
        data.close();
        return s->studentID;
    }
    else
        cout << "There was an error Creating your ID , Please Re-enter the program ";
    return -1;
}

void ListAllCourses(UniversityCourseList *listc)
{
    Course *c = listc->head;

    while (c->next != NULL)
    {
        cout << c->courseID << "." << c->courseName << " by DR. " << c->instructorName << " on " << c->day << " from " << c->startingHour << " to " << c->endingHour << endl;
        c = c->next;
    }
}

UniversityCourseList *LinkTheCourses(Course *c)
{
    ifstream file;
    string word;
    int i;
    bool forhead = true;
    Course *head = new Course;

    UniversityCourseList *ListC = new UniversityCourseList;

    file.open("courselist.csv");
    string line; // line of data , lezem asmo ba3d el ,
    while (getline(file, line))
    {

        stringstream ss(line);
        i = 1; // counter by column
        while (getline(ss, word, ','))
        {
            // filling data
            if (i == 1)
                c->courseID = stoi(word);
            else if (i == 2)
                c->coursecapacity = stoi(word);
            else if (i == 3)
                strcpy(c->courseName, word.c_str());
            else if (i == 4)
                strcpy(c->instructorName, word.c_str());
            else if (i == 5)
                c->startingHour = stoi(word);
            else if (i == 6)
                c->endingHour = stoi(word);
            else if (i == 7)
                c->day = word;
            i++;
        }

        // switch to second course

        Course *newCourse = new Course;
        c->next = newCourse;
        newCourse->previous = c;
        // saving the head
        // ListC hye Kel el CourseList
        if (forhead)
        {
            ListC->head = c;
            head = c;
            head->previous = NULL;
            forhead = false;
        }
        ListC->tail = c;
        c = newCourse;

        // c sar mtl lcurrent
    }
    file.close();

    return ListC;
}

void RegisterNewStudent(Student *s)
{

    cout << "Welcome to the registration page" << endl;
    cout << "Please enter your first name: ";
    cin.getline(s->firstName, 30);
    cout << "Please enter your last name: ";
    cin.getline(s->lastName, 30);
    cout << "Please enter your email address: ";
    cin.getline(s->emailAddress, 50);
    // check format of email
    while (emailformat(s->emailAddress) == false)
    {
        cout << "Please enter a valid email address: ";
        cin.getline(s->emailAddress, 50);
    }
    s->studentID = IDGen(s);
    cout << "Your Generated ID is: " << s->studentID << "please take a moment to write it down." << endl;
    ofstream data("data.csv", ios::app);
    data << s->studentID << "," << s->firstName << "," << s->lastName << "," << s->emailAddress << endl;
    data.close();
}

string convertToString(char *a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++)
    {
        s = s + a[i];
    }
    return s;
}

void StudentsEnrolledinCourse(string courseid)
{
    fstream file;
    file.open("enrollment.csv", ios::in);
    string line;
    string thestudent;
    int i = 1;
    cout << "The course has the following students enrolled in it: " << endl;
    while (getline(file, line))
    {
        stringstream ss(line);
        string word;
        while (getline(ss, word, ','))
        {
            if (i == 1)
                thestudent = word;
            else if (i == 2)
            {
                if (word == courseid)
                {
                    cout << thestudent << endl;
                }
            }
            i++;
        }
        i = 1;
    }
}

bool checkifenrolled(Student *s, string courseid)
{
    fstream file;
    file.open("enrollment.csv", ios::in);
    string line;
    bool chayik = false;

    while (getline(file, line))
    {
        stringstream ss(line);
        string word;
        int i = 1;
        while (getline(ss, word, ','))
        {
            if (i == 1)
            {
                if (to_string(s->studentID) == word)
                    chayik = true;
            }

            else if (i == 2)
                if (chayik == true)
                {
                    chayik = false;
                    if (word == courseid)
                        return true;
                } // ya3ne alrdy enrolled
            i++;
        }
    }
    return false;
}

Course *findcourse(UniversityCourseList *listC, string courseid)
{
    Course *c = listC->head;
    while (c->next != NULL)
    {
        if (c->courseID == stoi(courseid))
        {
            return c;
        }
        c = c->next;
    }
    return NULL;
}

bool checkcapacity(Student *s, string courseid, UniversityCourseList *listC)
{
    fstream file;
    file.open("enrollment.csv", ios::in);
    string line;
    int counter = 0;
    while (getline(file, line))
    {
        stringstream ss(line);
        string word;
        int i = 1;
        while (getline(ss, word, ','))
        {

            if (i == 2)
            {
                if (word == courseid)
                {
                    counter++;
                    break;
                }
            }

            i++;
        }
    }
    // find respective course id
    Course *respectivecourse = findcourse(listC, courseid);
    if (respectivecourse->coursecapacity > counter)
        return true; // fi ma7al
    return false;
}

void EnrollCourse(Student *s, UniversityCourseList *listC)
{

    fstream file;

    string courseid;
    cout << "Enter the course id you want to enroll in: ";
    cin >> courseid;

    // check if already enrolled in that course
    if (checkifenrolled(s, courseid))
    {
        cout << "Task failed, you are already enrolled in this course" << endl;
        return;
    }

    // check if course is full
    else if (!checkcapacity(s, courseid, listC))
    {
        cout << "Task failed, The course is full" << endl;
        return;
    }

    else
    {
        file.open("enrollment.csv", ios::app);
        file << s->studentID << "," << courseid << endl;
        file.close();
        cout << "You have succesfully enrolled in this course" << endl;
    }
    EnrolledCourse *n = new EnrolledCourse;

    n->courseReference = findcourse(listC, courseid);
    if (s->enrolledCourses == NULL)
    { // fixing core dump
        s->enrolledCourses = new EnrolledCourseList;
        s->enrolledCourses->head->courseReference = n->courseReference;
        s->enrolledCourses->head->next = NULL;
        s->enrolledCourses->head->previous = NULL;
        s->enrolledCourses->tail = n;
        s->enrolledCourses->tail->next = NULL;
        s->enrolledCourses->tail->previous = NULL;
        return;
    }
    s->enrolledCourses->tail->next = n;
    n->previous = s->enrolledCourses->tail;
    n->next = NULL;
}

void DropProcess(Student *s, string courseid)
{
    // delete from file
    fstream file;
    file.open("enrollment.csv");

    fstream temp;
    temp.open("temp.csv", ios::app);

    string line;
    stringstream ss;
    bool test1 = false; // ensure the same student is the one dropping
    while (getline(file, line))
    {
        stringstream ss(line);
        string word;
        int i = 1;
        while (getline(ss, word, ','))
        {
            if (i == 1)
            {
                if (to_string(s->studentID) == word)
                    test1 = true;
            }
            else if (i == 2)
            {
                if (test1)
                {
                    if (word != courseid)
                        temp << line << endl;
                    test1 = false;
                }
                else
                    temp << line << endl;
            }
            i++;
        }
    }
    file.close();
    temp.close();
    remove("enrollment.csv");
    rename("temp.csv", "enrollment.csv");

    EnrolledCourse *c = s->enrolledCourses->head;
    bool ishead = true;
    while (c->next != NULL)
    {
        if (c->courseReference->courseID == stoi(courseid))
        {
            if (ishead)
            {
                s->enrolledCourses->head = s->enrolledCourses->head->next;
                delete c;
                break;
            }
            else
            {
                c->previous->next = c->next;
                c->next->previous = c->previous;
                delete c;
                break;
            }
        }
        ishead = false;
        c = c->next;
    }
}

void DropCourse(Student *s)
{
    if (s->enrolledCourses == NULL)
    {
        cout << "You aren't enrolled in any courses yet to drop!" << endl;
        return;
    }
    fstream file;
    file.open("enrollment.csv", ios::in);
    string line;
    string courseid;
    cout << "Enter the course id you want to drop: ";
    cin >> courseid;
    if (checkifenrolled(s, courseid))
    {

        DropProcess(s, courseid);
        cout << "You have successfully dropped this course" << endl;
    }
    else
        cout << "You are not enrolled in this course" << endl;
}

void SwapCourse(Student *s, UniversityCourseList *ListC)
{
    if (s->enrolledCourses == NULL)
    {
        cout << "You aren't enrolled in any courses to swap!" << endl;
        return;
    }
    string current;
    string newcourse;
    cout << "Enter the course ID you want to swap: ";
    cin >> current;
    cout << "Enter the course ID you want to swap with: ";
    cin >> newcourse;
    if (checkifenrolled(s, current))
    {
        if (checkifenrolled(s, newcourse))
        {
            cout << "You are already enrolled in the second course!" << endl;
        }
        else if (checkcapacity(s, newcourse, ListC))
        {
            DropProcess(s, current);
            fstream file;
            file.open("enrollment.csv", ios::app);
            file << s->studentID << "," << newcourse << endl;
            file.close();
            // replace old linkedlist with the newone
            EnrolledCourse *c = s->enrolledCourses->head;
            bool ishead = true;
            while (c->next != NULL)
            {
                if (c->courseReference->courseID == stoi(current))
                {
                    c->courseReference = findcourse(ListC, newcourse);
                }
                c = c->next;
            }
            cout << "You have succesfully swapped the courses" << endl;
        }
    }
    else
        cout << "You are not enrolled in this course" << endl;
}

void CheckSchedule(Student *s)
{
    if (s->enrolledCourses == NULL)
    {
        cout << "You aren't enrolled in any courses yet to check your schedule!" << endl;
        return;
    }
    EnrolledCourse *c = s->enrolledCourses->head;
    while (c != NULL)
    {
        cout << c->courseReference->courseName << " by DR." << c->courseReference->instructorName << " from " << c->courseReference->startingHour << " till " << c->courseReference->endingHour << endl;
        c = c->next;
    }
}

void postlogin(Student *s, UniversityCourseList *ListC)
{
    while (true)
    {
        cout << "How can we help you today?" << endl;
        cout << "1. Enroll in a course." << endl;
        cout << "2. Drop a course." << endl;
        cout << "3. Swap a course." << endl;
        cout << "4. View my courses." << endl;
        cout << "5. View all courses offered by the university." << endl;
        cout << "6. View my schedule." << endl;
        cout << "7. Check my grade." << endl;
        int option;
        cout << "Enter your option: ";
        cin >> option;
        switch (option)
        {

        case 1:

            EnrollCourse(s, ListC);
            break;

        case 2:

            DropCourse(s);
            break;

        case 3:

            SwapCourse(s, ListC);
            break;

        case 4:
            if (s->enrolledCourses == NULL)
            {
                cout << "You aren't enrolled in any courses." << endl;
                break;
            }
            bubblesort(s);
            Showmycourses(s);
            break;

        case 5:
            ListAllCourses(ListC);

            break;

        case 6:
            CheckSchedule(s);
            break;
        case 7:
            viewmygrade(s);
            break;
        default:
            cout << "Invalid option" << endl;
            break;
        }
    }
    // case4 would cause problem though (flawed)
    // other cases will work normally since linkedlist is working properly
}

void Showmycourses(Student *s)
{
    cout << "You are enrolled in these courses:" << endl;

    EnrolledCourse *temp = s->enrolledCourses->head;
    while (temp != NULL)
    {
        cout << "-" << temp->courseReference->courseName << endl;
        temp = temp->next;
    } // flawed cause of bubble sort
}

void bubblesort(Student *s)
{
    // flawed
    bool swapped = true;
    EnrolledCourse *cur = new EnrolledCourse;
    EnrolledCourse *last;
    while (swapped)
    {
        swapped = false;
        cur = s->enrolledCourses->head;
        while (cur->next != NULL)
        {
            if (cur->courseReference->courseName[0] > cur->next->courseReference->courseName[0])
            {
                char temp[50]; // given in structure,
                strcpy(temp, cur->courseReference->courseName);
                strcpy(cur->courseReference->courseName, cur->next->courseReference->courseName);
                strcpy(cur->next->courseReference->courseName, temp);
                swapped = true;
            }
            cur = cur->next;
        }
        last = cur;
    }
}

void PrepareStudentData(Student *s, UniversityCourseList *ListC)
{
    // this is to create a linked list for the student enrolled courses(not sorted)
    EnrolledCourseList *list = new EnrolledCourseList;
    list->head = NULL;
    list->tail = NULL;

    EnrolledCourse *temp = new EnrolledCourse;
    temp->next = NULL;
    temp->previous = NULL;
    temp->courseReference = NULL;

    fstream file;
    file.open("enrollment.csv", ios::in);
    string line;
    stringstream ss;
    bool him = false;
    bool thehead = true;
    while (getline(file, line))
    {
        stringstream ss(line);
        string word;
        int i = 1;
        while (getline(ss, word, ','))
        {
            if (i == 1)
            {
                if (to_string(s->studentID) == word)
                {
                    him = true;
                }
            }
            else if (i == 2)
            {
                if (him)
                {
                    if (thehead)
                    {
                        temp->courseReference = findcourse(ListC, word);
                        thehead = false;
                        temp->previous = NULL;
                        temp->next = new EnrolledCourse;
                        list->head = temp;
                        temp->next->previous = temp;
                        temp = temp->next;
                        him = false;
                    }
                    else
                    {
                        temp->courseReference = findcourse(ListC, word);
                        temp->next = new EnrolledCourse;
                        him = false;
                        temp->next->previous = temp; //
                        list->tail = temp;
                        temp = temp->next;
                    }
                }
            }
            i++;
        }
    }

    if (list->head == NULL)
        return;
    if (list->tail != NULL)
        list->tail->next = NULL; // for students who have 1 course enrolled
    s->enrolledCourses = list;
    // now my kel el courses el student enrolled fyon fixed
}

void login(Student *s, UniversityCourseList *listC)
{
    char email[50];
    cout << "Please enter your email address: ";
    cin.getline(email, 50);
    bool verified = false;
    while (s->next != NULL)
    {
        if (strcmp(s->emailAddress, email) == 0)
        {
            cout << "Welcome back " << s->firstName << " " << s->lastName << "." << endl;
            verified = true;
            break;
        }
        s = s->next;
    }
    if (s->next == NULL)
    {
        cout << endl
             << "Email not found , please register first!" << endl;
        return;
    }

    if (verified == true)
        PrepareStudentData(s, listC); // links his courses
    postlogin(s, listC);
}

void liststudents(Student *s)
{
    while (s->next != NULL)
    {
        cout << s->studentID << " " << s->firstName << " " << s->lastName << endl;
        s = s->next;
    }
}

Student *LinkStudents(Student *s)
{
    ifstream file2;
    string word;
    int i;
    Student *head = new Student;
    bool forhead = true;
    file2.open("data.csv");
    string line;
    while (getline(file2, line))
    {
        stringstream ss(line);
        i = 1;

        while (getline(ss, word, ','))
        {

            if (i == 1)
                s->studentID = stoi(word);
            else if (i == 2)
                strcpy(s->firstName, word.c_str());
            else if (i == 3)
                strcpy(s->lastName, word.c_str());
            else if (i == 4)
                strcpy(s->emailAddress, word.c_str());
            i++;
        }
        Student *n = new Student;
        s->next = n;

        if (forhead)
        {
            head = s;
            forhead = false;
        }
        s = n;
    }
    return head;
}

int main()
{

    UniversityCourseList *UniCourseList = new UniversityCourseList;
    // later will be called as listC in the functions

    Course *c = new Course;
    Student *s = new Student;
    UniCourseList = LinkTheCourses(c);
    // t3abit el list , courses r linked wtaht student gets linked
    Student *head = LinkStudents(s);
    // preparing data

    int option;
    cout << "-----Antonine University SIS-----\t" << endl;
    cout << "Please choose an option to continue\t" << endl;
    cout << "1. Register as a new student." << endl;
    cout << "2. Login with an existing email" << endl;
    cout << "3. Login as an instructor" << endl;
    cout << "Your option:";
    cin >> option;
    cin.ignore();

    if (option == 1)
        RegisterNewStudent(s);
    else if (option == 2)
        login(head, UniCourseList);
    else if (option == 3)
    {
        Instructor *instructor = new Instructor;
        Instructor *instrhead = LinkInstructor(instructor);
        Instructorlogin(UniCourseList);
    }

    return 0;
}