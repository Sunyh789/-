#include "FileName.h"
#include <algorithm>

std::vector<User> users;
std::vector<Student> students;
User login_temp;
int user_count = 0;
int student_count = 0;

User::User(int id, const std::string& pwd, const std::string& iden)
    : user_id(id), password(pwd), identity(iden) {
}

Student::Student(int sid, const std::string& n, int a, const std::string& g,
    const std::string& p, float m, float e, float pr)
    : id(sid), name(n), age(a), grade(g), profession(p), Math(m), English(e), Programming(pr) {
}

void menumain1() {
    std::cout << "**************" << std::endl;
    std::cout << "成绩管理系统" << std::endl;
    std::cout << "1.注册" << std::endl;
    std::cout << "2.登录" << std::endl;
    std::cout << "**************" << std::endl;
}

void menumain2() {
    std::cout << "**************" << std::endl;
    std::cout << "==教师端==" << std::endl;
    std::cout << "1.添加学生信息" << std::endl;
    std::cout << "2.删除学生信息" << std::endl;
    std::cout << "3.修改学生信息" << std::endl;
    std::cout << "4.查询学生信息" << std::endl;
    std::cout << "5.打印所有学生信息" << std::endl;
    std::cout << "6.退出管理系统" << std::endl;
    std::cout << "**************" << std::endl;
}

void menumain3() {
    std::cout << "**************" << std::endl;
    std::cout << "==学生端==" << std::endl;
    std::cout << "1.查询学生信息" << std::endl;
    std::cout << "2.退出管理系统" << std::endl;
    std::cout << "**************" << std::endl;
}

float judgment(int a, int b) {
    float ach;
    while (true) {
        if (std::cin >> ach && ach >= a && ach <= b) {
            break;
        }
        else {
            std::cout << "输出有误请重新输入:";
            std::cin.clear();
            while (std::cin.get() != '\n');
        }
    }
    return ach;
}

void registers() {
    if (user_count >= MAX_USERS) {
        std::cout << "用户数量已达上限，无法注册！" << std::endl;
        return;
    }

    User newUser;
    std::cout << "用户名: ";
    std::cin >> newUser.user_id;

    for (const auto& u : users) {
        if (newUser.user_id == u.user_id) {
            std::cout << "用户名已被占用" << std::endl;
            return;
        }
    }

    std::cout << "密码: ";
    std::cin >> newUser.password;
    std::cout << "身份(教师/学生): ";
    std::cin >> newUser.identity;

    if (newUser.identity != "教师" && newUser.identity != "学生") {
        std::cout << "身份只能是'教师'或'学生'！" << std::endl;
        return;
    }

    users.push_back(newUser);
    user_count++;
    saveuser();
    std::cout << "注册成功！" << std::endl;
}

void saveuser() {
    std::ofstream foutt("user.txt");
    if (!foutt) {
        std::cout << "无法打开文件！" << std::endl;
        return;
    }
    for (const auto& u : users) {
        foutt << u.user_id << " " << u.password << " " << u.identity << std::endl;
    }
    foutt.close();
}

void login() {
    User temp;
    std::cout << "用户名: ";
    std::cin >> temp.user_id;
    std::cout << "密码: ";
    std::cin >> temp.password;
    std::cout << "身份(教师/学生): ";
    std::cin >> temp.identity;

    if (temp.identity != "学生" && temp.identity != "教师") {
        std::cout << "身份只能是'教师'或'学生'！" << std::endl;
        return;
    }

    bool login_success = false;
    for (const auto& u : users) {
        if (temp.user_id == u.user_id && temp.password == u.password && temp.identity == u.identity) {
            login_success = true;
            login_temp = temp;
            break;
        }
    }

    if (login_success) {
        std::cout << "登录成功！" << std::endl;
    }
    else {
        std::cout << "登录失败！用户名、密码或身份不正确" << std::endl;
    }
}

void loadusers() {
    std::ifstream finn("user.txt");
    if (!finn) {
        std::cout << "用户文件不存在，将创建新文件" << std::endl;
        return;
    }

    users.clear();
    int id;
    std::string pwd, iden;
    while (finn >> id >> pwd >> iden) {
        users.emplace_back(id, pwd, iden);
        user_count++;
    }
    finn.close();
}

