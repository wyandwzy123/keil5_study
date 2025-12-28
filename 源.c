#define _CRT_SECURE_NO_WARNINGS  // 禁用安全警告（兼容旧函数习惯，可选保留）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 定义用户登录结构体
struct User {
    char name[20];
    char password[20];
} typedef stuser;

// 定义学生信息结构体
struct Student {
    int number;
    char name[20];
    float cscore;   // C语言成绩
    float jscore;   // 计算机基础成绩
    float escore;   // 英语成绩
} typedef ststu;

#define MAX_STUDENTS 100  // 最大学生数量
#define MAX_NAME_LEN 19   // 姓名最大长度（留1位给结束符）
#define SCORE_MIN 0.0f    // 最低成绩
#define SCORE_MAX 100.0f  // 最高成绩

ststu students[MAX_STUDENTS];  // 存储学生信息的数组
int studentCount = 0;          // 当前学生数量

// 清除输入缓冲区（解决scanf残留问题）
void clearInputBuffer() {
    while (getchar() != '\n');
}

// 初始化管理员信息（使用fopen_s替代fopen，适配VS）
void initUser() {
    FILE* fp;
    // 二进制读方式打开，检查文件是否存在
    if (fopen_s(&fp, "user.txt", "rb") != 0 || fp == NULL) {
        // 首次运行创建管理员账号：用户名ycc，密码123
        if (fopen_s(&fp, "user.txt", "wb") != 0 || fp == NULL) {
            printf("管理员账号创建失败！请检查文件权限\n");
            exit(1);  // 权限不足时直接退出
        }
        stuser admin = { "ycc", "123" };
        fwrite(&admin, sizeof(stuser), 1, fp);
        printf("管理员账号初始化完成：用户名ycc，密码123\n");
    }
    fclose(fp);
}

// 登录验证功能（VS兼容，密码输入优化）
int login() {
    stuser user, temp;
    FILE* fp;
    // 打开用户信息文件
    if (fopen_s(&fp, "user.txt", "rb") != 0 || fp == NULL) {
        printf("用户信息文件打开失败！\n");
        return 0;
    }

    // 读取管理员信息
    if (fread(&temp, sizeof(stuser), 1, fp) != 1) {
        printf("用户信息损坏！\n");
        fclose(fp);
        return 0;
    }
    fclose(fp);

    printf("请输入用户名：");
    scanf("%s", user.name);
    clearInputBuffer();  // 清除缓冲区

    // 密码输入（VS兼容，隐藏输入）
    printf("请输入密码：");
    char ch;
    int pwdIdx = 0;
    while (1) {
        ch = getch();  // VS默认支持conio.h中的getch()
        if (ch == '\r' || ch == '\n') {  // 回车结束输入
            user.password[pwdIdx] = '\0';
            break;
        }
        else if (ch == 8 && pwdIdx > 0) {  // 退格键删除
            pwdIdx--;
            printf("\b \b");  // 清除屏幕上的字符
        }
        else if (isprint(ch) && pwdIdx < 19) {  // 只允许可见字符
            user.password[pwdIdx++] = ch;
            printf("*");  // 显示星号代替密码
        }
    }
    printf("\n");

    // 验证用户名和密码
    if (strcmp(user.name, temp.name) == 0 && strcmp(user.password, temp.password) == 0) {
        printf("登录成功！\n");
        return 1;
    }
    else {
        printf("用户名或密码错误，登录失败！\n");
        return 0;
    }
}

// 验证学号唯一性（避免重复学号）
int isNumberUnique(int number) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].number == number) {
            return 0;  // 学号已存在
        }
    }
    return 1;  // 学号唯一
}

// 验证成绩合法性（0-100分）
int isValidScore(float score) {
    return score >= SCORE_MIN && score <= SCORE_MAX;
}

// 从文件加载学生信息（使用fopen_s，修复兼容性）
void loadStudents() {
    FILE* fp;
    if (fopen_s(&fp, "students.txt", "rb") != 0 || fp == NULL) {
        printf("暂无学生信息，可新增学生数据\n");
        return;
    }

    // 读取学生数量（检查文件完整性）
    if (fread(&studentCount, sizeof(int), 1, fp) != 1) {
        printf("学生信息文件损坏，已重置\n");
        studentCount = 0;
        fclose(fp);
        return;
    }

    // 读取学生信息（限制最大数量，避免数组越界）
    if (studentCount > MAX_STUDENTS) {
        printf("学生数量超出上限，仅加载前%d名学生\n", MAX_STUDENTS);
        studentCount = MAX_STUDENTS;
    }
    fread(students, sizeof(ststu), studentCount, fp);
    fclose(fp);
    printf("学生信息加载成功，当前共%d名学生\n", studentCount);
}

