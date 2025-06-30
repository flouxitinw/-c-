#include "cainiao.h"

//--------------------用户投诉模块--------------------
//寄件人发起投诉，因为赔偿是给寄件人的
// 只有有寄出快递才能提交投诉
// 丢失或者损坏

// 生成唯一的投诉ID
int generate_complaint_id(ExpressNode* head) {
    int max_id = 0;// 初始化ID为0
    ExpressNode* current = head;

    while (current != NULL) {
        if (current->data.complaint_id > max_id) {// 在已有的投诉id找到更大的ID
            max_id = current->data.complaint_id;// 更新最大值
        }
        current = current->next;
    }

    return max_id + 1; // 返回最大ID+1作为新ID
}

// ========== 提交投诉 ==========
/*显示可投诉快递、选择快递、选择投诉类型、记录投诉信息和保存数据*/
void submit_complaint(User* user) {
    clear_screen();
    printf("\n========== 提交投诉 ==========\n");

    // 显示用户已寄出的快递//显示用户可投诉的快递列表
    printf("您的已寄出快递列表:\n");
    int count = 0;
    ExpressNode* current = send_list;
    while (current != NULL) {
        // 检查是否属于当前用户且状态为"已寄出"
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_SENT) {

            printf("%d. 快递单号: %s | 收件人: %s (%s) | 寄出时间: %s",
                ++count, current->data.tracking_num,
                current->data.receiver_name, current->data.receiver_phone,
                ctime(&current->data.create_time));
        }
        current = current->next;
    }

    if (count == 0) {
        printf("您没有已寄出的快递可以投诉。\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 选择要投诉的快递
    char tracking_num[20];
    printf("\n请输入要投诉的快递单号: ");
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    // 查找快递
    Express* express = NULL;
    current = send_list;
    while (current != NULL) {
        // 检查单号和手机号是否匹配
        if (strcmp(current->data.tracking_num, tracking_num) == 0 &&
            strcmp(current->data.sender_phone, user->phone) == 0) {
            express = &current->data;
            break;
        }
        current = current->next;
    }

    if (express == NULL) {
        printf("未找到匹配的快递记录！\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    // 选择投诉类型
    printf("\n请选择投诉类型:\n");
    printf("1. 包裹丢失\n");
    printf("2. 包裹损坏\n");
    int type_choice = input_int_in_range("请输入选项(1-2): ", 1, 2);

    // 设置投诉信息
    express->complaint_id = generate_complaint_id(send_list);// 生成投诉ID
    express->complaint_type = (type_choice == 1) ? COMPLAINT_LOST : COMPLAINT_DAMAGED;
    express->complaint_time = time(NULL);// 设置投诉时间为当前时间
    express->status = (type_choice == 1) ? STATUS_LOST : STATUS_DAMAGED;

    // 保存更改
    save_send_express_info(send_list, SEND_FILE);

    printf("\n投诉提交成功！投诉ID: %d\n", express->complaint_id);
    pause_program();
    clear_input_buffer();
}

// 显示投诉详情
void display_complaint(Express* express) {
    printf("\n============== 投诉详情 ==============\n");
    printf("投诉ID: %d\n", express->complaint_id);
    printf("快递单号: %s\n", express->tracking_num);
    printf("投诉类型: %s\n",
        express->complaint_type == COMPLAINT_LOST ? "包裹丢失" : "包裹损坏");
    printf("投诉时间: %s", ctime(&express->complaint_time));

    if (express->status == STATUS_ABNORMAL_RESOLVED) {//异常已处理
        printf("\n[处理结果]\n");
        printf("处理时间: %s", ctime(&express->resolve_time));
        printf("处理方案: %s\n", express->complaint_resolution);
        printf("赔偿金额: %.2f元\n", express->compensation);
    }
    else {
        printf("\n状态: 待处理\n");
    }
    printf("=======================================\n");
}

// ========== 查看我的投诉（用户） ==========
void view_user_complaints(User* user) {
    clear_screen();
    printf("\n========== 我的投诉记录 ==========\n");

    int count = 0;
    ExpressNode* current = send_list;

    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            (current->data.status == STATUS_LOST ||
                current->data.status == STATUS_DAMAGED ||
                current->data.status == STATUS_ABNORMAL_RESOLVED)) {
            printf("%d. 投诉ID: %d | 快递单号: %s | 类型: %s | 状态: %s\n",
                ++count, current->data.complaint_id,
                current->data.tracking_num,
                current->data.complaint_type == COMPLAINT_LOST ? "丢失" : "损坏",
                get_status_text(current->data.status));
        }
        current = current->next;
    }

    if (count == 0) {
        printf("您没有提交过任何投诉。\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    printf("\n操作选项:\n");
    printf("1. 查看投诉详情\n");
    printf("2. 返回\n");
    printf("请选择: ");

    int choice = input_int_in_range("", 1, 2);
    if (choice == 1) {
        int complaint_id;
        printf("请输入要查看的投诉ID: ");
        scanf_s("%d", &complaint_id);
        clear_input_buffer();

        // 查找投诉
        current = send_list;
        while (current != NULL) {
            if (current->data.complaint_id == complaint_id &&
                strcmp(current->data.sender_phone, user->phone) == 0) {
                display_complaint(&current->data);// 显示投诉详情
                break;
            }
            current = current->next;
        }

        if (current == NULL) {
            printf("未找到指定的投诉记录！\n");
        }
    }

    pause_program();
    clear_input_buffer();
}




//--------------------管理员投诉处理模块--------------------
// 显示投诉快递简要信息
void display_abnormal_express_brief(Express* express) {
    printf("单号: %s | 收件人: %s(%s) | 类型: %s\n",
        express->tracking_num,
        express->receiver_name,
        express->receiver_phone,
        get_status_text(express->status));
    printf("投诉ID: %d | 投诉类型: %s | 保价金额: ￥%.2f\n",
        express->complaint_id,
        express->complaint_type == COMPLAINT_LOST ? "丢失" : "损坏",
        express->insured_value);
    printf("投诉时间: %s", ctime(&express->complaint_time));
    printf("--------------------------------------------------------------\n");
}


// 显示投诉快递详细信息
void display_abnormal_express_detail(Express* express) {
    printf("\n============= 异常快递详情 ============\n");
    printf("快递单号: %s\n", express->tracking_num);
    printf("收件人: %s | 电话: %s\n", express->receiver_name, express->receiver_phone);
    printf("包裹类型: %s | 保价金额: ￥%.2f\n",
        get_package_type_name(express->package_type),
        express->insured_value);
    printf("投诉ID: %d | 状态: %s\n", express->complaint_id, get_status_text(express->status));
    printf("投诉类型: %s\n", express->complaint_type == COMPLAINT_LOST ? "包裹丢失" : "包裹损坏");
    printf("投诉时间: %s", ctime(&express->complaint_time));

    if (express->status == STATUS_ABNORMAL_RESOLVED) {
        printf("\n[已处理信息]\n");
        printf("处理时间: %s", ctime(&express->resolve_time));
        printf("处理结果: %s\n", express->complaint_resolution);
        printf("赔偿金额: ￥%.2f\n", express->compensation);
    }
    printf("=======================================\n");
}

// 计算赔偿金额
/*丢失按保价金额全额赔偿   损坏按保价金额的50%赔偿*/
double calculate_compensation(const Express* express) {
    if (express->complaint_type == COMPLAINT_LOST) {
        // 丢失按保价金额全额赔偿
        return express->insured_value;
    }
    else {
        // 损坏按保价金额的50%赔偿
        return express->insured_value * 0.5;
    }
}

// 处理单个投诉快递
void process_single_abnormal_express(Express* express) {
    if (express->status != STATUS_LOST && express->status != STATUS_DAMAGED) {
        printf("该快递不需要处理或已处理!\n");
        return;
    }

    double compensation = calculate_compensation(express);
    printf("\n建议赔偿金额: ￥%.2f\n", compensation);

    printf("请输入实际赔偿金额(当前建议:￥%.2f): ", compensation);
    scanf_s("%lf", &compensation);
    clear_input_buffer();

    char resolution[256];
    printf("请输入处理说明: ");
    fgets(resolution, sizeof(resolution), stdin);
    resolution[strcspn(resolution, "\n")] = '\0';

    express->status = STATUS_ABNORMAL_RESOLVED;
    express->compensation = compensation;
    strncpy_s(express->complaint_resolution, sizeof(express->complaint_resolution),
        resolution, _TRUNCATE);
    express->resolve_time = time(NULL);

    printf("\n处理成功! 已赔偿 ￥%.2f\n", compensation);
}

// ========== 按单号查询并处理单个投诉快递 ==========
/*显示未处理投诉详细信息*/
void query_and_process_by_tracking() {
    char tracking_num[20];
    printf("\n请输入要查询的快递单号: ");
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    Express* express = find_express_by_tracking(send_list, tracking_num);
    if (express == NULL) {
        printf("未找到该快递!\n");
        return;
    }

    if (express->status != STATUS_LOST && express->status != STATUS_DAMAGED) {
        printf("该快递不是异常状态!\n");
        return;
    }

    display_abnormal_express_detail(express);

    printf("\n是否要处理此投诉? (1-处理 0-取消): ");
    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();

    if (choice == 1) {
        process_single_abnormal_express(express);
        save_send_express_info(send_list, SEND_FILE);
    }
}

// ========== 批量处理所有投诉快递 ==========
void batch_process_all_abnormal_express() {
    ExpressNode* current = send_list;
    int processed_count = 0; //已处理数量
    double total_compensation = 0.0; //总赔偿金额

    while (current != NULL) {
        if (current->data.status == STATUS_LOST || current->data.status == STATUS_DAMAGED) {
            printf("\n正在处理快递: %s\n", current->data.tracking_num);
            double compensation = calculate_compensation(&current->data);
            printf("自动计算赔偿金额: ￥%.2f\n", compensation);

            current->data.status = STATUS_ABNORMAL_RESOLVED;
            current->data.compensation = compensation;
            snprintf(current->data.complaint_resolution,
                sizeof(current->data.complaint_resolution),
                "系统批量处理，赔偿金额:￥%.2f", compensation);
            current->data.resolve_time = time(NULL);

            processed_count++;
            total_compensation += compensation;

            printf("已处理: %s 赔偿 ￥%.2f\n", current->data.tracking_num, compensation);
        }
        current = current->next;
    }

    if (processed_count > 0) {
        printf("\n批量处理完成! 共处理 %d 个异常快递，总计赔偿 ￥%.2f\n",
            processed_count, total_compensation);
        save_send_express_info(send_list, SEND_FILE);
    }
    else {
        printf("\n当前没有需要处理的异常快递!\n");
    }
}

// 管理员投诉处理主界面
void admin_handle_complaints() {
    while (1) {
        clear_screen();
        printf("\n========== 管理员投诉处理 ==========\n");

        int total_abnormal = 0, total_lost = 0, total_damaged = 0;
        ExpressNode* current = send_list;
        while (current != NULL) {
            if (current->data.status == STATUS_LOST) {
                total_abnormal++;
                total_lost++;
            }
            else if (current->data.status == STATUS_DAMAGED) {

                total_abnormal++;
                total_damaged++;
            }
            current = current->next;
        }

        printf("当前投诉信息总数: %d (丢失:%d 损坏:%d)\n\n",
            total_abnormal, total_lost, total_damaged);

        printf("1. 查看所有投诉信息\n");
        printf("2. 按单号查询处理单个快递\n");
        printf("3. 批量处理所有投诉\n");
        printf("0. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            clear_screen();
            printf("\n======================== 所有投诉列表 ========================\n");
            int count = 0;
            current = send_list;
            while (current != NULL) {
                if (current->data.status == STATUS_LOST ||
                    current->data.status == STATUS_DAMAGED || current->data.status == STATUS_ABNORMAL_RESOLVED) {
                    display_abnormal_express_brief(&current->data);
                    count++;
                }
                current = current->next;
            }
            if (count == 0) {
                printf("当前没有投诉信息!\n");
            }
            pause_program();
            clear_input_buffer();
            break;
        }
        case 2:
            query_and_process_by_tracking();
            pause_program();
            clear_input_buffer();
            break;
        case 3:
            batch_process_all_abnormal_express();
            pause_program();
            clear_input_buffer();
            break;
        case 0:
            return;
        default:
            printf("\n无效的选择!\n");
            pause_program();
            clear_input_buffer();
        }
    }
}

