#pragma once
#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define MAX_USERS 1000

class User {
public:
    int user_id;
    std::string password;
    std::string identity;

    User() = default;
    User(int id, const std::string& pwd, const std::string& iden);
};

class Student {
public:
    int id;
    std::string name;
    int age;
    std::string grade;
    std::string profession;
    float Math;
    float English;
    float Programming;

    Student() = default;
    Student(int sid, const std::string& n, int a, const std::string& g, const std::string& p, float m, float e, float pr);
};

extern User login_temp;
extern std::vector<User> users;
extern std::vector<Student> students;
extern int user_count;
extern int student_count;

void login();
void registers();
void saveuser();
void loadusers();
void menumain1();
void menumain2();
void menumain3();
float judgment(int a, int b);
void AddStudent();
void DeleteStudent();
void ModifyStudent();
void QueryStudent();
void savestudent();
void loadstudent();
void printstudent();

#endif