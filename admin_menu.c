#include "cainiao.h"

// 管理员菜单
void admin_menu(Admin* admin) {
    int choice;
    bool is_super = is_super_admin(admin);

    do {
        clear_screen();
        printf("\n======================= 管理员控制面板 =======================\n");

        // 统计各种状态的数量
        int overdue_count = 0;
        int unprocessed_complaints = 0;
        int mispicked_count = 0;
        int unsent_count = 0;
        int rejected_count = 0;

        // 统计滞留包裹（pickup_list）
        ExpressNode* curr = pickup_list;
        while (curr) {
            // 计算滞留天数
            double days = difftime(time(NULL), curr->data.storage_time) / 86400;
            if (curr->data.status == STATUS_OVERDUE && days > 7) {
                overdue_count++;
                printf("货架:%-6s 单号:%-12s 收件人:%-10s 已滞留(%.1f天)\n",
                    curr->data.shelf_id,
                    curr->data.tracking_num,
                    curr->data.receiver_name,
                    days);
            }
            curr = curr->next;
        }

        // 统计投诉和拒收（pickup_list）
        curr = pickup_list; // 重置指针


        while (curr) {
            // 冒领件
            if (curr->data.status == STATUS_MISPICKED) {
                mispicked_count++;
            }
            // 待处理拒收件
            else if (curr->data.status == STATUS_REJECTED) {
                rejected_count++;
            }
            curr = curr->next;
        }

        // 统计待寄出快递（send_list）
        curr = send_list;
        while (curr) {
            // 未处理投诉
            if (curr->data.status == STATUS_LOST || curr->data.status == STATUS_DAMAGED) {
                unprocessed_complaints++;
            }
            if (curr->data.status == STATUS_UNSENT) {
                unsent_count++;
            }
            curr = curr->next;
        }

        // 显示简洁提醒
        if (overdue_count > 0 || unprocessed_complaints > 0 ||
            mispicked_count > 0 || unsent_count > 0 || rejected_count > 0) {
            printf("\n==================== 系统提醒 ====================\n");

            if (overdue_count > 0) {
                printf("[紧急] 有 %d 个包裹滞留超过7天！请电话联系客户处理！\n", overdue_count);
            }

            if (unprocessed_complaints > 0) {
                printf("[重要] 有 %d 个投诉未处理！请尽快处理客户投诉！\n", unprocessed_complaints);
            }

            if (mispicked_count > 0) {
                printf("[注意] 有 %d 个包裹疑似被冒领！请尽快处理！\n", mispicked_count);
            }

            if (unsent_count > 0) {
                printf("[待办] 有 %d 个待寄出包裹！请尽快安排寄出！\n", unsent_count);
            }

            if (rejected_count > 0) {
                printf("[待办] 有 %d 个拒收包裹待处理！请尽快处理！\n", rejected_count);
            }

            printf("==================================================\n");
        }
        else {
            printf("\n当前没有需要紧急处理的事项！\n");
        }
        printf("==================================================\n");



        printf("\n======= 管理员菜单 =======\n");
        printf("当前身份: %s %s\n",
            is_super ? "[超级管理员]" : "[普通管理员]",
            admin->username);

        printf("1. 用户管理\n");
        printf("2. 快递管理\n");
        printf("3. 货架管理\n");
        if (is_super) {
            printf("4. 管理员管理\n");
        }
        else {
            printf("4. 注销我的账户\n");
        }
        printf("5. 修改我的信息\n");
        printf("6. 投诉处理\n");
        printf("7. 拒收处理\n");
        printf("0. 退出登录\n");
        printf("请选择操作: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            choice = -1; // 无效输入
        }

        switch (choice) {
        case 1: // 用户管理
            user_management_menu(admin);
            break;

        case 2:
            manage_express(admin);
            pause_program();
            clear_input_buffer();
            break;

        case 3: // 货架管理
            shelf_management_menu(admin);
            break;

        case 4: // 管理员管理/注销账户
            if (is_super) {
                admin_management_menu(admin);
            }
            else {
                admin_delete_account(admin);
            }
            break;
        case 5: // 修改我的信息
            modify_admin_info(admin);
            break;
        case 6://投诉处理
            admin_handle_complaints();
            break;
        case 7://拒收处理
            admin_reject_management_menu();
            break;
        case 0: // 退出登录
            return;

        default:
            printf("无效的选择！\n");
            pause_program();
            clear_input_buffer();
        }
    } while (true);
}

