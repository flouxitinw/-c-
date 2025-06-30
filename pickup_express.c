#include "cainiao.h"

// 显示单个快递信息
void display_express_info(Express* express) {
    char time_buffer[26];
    printf("快递单号：%s\n", express->tracking_num);
    printf("寄件人：%s (%s)\n", express->sender_name, express->sender_phone);
    printf("物品类型：%s\n", get_package_type_name(express->package_type));
    ctime_s(time_buffer, sizeof(time_buffer), &express->storage_time);
    printf("到站时间：%s", time_buffer);
    printf("货架号：%s  取件码：%s\n", express->shelf_id, express->pickup_code);
    printf("--------------------------------\n");
}

// 显示用户待取快递列表
int display_pending_pickups(User* user, ExpressNode* list) {
    ExpressNode* curr = list;
    int count = 0;
    char time_buffer[26];

    printf("\n=== %s的待取快递 ===\n", user->username);
    while (curr != NULL) {
        if (strcmp(curr->data.receiver_phone, user->phone) == 0 &&
            (curr->data.status == STATUS_UNPICKED || curr->data.status == STATUS_OVERDUE || curr->data.status == STATUS_UNPICKED_AGAIN)) {

            printf("%d.\n", ++count);
            display_express_info(&curr->data);
        }
        curr = curr->next;
    }

    if (count == 0) {
        printf("当前没有您的待取快递\n");
    }

    return count;
}

// 验证手机号
bool verify_phone(User* user, Express* express) {
    int attempts = 3;
    char comfirm_phone[12];

    while (attempts > 0) {
        printf("\n请输入收件人手机号进行验证（剩余尝试次数：%d）：", attempts);
        input_phone(comfirm_phone, sizeof(comfirm_phone), "电话：");
        if (strcmp(comfirm_phone, express->receiver_phone) == 0) {
            return true;
        }

        attempts--;
        if (attempts > 0) {
            printf("手机号不匹配，请重试！\n");
        }
    }

    return false;
}

// 完成取件操作
void complete_pickup(User* user, Express* express) {
    express->status = STATUS_PICKED;
    express->create_time = time(NULL);
    strncpy(express->pickup_phone, user->phone, sizeof(express->pickup_phone) - 1);
    if (remove_express_from_shelf(&shelf_list, express->shelf_id, express->tracking_num)) {
        char time_buffer[26];
        printf("\n=== 取件成功 ===\n");
        printf("快递单号：%s\n", express->tracking_num);
        ctime_s(time_buffer, sizeof(time_buffer), &express->create_time);
        printf("签收时间：%s", time_buffer);

        if (express->package_type == VALUABLES && express->is_insured) {
            printf("请查验贵重物品是否完好，并在签收单上签字确认！\n");
        }
        save_pickup_express_info(pickup_list, PICKUP_FILE);
        save_shelves_to_file(shelf_list, "shelves.txt");
    }
    else {
        printf("取件失败");
        return;
    }
}

// 处理取件流程
void process_pickup(User* user) {
    char tracking_num[20] = { 0 };
    printf("\n请输入要取的快递单号（输入0取消）：");
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();

    if (strcmp(tracking_num, "0") == 0) {
        printf("已取消取件操作\n");
        return;
    }

    ExpressNode* express = find_express_by_tracking(pickup_list, tracking_num);
    if (!express) {
        printf("未找到匹配的快递记录\n");
        return;
    }
    printf("\n快递单号：%s 的取件码是：%s\n", express->data.tracking_num, express->data.pickup_code);

    printf("确认要取走这个快递吗？(y/n): ");
    char confirm;
    scanf_s(" %c", &confirm, 1);
    clear_input_buffer();

    if (tolower(confirm) != 'y') {
        printf("已取消取件操作\n");
        return;
    }

    if (!verify_phone(user, express)) {
        printf("\n验证失败！已达到最大尝试次数。\n");
        return;
    }

    complete_pickup(user, express);
}

