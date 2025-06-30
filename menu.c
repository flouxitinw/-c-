#include "cainiao.h"

// 主菜单
void main_menu() {
    while (1) {
        clear_screen();
        printf("\n========== 菜鸟驿站管理系统 ==========\n");
        printf("1. 用户注册\n");
        printf("2. 用户登录\n");
        printf("3. 管理员登录\n");
        printf("0. 退出系统\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            user_register();
            break;
        case 2:
            user_login();
            break;
        case 3:
            admin_login();
            break;
        case 0:
            printf("\n感谢使用菜鸟驿站管理系统，再见！\n");
            exit(0);
        default:
            printf("\n无效的选择，请重新输入！\n");
            pause_program();
            clear_input_buffer();
        }
    }
}