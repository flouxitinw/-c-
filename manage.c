#include "cainiao.h"
// 计算链表中节点的数量
int count_nodes(ExpressNode* head) {
    int count = 0;
    ExpressNode* current = head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}
// 保存寄快递信息（包含寄件和投诉相关字段）
void save_send_express_info(ExpressNode* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("无法打开寄件数据文件");
        return;
    }

    // 表头（包含所有字段）
    fprintf(fp, "send_num,tracking_num,status,sender_name,sender_phone,receiver_name,receiver_phone,"
        "weight,volume,package_type,method,region,is_insured,insured_value,"
        "express_type,fee,create_time,complaint_id,complaint_type,complaint_time,"
        "resolve_time,complaint_resolution,compensation\n");

    ExpressNode* current = head;
    while (current != NULL) {
        Express* e = &current->data;


        if (e->status == STATUS_LOST || e->status == STATUS_DAMAGED || e->status == STATUS_ABNORMAL_RESOLVED) {
            fprintf(fp, "%d,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d,%d,%d,%.2f,%d,%.2f,%lld,"
                "%d,%d,%lld,%lld,%s,%.2f\n",
                e->send_num,
                e->tracking_num,
                e->status,
                e->sender_name,
                e->sender_phone,
                e->receiver_name,
                e->receiver_phone,
                e->weight,
                e->volume,
                e->package_type,
                e->method,
                e->region,
                e->is_insured,
                e->insured_value,
                e->express_type,
                e->fee,
                (long long)e->create_time,
                e->complaint_id,
                e->complaint_type,
                (long long)e->complaint_time,
                (long long)e->resolve_time,
                e->complaint_resolution,
                e->compensation
            );
        }

        else {
            fprintf(fp, "%d,%s,%d,%s,%s,%s,%s,%.2f,%.2f,%d,%d,%d,%d,%.2f,%d,%.2f,%lld,"
                ",,,,0.0\n",  // 空字段用逗号占位，数值字段写 0
                e->send_num,
                e->tracking_num,
                e->status,
                e->sender_name,
                e->sender_phone,
                e->receiver_name,
                e->receiver_phone,
                e->weight,
                e->volume,
                e->package_type,
                e->method,
                e->region,
                e->is_insured,
                e->insured_value,
                e->express_type,
                e->fee,
                (long long)e->create_time);
        }
        current = current->next;
    }
    fclose(fp);
}
// 加载寄件快递数据（与save_send_express_info严格匹配）
void load_send_express(ExpressNode** express_head) {
    FILE* fp = fopen("send_express.txt", "r");
    if (!fp) {
        printf("无寄件记录或文件未创建\n");
        return;
    }

    char line[512];
    int count = 0;

    // 跳过表头
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return;
    }
    // 使用字段序号确保与保存顺序严格一致
    while (fgets(line, sizeof(line), fp)) {
        Express e = { 0 };
        line[strcspn(line, "\n")] = '\0';

        char* token = strtok(line, ",");
        if (token) e.send_num = atoi(token);

        token = strtok(NULL, ",");
        if (token) {
            strncpy(e.tracking_num, token, sizeof(e.tracking_num) - 1);
        }

        token = strtok(NULL, ",");
        if (token) e.status = (PackageStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) strncpy(e.sender_name, token, sizeof(e.sender_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(e.sender_phone, token, sizeof(e.sender_phone) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(e.receiver_name, token, sizeof(e.receiver_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(e.receiver_phone, token, sizeof(e.receiver_phone) - 1);

        token = strtok(NULL, ",");
        if (token) e.weight = atof(token);

        token = strtok(NULL, ",");
        if (token) e.volume = atof(token);

        token = strtok(NULL, ",");
        if (token) e.package_type = (PackageType)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.method = (ShippingMethod)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.region = (RegionType)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.is_insured = atoi(token);

        token = strtok(NULL, ",");
        if (token) e.insured_value = atof(token);

        token = strtok(NULL, ",");
        if (token) e.express_type = (DeliveryType)atoi(token);

        token = strtok(NULL, ",");
        if (token) e.fee = atof(token);

        token = strtok(NULL, ",");
        if (token) e.create_time = (time_t)atol(token);

        // 投诉相关字段
        if (e.status == STATUS_LOST || e.status == STATUS_DAMAGED || e.status == STATUS_ABNORMAL_RESOLVED) {
            // 只有投诉状态才解析 complaint_* 字段
            token = strtok(NULL, ",");
            if (token) e.complaint_id = atoi(token);

            token = strtok(NULL, ",");
            if (token) e.complaint_type = (ComplaintType)atoi(token);

            token = strtok(NULL, ",");
            if (token) e.complaint_time = (time_t)atoll(token);

            token = strtok(NULL, ",");
            if (token) e.resolve_time = (time_t)atoll(token);

            token = strtok(NULL, ",");
            if (token) strncpy(e.complaint_resolution, token, sizeof(e.complaint_resolution) - 1);

            token = strtok(NULL, ",");
            if (token) e.compensation = atof(token);
        }
        else {
            // 非投诉状态，跳过这些字段（避免读取错误）
            for (int i = 0; i < 6; i++) {
                strtok(NULL, ",");  // 跳过 6 个 complaint_* 字段
            }
        }

        // 创建节点并插入链表头部
        ExpressNode* new_node = create_express_node(&e);
        if (new_node) {
            new_node->next = *express_head;
            *express_head = new_node;
            count++;
        }
    }

    fclose(fp);
    printf("成功加载 %d 条寄件记录\n", count);
}

//--------------------待入库包裹管理--------------------
// 待入库文件格式：
void load_pickup_express(ExpressNode** express_head) {
    FILE* fp = fopen("pickup_express.txt", "r");
    if (!fp) {
        printf("无待取快递记录或文件未创建\n");
        return;
    }

    char line[512];
    fgets(line, sizeof(line), fp); // 跳过表头

    while (fgets(line, sizeof(line), fp)) {
        Express new_express = { 0 };
        line[strcspn(line, "\n")] = '\0';

        char* token = strtok(line, ",");
        if (token) strncpy(new_express.tracking_num, token, sizeof(new_express.tracking_num) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.sender_name, token, sizeof(new_express.sender_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.sender_phone, token, sizeof(new_express.sender_phone) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.receiver_name, token, sizeof(new_express.receiver_name) - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(new_express.receiver_phone, token, sizeof(new_express.receiver_phone) - 1);

        token = strtok(NULL, ",");
        if (token) new_express.package_type = (PackageType)atoi(token);

        token = strtok(NULL, ",");
        if (token) new_express.method = (ShippingMethod)atoi(token);

        token = strtok(NULL, ",");
        if (token) new_express.status = (PackageStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) new_express.create_time = (time_t)atol(token);

        // 处理特殊默认值
        token = strtok(NULL, ",");
        if (token && strcmp(token, "-1") != 0)
            strncpy(new_express.pickup_code, token, sizeof(new_express.pickup_code) - 1);
        else
            memset(new_express.pickup_code, 0, sizeof(new_express.pickup_code));

        token = strtok(NULL, ",");
        if (token && strcmp(token, "-1") != 0)
            strncpy(new_express.shelf_id, token, sizeof(new_express.shelf_id) - 1);
        else
            memset(new_express.shelf_id, 0, sizeof(new_express.shelf_id));

        token = strtok(NULL, ",");
        if (token) new_express.storage_time = (time_t)atol(token);


        token = strtok(NULL, ",");
        if (token && strcmp(token, "-1") != 0)
            strncpy(new_express.pickup_phone, token, sizeof(new_express.pickup_phone) - 1);
        else
            strcpy(new_express.pickup_phone, "0");  // 默认值为"0"

        // 处理拒收相关字段
        if (new_express.status == STATUS_REJECTED || new_express.status == STATUS_RETURNED || new_express.status == STATUS_UNPICKED_AGAIN) {
            token = strtok(NULL, ",");
            if (token && strcmp(token, "") != 0)
                strncpy(new_express.reject_reason, token, sizeof(new_express.reject_reason) - 1);
            else
                memset(new_express.reject_reason, 0, sizeof(new_express.reject_reason));

            token = strtok(NULL, ",");
            if (token && strcmp(token, "") != 0)
                new_express.reject_time = (time_t)atoll(token);
            else
                new_express.reject_time = 0;
        }
        else {
            // 非拒收/退回状态，跳过这两个字段
            strtok(NULL, ","); // 跳过reject_reason
            strtok(NULL, ","); // 跳过reject_time
        }


        // 添加到链表
        ExpressNode* new_node = create_express_node(&new_express);
        if (new_node) {
            new_node->next = *express_head;
            *express_head = new_node;
        }
    }
    fclose(fp);
}
void save_pickup_express_info(ExpressNode* head, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("无法保存快递数据");
        return;
    }

    // 写入表头
    fprintf(fp, "tracking_num,sender_name,sender_phone,receiver_name,receiver_phone,package_type,method,status,create_time,pickup_code,shelf_id,storage_time,pickup_phone,reject_reason,reject_time\n");

    ExpressNode* current = head;
    while (current) {
        if (current->data.status == STATUS_REJECTED || current->data.status == STATUS_RETURNED || current->data.status == STATUS_UNPICKED_AGAIN) {

            fprintf(fp, "%s,%s,%s,%s,%s,%d,%d,%d,%lld,%s,%s,%lld,%s,%s,%lld\n",
                current->data.tracking_num,
                current->data.sender_name,
                current->data.sender_phone,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.package_type,
                current->data.method,
                current->data.status,
                current->data.create_time,
                current->data.pickup_code,
                current->data.shelf_id,
                current->data.storage_time,
                current->data.pickup_phone,
                current->data.reject_reason,
                current->data.reject_time);
        }
        else {
            // 其他状态，拒收相关字段留空
            fprintf(fp, "%s,%s,%s,%s,%s,%d,%d,%d,%lld,%s,%s,%lld,%s,,\n",
                current->data.tracking_num,
                current->data.sender_name,
                current->data.sender_phone,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.package_type,
                current->data.method,
                current->data.status,
                (long long)current->data.create_time,
                current->data.pickup_code,
                current->data.shelf_id,
                (long long)current->data.storage_time,
                current->data.pickup_phone);
        }

        current = current->next;
    }

    fclose(fp);
}


//--------------------待入库包裹管理--------------------

// 获取运输方式名称
const char* get_shipping_method_name(ShippingMethod method) {
    static const char* names[] = {
        "普通快递",
        "大件邮寄",
        "冷链邮寄"
    };
    return (method >= SHIPPING_NORMAL && method <= SHIPPING_COLD_CHAIN)
        ? names[method]
        : "未知运输方式";
}
void manage_pending_express() {
    while (1) {
        clear_screen();
        printf("\n========== 待入库包裹管理 ==========\n");

        // 显示待入库列表
        int count = 0;
        ExpressNode* current = pickup_list;
        while (current) {
            if (current->data.status == STATUS_PENDING_IN) {
                printf("%2d. %s | %s -> %s | %10s | 运输方式: %s\n",
                    ++count,
                    current->data.tracking_num,
                    current->data.sender_name,
                    current->data.receiver_name,
                    get_package_type_name(current->data.package_type),
                    get_shipping_method_name(current->data.method));
            }
            current = current->next;
        }

        if (count == 0) {
            printf("\n当前没有待入库的包裹\n");
            pause_program();
            return;
        }

        printf("\n操作选项:\n");
        printf("1. 全部入库\n");
        printf("2. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // 全部入库
            int success_count = 0;
            current = pickup_list;
            time_t now = time(NULL);
            time_t start_time = now - (5 * 24 * 3600); // 4天前的时间戳

            while (current) {
                if (current->data.status == STATUS_PENDING_IN) {
                    // 定义四天的时间段
                    time_t day_span = 24 * 3600; // 一天的秒数
                    time_t four_days_span = 5 * day_span;

                    // 随机选择哪一天（0-4）
                    int random_day = rand() % 5;

                    // 在选定的天内随机选择时间
                    time_t random_time_in_day = rand() % day_span;

                    // 计算最终的随机时间
                    time_t random_time = start_time + (random_day * day_span) + random_time_in_day;

                    // 确保不超过now
                    if (random_time > now) {
                        random_time = now;
                    }
                    // 分配货架
                    char shelf_prefix = (current->data.package_type == FRAGILE) ? 'F'
                        : get_shelf_prefix_by_shipping(current->data.method);
                    if (assign_to_shelf(&current->data, shelf_list, shelf_prefix, random_time)) {
                        success_count++;
                    }
                }
                current = current->next;
            }
            printf("\n成功入库 %d 个包裹\n", success_count);
            break;
        }
        case 2:
            // 返回
            return;
        default:
            printf("\n无效的选择！\n");
        }

        // 保存更改
        save_pickup_express_info(pickup_list, "pickup_express.txt");
        save_shelves_to_file(shelf_list, "shelves.txt");

        pause_program();
    }
}
//--------------------未取包裹管理--------------------
//// 显示所有未取快递（包括滞留件）
void display_all_unpicked_express() {
    clear_screen();
    printf("\n========== 所有未取快递列表 ==========\n");

    int count = 0;
    ExpressNode* current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_UNPICKED ||
            current->data.status == STATUS_OVERDUE || current->data.status == STATUS_UNPICKED_AGAIN) {

            char time_str[20];
            strftime(time_str, sizeof(time_str), "%m-%d %H:%M",
                localtime(&current->data.storage_time));

            printf("%2d. %s | %s (%s) | 货架: %s | %10s | 入库: %s",
                ++count,
                current->data.tracking_num,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.shelf_id,
                get_package_type_name(current->data.package_type),
                time_str);

            // 显示滞留天数
            if (current->data.status == STATUS_OVERDUE) {
                double days = difftime(time(NULL), current->data.storage_time) / 86400;
                printf(" | 已滞留 %.1f 天", days);
            }
            printf("\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n当前没有未取的包裹\n");
    }

    pause_program();
}
void display_all_STATUS_OVERDUE_express() {
    clear_screen();
    printf("\n========== 所有滞留快递列表 ==========\n");

    int count = 0;
    ExpressNode* current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_OVERDUE) {

            char time_str[20];
            strftime(time_str, sizeof(time_str), "%m-%d %H:%M",
                localtime(&current->data.storage_time));

            printf("%3d. %s | %s (%s) | 货架: %s | %12s | 入库: %s",
                ++count,
                current->data.tracking_num,
                current->data.receiver_name,
                current->data.receiver_phone,
                current->data.shelf_id,
                get_package_type_name(current->data.package_type),
                time_str);
            double days = difftime(time(NULL), current->data.storage_time) / 86400;
            printf(" | 已滞留 %.1f 天", days);

            printf("\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n当前没有滞留的包裹\n");
    }
    pause_program();
}
// 按收件人信息查找快递
void search_express_by_receiver_info() {
    clear_screen();
    printf("\n========== 按收件人信息查找快递 ==========\n");

    // 选择查询方式
    printf("\n请选择查询方式：\n");
    printf("1. 按收件人手机号查询\n");
    printf("2. 按收件人姓名查询\n");
    printf("3. 按快递单号查询\n");
    printf("0. 返回\n");

    int choice = input_int_in_range("请输入选项(0-3): ", 0, 3);
    if (choice == 0) return;

    char search_phone[12] = { 0 };
    char search_name[50] = { 0 };
    char tracking_num[20] = { 0 };

    // 获取查询条件
    switch (choice) {
    case 1:
        clear_input_buffer();
        input_phone(search_phone, sizeof(search_phone), "请输入收件人手机号: ");
        break;
    case 2:
        safe_input(search_name, sizeof(search_name), "请输入收件人姓名(支持模糊查询): ");
        break;
    case 3:
        printf("请输入快递单号: ");
        scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
        clear_input_buffer();
        break;
    }

    // 查找匹配的快递
    ExpressNode* current = pickup_list;
    int found_count = 0;

    printf("\n=== 查询结果 ===\n");
    while (current != NULL) {
        int match = 0;

        // 根据选择的条件检查匹配
        switch (choice) {
        case 1: // 手机号
            match = strcmp(current->data.receiver_phone, search_phone) == 0;
            break;
        case 2: // 姓名
            match = strstr(current->data.receiver_name, search_name) != NULL;
            break;
        case 3: // 快递单号
            match = strcmp(current->data.tracking_num, tracking_num) == 0;
            break;
        }

        if (match) {
            found_count++;
            printf("\n%d. 包裹信息:\n", found_count);
            printf("快递单号: %s\n", current->data.tracking_num);
            printf("收件人: %s (%s)\n", current->data.receiver_name, current->data.receiver_phone);
            printf("寄件人: %s (%s)\n", current->data.sender_name, current->data.sender_phone);
            printf("货架位置: %s | 取件码: %s\n", current->data.shelf_id, current->data.pickup_code);
            printf("包裹类型: %s\n", get_package_type_name(current->data.package_type));

            char time_str[20];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S",
                localtime(&current->data.storage_time));
            printf("入库时间: %s\n", time_str);

            // 显示状态信息
            if (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) {
                double days = difftime(time(NULL), current->data.storage_time) / 86400;
                if (days > 3) {
                    printf("状态: 未取 (已存放 %.1f 天)\n", days);
                }
                else {
                    printf("状态: 未取\n");
                }
            }
            else if (current->data.status == STATUS_OVERDUE) {
                double days = difftime(time(NULL), current->data.storage_time) / 86400;
                printf("状态: 已滞留 (%.1f 天)\n", days);
            }
            else {
                printf("状态: %s\n", get_status_text(current->data.status));
            }
            printf("--------------------------------\n");
        }
        current = current->next;
    }

    if (found_count == 0) {
        printf("\n未找到匹配的快递记录\n");
    }
    else {
        printf("\n共找到 %d 条匹配记录\n", found_count);
    }

    pause_program();
}
// 辅助函数：检查并自动标记滞留包裹
void check_and_mark_overdue_express() {
    time_t now = time(NULL);
    ExpressNode* current = pickup_list;
    int marked_count = 0;

    while (current != NULL) {
        // 超过3天未取的包裹自动标记为滞留
        if ((current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) &&
            difftime(now, current->data.storage_time) > 3 * 86400) {
            current->data.status = STATUS_OVERDUE;
            marked_count++;
        }
        current = current->next;
    }

    if (marked_count > 0) {
        save_pickup_express_info(pickup_list, "pickup_express.txt");
    }
}
// 检查滞留包裹并显示新增的滞留包裹
void check_overdue_express_with_display() {
    clear_screen();
    printf("\n========== 检查滞留包裹 ==========\n");

    // 获取当前未取和滞留包裹数量
    int before_unpicked = 0, before_overdue = 0;
    ExpressNode* current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) before_unpicked++;
        if (current->data.status == STATUS_OVERDUE) before_overdue++;
        current = current->next;
    }

    // 执行检查
    check_and_mark_overdue_express();

    // 获取检查后的数量
    int after_unpicked = 0, after_overdue = 0;
    current = pickup_list;
    while (current != NULL) {
        if (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_UNPICKED_AGAIN) after_unpicked++;
        if (current->data.status == STATUS_OVERDUE) after_overdue++;
        current = current->next;
    }

    // 显示结果
    printf("\n检查结果:\n");
    printf("原未取包裹数量: %d\n", before_unpicked);
    printf("原滞留包裹数量: %d\n", before_overdue);
    printf("新标记为滞留的包裹数量: %d\n", after_overdue - before_overdue);
    printf("当前未取包裹数量: %d\n", after_unpicked);
    printf("当前滞留包裹数量: %d\n", after_overdue);
    printf("\n操作选项:\n");
    printf("1. 查看滞留包裹详情\n");
    printf("2. 返回上级菜单\n");
    printf("请选择: ");
    int choice;
    scanf_s("%d", &choice);
    clear_input_buffer();
    if (choice == 1) {
        display_all_STATUS_OVERDUE_express();  // 显示详情
    }
    pause_program();
}
// 未取包裹管理主菜单
void manage_unpicked_express() {
    while (1) {
        clear_screen();
        printf("\n========== 未取包裹管理 ==========\n");

        // 操作菜单
        printf("1. 显示所有未取快递\n");
        printf("2. 查找未取快递\n");
        printf("3. 检查滞留包裹\n");
        printf("4. 返回上级菜单\n");
        printf("请选择操作: ");
        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        // 执行操作
        switch (choice) {
        case 1:
            display_all_unpicked_express();
            break;
        case 2:
            search_express_by_receiver_info();
            break;
        case 3:
            check_overdue_express_with_display();
            break;
        case 4:
            return;
        default:
            printf("\n无效的操作选择！\n");
            pause_program();
        }
    }
}