void AddStudent() {
    Student s;
    std::cout << "请输入学生学号：" << std::endl;
    std::cin >> s.id;
    std::cout << "请输入学生姓名：" << std::endl;
    std::cin >> s.name;
    std::cout << "请输入学生年龄：" << std::endl;
    s.age = (int)judgment(1, 150);
    std::cout << "请输入学生年级：" << std::endl;
    std::cin >> s.grade;
    std::cout << "请输入学生专业：" << std::endl;
    std::cin >> s.profession;
    std::cout << "请输入数学成绩：" << std::endl;
    s.Math = judgment(0, 100);
    std::cout << "请输入英语成绩：" << std::endl;
    s.English = judgment(0, 100);
    std::cout << "请输入程序设计成绩：" << std::endl;
    s.Programming = judgment(0, 100);
    students.push_back(s);
    student_count++;
    std::cout << "添加成功！" << std::endl;
}

void DeleteStudent() {
    int id;
    std::cout << "请输入要删除的学生学号：";
    std::cin >> id;
    auto it = std::find_if(students.begin(), students.end(), [id](const Student& s) { return s.id == id; });
    if (it != students.end()) {
        students.erase(it);
        student_count--;
        std::cout << "删除成功！" << std::endl;
    }
    else {
        std::cout << "未找到学生信息！" << std::endl;
    }
}

void ModifyStudent() {
    int id;
    std::cout << "请输入要修改的学生学号：";
    std::cin >> id;
    for (auto& s : students) {
        if (s.id == id) {
            std::cout << "请输入新的数学成绩：";
            s.Math = judgment(0, 100);
            std::cout << "请输入新的英语成绩：";
            s.English = judgment(0, 100);
            std::cout << "请输入新的程序设计成绩：";
            s.Programming = judgment(0, 100);
            std::cout << "修改成功！" << std::endl;
            return;
        }
    }
    std::cout << "未找到学生信息！" << std::endl;
}

void QueryStudent() {
    int choice;
    std::cout << "请选择查询方式：1. 按学号查询 2. 按姓名查询" << std::endl;
    std::cin >> choice;
    switch (choice) {
    case 1: {
        int id;
        std::cout << "请输入要查询的学生学号：";
        std::cin >> id;
        for (const auto& s : students) {
            if (s.id == id) {
                std::cout << "学号：" << s.id << " 姓名：" << s.name
                    << " 年龄：" << s.age << " 年级：" << s.grade
                    << " 专业：" << s.profession
                    << " 数学：" << s.Math << " 英语：" << s.English
                    << " 程序设计：" << s.Programming << std::endl;
                return;
            }
        }
        std::cout << "未找到学生信息！" << std::endl;
        break;
    }
    case 2: {
        std::string name;
        std::cout << "请输入要查询的学生姓名：";
        std::cin >> name;
        for (const auto& s : students) {
            if (s.name == name) {
                std::cout << "学号：" << s.id << " 姓名：" << s.name
                    << " 年龄：" << s.age << " 年级：" << s.grade
                    << " 专业：" << s.profession
                    << " 数学：" << s.Math << " 英语：" << s.English
                    << " 程序设计：" << s.Programming << std::endl;
                return;
            }
        }
        std::cout << "未找到学生信息！" << std::endl;
        break;
    }
    default:
        std::cout << "选择错误！" << std::endl;
    }
}

void printstudent() {
    for (const auto& s : students) {
        std::cout << "学号\t姓名\t年龄\t年级\t专业\t数学\t英语\t程序设计" << std::endl;
        std::cout << s.id << "\t" 
            << s.name << "\t"
            << s.age << "\t"
            << s.grade << "\t"
            << s.profession << "\t"
            << s.Math << "\t"
            << s.English << "\t"
            << s.Programming << std::endl;
        std::cout << "=================================================================" << std::endl;
    }
}

void savestudent() {
    std::ofstream fout("students.txt");
    if (!fout) {
        std::cout << "无法打开文件！" << std::endl;
        return;
    }
    for (const auto& s : students) {
        fout << s.id << " " << s.name << " " << s.age << " "
            << s.grade << " " << s.profession << " " << s.Math << " " << s.English
            << " " << s.Programming << std::endl;
    }
    fout.close();
}

void loadstudent() {
    std::ifstream fin("students.txt");
    if (!fin) {
        return;
    }
    students.clear();
    int id, age;
    std::string name, grade, profession;
    float math, eng, prog;
    while (fin >> id >> name >> age >> grade >> profession >> math >> eng >> prog) {
        students.emplace_back(id, name, age, grade, profession, math, eng, prog);
        student_count++;
    }
    fin.close();
}