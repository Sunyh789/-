#include "FileName.h"

int main() {
    loadusers();
    int choice1;
    menumain1();
    std::cin >> choice1;
    switch (choice1) {
    case 1:
        registers();
        break;
    case 2:
        login();
        if (login_temp.identity == "教师") {
            loadstudent();
            int choice;
            bool exitFlag = false;
            while (!exitFlag) {
                menumain2();
                std::cout << "请选择您要使用的功能：";
                std::cin >> choice;
                switch (choice) {
                case 1:
                    AddStudent();
                    break;
                case 2:
                    DeleteStudent();
                    break;
                case 3:
                    ModifyStudent();
                    break;
                case 4:
                    QueryStudent();
                    break;
                case 5:
                    printstudent();
                    break;
                case 6:
                    savestudent();
                    exitFlag = true;
                    break;
                default:
                    std::cout << "输入错误，请重新选择！" << std::endl;
                }
            }
        }
        if (login_temp.identity == "学生") {
            loadstudent();
            int choice2;
            bool exitFlag = false;
            while (!exitFlag) {
                menumain3();
                std::cout << "请选择您要使用的功能：";
                std::cin >> choice2;
                switch (choice2) {
                case 1:
                    QueryStudent();
                    break;
                case 2:
                    savestudent();
                    exitFlag = true;
                    break;
                default:
                    std::cout << "输入错误，请重新选择！" << std::endl;
                }
            }
        }
        break;
    default:
        std::cout << "输入错误！" << std::endl;
    }
    return 0;
}