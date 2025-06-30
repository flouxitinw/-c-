#include "cainiao.h"

// 用户菜单
void user_menu(User* user) {
    while (1) {
        clear_screen();

        // 顶部横幅 - 固定宽度40字符
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║              用户中心                  ║\n");
        printf("╚════════════════════════════════════════╝\n\n");

        // 用户欢迎信息 - 固定宽度对齐
        printf(" 欢迎回来，%-15s", user->username);
        switch (user->level) {
        case USER_NORMAL:    printf("(普通用户)\n"); break;
        case USER_STUDENT:   printf("(学生用户)\n"); break;
        case USER_VIP:      printf("(VIP用户)\n"); break;
        case USER_ENTERPRISE: printf("(企业用户)\n"); break;
        case USER_PARTNER:  printf("(合作商户)\n"); break;
        }
        printf(" 手机号: %-15s\n\n", user->phone);
        int overdue_count = 0;
        int fresh_count = 0;
        int mispicked_count = 0;
        ExpressNode* curr = pickup_list;

        // 统计数量
        while (curr) {
            if (strcmp(curr->data.receiver_phone, user->phone) == 0) {
                if (curr->data.status == STATUS_OVERDUE) overdue_count++;
                if (curr->data.package_type == FRESH && curr->data.status == STATUS_UNPICKED) fresh_count++;
            }
            if (strcmp(curr->data.receiver_phone, user->phone) != 0 &&
                curr->data.status == STATUS_PICKED && strcmp(curr->data.pickup_phone, user->phone) == 0) mispicked_count++;
            curr = curr->next;
        }

        // 显示提醒 - 所有框宽度统一为40字符
        if (overdue_count > 0 || fresh_count > 0 || mispicked_count > 0) {
            printf("╔════════════════════════════════════════╗\n");
            printf("║              重要提醒                  ║\n");
            printf("╚════════════════════════════════════════╝\n");

            // 显示滞留包裹详情
            if (overdue_count > 0) {
                printf("\n 您有 %-2d 个滞留包裹需要尽快取走：\n", overdue_count);
                curr = pickup_list;
                while (curr) {
                    if (strcmp(curr->data.receiver_phone, user->phone) == 0 &&
                        curr->data.status == STATUS_OVERDUE) {

                        printf("┌────────────────────────────────────────┐\n");
                        printf("│ 快递单号: %-29s│\n", curr->data.tracking_num);
                        printf("│ 寄件人: %-14s %-15s │\n", curr->data.sender_name, curr->data.sender_phone);
                        printf("│ 包裹类型: %-28s │\n", get_package_type_name(curr->data.package_type));

                        // 计算滞留天数
                        double days = difftime(time(NULL), curr->data.storage_time) / 86400;
                        printf("│ 已滞留: %-6.1f天                       │\n", days);
                        printf("└────────────────────────────────────────┘\n");
                    }
                    curr = curr->next;
                }
            }

            // 显示生鲜包裹详情
            if (fresh_count > 0) {
                printf("\n 您有 %-2d 个生鲜包裹需要及时取走：\n", fresh_count);
                curr = pickup_list;
                while (curr) {
                    if (strcmp(curr->data.receiver_phone, user->phone) == 0 &&
                        curr->data.package_type == FRESH &&
                        curr->data.status == STATUS_UNPICKED) {

                        printf("┌────────────────────────────────────────┐\n");
                        printf("│ 快递单号: %-29s│\n", curr->data.tracking_num);
                        printf("│ 寄件人: %-14s %-15s │\n", curr->data.sender_name, curr->data.sender_phone);
                        printf("│ 包裹类型: %-28s │\n", get_package_type_name(curr->data.package_type));

                        // 计算入库时间
                        char time_str[20];
                        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                            localtime(&curr->data.storage_time));
                        printf("│ 入库时间: %-28s │\n", time_str);
                        printf("└────────────────────────────────────────┘\n");
                    }
                    curr = curr->next;
                }
            }

            if (overdue_count > 0 || fresh_count > 0) {
                printf("\n！ 请尽快前往快递点取件！\n");
            }
        }

        int unsent_count = 0;
        int sent_count = 0;
        ExpressNode* current = send_list;

        // 统计数量
        while (current) {
            if (strcmp(current->data.sender_phone, user->phone) == 0) {
                if (current->data.status == STATUS_UNSENT) {
                    unsent_count++;
                }
                else if (current->data.status == STATUS_SENT) {
                    sent_count++;
                }
            }
            current = current->next;
        }

        // 显示寄件提醒 - 统一宽度40字符
        if (unsent_count > 0 || sent_count > 0) {
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║              寄件提醒                  ║\n");
            printf("╚════════════════════════════════════════╝\n");

            if (unsent_count > 0) {
                printf("\n 您有 %-2d 个待寄出包裹\n", unsent_count);
            }

            if (sent_count > 0) {
                current = send_list;
                printf("\n 以下包裹已寄出：\n");
                while (current) {
                    if (strcmp(current->data.sender_phone, user->phone) == 0 &&
                        current->data.status == STATUS_SENT) {

                        char time_str[20];
                        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                            localtime(&current->data.create_time));

                        printf("┌────────────────────────────────────────┐\n");
                        printf("│ 快递单号: %-28s │\n", current->data.tracking_num);
                        printf("│ 寄往: %-14s %-17s │\n", current->data.receiver_name, current->data.receiver_phone);
                        printf("│ 寄出时间: %-28s │\n", time_str);
                        printf("└────────────────────────────────────────┘\n");
                    }
                    current = current->next;
                }
            }
        }

        // 显示冒领提醒 - 统一宽度40字符
        if (mispicked_count > 0) {
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║              冒领提醒                  ║\n");
            printf("╚════════════════════════════════════════╝\n");

            printf("\n 您有 %-2d 个快递疑似冒领！\n", mispicked_count);
            curr = pickup_list;
            while (curr) {
                if (strcmp(curr->data.receiver_phone, user->phone) != 0 &&
                    curr->data.status == STATUS_PICKED && strcmp(curr->data.pickup_phone, user->phone) == 0) {

                    printf("┌────────────────────────────────────────┐\n");
                    printf("│ 快递单号: %-28s │\n", curr->data.tracking_num);
                    printf("│ 寄件人: %-14s %-15s │\n", curr->data.sender_name, curr->data.sender_phone);
                    printf("│ 包裹类型: %-28s │\n", get_package_type_name(curr->data.package_type));
                    printf("└────────────────────────────────────────┘\n");
                }
                curr = curr->next;
            }
            printf("\n 如确认冒领，请点击误领选项！\n");
        }

        // 菜单选项 - 统一宽度40字符
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║              功能菜单                  ║\n");
        printf("╠════════════════════════════════════════╣\n");
        printf("║ 1. 查看我的信息                        ║\n");
        printf("║ 2. 修改个人信息                        ║\n");
        printf("║ 3. 快递服务                            ║\n");
        printf("║ 4. 注销自己                            ║\n");
        printf("║ 0. 退出登录                            ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: // 查看我的信息
            view_my_info(user);
            break;
        case 2://修改个人信息
            modify_user_info(user);
            break;
        case 3://快递服务
            express_service_menu(user);
            break;
        case 4: { // 用户注销
            if (user_unregister(&user_list, &user_count, &user)) {
                return; // 如果注销成功，返回主菜单
            }
            break;
        }
        case 0:
            return;
        default:
            printf("\n 无效的选择，请重新输入！\n");
            pause_program();
            clear_input_buffer();
        }
    }
}
// 投诉菜单 - 统一宽度40字符
void express_complaint_menu(User* user) {
    while (1) {
        clear_screen();
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║              投诉服务                  ║\n");
        printf("╠════════════════════════════════════════╣\n");
        printf("║ 1. 提交投诉                            ║\n");
        printf("║ 2. 查询投诉                            ║\n");
        printf("║ 0. 返回上级菜单                        ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            submit_complaint(user);
            break;
        case 2:
            view_user_complaints(user);
            break;
        case 0:
            return;
        default:
            printf("\n无效的选择，请重新输入！\n");
            pause_program();
            clear_input_buffer();
        }
    }
}


// 快递服务菜单 - 统一宽度40字符
void express_service_menu(User* user) {
    while (1) {
        clear_screen();
        printf("\n╔════════════════════════════════════════╗\n");
        printf("║              快递服务                  ║\n");
        printf("╠════════════════════════════════════════╣\n");
        printf("║ 1. 寄快递                              ║\n");
        printf("║ 2. 取快递                              ║\n");
        printf("║ 3. 投诉                                ║\n");
        printf("║ 4. 拒收                                ║\n");
        printf("║ 0. 返回上级菜单                        ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1:
            send_express_menu(user);
            break;
        case 2:
            pickup_express(user);
            break;
        case 3:
            express_complaint_menu(user);
            break;
        case 4:
            reject_package_menu(user);
            break;
        case 0:
            return;
        default:
            printf("\n无效的选择，请重新输入！\n");
            pause_program();
            clear_input_buffer();
        }
    }
}