// 保存学生信息到文件（使用fopen_s，确保数据安全）
void saveStudents() {
    FILE* fp;
    if (fopen_s(&fp, "students.txt", "wb") != 0 || fp == NULL) {
        printf("文件保存失败！请检查文件权限\n");
        return;
    }

    // 先写入学生数量，再写入所有学生信息
    fwrite(&studentCount, sizeof(int), 1, fp);
    fwrite(students, sizeof(ststu), studentCount, fp);
    fclose(fp);
    printf("学生信息保存成功！\n");
}

// 新增学生信息（输入校验完善）
void addStudent() {
    if (studentCount >= MAX_STUDENTS) {
        printf("学生数量已达上限（%d人），无法新增！\n", MAX_STUDENTS);
        return;
    }

    ststu newStu;
    // 学号输入（唯一且非负）
    while (1) {
        printf("请输入学生学号（非负整数）：");
        if (scanf("%d", &newStu.number) != 1 || newStu.number < 0) {
            printf("学号输入错误！请输入非负整数\n");
            clearInputBuffer();
        }
        else if (!isNumberUnique(newStu.number)) {
            printf("学号已存在！请重新输入\n");
        }
        else {
            clearInputBuffer();
            break;
        }
    }

    // 姓名输入（非空且长度限制）
    while (1) {
        printf("请输入学生姓名（最多%d个字符）：", MAX_NAME_LEN);
        fgets(newStu.name, sizeof(newStu.name), stdin);
        // 去除fgets读取的换行符
        newStu.name[strcspn(newStu.name, "\n")] = '\0';
        if (strlen(newStu.name) == 0) {
            printf("姓名不能为空！请重新输入\n");
        }
        else {
            break;
        }
    }

    // 成绩输入（合法范围：0-100）
    while (1) {
        printf("请输入C语言成绩（%.1f-%.1f）：", SCORE_MIN, SCORE_MAX);
        if (scanf("%f", &newStu.cscore) != 1 || !isValidScore(newStu.cscore)) {
            printf("成绩输入错误！请输入%.1f-%.1f之间的数值\n", SCORE_MIN, SCORE_MAX);
            clearInputBuffer();
        }
        else {
            break;
        }
    }

    while (1) {
        printf("请输入计算机基础成绩（%.1f-%.1f）：", SCORE_MIN, SCORE_MAX);
        if (scanf("%f", &newStu.jscore) != 1 || !isValidScore(newStu.jscore)) {
            printf("成绩输入错误！请输入%.1f-%.1f之间的数值\n", SCORE_MIN, SCORE_MAX);
            clearInputBuffer();
        }
        else {
            break;
        }
    }

    while (1) {
        printf("请输入英语成绩（%.1f-%.1f）：", SCORE_MIN, SCORE_MAX);
        if (scanf("%f", &newStu.escore) != 1 || !isValidScore(newStu.escore)) {
            printf("成绩输入错误！请输入%.1f-%.1f之间的数值\n", SCORE_MIN, SCORE_MAX);
            clearInputBuffer();
        }
        else {
            clearInputBuffer();
            break;
        }
    }

    students[studentCount++] = newStu;
    printf("学生信息新增成功！\n");
}

// 显示所有学生信息（格式对齐优化）
void showAllStudents() {
    if (studentCount == 0) {
        printf("暂无学生信息！\n");
        return;
    }

    printf("\n====================学生信息列表====================\n");
    printf("%-8s%-10s%-8s%-12s%-8s\n", "学号", "姓名", "C语言", "计算机基础", "英语");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < studentCount; i++) {
        printf("%-8d%-10s%-8.1f%-12.1f%-8.1f\n",
            students[i].number,
            students[i].name,
            students[i].cscore,
            students[i].jscore,
            students[i].escore);
    }
    printf("==================================================\n\n");
}

// 根据姓名查找学生信息（支持模糊查询）
void findStudentByName() {
    if (studentCount == 0) {
        printf("暂无学生信息！\n");
        return;
    }

    char targetName[20];
    printf("请输入欲查找的姓名（支持模糊查询）：");
    fgets(targetName, sizeof(targetName), stdin);
    targetName[strcspn(targetName, "\n")] = '\0';
    clearInputBuffer();

    printf("\n====================查找结果====================\n");
    printf("%-8s%-10s%-8s%-12s%-8s\n", "学号", "姓名", "C语言", "计算机基础", "英语");
    printf("--------------------------------------------------\n");
    int findFlag = 0;
    for (int i = 0; i < studentCount; i++) {
        // 模糊查询：包含目标字符串即匹配
        if (strstr(students[i].name, targetName) != NULL) {
            printf("%-8d%-10s%-8.1f%-12.1f%-8.1f\n",
                students[i].number,
                students[i].name,
                students[i].cscore,
                students[i].jscore,
                students[i].escore);
            findFlag = 1;
        }
    }
    if (!findFlag) {
        printf("未找到包含\"%s\"的学生！\n", targetName);
    }
    printf("==================================================\n\n");
}

