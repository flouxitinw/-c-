#include "cainiao.h"

//--------------------用户拒收模块--------------------
// 用户查看自己的拒收信息
/*输入快递单号查看收件人和寄件人的电话*/
void view_rejected_packages(User* current_user) {
    clear_screen();
    printf("\n============= 我的拒收记录 =============\n");

    int count = 0;// 拒收记录计数器
    ExpressNode* current = pickup_list;// 从取件列表开始遍历
    char time_buffer[26];

    // 遍历查找该用户的拒收快递(手机号和状态)
    while (current != NULL) {
        if (strcmp(current->data.receiver_phone, current_user->phone) == 0 &&
            (current->data.status == STATUS_REJECTED || current->data.status == STATUS_RETURNED || current->data.status == STATUS_UNPICKED_AGAIN)) {

            printf("\n%d. %s | %s -> %s\n",
                ++count,
                current->data.tracking_num,
                current->data.sender_name,
                current->data.receiver_name);

            printf("包裹类型: %s\n", get_package_type_name(current->data.package_type));

            // 格式化时间显示
            ctime_s(time_buffer, sizeof(time_buffer), &current->data.storage_time);
            printf("到站时间: %s", time_buffer);

            ctime_s(time_buffer, sizeof(time_buffer), &current->data.reject_time);
            printf("拒收时间: %s", time_buffer);

            printf("拒收原因: %s\n", current->data.reject_reason);

            // 显示处理状态
            if (current->data.status == STATUS_RETURNED) {
                printf("处理状态: 已退回寄件人\n");
            }
            else if (current->data.status == STATUS_UNPICKED_AGAIN) {
                printf("\n处理结果: 管理员已拒绝您的拒收申请\n");
            }
            else {
                printf("\n处理状态: 等待管理员处理\n");
            }


            printf("-------------------------------------\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n您没有拒收记录\n");
        pause_program();
        clear_input_buffer();
    }
    else {
        printf("\n共找到 %d 条拒收记录\n", count);

        // 提供查看详情选项
        printf("\n输入快递单号查看详情（输入0返回）: ");
        char tracking_num[20];
        scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
        clear_input_buffer();

        if (strcmp(tracking_num, "0") != 0) {
            // 查找指定快递
            current = pickup_list;
            Express* target_express = NULL;

            while (current != NULL) {
                if (strcmp(current->data.tracking_num, tracking_num) == 0 &&
                    strcmp(current->data.receiver_phone, current_user->phone) == 0 &&
                    (current->data.status == STATUS_REJECTED || current->data.status == STATUS_RETURNED || current->data.status == STATUS_UNPICKED_AGAIN)) {
                    target_express = &current->data;
                    break;
                }
                current = current->next;
            }

            if (target_express != NULL) {
                clear_screen();
                printf("\n========== 拒收快递详情 ==========\n");
                printf("快递单号: %s\n", target_express->tracking_num);
                printf("寄件人: %s (%s)\n",
                    target_express->sender_name,
                    target_express->sender_phone);
                printf("收件人: %s (%s)\n",
                    target_express->receiver_name,
                    target_express->receiver_phone);
                printf("包裹类型: %s\n",
                    get_package_type_name(target_express->package_type));

                ctime_s(time_buffer, sizeof(time_buffer), &target_express->storage_time);
                printf("到站时间: %s", time_buffer);

                ctime_s(time_buffer, sizeof(time_buffer), &target_express->reject_time);
                printf("拒收时间: %s", time_buffer);

                printf("拒收原因: %s\n", target_express->reject_reason);

                if (target_express->status == STATUS_RETURNED) {
                    printf("\n处理结果: 已退回给寄件人\n");
                    printf("寄件人联系方式: %s\n", target_express->sender_phone);
                }
                else if (target_express->status == STATUS_UNPICKED_AGAIN) {
                    printf("\n处理结果: 管理员已拒绝您的拒收申请\n");
                }
                else {
                    printf("\n处理状态: 等待管理员处理\n");
                }

                printf("\n==================================\n");
            }
            else {
                printf("\n未找到该快递或无权查看\n");
            }

            pause_program();
            clear_input_buffer();
        }
    }

}

//用户提交拒收
void reject_package(User* current_user) {
    clear_screen();
    printf("\n============== 快递拒收 ===============\n");

    // 显示用户可拒收的快递
    int count = 0;// 可拒收快递计数
    ExpressNode* current = pickup_list;
    char time_buffer[26];
    while (current != NULL) {
        // 检查收件人手机号匹配且状态为未取件或拒收被拒
        if (strcmp(current->data.receiver_phone, current_user->phone) == 0 &&
            (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN)) {

            printf("%d. %s | %s -> %s | %s\n",
                ++count,
                current->data.tracking_num,
                current->data.sender_name,
                current->data.receiver_name,
                get_package_type_name(current->data.package_type));
            ctime_s(time_buffer, sizeof(time_buffer), &current->data.storage_time);
            printf("到站时间：%s", time_buffer);
            printf("货架号：%s  取件码：%s\n", current->data.shelf_id, current->data.pickup_code);
            printf("---------------------------------------\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n当前没有可拒收的快递\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    printf("\n请输入要拒收的快递单号（输入0取消）：");
    char tracking_num[20];
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    if (strcmp(tracking_num, "0") == 0) {
        printf("\n已取消拒收操作\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 查找快递并验证权限
    Express* target_express = NULL;
    current = pickup_list;
    while (current != NULL) {
        if (strcmp(current->data.tracking_num, tracking_num) == 0 &&
            strcmp(current->data.receiver_phone, current_user->phone) == 0) {
            target_express = &current->data;
            break;
        }
        current = current->next;
    }

    if (target_express == NULL) {
        printf("\n未找到该快递或无权操作\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 验证快递状态是否符合拒收条件
    if (target_express->status != STATUS_UNPICKED &&
        target_express->status != STATUS_UNPICKED_AGAIN) {
        printf("\n该快递状态不符合拒收条件\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 显示快递详情
    printf("\n快递详情：\n");
    printf("快递单号: %s\n", target_express->tracking_num);
    printf("寄件人: %s (%s)\n", target_express->sender_name, target_express->sender_phone);
    printf("收件人: %s (%s)\n", target_express->receiver_name, target_express->receiver_phone);
    printf("货架: %s | 取件码: %s\n", target_express->shelf_id, target_express->pickup_code);
    printf("包裹类型: %s\n", get_package_type_name(target_express->package_type));

    // 选择拒收原因
    const char* reasons[] = {
        "商品与描述不符",
        "商品少件",
        "配送错误",
        "其他原因"
    };

    int reason_choice = 0;
    do {
        printf("\n请选择拒收原因:\n");
        for (int i = 0; i < 4; i++) {
            printf("%d. %s\n", i + 1, reasons[i]);
        }
        printf("请选择(1-4): ");

        if (scanf_s("%d", &reason_choice) != 1) {
            clear_input_buffer();
            printf("\n错误：请输入有效的数字选项！\n");
        }
        else if (reason_choice < 1 || reason_choice > 4) {
            printf("\n错误：请输入1-4之间的数字！\n");
        }
    } while (reason_choice < 1 || reason_choice > 4);
    clear_input_buffer();

    // 确认操作
    printf("\n确认要拒收此快递吗？(y表示确认): ");
    char confirm;
    scanf_s(" %c", &confirm, 1);
    clear_input_buffer();

    if (tolower(confirm) == 'y') {
        // 保存拒收原因
        strncpy(target_express->reject_reason, reasons[reason_choice - 1],
            sizeof(target_express->reject_reason) - 1);
        target_express->reject_time = time(NULL);
        target_express->status = STATUS_REJECTED;

        save_pickup_express_info(pickup_list, "pickup_express.txt");
        printf("\n已成功提交拒收申请\n");
    }
    else {
        printf("\n已取消拒收操作\n");
    }

    pause_program();
    clear_input_buffer();
}

// 用户拒收功能菜单
void reject_package_menu(User* current_user) {
    int choice;

    do {
        clear_screen();
        printf("\n============= 快递拒收服务 =============\n");
        printf("当前用户: %s (%s)\n", current_user->username, current_user->phone);
        printf("\n请选择操作:\n");
        printf("1. 查看我的拒收记录\n");
        printf("2. 提交拒收申请\n");
        printf("0. 返回上级菜单\n");
        printf("---------------------------------------\n");
        printf("请输入选择: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            printf("\n错误：请输入有效的数字选项！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
        case 1:
            // 查看拒收记录
            view_rejected_packages(current_user);
            break;

        case 2:
            // 提交拒收申请
            reject_package(current_user);
            break;

        case 0:
            // 返回上级菜单
            return;

        default:
            printf("\n无效的选择，请输入0-2之间的数字！\n");
            pause_program();
            clear_input_buffer();
        }
    } while (true);
}


//--------------------管理员拒收模块--------------------
// 管理员查看所有拒收记录
void admin_view_all_rejected_packages() {
    clear_screen();
    printf("\n============= 所有拒收记录 =============\n");

    int count = 0;
    ExpressNode* current = pickup_list;
    char time_buffer[26];

    // 遍历查找所有拒收快递
    while (current != NULL) {
        if (current->data.status == STATUS_REJECTED ||
            current->data.status == STATUS_RETURNED ||
            current->data.status == STATUS_UNPICKED_AGAIN) {

            printf("\n%d. %s | %s -> %s\n",
                ++count,
                current->data.tracking_num,
                current->data.sender_name,
                current->data.receiver_name);

            printf("收件人电话: %s\n", current->data.receiver_phone);
            printf("包裹类型: %s\n", get_package_type_name(current->data.package_type));

            // 格式化时间显示//将时间值（time_t）转换为可读的字符串形式
            ctime_s(time_buffer, sizeof(time_buffer), &current->data.storage_time);
            printf("到站时间: %s", time_buffer);

            if (current->data.status == STATUS_REJECTED ||
                current->data.status == STATUS_RETURNED ||
                current->data.status == STATUS_UNPICKED_AGAIN) {
                ctime_s(time_buffer, sizeof(time_buffer), &current->data.reject_time);
                printf("拒收时间: %s", time_buffer);
            }

            printf("拒收原因: %s\n", current->data.reject_reason);

            // 显示处理状态
            if (current->data.status == STATUS_RETURNED) {
                printf("处理状态: 已退回寄件人\n");
            }
            else if (current->data.status == STATUS_UNPICKED_AGAIN) {
                printf("处理状态: 已拒绝拒收申请(待取件)\n");
            }
            else {
                printf("处理状态: 等待处理\n");
            }

            printf("货架号: %s | 取件码: %s\n", current->data.shelf_id, current->data.pickup_code);
            printf("-------------------------------------\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n当前没有拒收记录\n");
    }
    else {
        printf("\n共找到 %d 条拒收记录\n", count);

        // 提供查看详情选项
        printf("\n输入快递单号查看详情（输入0返回）: ");
        char tracking_num[20];
        scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
        clear_input_buffer();

        if (strcmp(tracking_num, "0") != 0) {
            // 查找指定快递
            current = pickup_list;
            Express* target_express = NULL;

            while (current != NULL) {
                if (strcmp(current->data.tracking_num, tracking_num) == 0 &&
                    (current->data.status == STATUS_REJECTED ||
                        current->data.status == STATUS_RETURNED ||
                        current->data.status == STATUS_UNPICKED_AGAIN)) {
                    target_express = &current->data;
                    break;
                }
                current = current->next;
            }

            if (target_express != NULL) {
                clear_screen();
                printf("\n========== 拒收快递详情 ==========\n");
                printf("快递单号: %s\n", target_express->tracking_num);
                printf("寄件人: %s (%s)\n",
                    target_express->sender_name,
                    target_express->sender_phone);
                printf("收件人: %s (%s)\n",
                    target_express->receiver_name,
                    target_express->receiver_phone);
                printf("包裹类型: %s\n",
                    get_package_type_name(target_express->package_type));
                printf("货架号: %s | 取件码: %s\n",
                    target_express->shelf_id,
                    target_express->pickup_code);

                ctime_s(time_buffer, sizeof(time_buffer), &target_express->storage_time);
                printf("到站时间: %s", time_buffer);

                if (target_express->status == STATUS_REJECTED ||
                    target_express->status == STATUS_RETURNED ||
                    current->data.status == STATUS_UNPICKED_AGAIN) {
                    ctime_s(time_buffer, sizeof(time_buffer), &target_express->reject_time);
                    printf("拒收时间: %s", time_buffer);
                }

                printf("拒收原因: %s\n", target_express->reject_reason);

                if (target_express->status == STATUS_RETURNED) {
                    printf("\n处理结果: 已退回给寄件人\n");
                    printf("寄件人联系方式: %s\n", target_express->sender_phone);
                }
                else if (target_express->status == STATUS_UNPICKED_AGAIN) {
                    printf("\n处理结果: 已拒绝拒收申请\n");
                    printf("当前状态: 待取件\n");
                }
                else {
                    printf("\n处理状态: 等待处理\n");
                }

                printf("\n==================================\n");
            }
            else {
                printf("\n未找到该拒收快递\n");
            }

            pause_program();
            clear_input_buffer();
        }
    }

}

//管理员处理拒收
void admin_process_rejected_packages() {
    while (1) {
        clear_screen();
        printf("\n========== 拒收快递处理 ==========\n");

        // 显示拒收快递列表
        int rejected_count = 0;
        ExpressNode* current = pickup_list;

        printf("\n=== 待处理的拒收快递 ===\n");
        char time_buffer[26];
        while (current != NULL) {
            if (current->data.status == STATUS_REJECTED) {
                printf("%d. %s | %s -> %s | %s\n",
                    ++rejected_count,
                    current->data.tracking_num,
                    current->data.sender_name,
                    current->data.receiver_name,
                    get_package_type_name(current->data.package_type));
                ctime_s(time_buffer, sizeof(time_buffer), &current->data.storage_time);
                printf("到站时间：%s", time_buffer);
                printf("货架号：%s  取件码：%s\n", current->data.shelf_id, current->data.pickup_code);
                printf("拒收原因: %s\n", current->data.reject_reason);
                printf("---------------------------------\n");
            }
            current = current->next;
        }

        if (rejected_count == 0) {
            printf("\n当前没有待处理的拒收快递\n");
            pause_program();
            clear_input_buffer();
            return;
        }

        // 操作菜单
        printf("\n操作选项:\n");
        printf("1. 处理拒收快递\n");
        printf("2. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // 处理拒收快递
            printf("\n请输入要处理的快递单号: ");
            char tracking_num[20];
            scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
            clear_input_buffer();

            // 查找目标快递
            ExpressNode* target_node = pickup_list;
            while (target_node != NULL) {
                if (strcmp(target_node->data.tracking_num, tracking_num) == 0 &&
                    target_node->data.status == STATUS_REJECTED) {
                    break;
                }
                target_node = target_node->next;
            }

            if (target_node == NULL) {
                printf("\n错误：未找到该拒收快递\n");
                pause_program();
                clear_input_buffer();
                continue;
            }

            // 显示详细信息
            printf("\n===== 快递详细信息 =====\n");
            printf("快递单号：%s\n", target_node->data.tracking_num);
            printf("原寄件人：%s (%s)\n", target_node->data.sender_name, target_node->data.sender_phone);
            printf("原收件人：%s (%s)\n", target_node->data.receiver_name, target_node->data.receiver_phone);
            printf("包裹类型：%s\n", get_package_type_name(target_node->data.package_type));

            // 处理选项
            printf("\n请选择处理方式：\n");
            printf("1. 退回给寄件人\n");
            printf("2. 重新联系收件人\n");
            printf("3. 取消处理\n");
            printf("选择操作：");

            int action;
            scanf_s("%d", &action);
            clear_input_buffer();

            // 执行处理
            switch (action) {
            case 1: {
                // 退回给寄件人 - 完整填写退回快递信息 - 寄件
                Express* return_express = (Express*)malloc(sizeof(Express));// 创建退回快递
                if (!return_express) {
                    perror("内存分配失败");
                    pause_program();
                    clear_input_buffer();
                    continue;
                }
                memset(return_express, 0, sizeof(Express));// 初始化内存

                printf("\n=== 填写退回快递信息 ===\n");

                // 交换寄收件人信息
                strcpy(return_express->sender_name, target_node->data.receiver_name);
                strcpy(return_express->sender_phone, target_node->data.receiver_phone);
                strcpy(return_express->receiver_name, target_node->data.sender_name);
                strcpy(return_express->receiver_phone, target_node->data.sender_phone);

                // 填写包裹信息
                printf("\n[包裹信息]\n");
                return_express->weight = input_positive_double("包裹重量(千克): ");
                return_express->volume = input_positive_double("包裹体积(立方厘米): ");

                // 检查包裹尺寸限制
                if (return_express->weight > 50 || return_express->volume >= 3000) {
                    printf("\n包裹过大不可邮寄\n");
                    free(return_express);
                    pause_program();
                    clear_input_buffer();
                    continue;
                }

                // 运输方式选择
                printf("\n请选择运输方式：\n");
                printf("0. 普通快递\n");
                printf("1. 大件邮寄\n");
                printf("2. 冷链邮寄\n");
                int method_choice = input_int_in_range("请输入选项(0-2): ", 0, 2);
                return_express->method = (ShippingMethod)method_choice;

                // 地区选择
                printf("\n请选择地区类型：\n");
                printf("0. 省内\n");
                printf("1. 省外\n");
                printf("2. 国际\n");
                printf("3. 港澳台\n");
                int region_choice = input_int_in_range("请输入选项(0-3): ", 0, 3);
                return_express->region = (RegionType)region_choice;

                // 自动继承原包裹类型，不重新输入
                return_express->package_type = target_node->data.package_type;
                printf("\n包裹类型已自动设置为: %s\n", get_package_type_name(return_express->package_type));

                // 保价信息
                clear_input_buffer();
                return_express->insured_value = input_positive_double("物品价值：");
                return_express->is_insured = input_int_in_range("是否保价（0-否 1-是）：", 0, 1);

                // 运输时效
                printf("\n请选择运输时效：\n");
                printf("0. 标快（普通时效）\n");
                printf("1. 特快（加急）\n");
                int express_choice = input_int_in_range("请输入选项(0-1): ", 0, 1);
                return_express->express_type = (DeliveryType)express_choice;

                // 8. 设置其他信息
                return_express->status = STATUS_UNSENT;//待寄出状态
                return_express->create_time = time(NULL);
                return_express->send_num = count_nodes(send_list) + 1;
                strcpy(return_express->reject_reason, "退回原寄件人-拒收");

                // 计算运费
                return_express->fee = 0.00;//拒收不收运费

                // 确认信息
                printf("\n=== 退回快递确认 ===\n");
                printf("新寄件人: %s (%s)\n", return_express->sender_name, return_express->sender_phone);
                printf("新收件人: %s (%s)\n", return_express->receiver_name, return_express->receiver_phone);
                printf("包裹类型: %s\n", get_package_type_name(return_express->package_type));
                printf("运输方式: ");
                switch (return_express->method) {
                case SHIPPING_NORMAL:    printf("普通快递\n"); break;
                case SHIPPING_BULK:      printf("大件邮寄\n"); break;
                case SHIPPING_COLD_CHAIN:printf("冷链邮寄\n"); break;
                default:                 printf("未知运输方式\n"); break;
                }
                printf("重量: %.2f千克 | 体积: %.2f立方厘米\n", return_express->weight, return_express->volume);
                printf("运费: %.2f元\n", return_express->fee);
                printf("确认创建退回快递？(y): ");

                char confirm;
                scanf_s(" %c", &confirm, 1);
                clear_input_buffer();

                if (tolower(confirm) == 'y') {
                    // 添加到寄件列表
                    ExpressNode* new_node = create_express_node(return_express);
                    if (new_node) {
                        new_node->next = send_list;
                        send_list = new_node;
                        target_node->data.status = STATUS_RETURNED;//拒收已处理

                        printf("\n已成功创建退回快递！\n");

                        save_send_express_info(send_list, "send_express.txt");
                        save_pickup_express_info(pickup_list, "pickup_express.txt");
                    }
                }
                else {
                    printf("\n已取消操作\n");
                }
                free(return_express);
                break;
            }
            case 2:
                // 重新联系收件人
                target_node->data.status = STATUS_UNPICKED_AGAIN;
                printf("\n已重新将快递状态重置为待取件\n");
                printf("请电话通知收件人 %s (%s) 已拒绝其拒收申请\n",
                    target_node->data.receiver_name, target_node->data.receiver_phone);
                save_pickup_express_info(pickup_list, "pickup_express.txt");
                break;
            case 3:
                printf("\n已取消处理操作\n");
                break;
            default:
                printf("\n无效的选择！\n");
            }
            break;
        }
        case 2:
            return;
        default:
            printf("\n无效的选择！\n");
        }

        //pause_program();
        clear_input_buffer();

    }
}

// 管理员拒收管理菜单
void admin_reject_management_menu() {
    int choice;

    do {
        clear_screen();
        printf("\n============= 拒收快递管理 =============\n");
        printf("\n请选择操作:\n");
        printf("1. 查看所有拒收记录\n");
        printf("2. 处理待处理拒收快递\n");
        printf("0. 返回上级菜单\n");
        printf("---------------------------------------\n");
        printf("请输入选择: ");

        if (scanf_s("%d", &choice) != 1) {
            clear_input_buffer();
            printf("\n错误：请输入有效的数字选项！\n");
            pause_program();
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
        case 1:
            // 查看所有拒收记录
            admin_view_all_rejected_packages();
            break;

        case 2:
            // 处理拒收快递
            admin_process_rejected_packages();
            break;

        case 0:
            // 返回上级菜单
            return;

        default:
            printf("\n无效的选择，请输入0-2之间的数字！\n");
            pause_program();
            clear_input_buffer();
        }
    } while (true);
}