//--------------------已取包裹管理--------------------
// 已取包裹管理
void manage_picked_express() {
    while (1) {
        clear_screen();
        printf("\n========== 已取包裹管理 ==========\n");

        // 显示已取包裹列表（按取件时间倒序）
        int count = 0;
        ExpressNode* current = pickup_list;
        while (current != NULL) {
            if (current->data.status == STATUS_PICKED) {
                char time_str[20];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                    localtime(&current->data.create_time));

                printf("%d. %s | %s (%s) | 货架: %s | 取件人手机号: %s  | 取件时间: %s \n",
                    ++count,
                    current->data.tracking_num,
                    current->data.receiver_name,
                    current->data.receiver_phone,
                    current->data.shelf_id,
                    current->data.pickup_phone,
                    time_str);
            }
            current = current->next;
        }

        if (count == 0) {
            printf("\n当前没有已取包裹记录\n");
            pause_program();
            return;
        }

        // 操作菜单
        printf("\n操作选项:\n");
        printf("1. 删除所有已取包裹记录\n");
        printf("2. 选择删除\n");
        printf("3. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // 删除所有已取包裹记录
            char confirm;
            printf("\n警告：这将永久删除所有已取包裹记录！\n");
            printf("确认删除吗？(y/n): ");
            scanf_s(" %c", &confirm, 1);
            clear_input_buffer();

            if (tolower(confirm) == 'y') {
                int deleted_count = 0;
                ExpressNode** ptr = &pickup_list;

                while (*ptr != NULL) {
                    if ((*ptr)->data.status == STATUS_PICKED) {
                        ExpressNode* to_delete = *ptr;
                        *ptr = (*ptr)->next;
                        free(to_delete);
                        deleted_count++;
                    }
                    else {
                        ptr = &(*ptr)->next;
                    }
                }

                printf("\n已删除 %d 条已取包裹记录\n", deleted_count);
                save_pickup_express_info(pickup_list, "pickup_express.txt");
            }
            else {
                printf("\n取消删除操作\n");
            }
            break;
        }

        case 2: {
            // 选择删除
            char tracking_num[20] = { 0 };
            printf("\n请输入要删除的快递单号: ");
            scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
            clear_input_buffer();

            ExpressNode** ptr = &pickup_list;
            int found = 0;

            while (*ptr != NULL) {
                if (strcmp((*ptr)->data.tracking_num, tracking_num) == 0 &&
                    (*ptr)->data.status == STATUS_PICKED) {

                    ExpressNode* to_delete = *ptr;
                    *ptr = (*ptr)->next;
                    free(to_delete);
                    found = 1;

                    printf("\n快递 %s 记录已删除\n", tracking_num);
                    save_pickup_express_info(pickup_list, "pickup_express.txt");
                    break;
                }
                ptr = &(*ptr)->next;
            }

            if (!found) {
                printf("\n未找到该快递或快递状态不符！\n");
            }
            break;
        }

        case 3:
            return; // 返回上级菜单

        default:
            printf("\n无效的选择！\n");
        }

        pause_program();
    }
}
//--------------------待寄出包裹管理--------------------
// 待寄出包裹管理
// 生成不重复的快递单号（格式：年月日+4位随机数，如202308250001）
void generate_tracking_num(Express* express) {
    // 只有未生成单号的快递才需要生成
    if (strcmp(express->tracking_num, "0") == 0) {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char buffer[20];

        strftime(buffer, 9, "%Y%m%d", tm_info);
        int random_num = rand() % 10000;
        sprintf(buffer + 8, "%04d", random_num);
        buffer[12] = '\0';

        // 检查是否重复
        while (find_express_by_tracking(send_list, buffer)) {
            random_num = (random_num + 1) % 10000;
            sprintf(buffer + 8, "%04d", random_num);
        }

        strcpy(express->tracking_num, buffer);
    }
}
void manage_unsent_express() {
    clear_screen();
    printf("\n=== 待寄出包裹管理 ===\n");

    // 统计数据
    int total_count = 0;
    int invalid_count = 0;
    ExpressNode* current = send_list;

    // 第一次遍历：统计总数和无效包裹
    while (current != NULL) {
        if (current->data.status == STATUS_UNSENT) {
            total_count++;
            if (current->data.package_type == ALCOHOL || current->data.package_type == ANIMAL || current->data.package_type == WEAPON ||
                current->data.package_type == HAZARDOUS || current->data.weight > 50.0 || current->data.volume > 3000000.0) {
                invalid_count++;
            }
        }
        current = current->next;
    }

    if (total_count == 0) {
        printf("\n当前没有待寄出的包裹\n");
        pause_program();
        return;
    }

    // 显示警告（如果有不可邮寄的）
    if (invalid_count > 0) {
        printf("\n发现 %d 件不可邮寄包裹（禁运/超重/超大）\n", invalid_count);
    }

    // 显示表格
    printf("\n待寄出包裹列表（共%d件）：\n", total_count);
    printf("┌────┬────────────────┬──────────────┬────────────────┬──────────────┬──────────────┐\n");
    printf("│%-4s│ %-14s │ %-12s │ %-14s │ %-12s │ %-12s │\n",
        "序号", "寄件人", "寄件电话", "收件人", "收件电话", "状态");
    printf("├────┼────────────────┼──────────────┼────────────────┼──────────────┼──────────────┤\n");

    // 第二次遍历：显示数据
    current = send_list;
    int index = 0;
    while (current != NULL) {
        if (current->data.status == STATUS_UNSENT) {
            Express* e = &current->data;
            const char* status = "可邮寄";

            if (e->package_type == ALCOHOL || e->package_type == ANIMAL || e->package_type == WEAPON ||
                e->package_type == HAZARDOUS) {
                status = "不可邮寄（禁运）";
            }
            else if (e->weight > 50.0) {
                status = "不可邮寄（超重）";
            }
            else if (e->volume > 3000000.0) {
                status = "不可邮寄（超大）";
            }

            printf("│%-4d│ %-14s │ %-12s │ %-14s │ %-12s │ %-12s │\n",
                ++index,
                e->sender_name,
                e->sender_phone,
                e->receiver_name,
                e->receiver_phone,
                status);
        }
        current = current->next;
    }
    printf("└────┴────────────────┴──────────────┴────────────────┴──────────────┴──────────────┘\n");

    // 操作菜单
    printf("\n操作选项:\n");
    printf("1. 批量寄出所有可邮寄包裹\n");
    printf("2. 返回上级菜单\n");

    int choice = input_int_in_range("请选择操作(1-2): ", 1, 2);
    clear_input_buffer();

    if (choice == 1) {
        // 批量寄出（仅处理可邮寄的）
        int success_count = 0;
        current = send_list;
        while (current != NULL) {
            if (current->data.status == STATUS_UNSENT) {
                Express* e = &current->data;

                // 跳过不可邮寄的
                if (e->package_type != ALCOHOL && e->package_type != ANIMAL && e->package_type != WEAPON &&
                    e->package_type != HAZARDOUS &&
                    e->weight <= 50.0 &&
                    e->volume <= 3000000.0) {
                    e->status = STATUS_SENT;
                    if (strcmp(e->tracking_num, "0") == 0) {
                        generate_tracking_num(e);
                    }
                    e->create_time = time(NULL);
                    success_count++;
                }
            }
            current = current->next;
        }

        save_send_express_info(send_list, SEND_FILE);
        printf("\n操作结果：\n");
        printf("  成功寄出: %d 件\n", success_count);
        printf("  跳过不可邮寄: %d 件\n", invalid_count);

        if (invalid_count > 0) {
            printf("\n注意：请人工联系寄件人处理不可邮寄包裹！\n");
        }
    }

    pause_program();
}
// 管理员处理误领快递
void manage_mispicked_express() {
    while (1) {
        clear_screen();
        printf("\n========== 误取包裹管理 ==========\n");

        // 显示误取包裹列表
        int count = 0;
        ExpressNode* current = pickup_list;
        while (current) {
            if (current->data.status == STATUS_MISPICKED) {
                printf("%2d. %s | %s -> %s | %8s | 运输方式: %s\n",
                    ++count,
                    current->data.tracking_num,
                    current->data.sender_name,
                    current->data.receiver_name,
                    get_package_type_name(current->data.package_type),
                    get_shipping_method_name(current->data.method));
            }
            current = current->next;
        }

        if (count == 0) {
            printf("\n当前没有误取的包裹\n");
            pause_program();
            return;
        }

        printf("\n操作选项:\n");
        printf("1. 一键处理\n");
        printf("2. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            // 全部重新入库
            int success_count = 0;
            current = pickup_list;
            time_t now = time(NULL);
            int day_offset = 4; // 从4天前开始

            while (current) {
                if (current->data.status == STATUS_MISPICKED) {
                    // 根据包裹类型和运输方式确定货架前缀
                    char shelf_prefix;
                    if (current->data.package_type == FRAGILE) {
                        shelf_prefix = 'F'; // 易碎品专用货架
                    }
                    else {
                        shelf_prefix = get_shelf_prefix_by_shipping(current->data.method);
                    }

                    // 调用独立的分配函数
                    if (assign_to_shelf(&current->data, shelf_list, shelf_prefix,
                        current->data.storage_time)) {
                        success_count++;
                        current->data.status = STATUS_UNPICKED; // 更新状态为待取件
                    }
                }
                current = current->next;
            }
            printf("\n成功处理 %d 个误取包裹\n", success_count);
            break;
        }
        case 2:
            // 返回
            return;
        default:
            printf("\n无效的选择！\n");
        }

        // 保存更改
        save_pickup_express_info(pickup_list, "pickup_express.txt");
        save_shelves_to_file(shelf_list, "shelves.txt");

        pause_program();
    }
}
// 快递管理主函数
void manage_express(Admin* admin) {
    while (1) {
        clear_screen();
        printf("\n========== 快递管理 ==========\n");
        printf("1. 待入库包裹\n");
        printf("2. 待寄出包裹\n");
        printf("3. 未取包裹\n");
        printf("4. 已取包裹\n");
        printf("5. 误取包裹\n");
        printf("6. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice;
        scanf_s("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: manage_pending_express(); break;
        case 2: manage_unsent_express(); break;
        case 3: manage_unpicked_express(); break;
        case 4: manage_picked_express(); break;
        case 5: manage_mispicked_express(); break;
        case 6: return;
        default:
            printf("无效的选择！\n");
            pause_program();
        }
    }
}