// 根据学号删除学生信息（防误删确认）
void deleteStudentByNumber() {
    if (studentCount == 0) {
        printf("暂无学生信息！\n");
        return;
    }

    int targetNum;
    printf("请输入欲删除的学生学号：");
    if (scanf("%d", &targetNum) != 1) {
        printf("学号输入错误！\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int deleteIndex = -1;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].number == targetNum) {
            deleteIndex = i;
            break;
        }
    }

    if (deleteIndex == -1) {
        printf("未找到学号为%d的学生！\n", targetNum);
        return;
    }

    // 确认删除（防误删）
    char confirm;
    printf("确定要删除学号%d的学生信息吗？（y/n）：", targetNum);
    scanf("%c", &confirm);
    clearInputBuffer();
    if (confirm != 'y' && confirm != 'Y') {
        printf("删除操作已取消！\n");
        return;
    }

    // 移动数组元素覆盖待删除数据
    for (int i = deleteIndex; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;
    printf("学号为%d的学生信息删除成功！\n", targetNum);
}

// 修改学生信息（输入校验完善）
void modifyStudent() {
    if (studentCount == 0) {
        printf("暂无学生信息！\n");
        return;
    }

    int targetNum;
    printf("请输入欲修改的学生学号：");
    if (scanf("%d", &targetNum) != 1) {
        printf("学号输入错误！\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    int modifyIndex = -1;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].number == targetNum) {
            modifyIndex = i;
            break;
        }
    }

    if (modifyIndex == -1) {
        printf("未找到学号为%d的学生！\n", targetNum);
        return;
    }

    // 显示原信息
    printf("\n当前学生信息：\n");
    printf("学号：%d，姓名：%s，C语言：%.1f，计算机基础：%.1f，英语：%.1f\n",
        students[modifyIndex].number,
        students[modifyIndex].name,
        students[modifyIndex].cscore,
        students[modifyIndex].jscore,
        students[modifyIndex].escore);

    // 修改姓名
    while (1) {
        printf("请输入新的学生姓名（最多%d个字符）：", MAX_NAME_LEN);
        fgets(students[modifyIndex].name, sizeof(students[modifyIndex].name), stdin);
        students[modifyIndex].name[strcspn(students[modifyIndex].name, "\n")] = '\0';
        if (strlen(students[modifyIndex].name) == 0) {
            printf("姓名不能为空！请重新输入\n");
        }
        else {
            break;
        }
    }

    // 修改成绩（校验合法性）
    while (1) {
        printf("请输入新的C语言成绩（%.1f-%.1f）：", SCORE_MIN, SCORE_MAX);
        if (scanf("%f", &students[modifyIndex].cscore) != 1 || !isValidScore(students[modifyIndex].cscore)) {
            printf("成绩输入错误！请输入%.1f-%.1f之间的数值\n", SCORE_MIN, SCORE_MAX);
            clearInputBuffer();
        }
        else {
            break;
        }
    }

    while (1) {
        printf("请输入新的计算机基础成绩（%.1f-%.1f）：", SCORE_MIN, SCORE_MAX);
        if (scanf("%f", &students[modifyIndex].jscore) != 1 || !isValidScore(students[modifyIndex].jscore)) {
            printf("成绩输入错误！请输入%.1f-%.1f之间的数值\n", SCORE_MIN, SCORE_MAX);
            clearInputBuffer();
        }
        else {
            break;
        }
    }

    while (1) {
        printf("请输入新的英语成绩（%.1f-%.1f）：", SCORE_MIN, SCORE_MAX);
        if (scanf("%f", &students[modifyIndex].escore) != 1 || !isValidScore(students[modifyIndex].escore)) {
            printf("成绩输入错误！请输入%.1f-%.1f之间的数值\n", SCORE_MIN, SCORE_MAX);
            clearInputBuffer();
        }
        else {
            clearInputBuffer();
            break;
        }
    }

    printf("学号为%d的学生信息修改成功！\n", targetNum);
}

// 成绩汇总统计（分数段计算精准）
void scoreSummary() {
    if (studentCount == 0) {
        printf("暂无学生信息，无法统计成绩！\n");
        return;
    }

    int cScoreRange[10] = { 0 };  // C语言各分数段人数（0-9,10-19,...,90-100）
    int jScoreRange[10] = { 0 };  // 计算机基础各分数段人数
    int eScoreRange[10] = { 0 };  // 英语各分数段人数

    // 统计各科分数段分布（修复浮点数精度误差）
    for (int i = 0; i < studentCount; i++) {
        // 浮点数四舍五入后计算分数段
        int cRange = (int)(students[i].cscore + 0.5f) / 10;
        int jRange = (int)(students[i].jscore + 0.5f) / 10;
        int eRange = (int)(students[i].escore + 0.5f) / 10;

        // 边界处理：100分归入90-100段，负数归入0-9段
        cRange = (cRange > 9) ? 9 : (cRange < 0 ? 0 : cRange);
        jRange = (jRange > 9) ? 9 : (jRange < 0 ? 0 : jRange);
        eRange = (eRange > 9) ? 9 : (eRange < 0 ? 0 : eRange);

        cScoreRange[cRange]++;
        jScoreRange[jRange]++;
        eScoreRange[eRange]++;
    }

    // 输出统计结果（含百分比）
    printf("\n====================成绩汇总统计====================\n");
    // C语言成绩统计
    printf("C语言成绩分布：\n");
    for (int i = 0; i < 10; i++) {
        if (i == 9) {
            printf("90-100分：%d人（%.1f%%）\n", cScoreRange[i], (float)cScoreRange[i] / studentCount * 100);
        }
        else {
            printf("%2d-%2d分：%d人（%.1f%%）\n", i * 10, (i + 1) * 10 - 1, cScoreRange[i], (float)cScoreRange[i] / studentCount * 100);
        }
    }

    // 计算机基础成绩统计
    printf("\n计算机基础成绩分布：\n");
    for (int i = 0; i < 10; i++) {
        if (i == 9) {
            printf("90-100分：%d人（%.1f%%）\n", jScoreRange[i], (float)jScoreRange[i] / studentCount * 100);
        }
        else {
            printf("%2d-%2d分：%d人（%.1f%%）\n", i * 10, (i + 1) * 10 - 1, jScoreRange[i], (float)jScoreRange[i] / studentCount * 100);
        }
    }

    // 英语成绩统计
    printf("\n英语成绩分布：\n");
    for (int i = 0; i < 10; i++) {
        if (i == 9) {
            printf("90-100分：%d人（%.1f%%）\n", eScoreRange[i], (float)eScoreRange[i] / studentCount * 100);
        }
        else {
            printf("%2d-%2d分：%d人（%.1f%%）\n", i * 10, (i + 1) * 10 - 1, eScoreRange[i], (float)eScoreRange[i] / studentCount * 100);
        }
    }
    printf("==================================================\n\n");
}

// 显示主菜单
void showMainMenu() {
    printf("\n====================学生信息管理系统====================\n");
    printf("1. 新增学生信息\n");
    printf("2. 显示所有学生信息\n");
    printf("3. 根据姓名查找学生\n");
    printf("4. 根据学号删除学生\n");
    printf("5. 修改学生信息\n");
    printf("6. 成绩汇总统计\n");
    printf("7. 保存学生信息\n");
    printf("0. 退出系统\n");
    printf("======================================================\n");
    printf("请选择您的操作（0-7）：");
}

// 主函数（VS完全兼容）
int main() {
    // 设置文件缓冲模式，避免数据残留
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // 初始化管理员信息和加载学生数据
    initUser();
    loadStudents();

    // 登录验证（最多3次登录机会）
    int loginCount = 0;
    while (loginCount < 3) {
        if (login()) {
            break;
        }
        loginCount++;
        if (loginCount < 3) {
            printf("还有%d次登录机会\n", 3 - loginCount);
        }
    }
    if (loginCount >= 3) {
        printf("登录失败次数过多，程序退出！\n");
        return 0;
    }

    int choice;
    while (1) {
        showMainMenu();
        if (scanf("%d", &choice) != 1) {
            printf("输入错误！请输入0-7之间的数字\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
        case 1:
            addStudent();
            break;
        case 2:
            showAllStudents();
            break;
        case 3:
            findStudentByName();
            break;
        case 4:
            deleteStudentByNumber();
            break;
        case 5:
            modifyStudent();
            break;
        case 6:
            scoreSummary();
            break;
        case 7:
            saveStudents();
            break;
        case 0:
            printf("是否保存当前学生信息？（1-保存，0-不保存）：");
            int saveChoice;
            while (scanf("%d", &saveChoice) != 1 || (saveChoice != 0 && saveChoice != 1)) {
                printf("输入错误！请输入1（保存）或0（不保存）：");
                clearInputBuffer();
            }
            if (saveChoice == 1) {
                saveStudents();
            }
            printf("感谢使用，退出系统成功！\n");
            return 0;
        default:
            printf("输入错误，请选择0-7之间的操作！\n");
            break;
        }
    }
}