// 用户管理菜单
void user_management_menu(Admin* admin) {
    int choice;

    while (1) {
        clear_screen();
        printf("\n========== 用户管理菜单 ==========\n");
        printf("当前管理员: %s\n", admin->username);
        printf("1. 查看所有用户\n");
        printf("2. 添加用户\n");
        printf("3. 修改用户信息\n");
        printf("4. 注销用户\n");
        printf("0. 返回主菜单\n");
        printf("请选择操作: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }

        switch (choice) {
        case 1://查看所有用户
            view_all_users();
            break;

        case 2://添加用户
            admin_add_user();
            break;
        case 3: // 修改用户信息
            /*先显示所有用户信息，按用户名找到用户，调用用户修改自己信息的函数*/

            // 先显示所有用户
            view_all_users();
            // 用户选择部分
            char username[50];
            printf("\n输入要修改的用户名（输入0返回）: ");
            if (fgets(username, sizeof(username), stdin) == NULL) {
                clear_input_buffer();
                break;
            }
            username[strcspn(username, "\n")] = '\0'; // 去除换行符

            if (strcmp(username, "0") == 0) break;

            User* user = find_user_by_username(user_list, username);//找到用户
            if (user) {//调用修改指定用户信息函数
                if (modify_user_info(user)) {
                    save_users_to_file(user_list, "users.txt");
                    printf("\n用户信息已更新！\n");
                }
            }
            else {
                printf("\n错误：用户 %s 不存在！\n", username);
            }

            pause_program();
            clear_input_buffer();
            break;
        case 4: // 注销用户
        {
            // 先显示所有用户
            view_all_users();
            printf("输入要注销的用户名: ");

            char username[50];
            if (scanf_s("%49s", username, (unsigned)_countof(username)) != 1) {
                clear_input_buffer();
                printf("输入无效！\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // 清除输入缓冲区中的剩余字符（包括换行符）
            clear_input_buffer();

            // 查找用户是否存在
            User* user_to_delete = find_user_by_username(user_list, username);
            if (!user_to_delete) {
                printf("用户 %s 不存在\n", username);
                pause_program();
                clear_input_buffer();
                break;
            }

            // 显示要删除的用户详细信息
            printf("\n=== 即将删除的用户信息 ===\n");
            printf("用户名: %s\n", user_to_delete->username);
            printf("手机号: %s\n", user_to_delete->phone);
            printf("用户类型: ");
            switch (user_to_delete->level) {
            case USER_NORMAL:    printf("普通用户"); break;
            case USER_STUDENT:   printf("学生用户"); break;
            case USER_VIP:      printf("VIP用户"); break;
            case USER_ENTERPRISE: printf("企业用户"); break;
            case USER_PARTNER:  printf("合作商户"); break;
            }
            printf("\n");

            // 确认步骤
            printf("\n确定要永久删除该用户吗？(y): ");
            char confirm;
            scanf(" %c", &confirm);  // 注意前面的空格，这会跳过空白字符
            clear_input_buffer();

            if (tolower(confirm) != 'y') {
                printf("已取消删除操作\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // 执行删除
            if (delete_user(&user_list, username)) {
                user_count--;
                printf("\n用户 %s 已成功注销\n", username);
                save_users_to_file(user_list, "users.txt");
            }
            pause_program();
            clear_input_buffer();
            break;
        }
        case 0:
            return;

        default:
            printf("无效选择!\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

// 货架管理菜单
void shelf_management_menu(Admin* admin) {
    if (!admin) return;

    int choice;

    do {
        clear_screen();
        printf("\n========== 货架管理系统 ==========\n");
        printf("当前用户: %s\n", admin->username);
        printf("-----------------------------------\n");
        printf("1. 创建新货架\n");
        printf("2. 查看货架状态\n");
        printf("3. 删除货架\n");
        printf("4. 货架盘点\n");
        printf("5. 库存预警\n");
        printf("0. 返回上级菜单\n");
        printf("-----------------------------------\n");
        printf("请选择操作: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            choice = -1;
        }

        switch (choice) {
        case 1:   // 创建新货架
        {
            char shelf_prefix;//货架类型前缀
            int max_capacity = 0;//最大容量
            bool is_valid = false;

            // 货架类型输入循环
            while (!is_valid) {
                clear_screen();
                printf("\n========== 创建新货架 ==========\n");
                printf("请选择货架类型：\n");
                printf("N - 普通货架\n");
                printf("F - 易碎品货架\n");
                printf("C - 冷链货架\n");
                printf("B - 大件货架\n");
                printf("Q - 取消操作\n");
                printf("--------------------------------\n");
                printf("请输入选择(N,F,C,B,Q): ");

                // 读取输入
                if (scanf_s(" %c", &shelf_prefix, 1) != 1) {
                    clear_input_buffer();
                    printf("\n输入错误！\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();

                shelf_prefix = toupper(shelf_prefix);//转为大写

                // 检查是否要求退出
                if (shelf_prefix == 'Q') {
                    break;  // 跳出循环，结束case
                }

                // 验证输入
                if (strchr("NFCB", shelf_prefix) == NULL) {
                    printf("\n错误：只能输入N、F、C或B！\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }

                is_valid = true;
            }

            if (!is_valid) break; // 用户取消了操作

            // 自动生成货架ID（如N01、N02等）
            int max_num = 0;
            Shelf* curr = shelf_list;
            while (curr) {
                // 检查同类型货架，找到他
                if (toupper(curr->shelf_id[0]) == shelf_prefix) {
                    // 提取编号部分(如N01中的01) 然后+1作为自己的
                    int num = atoi(curr->shelf_id + 1);
                    if (num > max_num) {
                        max_num = num;
                    }
                }
                curr = curr->next;
            }

            char shelf_id[10];// 存储生成的ID
            // 格式化新ID：前缀+两位数字(如N01)
            snprintf(shelf_id, sizeof(shelf_id), "%c%02d", shelf_prefix, max_num + 1);

            // 货架容量输入循环（带重试机制）
            is_valid = false;
            while (!is_valid) {
                clear_screen();
                printf("\n货架类型: %c\n", shelf_prefix);
                printf("自动生成货架ID: %s\n", shelf_id);
                printf("--------------------------------\n");
                printf("请输入货架容量（1-999，输入0取消）: ");

                if (scanf_s("%d", &max_capacity) != 1) {
                    clear_input_buffer();
                    printf("\n必须输入数字！\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();

                // 检查取消操作
                if (max_capacity == 0) {
                    printf("\n已取消创建货架\n");
                    pause_program();
                    clear_input_buffer();
                    break;
                }

                // 验证范围
                if (max_capacity < 1 || max_capacity > 999) {
                    printf("\n容量必须在1-999之间！\n");
                    pause_program();
                    clear_input_buffer();
                }
                else {
                    is_valid = true;
                }
            }

            if (!is_valid) break; // 用户取消了容量输入

            // 创建货架
            Shelf* new_shelf = create_shelf(shelf_id, max_capacity);
            if (!new_shelf) {
                printf("\n创建货架失败（内存不足）！\n");
                pause_program();
                clear_input_buffer();
                break;
            }

            // 设置货架类型
            switch (shelf_prefix) {
            case 'F': new_shelf->type = SHELF_FRAGILE; break;// 易碎品
            case 'C': new_shelf->type = SHELF_COLD; break;// 冷链
            case 'B': new_shelf->type = SHELF_BULK; break;// 大件
            default: new_shelf->type = SHELF_NORMAL; // 默认普通
            }

            // 添加到链表
            add_shelf(&shelf_list, new_shelf);

            // 保存数据
            if (save_shelves_to_file(shelf_list, "shelves.txt")) {
                // 绿色显示成功信息
                printf("\n\033[32m[成功] 货架 %s 已创建（容量：%d）\033[0m\n",
                    new_shelf->shelf_id, new_shelf->max_capacity);
                printf("当前货架总数：%d\n", count_shelves(shelf_list));
            }
            else {
                // 黄色显示警告信息
                printf("\n\033[33m警告：货架创建成功但保存失败！\033[0m\n");
                printf("请手动备份数据并联系管理员\n");
            }

            pause_program();
            clear_input_buffer();
            break;
        }
        case 2: // 查看货架状态
        {
            clear_screen();
            // 检查是否有货架数据
            if (!shelf_list) {// 检查货架链表是否为空
                printf("\n系统错误：没有可用的货架数据！\n");
                printf("可能原因：\n");
                printf("1. 尚未创建任何货架\n");
                printf("2. 数据未正确加载\n");
                pause_program();
                clear_input_buffer();
                break;
            }
            // 显示所有货架状态
            display_shelves(shelf_list);
            break;
        }
        case 3: // 删除货架
        {
            char shelf_id[10] = { 0 };
            bool success = false;

            while (!success) {
                clear_screen();
                printf("\n====== 删除货架 ======\n");
                printf("当前货架列表：\n");
                display_shelves(shelf_list);  // 显示现有货架


                printf("\n请输入要删除的货架ID（输入Q退出）: ");
                if (scanf_s("%9s", shelf_id, (unsigned)_countof(shelf_id)) != 1) {
                    clear_input_buffer();
                    printf("\n输入错误！\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();

                // 检查是否要求退出
                if (strcmp(shelf_id, "Q") == 0 || strcmp(shelf_id, "q") == 0) {
                    break;
                }

                // 验证货架ID格式
                if (!is_valid_shelf_id(shelf_id)) {
                    printf("\n错误：无效的货架ID格式！\n");
                    printf("货架ID格式要求：\n");
                    printf("1. 以字母开头（如A-Z）\n");
                    printf("2. 只包含字母和数字\n");
                    printf("3. 长度2-9个字符\n");
                    printf("正确格式示例：A01, B02\n");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }

                // 尝试删除
                success = delete_shelf(&shelf_list, shelf_id);
                if (!success) {
                    pause_program();
                    clear_input_buffer();// 让用户看到错误信息
                }
            }
            save_shelves_to_file(shelf_list, "shelves.txt");
            pause_program();
            clear_input_buffer();
            break;
        }
        case 4: // 货架盘点
        {// 调用盘点菜单
            inventory_menu(shelf_list);
            break;
        }

        case 5:  // 库存预警
            check_shelf_capacity(shelf_list);
            break;

        case 0:  // 返回上级菜单
            return;

        default:
            printf("无效的选择！\n");
            pause_program();
            clear_input_buffer();
        }
    } while (true);
}

// 货架盘点菜单
void inventory_menu(Shelf* head) {
    int choice;
    char shelf_id[16];
    Shelf* target_shelf = NULL;// 用于查找目标货架

    while (1) {
        clear_screen();
        printf("\n====== 驿站库存盘点系统 ======\n");
        printf("1. 盘点单个货架\n");
        printf("2. 全站盘点\n");
        printf("0. 返回主菜单\n");
        printf("请选择操作: ");

        if (scanf_s("%d", &choice) != 1) {
            printf("输入无效，请输入数字！\n");
            clear_input_buffer();
            continue;
        }

        // 处理用户选择
        switch (choice) {
        case 1: { // 单个货架盘点
            printf("请输入货架ID: ");
            if (scanf_s("%15s", shelf_id, (unsigned)_countof(shelf_id)) != 1) {
                clear_input_buffer();
                // 处理错误情况
            }
            clear_input_buffer();// 清除换行符

            target_shelf = shelf_list;
            while (target_shelf && strcmp(target_shelf->shelf_id, shelf_id) != 0) {
                target_shelf = target_shelf->next;
            }

            if (target_shelf) {
                inventory_shelf(target_shelf);  // 显示货架库存 调用盘点函数

            }
            else {
                printf("\n错误：未找到指定货架！\n");
                printf("可能原因：\n");
                printf("尚未创建该货架\n");
            }
            pause_program();
            clear_input_buffer();
            break;
        }

        case 2: // 全站盘点
            if (!head) {
                printf("\n错误：当前没有货架数据！\n");
            }
            else {
                full_inventory(head);  // 显示所有货架状态
                printf("\n");
                show_all_overdue_items_with_stats(); // 显示完整滞留件报告
            }
            pause_program();
            clear_input_buffer();
            break;

        case 0: // 退出
            return;

        default:
            printf("无效选择，请重新输入！\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

// 管理员管理菜单
void admin_management_menu(Admin* current_admin) {
    if (!current_admin) return;

    // 权限检查 - 只有超级管理员可以访问此菜单
    if (!is_super_admin(current_admin)) {
        printf("\n权限不足！只有超级管理员可以访问管理员管理系统。\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    int choice;
    Admin* target_admin = NULL;

    while (1) {
        clear_screen();
        printf("\n========== 管理员管理系统 ==========\n");
        printf("当前用户: %s [超级管理员]\n", current_admin->username);
        printf("-----------------------------------\n");
        printf("1. 查看所有管理员\n");
        printf("2. 注册新管理员\n");
        printf("3. 修改其他管理员信息\n");
        printf("4. 注销普通管理员账户\n");
        printf("0. 返回主菜单\n");
        printf("-----------------------------------\n");
        printf("请选择操作: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            printf("无效输入，请重新选择！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }

        switch (choice) {
        case 0:  // 返回主菜单
            return;
        case 1:  // 查看所有管理员
            view_all_admins();
            break;
        case 2:  // 注册新管理员
            admin_register(current_admin);
            break;

        case 3:  // 修改其他管理员信息
            super_modify_admin_info(current_admin);
            break;

        case 4:  // 注销普通管理员账户
            admin_delete_account(current_admin);
            break;

        default:
            printf("无效选择，请重新输入！\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

