
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100
#define FILE_NAME "students.dat"

/* Terminal Colors */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

struct Student
{
    int rollNo;
    char name[50];
    float mark1, mark2, mark3;
    float total;
    float percentage;
    int rank;
};

struct Student students[MAX];
int count = 0;

/* Print a horizontal border */
void printLine(char symbol, int length)
{
    int i;

    printf(CYAN "+");

    for (i = 0; i < length; i++)
        printf("%c", symbol);

    printf("+\n" RESET);
}

/* Display a section heading */
void printHeader(const char *title)
{
    int length = 60;

    printf("\n");
    printLine('=', length);
    printf(CYAN BOLD "|%60s|\n" RESET, "");
    printf(CYAN BOLD "|%*s%s%*s|\n" RESET,
           (length - (int)strlen(title)) / 2, "",
           title,
           (length - (int)strlen(title) + 1) / 2, "");
    printf(CYAN BOLD "|%60s|\n" RESET, "");
    printLine('=', length);
}

/* Display a success message */
void successMessage(const char *message)
{
    printf("\n" GREEN BOLD "  [SUCCESS] %s\n" RESET, message);
}

/* Display an error message */
void errorMessage(const char *message)
{
    printf("\n" RED BOLD "  [ERROR] %s\n" RESET, message);
}

/* Calculate total and percentage */
void calculateMarks(struct Student *s)
{
    s->total = s->mark1 + s->mark2 + s->mark3;
    s->percentage = s->total / 3.0f;
}

/* Sort students by total marks and assign ranks */
void sortStudents(void)
{
    int i, j;
    struct Student temp;

    for (i = 0; i < count - 1; i++)
    {
        for (j = 0; j < count - i - 1; j++)
        {
            if (students[j].total < students[j + 1].total)
            {
                temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    for (i = 0; i < count; i++)
    {
        if (i > 0 &&
            students[i].total == students[i - 1].total)
        {
            students[i].rank = students[i - 1].rank;
        }
        else
        {
            students[i].rank = i + 1;
        }
    }
}

/* Save student records to a binary file */
void saveStudents(void)
{
    FILE *fp;

    fp = fopen(FILE_NAME, "wb");

    if (fp == NULL)
    {
        errorMessage("Unable to save student records.");
        return;
    }

    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(struct Student), count, fp);

    fclose(fp);

    successMessage("Student records saved successfully.");
}

/* Load student records from a binary file */
void loadStudents(void)
{
    FILE *fp;
    int savedCount;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL)
    {
        return;
    }

    if (fread(&savedCount, sizeof(int), 1, fp) != 1 ||
        savedCount < 0 || savedCount > MAX)
    {
        fclose(fp);
        errorMessage("Invalid student data file.");
        return;
    }

    if (fread(students, sizeof(struct Student),
              savedCount, fp) != (size_t)savedCount)
    {
        fclose(fp);
        count = 0;
        errorMessage("Unable to read student records.");
        return;
    }

    count = savedCount;

    fclose(fp);

    sortStudents();
}

/* Check whether a roll number already exists */
int rollNumberExists(int rollNo)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (students[i].rollNo == rollNo)
            return 1;
    }

    return 0;
}

/* Add a new student */
void addStudent(void)
{
    struct Student s;

    printHeader("ADD NEW STUDENT");

    if (count >= MAX)
    {
        errorMessage("Student storage is full.");
        return;
    }

    printf(YELLOW "\n  Enter Roll Number : " RESET);

    if (scanf("%d", &s.rollNo) != 1)
    {
        errorMessage("Invalid roll number.");
        return;
    }

    if (rollNumberExists(s.rollNo))
    {
        errorMessage("This roll number already exists.");
        return;
    }

    printf(YELLOW "  Enter Student Name: " RESET);
    scanf(" %49[^\n]", s.name);

    printf(YELLOW "  Enter Mark 1 (0-100): " RESET);

    if (scanf("%f", &s.mark1) != 1 ||
        s.mark1 < 0 || s.mark1 > 100)
    {
        errorMessage("Mark must be between 0 and 100.");
        return;
    }

    printf(YELLOW "  Enter Mark 2 (0-100): " RESET);

    if (scanf("%f", &s.mark2) != 1 ||
        s.mark2 < 0 || s.mark2 > 100)
    {
        errorMessage("Mark must be between 0 and 100.");
        return;
    }

    printf(YELLOW "  Enter Mark 3 (0-100): " RESET);

    if (scanf("%f", &s.mark3) != 1 ||
        s.mark3 < 0 || s.mark3 > 100)
    {
        errorMessage("Mark must be between 0 and 100.");
        return;
    }

    calculateMarks(&s);

    s.rank = 0;
    students[count] = s;
    count++;

    sortStudents();
    saveStudents();

    successMessage("Student added successfully.");

    printf(CYAN "\n  Total Marks : " RESET "%.2f / 300\n",
           s.total);
    printf(CYAN "  Percentage  : " RESET "%.2f%%\n",
           s.percentage);
}