// 查找待取快递
void search_pickup_express(User* user) {
    clear_screen();
    printf("\n========== 待取快递查询 ==========\n");

    printf("\n请选择查询方式：\n");
    printf("1. 按寄件人姓名查询\n");
    printf("2. 按寄件人手机号查询\n");
    printf("3. 按物品类型查询\n");
    printf("0. 返回\n");

    int choice = input_int_in_range("请输入选项(0-3): ", 0, 3);
    if (choice == 0) return;

    char search_name[50] = { 0 };
    char search_phone[12] = { 0 };
    int search_type = -1;

    switch (choice) {
    case 1:
        safe_input(search_name, sizeof(search_name), "请输入寄件人姓名关键词: ");
        break;
    case 2:
        clear_input_buffer();
        input_phone(search_phone, sizeof(search_phone), "请输入寄件人完整手机号: ");
        break;
    case 3:
        printf("\n请选择物品类型：\n");
        for (int i = 0; i < 13; i++) {
            printf("%d. %s\n", i, get_package_type_name((PackageType)i));
        }
        search_type = input_int_in_range("请输入物品类型编号(0-12): ", 0, 12);
        break;
    }

    ExpressNode* current = pickup_list;
    int total_matches = 0;

    printf("\n=== 查询结果 ===\n");
    while (current != NULL) {
        if (strcmp(current->data.receiver_phone, user->phone) == 0 &&
            (current->data.status == STATUS_UNPICKED || current->data.status == STATUS_OVERDUE || current->data.status == STATUS_UNPICKED_AGAIN)) {

            int match = 0;
            switch (choice) {
            case 1:
                match = strstr(current->data.sender_name, search_name) != NULL;
                break;
            case 2:
                match = strcmp(current->data.sender_phone, search_phone) == 0;
                break;
            case 3:
                match = current->data.package_type == search_type;
                break;
            }

            if (match) {
                total_matches++;
                printf("\n%d.\n", total_matches);
                display_express_info(&current->data);
            }
        }
        current = current->next;
    }

    if (total_matches == 0) {
        printf("\n未找到符合条件的快递。\n");
    }
    else {
        printf("\n共找到 %d 条待取快递记录。\n", total_matches);
        process_pickup(user);
    }
}

// 主取件函数
void pickup_express(User* user) {
    if (!user) {
        printf("系统错误：参数无效\n");
        return;
    }

    while (1) {
        clear_screen();
        printf("\n=== 取件服务 ===\n");
        printf("1. 查看所有待取快递并取件\n");
        printf("2. 查找待取快递并取件\n");
        printf("3. 误领报备\n");
        printf("4. 返回上级菜单\n");
        printf("请选择操作: ");

        int choice = input_int_in_range("", 1, 4);
        clear_input_buffer();

        switch (choice) {
        case 1:
            if (display_pending_pickups(user, pickup_list) > 0) {
                process_pickup(user);
            }
            pause_program();
            break;
        case 2:
            search_pickup_express(user);
            pause_program();
            break;
        case 3:
            mistaken_pickup(user);
            break;
        case 4:
            return;
        default:
            printf("无效的选择！\n");
            pause_program();
        }
    }
}

// 显示用户已取快递列表
int display_picked(User* user, ExpressNode* list) {
    ExpressNode* curr = list;
    int count = 0;

    printf("\n=== %s的已取快递 ===\n", user->username);
    while (curr != NULL) {
        if (strcmp(curr->data.pickup_phone, user->phone) == 0 &&
            (curr->data.status == STATUS_PICKED)) {
            printf("%d.\n", ++count);
            display_express_info(&curr->data);
        }
        curr = curr->next;
    }

    if (count == 0) {
        printf("您未取过快递\n");
    }

    return count;
}
// 误领处理函数
void mistaken_pickup(User* user) {
    clear_screen();
    printf("\n========== 快递误领处理 ==========\n");

    // 显示用户已取件的快递
    int count = display_picked(user, pickup_list);
    if (count == 0) {
        pause_program();
        return;
    }
    printf("\n请输入误领的快递单号（输入0取消）：");
    char tracking_num[20] = { 0 };
    scanf_s("%19s", tracking_num, (unsigned)_countof(tracking_num));
    clear_input_buffer();
    if (strcmp(tracking_num, "0") == 0) {
        printf("已取消操作\n");
        return;
    }
    ExpressNode* express = find_express_by_tracking(pickup_list, tracking_num);
    if (!express || strcmp(express->data.pickup_phone, user->phone) != 0) {
        printf("\n未找到该快递\n");
        pause_program();
        return;
    }
    if (strcmp(express->data.pickup_phone, express->data.receiver_phone) == 0)
    {
        printf("\n您为此快递的收件人，无需申请误取\n");
        pause_program();
        return;
    }
    if (express->data.status != STATUS_PICKED &&
        express->data.status != STATUS_WRONGPICKED &&
        express->data.status != STATUS_LOST) {
        printf("\n该快递状态不符合误领申请条件\n");
        pause_program();
        return;
    }
    printf("\n确认提交误领申请吗？(y/n): ");
    char confirm;
    scanf_s(" %c", &confirm, 1);
    clear_input_buffer();

    if (tolower(confirm) == 'y') {
        express->data.status = STATUS_MISPICKED;
        save_pickup_express_info(pickup_list, PICKUP_FILE);
        printf("\n已成功提交误领申请\n");
    }
    else {
        printf("\n已取消操作\n");
    }
    pause_program();
}