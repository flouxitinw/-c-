// cainiao.c
#include "cainiao.h"
//--------------------辅助函数--------------------
//获取物品类型名称
const char* get_package_type_name(PackageType type) {
    const char* names[] = {
        "日用品", "食品", "文件", "衣物",
        "数码产品", "酒", "动物", "武器",
        "易燃/易爆/有毒物品", "贵重物品", "生鲜",
        "易碎品", "其他"
    };
    return names[type];
}
// 检查是否是有效的 11 位手机号
int is_valid_phone(const char* phone) {
    if (strlen(phone) != 11) {
        return 0;
    }
    for (int i = 0; i < 11; i++) {
        if (!isdigit(phone[i])) {
            return 0;
        }
    }
    return 1;
}
// 获取用户输入范围内的整数
int input_int_in_range(const char* prompt, int min, int max) {
    int choice;
    while (1) {
        printf("%s", prompt);
        if (scanf_s("%d", &choice) != 1) {  // 检查是否成功读取整数
            printf("输入无效，请重新输入！\n");
            while (getchar() != '\n');  // 清空输入缓冲区
            continue;
        }
        if (choice < min || choice > max) {  // 检查是否在范围内
            printf("输入必须介于 %d 和 %d 之间，请重新输入！\n", min, max);
            continue;
        }
        break;  // 输入正确，退出循环
    }
    return choice;
}
// 输入一个正浮点数（必须 > 0）
double input_positive_double(const char* prompt) {
    char input[256]; // 足够大的缓冲区存储输入

    while (1) {
        printf("%s", prompt);
        fflush(stdout);//避免提示信息延迟显示

        // 1. 读取完整行输入
        if (fgets(input, sizeof(input), stdin) == NULL) {
            clear_input_buffer();
            printf("输入错误，请重新输入！\n");
            continue;
        }

        // 2. 去除换行符
        input[strcspn(input, "\n")] = '\0';

        // 3. 检查是否为空输入
        if (input[0] == '\0') {
            printf("输入不能为空！\n");
            continue;
        }

        // 4. 验证是否为有效数字格式
        int valid = 1;
        int dot_count = 0;

        for (int i = 0; input[i] != '\0'; i++) {
            if (i == 0 && input[i] == '-') {
                valid = 0; // 不允许负数
                break;
            }

            if (input[i] == '.') {
                dot_count++;
                if (dot_count > 1) { // 多个小数点
                    valid = 0;
                    break;
                }
            }
            else if (!isdigit((unsigned char)input[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("请输入有效的正数（如12.34）！\n");
            continue;
        }

        // 5. 转换为double并验证是否为正
        char* endptr;
        double value = strtod(input, &endptr);

        if (endptr == input || *endptr != '\0') {
            printf("转换失败，请检查输入格式！\n");
            continue;
        }

        if (value <= 0) {
            printf("输入必须大于0！\n");
            continue;
        }

        return value; // 验证通过
    }
}
void input_phone(char* phone, size_t size, const char* prompt) {
    char input[256]; // 临时存储任意长度输入

    while (1) {
        // 第一步：读取原始输入
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            clear_input_buffer();
            printf("输入错误，请重试！\n");
            continue;
        }

        // 第二步：去除换行符
        input[strcspn(input, "\n")] = '\0';

        // 第三步：验证长度是否为11
        if (strlen(input) != 11) {
            printf("手机号必须为11位！当前输入了%zu位\n", strlen(input));
            continue;
        }

        // 第四步：验证是否全为数字
        int is_valid = 1;
        for (int i = 0; i < 11; i++) {
            if (!isdigit((unsigned char)input[i])) {
                is_valid = 0;
                break;
            }
        }

        if (!is_valid) {
            printf("手机号只能包含数字！\n");
            continue;
        }

        // 第五步：安全复制到输出缓冲区
        strncpy(phone, input, size - 1);
        phone[size - 1] = '\0';
        return;
    }
}
void safe_input(char* buf, size_t size, const char* prompt) {
    int input_valid = 0;
    while (!input_valid) {
        printf("%s", prompt);
        fflush(stdout);
        clear_input_buffer();
        if (fgets(buf, size, stdin)) {
            // 检查是否读取了完整行（检测换行符是否存在）
            char* newline = strchr(buf, '\n');
            if (newline) {
                *newline = '\0';  // 正常情况：去除换行符
                // 检查输入是否为空（包括只输入空格）
                if (strlen(buf) == 0) {
                    printf("输入不能为空，请重新输入。\n");
                }
                else {
                    input_valid = 1;  // 输入有效
                }
            }
            else {
                // 情况1：用户输入超过缓冲区大小
                clear_input_buffer();
                printf("错误：输入长度不能超过%zu字符！\n", size - 1);

                // 情况2：EOF触发（如Ctrl+Z/D）
                if (feof(stdin)) {
                    clearerr(stdin);
                    printf("检测到输入终止，请重新输入\n");
                }
            }
        }
        else {
            // 处理fgets失败情况（如流错误）
            clear_input_buffer();
            if (feof(stdin)) {
                clearerr(stdin);
                printf("检测到输入终止，请重新输入\n");
            }
            else {
                printf("输入错误，请重试\n");
            }
        }
    }
}
//--------------------寄快递核心函数--------------------
void send_express(User* user) {
    Express* new_express = (Express*)malloc(sizeof(Express));
    if (!new_express) {
        perror("内存分配失败");
        return;
    }
    memset(new_express, 0, sizeof(Express)); // 初始化清零

    new_express->create_time = time(NULL);
    printf("== = 寄快递信息录入 == =\n");
    // 1. 寄件人信息
    printf("\n[寄件人信息]\n");
    printf("姓名: %s\n", user->username);
    printf("电话: %s\n\n", user->phone);

    strncpy(new_express->sender_name, user->username, sizeof(new_express->sender_name) - 1);
    strncpy(new_express->sender_phone, user->phone, sizeof(new_express->sender_phone) - 1);

    // 2. 收件人信息

    printf("[收件人信息]\n");
    safe_input(new_express->receiver_name, sizeof(new_express->receiver_name), "姓名：");
    input_phone(new_express->receiver_phone, sizeof(new_express->receiver_phone), "电话：");

    if (strcmp(new_express->sender_phone, new_express->receiver_phone) == 0 &&
        strcmp(new_express->sender_name, new_express->receiver_name) != 0) {
        printf("\n警告：一个手机号仅可登录一个账户\n");
        clear_input_buffer();  // 先清空输入缓冲区
        char ch = getchar();             // 等待用户按任意键
        free(new_express);
        return;                // 返回主菜单
    }

    // 包裹信息
    //包裹重量和体积
    new_express->weight = input_positive_double("包裹重量（千克)：");
    new_express->volume = input_positive_double("包裹体积(立方厘米)：");
    if (new_express->weight > 50 || new_express->volume >= 3000000) {
        printf("包裹过大不可邮寄");
        printf("按任意键返回主菜单...");
        clear_input_buffer();  // 先清空缓冲区
        char ch = getchar();             // 等待用户按键
        free(new_express);     // 释放已分配的内存
        return;                // 返回到调用函数（通常是主菜单）
    }
    // 物品类型选择
    printf("请选择物品类型：\n");
    for (int i = 0; i < 13; i++) {
        printf("%d. %s", i, get_package_type_name((PackageType)i));
    }
    int type_choice = input_int_in_range("请输入选项（0-12）：", 0, 12);
    if (type_choice >= 5 && type_choice <= 8) {
        printf("根据国家邮政法规禁止邮寄");
        printf("按任意键返回主菜单...");
        clear_input_buffer();  // 先清空输入缓冲区
        char ch = getchar();             // 等待用户按任意键
        free(new_express);
        return;                // 返回主菜单

    }
    new_express->package_type = (PackageType)(type_choice);
    if (new_express->package_type == FRESH) {
        printf("建议选择冷链运输\n ");
    }
    if (new_express->package_type == VALUABLES || new_express->package_type == DIGITAL) {
        printf("建议开启保价服务\n");
    }
    if (new_express->weight > 20.0) {
        printf("建议选择大件邮寄方式\n");
    }


    // 运输方式选择
    printf("请选择运输方式\n");
    printf("0. 普通快递\n1. 大件邮寄\n2. 冷链邮寄\n");
    int method_choice = input_int_in_range("请输入选项（0-2）：", 0, 2);
    new_express->method = (ShippingMethod)method_choice;
    if (new_express->method == SHIPPING_BULK && new_express->weight < 20) {
        printf("警告：大件邮寄要求重量大于20千克！\n");
        clear_input_buffer();  // 先清空输入缓冲区
        char ch = getchar();             // 等待用户按任意键
        free(new_express);
        return;                // 返回主菜单          
    }
    // 地区选择
    printf("请选择地区类型：\n");
    printf("0. 省内\n1. 省外\n2. 国际\n3. 港澳台\n");
    int region_choice = input_int_in_range("请输入选项（0-3）：", 0, 3);
    new_express->region = (RegionType)region_choice;


    // 保价信息
    clear_input_buffer();
    new_express->insured_value = input_positive_double("物品价值：");
    new_express->is_insured = input_int_in_range("是否保价（0-否 1-是）：", 0, 1);
    //运输时效
    printf("\n请选择运输时效：\n");
    printf("0. 标快（普通时效）\n");
    printf("1. 特快（加急）\n");
    int choice = input_int_in_range("请输入选项（0-1）：", 0, 1);
    new_express->express_type = (DeliveryType)choice;
    // 设置默认值
    new_express->status = STATUS_UNSENT;
    strcpy(new_express->tracking_num, "0");
    new_express->fee = calculate_shipping_fee(new_express, user);
    printf("用户[%s] 等级[%d] 应付款: %.2f\n",
        user->username, user->level, new_express->fee);

    switch (user->payment_mode) {
    case PAY_CASH:
        printf("请现场支付: %.2f\n", new_express->fee);
        printf("是否确认支付？（0-否 1-是）");
        int confirm = input_int_in_range("", 0, 1);
        if (confirm == 0) {
            printf("支付未完成，快递寄出失败\n");
            free(new_express);
            return;
        }
        else printf("支付成功，快递寄出成功\n");
        break;

    case PAY_MONTHLY:
        printf("已计入月结账户 \n");

        break;
    case PAY_CONTRACT:
        printf("将按协议周期结算\n");
        break;
    }
    // 创建链表节点
    ExpressNode* new_node = create_express_node(new_express); // 自动复制数据
    if (!new_node) {
        perror("内存分配失败");
        free(new_express);  // 创建节点失败时释放
        return;
    }

    // 插入到链表头部
    new_node->next = send_list;  // 新节点指向原头节点
    send_list = new_node;        // 更新头指针指向新节点

    // 保存到文件
    save_send_express_info(send_list, "send_express.txt");
    free(new_express);  // 创建节点失败时释放
    printf("按任意键返回主菜单...");
    clear_input_buffer();  // 先清空输入缓冲区
    char ch = getchar();             // 等待用户按任意键
    return;                // 返回主菜单

}


void modify_express(User* user) {
    clear_screen();
    printf("\n========== 修改快递信息 ==========\n");

    // 1. 显示用户未寄出的快递列表
    int count = 0;
    ExpressNode* current = send_list;
    ExpressNode** express_ptrs = NULL; // 存储匹配的快递节点指针
    int express_count = 0;

    // 第一次遍历：统计匹配的快递数量
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            express_count++;
        }
        current = current->next;
    }

    if (express_count == 0) {
        printf("您没有未寄出的快递可修改。\n");
        pause_program();
        return;
    }

    // 分配内存存储快递指针
    express_ptrs = (ExpressNode**)malloc(express_count * sizeof(ExpressNode*));
    if (!express_ptrs) {
        perror("内存分配失败");
        return;
    }

    // 第二次遍历：填充快递指针数组
    current = send_list;
    int index = 0;
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            express_ptrs[index++] = current;
            printf("%3d. | 收件人: %12s(%s)  | 包裹类型: %12s |  创建时间: %s",
                index,
                current->data.receiver_name, current->data.receiver_phone, get_package_type_name(current->data.package_type),
                ctime(&current->data.create_time));
        }
        current = current->next;
    }

    // 2. 选择要修改的快递
    int choice = input_int_in_range("\n请选择要修改的快递编号 (0取消): ", 0, express_count);
    if (choice == 0) {
        free(express_ptrs);
        return;
    }

    Express* express = &(express_ptrs[choice - 1]->data);
    free(express_ptrs);

    // 3. 显示当前信息并选择修改字段
    while (1) {
        clear_screen();
        printf("\n当前快递信息:\n");
        printf("1. 收件人姓名: %s\n", express->receiver_name);
        printf("2. 收件人电话: %s\n", express->receiver_phone);
        printf("3. 包裹重量: %.2f 千克\n", express->weight);
        printf("4. 包裹体积: %.2f 立方厘米\n", express->volume);
        printf("5. 物品类型: %s\n", get_package_type_name(express->package_type));
        printf("6. 运输方式: %s\n",
            (express->method == SHIPPING_NORMAL) ? "普通快递" :
            (express->method == SHIPPING_BULK) ? "大件邮寄" : "冷链邮寄");
        printf("7. 地区类型: %s\n",
            (express->region == REGION_PROVINCE) ? "省内" :
            (express->region == REGION_OUTOFPROVINCE) ? "省外" :
            (express->region == REGION_INTERNATIONAL) ? "国际" : "港澳台");
        printf("8. 保价状态: %s\n", express->is_insured ? "是" : "否");
        printf("9. 物品价值: %.2f\n", express->insured_value);
        printf("10. 运输时效: %s\n",
            (express->express_type == STANDARD_DELIVERY) ? "标快" : "特快");
        printf("0. 保存并返回\n");

        int field = input_int_in_range("\n选择要修改的字段 (0-10): ", 0, 10);
        if (field == 0) break;
        char original_name[50];
        strncpy(original_name, express->receiver_name, sizeof(original_name));
        switch (field) {
        case 1: { // 收件人姓名
            safe_input(express->receiver_name, sizeof(express->receiver_name), "新收件人姓名: ");

            if (strcmp(express->sender_phone, express->receiver_phone) == 0 &&
                strcmp(express->sender_name, express->receiver_name) != 0) {
                printf("\n警告：收件人与寄件人手机号相同但姓名不同！\n");
                printf("原收件人姓名: %s\n", original_name);

                // 恢复原始值
                strncpy(express->receiver_name, original_name, sizeof(express->receiver_name));

                printf("按任意键继续修改...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 2: { // 收件人电话
            char original_phone[12];
            strncpy(original_phone, express->receiver_phone, sizeof(original_phone));

            clear_input_buffer();
            input_phone(express->receiver_phone, sizeof(express->receiver_phone), "新收件人电话: ");

            if (strcmp(express->sender_phone, express->receiver_phone) == 0 &&
                strcmp(express->sender_name, express->receiver_name) != 0) {
                printf("\n警告：一个手机号仅可登录一个账户\n");
                printf("原收件人电话: %s\n", original_phone);

                // 恢复原始值
                strncpy(express->receiver_phone, original_phone, sizeof(express->receiver_phone));

                printf("按任意键继续修改...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 3: { // 重量
            double original_weight = express->weight;

            clear_input_buffer();
            express->weight = input_positive_double("新包裹重量 (千克): ");

            if (express->weight > 50) {
                printf("警告：包裹重量超过50千克，无法邮寄！\n");
                printf("原重量: %.2f千克\n", original_weight);

                // 恢复原始值
                express->weight = original_weight;

                printf("按任意键继续修改...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 4: { // 体积
            double original_volume = express->volume;

            clear_input_buffer();
            express->volume = input_positive_double("新包裹体积 (立方厘米): ");

            if (express->volume >= 3000000) {
                printf("警告：包裹体积过大，无法邮寄！\n");
                printf("原体积: %.2f立方厘米\n", original_volume);

                // 恢复原始值
                express->volume = original_volume;

                printf("按任意键继续修改...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            break;
        }

        case 5: { // 物品类型
            PackageType original_type = express->package_type;

            printf("请选择物品类型：\n");
            for (int i = 0; i < 13; i++) {
                printf("%d. %s\n", i, get_package_type_name((PackageType)i));
            }
            int new_type = input_int_in_range("选择 (0-12): ", 0, 12);

            if (new_type >= 5 && new_type <= 8) {
                printf("根据国家邮政法规禁止邮寄此类物品！\n");
                printf("原物品类型: %s\n", get_package_type_name(original_type));

                // 恢复原始值
                express->package_type = original_type;

                printf("按任意键继续修改...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
            express->package_type = (PackageType)new_type;
            break;
        }
        case 6: // 运输方式
            printf("请选择运输方式：\n");
            printf("0. 普通快递\n1. 大件邮寄\n2. 冷链邮寄\n");
            express->method = (ShippingMethod)input_int_in_range("选择 (0-2): ", 0, 2);
            break;
            if (express->method == SHIPPING_BULK && express->weight < 20) {
                printf("警告：大件邮寄要求重量大于20千克！\n");
                printf("按任意键继续修改...");
                clear_input_buffer();
                char ch = getchar();
                continue;
            }
        case 7: // 地区
            printf("请选择地区类型：\n");
            printf("0. 省内\n1. 省外\n2. 国际\n3. 港澳台\n");
            express->region = (RegionType)input_int_in_range("选择 (0-3): ", 0, 3);
            break;
        case 8: // 保价
            express->is_insured = input_int_in_range("是否保价 (0-否 1-是): ", 0, 1);
            break;
        case 9: // 物品价值
            clear_input_buffer();
            express->insured_value = input_positive_double("新物品价值: ");
            break;
        case 10: // 运输时效
            printf("请选择运输时效：\n");
            printf("0. 标快（普通时效）\n");
            printf("1. 特快（加急）\n");
            express->express_type = (DeliveryType)input_int_in_range("选择 (0-1): ", 0, 1);
        }
    }
    express->fee = calculate_shipping_fee(express, user);
    // 4. 保存修改
    save_send_express_info(send_list, SEND_FILE);
    printf("\n修改已保存！\n");
    pause_program();
}


void delete_express(User* user) {
    clear_screen();
    printf("\n========== 取消邮寄 ==========\n");

    // 1. 遍历链表，统计并显示所有未寄出的快递
    ExpressNode* current = send_list;
    int item_count = 0;

    // 第一次遍历：统计数量并显示
    printf("您的未寄出快递列表：\n");
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            item_count++;
            printf("%2d. 收件人: %s (%s) \n    包裹类型: %12s | 创建时间: %s",
                item_count,
                current->data.receiver_name,
                current->data.receiver_phone,
                get_package_type_name(current->data.package_type),
                ctime(&current->data.create_time));
        }
        current = current->next;
    }

    if (item_count == 0) {
        printf("您没有未寄出的快递可取消。\n");
        pause_program();
        return;
    }

    // 2. 选择要删除的快递
    int choice = input_int_in_range("\n请选择要取消的快递编号 (0返回): ", 0, item_count);
    if (choice == 0) {
        return;
    }

    // 3. 定位到选择的快递
    current = send_list;
    ExpressNode* prev = NULL;
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {
            count++;

            if (count == choice) {
                // 显示详情并确认
                printf("\n即将取消的快递信息:\n");
                printf("收件人: %s (%s)\n", current->data.receiver_name, current->data.receiver_phone);
                printf("重量: %.2f千克 | 体积: %.2f立方厘米\n", current->data.weight, current->data.volume);
                printf("物品类型: %s\n", get_package_type_name(current->data.package_type));
                printf("创建时间: %s", ctime(&current->data.create_time));

                int confirm = input_int_in_range("\n确认取消？(0-返回 1-确认取消): ", 0, 1);
                if (confirm == 1) {
                    // 从链表中删除节点
                    if (prev == NULL) {  // 删除头节点
                        send_list = current->next;
                    }
                    else {  // 删除中间或尾节点
                        prev->next = current->next;
                    }

                    free(current);  // 释放内存
                    save_send_express_info(send_list, SEND_FILE);  // 保存到文件
                    printf("\n取消邮寄成功！\n");
                }
                else {
                    printf("\n已保留该快递。\n");
                }
                pause_program();
                return;
            }
        }
        prev = current;
        current = current->next;
    }

    printf("选择无效，请重新操作。\n");
    pause_program();
}


void find_pending_express(User* user) {
    clear_screen();
    printf("\n========== 待寄出快递查询 ==========\n");

    // 1. 选择查询方式
    printf("\n请选择查询方式：\n");
    printf("1. 按收件人姓名查询\n");
    printf("2. 按收件人手机号查询\n");
    printf("3. 按物品类型查询\n");
    printf("0. 返回\n");

    int choice = input_int_in_range("请输入选项(0-3): ", 0, 3);
    if (choice == 0) return;

    // 2. 根据选择获取查询条件
    char search_name[50] = { 0 };
    char search_phone[12] = { 0 };
    int search_type = -1;

    switch (choice) {
    case 1: // 按姓名查询
        safe_input(search_name, sizeof(search_name), "请输入收件人姓名关键词: ");
        break;

    case 2: // 按手机号查询
        clear_input_buffer();
        input_phone(search_phone, sizeof(search_phone), "请输入收件人完整手机号(11位): ");
        break;

    case 3: // 按物品类型查询
        printf("\n请选择物品类型：\n");
        for (int i = 0; i < 13; i++) {
            printf("%d. %s\n", i, get_package_type_name((PackageType)i));
        }
        search_type = input_int_in_range("请输入物品类型编号(0-12): ", 0, 12);
        break;
    }

    // 3. 遍历并显示匹配结果
    ExpressNode* current = send_list;
    int total_matches = 0;

    printf("\n=== 查询结果 ===\n");
    while (current != NULL) {
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            current->data.status == STATUS_UNSENT) {

            // 检查查询条件
            int match = 0;
            switch (choice) {
            case 1: // 姓名匹配
                match = strstr(current->data.receiver_name, search_name) != NULL;
                break;
            case 2: // 手机号匹配
                match = strcmp(current->data.receiver_phone, search_phone) == 0;
                break;
            case 3: // 物品类型匹配
                match = current->data.package_type == search_type;
                break;
            }

            if (match) {
                total_matches++;
                printf("\n%d.\n", total_matches);
                printf("收件人: %-20s\n电话: %s\n",
                    current->data.receiver_name,
                    current->data.receiver_phone);
                printf("重量: %.2f千克  体积: %.2f立方厘米  类型: %s\n",
                    current->data.weight,
                    current->data.volume,
                    get_package_type_name(current->data.package_type));
                printf("创建时间: %s", ctime(&current->data.create_time));
            }
        }
        current = current->next;
    }

    if (total_matches == 0) {
        printf("\n未找到符合条件的快递。\n");
        clear_input_buffer();  // 先清空输入缓冲区
        char ch = getchar();             // 等待用户按任意键
        return;                // 返回主菜单
    }
    else {
        printf("\n共找到 %d 条记录。\n", total_matches);
    }

    pause_program();
}

void show_pending_bills(User* user) {
    clear_screen();
    printf("\n========== 待支付账单 ==========\n");

    // 1. 检查用户类型，只有VIP/企业/合作商户才有待支付账单
    if (user->level == USER_NORMAL || user->level == USER_STUDENT) {
        printf("普通用户和学生用户无待支付账单（现付模式）\n");
        pause_program();
        return;
    }

    // 2. 设置时间范围
    time_t now = time(NULL);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    // 设置起始时间为当月1日或当年1月1日
    struct tm start_tm = { 0 };
    start_tm.tm_year = timeinfo.tm_year;

    if (user->level == USER_VIP || user->level == USER_ENTERPRISE) {
        start_tm.tm_mon = timeinfo.tm_mon;  // 当月1日
        printf("统计范围: 本月(1日至今)的待支付账单\n");
    }
    else { // USER_PARTNER
        start_tm.tm_mon = 0;  // 1月1日
        printf("统计范围: 本年(1月1日至今)的待支付账单\n");
    }
    start_tm.tm_mday = 1;
    time_t start_time = mktime(&start_tm);

    // 3. 遍历快递链表查找待支付账单
    ExpressNode* current = send_list;
    double total_amount = 0.0;
    int bill_count = 0;

    printf("\n%-15s %-15s %-20s %-12s %-10s\n",
        "收件人", "手机号", "物品类型", "寄出时间", "金额");
    printf("----------------------------------------------------------------\n");

    while (current != NULL) {
        // 检查是否是该用户的、已寄出、未支付的快递
        if (strcmp(current->data.sender_phone, user->phone) == 0 &&
            (current->data.status == STATUS_SENT || current->data.status == STATUS_UNSENT) &&
            current->data.fee > 0.01 &&  // 未支付
            current->data.create_time >= start_time) {

            // 格式化时间
            char time_str[20];
            struct tm create_tm;
            localtime_s(&create_tm, &current->data.create_time);
            strftime(time_str, sizeof(time_str), "%m-%d %H:%M", &create_tm);

            // 获取物品类型名称
            const char* type_name = get_package_type_name(current->data.package_type);

            printf("%-15s %-15s %-20s %-12s ￥%-8.2f\n",
                current->data.receiver_name,
                current->data.receiver_phone,
                type_name,
                time_str,
                current->data.fee
            );

            total_amount += current->data.fee;
            bill_count++;
        }
        current = current->next;
    }

    // 4. 显示统计结果
    if (bill_count == 0) {
        printf("\n当前没有待支付账单\n");
        clear_input_buffer();  // 先清空输入缓冲区
        char ch = getchar();             // 等待用户按任意键
        return;                // 返回主菜单
    }
    else {
        printf("\n总计: %d 笔账单  应付款总额: ￥%.2f\n", bill_count, total_amount);

        // 5. 支付确认
        int choice = input_int_in_range("\n是否现在支付？(1-确认支付 0-取消): ", 0, 1);
        if (choice == 1) {
            // 标记为已支付
            current = send_list;
            while (current != NULL) {
                if (strcmp(current->data.sender_phone, user->phone) == 0 &&
                    (current->data.status == STATUS_SENT || current->data.status == STATUS_UNSENT) &&
                    current->data.fee > 0.01 &&
                    current->data.create_time >= start_time) {
                    current->data.fee = 0.0;  // 设置为已支付
                }
                current = current->next;
            }
            // 保存数据
            save_send_express_info(send_list, SEND_FILE);
            printf("\n支付成功！已结算 ￥%.2f\n", total_amount);
        }
    }
    pause_program();
}


void send_express_menu(User* user) {
    while (1) {

        clear_screen();
        printf("\n========== 寄快递菜单 ==========\n");
        printf("用户: %s (%s)\n", user->username,
            user->level == USER_NORMAL ? "普通用户" :
            user->level == USER_STUDENT ? "学生用户" :
            user->level == USER_VIP ? "VIP用户" :
            user->level == USER_ENTERPRISE ? "企业用户" : "合作商户");

        // 动态菜单选项
        printf("\n1. 寄出新快递\n");
        printf("2. 修改待寄出快递\n");
        printf("3. 删除待寄出快递\n");
        printf("4. 查询待寄出快递\n");
        // 根据用户类型显示支付选项
        if (user->level != USER_NORMAL && user->level != USER_STUDENT) {
            printf("5. 支付账单\n");
            printf("6. 返回主菜单\n");
            printf("\n请选择操作 (1-6): ");
        }
        else {
            printf("5. 返回主菜单\n");
            printf("\n请选择操作 (1-5): ");
        }

        // 获取用户选择
        int max_choice = (user->level != USER_NORMAL && user->level != USER_STUDENT) ? 6 : 5;
        int choice = input_int_in_range("", 1, max_choice);

        switch (choice) {
        case 1:  // 寄出新快递
            send_express(user);
            break;

        case 2:  // 修改快递
            modify_express(user);
            break;

        case 3:  // 删除快递
            delete_express(user);
            break;

        case 4:  // 查询待寄出快递
            find_pending_express(user);
            break;

        case 5:  // 支付或返回
            if (user->level != USER_NORMAL && user->level != USER_STUDENT) {
                show_pending_bills(user);
            }
            else {
                return;  // 普通用户直接返回
            }
            break;

        case 6:  // 仅VIP/企业/合作商户可见
            return;

        default:
            printf("无效选项！\n");
            pause_program();
        }
    }
}