/* Display all student records */
void displayStudents(void)
{
    int i;

    printHeader("STUDENT RANK LIST");

    if (count == 0)
    {
        printf(YELLOW "\n  No student records available.\n" RESET);
        return;
    }

    sortStudents();

    printf(CYAN
           "\n+--------+----------------------+----------+"
           "----------+----------+----------+------+\n"
           "| RollNo | Student Name         |  Mark 1  |"
           "  Mark 2  |  Mark 3  |  Total   | Rank |\n"
           "+--------+----------------------+----------+"
           "----------+----------+----------+------+\n"
           RESET);

    for (i = 0; i < count; i++)
    {
        if (students[i].rank == 1)
            printf(YELLOW BOLD);
        else
            printf(WHITE);

        printf("| %-6d | %-20.20s | %8.2f | %8.2f |"
               " %8.2f | %8.2f | %4d |\n",
               students[i].rollNo,
               students[i].name,
               students[i].mark1,
               students[i].mark2,
               students[i].mark3,
               students[i].total,
               students[i].rank);

        printf(RESET);
    }

    printf(CYAN
           "+--------+----------------------+----------+"
           "----------+----------+----------+------+\n"
           RESET);

    printf(GREEN "\n  Total Students: %d\n" RESET, count);
}

/* Search for a student by roll number */
void searchStudent(void)
{
    int rollNo, i;
    int found = 0;

    printHeader("SEARCH STUDENT");

    if (count == 0)
    {
        errorMessage("No student records available.");
        return;
    }

    printf(YELLOW "\n  Enter Roll Number to Search: " RESET);

    if (scanf("%d", &rollNo) != 1)
    {
        errorMessage("Invalid roll number.");
        return;
    }

    sortStudents();

    for (i = 0; i < count; i++)
    {
        if (students[i].rollNo == rollNo)
        {
            found = 1;

            printLine('-', 38);

            printf(CYAN BOLD "|           STUDENT DETAILS            |\n" RESET);
            printLine('-', 38);

            printf(WHITE "| Roll Number : %-22d|\n" RESET,
                   students[i].rollNo);
            printf(WHITE "| Name        : %-22.22s|\n" RESET,
                   students[i].name);
            printf(WHITE "| Mark 1      : %-22.2f|\n" RESET,
                   students[i].mark1);
            printf(WHITE "| Mark 2      : %-22.2f|\n" RESET,
                   students[i].mark2);
            printf(WHITE "| Mark 3      : %-22.2f|\n" RESET,
                   students[i].mark3);
            printf(WHITE "| Total Marks : %-22.2f|\n" RESET,
                   students[i].total);
            printf(WHITE "| Percentage  : %-21.2f%%|\n" RESET,
                   students[i].percentage);

            if (students[i].rank == 1)
                printf(YELLOW BOLD "| Rank        : %-22d|\n" RESET,
                       students[i].rank);
            else
                printf(WHITE "| Rank        : %-22d|\n" RESET,
                       students[i].rank);

            printLine('-', 38);
            break;
        }
    }

    if (!found)
    {
        errorMessage("Student with this roll number was not found.");
    }
}

/* Display the main menu */
void displayMenu(void)
{
    printHeader("STUDENT RANK MANAGEMENT SYSTEM");

    printf(CYAN "\n  +------------------------------------------------------------+\n");
    printf("  |" RESET "                    " BOLD "MAIN MENU" RESET
           CYAN "                           |\n");
    printf("  +------------------------------------------------------------+\n");

    printf("  |" RESET "  " YELLOW "[1]" RESET "  Add Student"
           CYAN "                                           |\n");
    printf("  |" RESET "  " YELLOW "[2]" RESET "  Display All Students"
           CYAN "                                    |\n");
    printf("  |" RESET "  " YELLOW "[3]" RESET "  Search Student"
           CYAN "                                         |\n");
    printf("  |" RESET "  " YELLOW "[4]" RESET "  Save Student Records"
           CYAN "                                    |\n");
    printf("  |" RESET "  " YELLOW "[5]" RESET "  Exit"
           CYAN "                                                  |\n");

    printf("  +------------------------------------------------------------+\n" RESET);
}

/* Main function */
int main(void)
{
    int choice;

    loadStudents();

    do
    {
        displayMenu();

        printf(YELLOW "\n  Enter your choice: " RESET);

        if (scanf("%d", &choice) != 1)
        {
            errorMessage("Please enter a valid menu number.");
            break;
        }

        switch (choice)
        {
            case 1:
                addStudent();
                break;

            case 2:
                displayStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                saveStudents();
                break;

            case 5:
                saveStudents();
                printf(GREEN BOLD
                       "\n  Thank you for using Student Rank Management System!\n"
                       RESET);
                printf(CYAN
                       "  Program closed successfully.\n\n"
                       RESET);
                break;

            default:
                errorMessage("Invalid choice. Please select 1 to 5.");
        }

    } while (choice != 5);

    return 0;
}

