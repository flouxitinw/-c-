#include "cainiao.h"

// 创建货架
Shelf* create_shelf(const char* shelf_id, int max_capacity) {
    Shelf* new_shelf = (Shelf*)malloc(sizeof(Shelf));
    if (!new_shelf) {
        perror("Failed to create shelf");
        return NULL;
    }
    strncpy_s(new_shelf->shelf_id, sizeof(new_shelf->shelf_id), shelf_id, _TRUNCATE);
    new_shelf->shelf_id[sizeof(new_shelf->shelf_id) - 1] = '\0';
    new_shelf->max_capacity = max_capacity;
    new_shelf->current_count = 0;
    new_shelf->pickup_list = NULL;
    new_shelf->next = NULL;
    return new_shelf;
}

// 增加货架（头插法）
void add_shelf(Shelf** head, Shelf* new_shelf) {
    if (!head || !new_shelf) return;
    new_shelf->next = *head;
    *head = new_shelf;
}

//删除货架
bool delete_shelf(Shelf** head, const char* shelf_id) {
    // 参数有效性检查（这部分应该放在调用函数中）
    if (!head || !shelf_id || shelf_id[0] == '\0') {
        printf("\n错误：无效参数！\n");
        return false;
    }

    // 检查链表是否为空
    if (!*head) {
        printf("\n错误：当前没有货架数据！\n");
        return false;
    }

    Shelf* prev = NULL, * curr = *head;
    bool found = false;

    // 查找要删除的货架
    while (curr) {
        if (strcmp(curr->shelf_id, shelf_id) == 0) {
            found = true;

            // 检查货架是否为空
            if (curr->current_count > 0) {
                printf("\n错误：货架 %s 中仍有 %d 件快递，请先移出所有快递！\n",
                    shelf_id, curr->current_count);
                return false;
            }

            // 从链表中移除
            if (prev) {
                prev->next = curr->next;
            }
            else {
                *head = curr->next;
            }

            // 释放快递列表内存
            ExpressNode* express = curr->pickup_list;
            while (express) {
                ExpressNode* temp = express;
                express = express->next;
                free(temp);
            }

            // 释放货架内存
            free(curr);
            printf("\n\033[32m成功：货架 %s 已删除\033[0m\n", shelf_id);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) {
        printf("\n错误：未找到货架 %s\n", shelf_id);
    }
    return false;
}

// ========== 查看货架状态 ==========
void display_shelves(Shelf* shelves) {
    // 检查货架链表是否为空
    if (!shelves) {
        printf("\n当前没有货架数据！\n");
        pause_program();
        clear_input_buffer();
        return;
    }

    printf("\n====== 货架状态总览 ======\n");
    // 绘制表格上边框（使用Unicode制表符）
    printf("┌─────────┬───────────────┬────────────┬────────────┬────────────┐\n");
    printf("   货架ID      使用情况      快递数量     货架类型      状态     \n");
    printf("├─────────┼───────────────┼────────────┼────────────┼────────────┤\n");

    // 遍历货架链表
    Shelf* curr = shelves;
    while (curr) {
        // 计算当前使用率（当前数量/最大容量）
        float ratio = (float)curr->current_count / curr->max_capacity;

        // 根据使用率设置颜色：
        // >80%: 红色 \033[31m
        // >50%: 黄色 \033[33m
        // 其他: 绿色 \033[32m
        const char* color = ratio > 0.8 ? "\033[31m" : ratio > 0.5 ? "\033[33m" : "\033[32m";

        // 设置状态文本：
        // 空货架: "空"
        // 使用率100%: "已满！"
        // 其他: "正常"
        const char* status = (curr->current_count == 0) ? "空" : (ratio >= 1.0) ? "已满！" : "正常";

        char shelf_type = toupper(curr->shelf_id[0]);//获取货架id首字母
        const char* type_name =
            (shelf_type == 'F') ? "易碎品" :
            (shelf_type == 'C') ? "冷链" :
            (shelf_type == 'B') ? "大件" : "普通";

        printf("│ %-7s │ ", curr->shelf_id);
        // 绘制使用率条形图
        for (int i = 0; i < 10; i++) printf(i < ratio * 10 ? "%s█\033[0m" : " ", color);//%s：插入颜色代码
        // 打印剩余数据（使用率百分比、当前数量、类型、状态）
        printf(" %-.0f%%   %-10d   %-10s   %-10s  \n",
            ratio * 100, curr->current_count, type_name, status);

        curr = curr->next;
    }

    printf("└─────────┴───────────────┴────────────┴────────────┴────────────┘\n");
    pause_program();
    clear_input_buffer();
}

// 盘点单个货架
void inventory_shelf(Shelf* shelf) {
    if (!shelf) {
        printf("\n错误：无效的货架指针！\n");
        return;
    }

    printf("\n========================= 货架 %s 盘点 =========================\n", shelf->shelf_id);
    // 计算并显示容量使用率（带颜色预警）
    float usage = (float)shelf->current_count / shelf->max_capacity;// 计算使用率(0.0~1.0)
    // 根据使用率选择颜色 // >80% 红色预警  // 50%~80% 黄色提醒  // <50% 绿色正常
    const char* capacity_color = usage > 0.8 ? "\033[31m" : usage > 0.5 ? "\033[33m" : "\033[32m";
    printf(" 容量: %s%d/%d (%.0f%%) \033[0m\n",
        capacity_color,
        shelf->current_count,
        shelf->max_capacity,
        usage * 100);
    // 快递列表详情
    printf("\n 快递清单（共 %d 件）:\n", shelf->current_count);
    printf("┌──────────────┬──────────────────────┬────────────┬────────────┐\n");
    printf("    快递单号          入库时间            状态        取件码     \n");
    printf("├──────────────┼──────────────────────┼────────────┼────────────┤\n");
    // 遍历快递列表
    ExpressNode* curr = shelf->pickup_list;
    if (!curr) {
        printf("                             暂无快递                             \n");
    }
    else {
        while (curr) {
            // 格式化时间显示
            char time_str[20]; // time_t类型的时间戳（从1970 - 01 - 01开始的秒数）
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M",
                localtime(&curr->data.storage_time));// 时间戳转换

            printf("│ %-12s │ %-20s │ %-10s │ %-10s │\n",
                curr->data.tracking_num, // 快递单号
                time_str,// 入库时间
                get_status_text(curr->data.status),// 状态
                curr->data.pickup_code);// 取件码
            curr = curr->next;
        }
    }
    printf("└──────────────┴──────────────────────┴────────────┴────────────┘\n");

    // 添加分隔线
    printf("\n ───────────────────────────────────────────────────────────────\n");
}

// 全站盘点
void full_inventory(Shelf* head) {


    printf("\n========================== 全站库存盘点 =========================\n");
    // 遍历所有货架
    while (head) {
        inventory_shelf(head);// 显示单个货架详情
        head = head->next;
    }

    clear_input_buffer();
}

// 全站滞留件报告
void show_all_overdue_items_with_stats() {
    time_t now = time(NULL);// 获取当前时间戳
    int total_overdue = 0; //总滞留件数（ > 3天）
    int overdue_count = 0;    // 3-7天
    int very_overdue_count = 0; // >7天

    printf("\n========================== 滞留件报告 =========================\n");

    // 第一遍：统计数量//遍历所有快递
    ExpressNode* curr = pickup_list;
    while (curr) {
        if (curr->data.status == STATUS_OVERDUE) {
            double days = difftime(now, curr->data.storage_time) / 86400;// 计算滞留天数
            if (days > 3) {
                total_overdue++;
                if (days > 7) {
                    very_overdue_count++;
                }
                else {
                    overdue_count++;
                }
            }
        }
        curr = curr->next;
    }

    // 显示统计摘要
    printf("\n====== 统计摘要 ======\n");
    printf("紧急 (>7天):  %d 件\n", very_overdue_count);
    printf("逾期 (3-7天): %d 件\n", overdue_count);
    printf("----------------------\n");
    printf("总计:       %d 件\n", total_overdue);

    if (total_overdue == 0) {
        printf("\n当前没有滞留超过3天的包裹\n");
        return;
    }

    // 第二遍：按货架分组显示详情
    printf("\n===================== 详情列表 ======================\n");

    // 先显示3-7天的
    if (overdue_count > 0) {
        printf("\n================= 逾期包裹 (3-7天) ==================\n");


        char displayed_shelves[100][10] = { 0 }; // 记录已显示的货架
        int shelf_count = 0;

        // 遍历所有快递
        curr = pickup_list;
        while (curr) {
            if (curr->data.status == STATUS_OVERDUE) {
                double days = difftime(now, curr->data.storage_time) / 86400;
                if (days > 3 && days <= 7) {
                    // 检查这个货架是否已经显示过
                    int already_displayed = 0;
                    for (int i = 0; i < shelf_count; i++) {
                        if (strcmp(displayed_shelves[i], curr->data.shelf_id) == 0) {
                            already_displayed = 1;
                            break;
                        }
                    }

                    if (!already_displayed) {
                        // 记录这个货架已经显示
                        strcpy(displayed_shelves[shelf_count++], curr->data.shelf_id);

                        // 显示这个货架上的所有逾期件
                        printf("\n--------------------- 货架: %s ---------------------\n\n", curr->data.shelf_id);
                        ExpressNode* shelf_curr = pickup_list;
                        while (shelf_curr) {
                            if (shelf_curr->data.status == STATUS_OVERDUE &&
                                strcmp(shelf_curr->data.shelf_id, curr->data.shelf_id) == 0) {
                                double shelf_days = difftime(now, shelf_curr->data.storage_time) / 86400;
                                if (shelf_days > 3 && shelf_days <= 7) {
                                    printf("  单号:%-20s 收件人:%-10s (%.1f天)\n",
                                        shelf_curr->data.tracking_num,
                                        shelf_curr->data.receiver_name,
                                        shelf_days);
                                }
                            }
                            shelf_curr = shelf_curr->next;
                        }
                        printf("-----------------------------------------------------\n");
                    }
                }
            }
            curr = curr->next;
        }
    }

    // 再显示超过7天的
    if (very_overdue_count > 0) {
        printf("\n==================== 紧急包裹 (>7天) =====================\n");

        // 遍历所有货架
        char displayed_shelves[100][10] = { 0 }; // 记录已显示的货架
        int shelf_count = 0;

        curr = pickup_list;
        while (curr) {
            if (curr->data.status == STATUS_OVERDUE) {
                double days = difftime(now, curr->data.storage_time) / 86400;
                if (days > 7) {
                    // 检查这个货架是否已经显示过
                    int already_displayed = 0;
                    for (int i = 0; i < shelf_count; i++) {
                        if (strcmp(displayed_shelves[i], curr->data.shelf_id) == 0) {
                            already_displayed = 1;
                            break;
                        }
                    }

                    if (!already_displayed) {
                        // 记录这个货架已经显示
                        strcpy(displayed_shelves[shelf_count++], curr->data.shelf_id);

                        // 显示这个货架上的所有逾期件
                        printf("\n--------------------- 货架: %s ---------------------\n\n", curr->data.shelf_id);
                        ExpressNode* shelf_curr = pickup_list;
                        while (shelf_curr) {
                            if (shelf_curr->data.status == STATUS_OVERDUE &&
                                strcmp(shelf_curr->data.shelf_id, curr->data.shelf_id) == 0) {
                                double shelf_days = difftime(now, shelf_curr->data.storage_time) / 86400;
                                if (shelf_days > 7) {
                                    printf("  单号:%-20s 收件人:%-10s (%.1f天)\n",
                                        shelf_curr->data.tracking_num,
                                        shelf_curr->data.receiver_name,
                                        shelf_days);
                                }
                            }
                            shelf_curr = shelf_curr->next;
                        }
                        printf("-----------------------------------------------------\n");
                    }
                }
            }
            curr = curr->next;
        }
        printf("\n请立即电话通知以上 %d 位收件人处理紧急滞留件！\n", very_overdue_count);
    }
}

// 货架容量预警
void check_shelf_capacity(Shelf* head) {
    clear_screen();
    printf("\n=============== 货架容量预警 ===============\n");
    printf("%-8s %-8s %-8s %-12s %s\n",
        "货架号", "当前", "最大", "使用率", "状态");
    printf("--------------------------------------------\n");

    bool has_overload = false;// 是否有超载货架标志
    int total_shelves = 0;// 货架总数计数
    int warning_count = 0;// 超载货架计数

    // 先遍历一次显示所有货架状态
    Shelf* current = head;
    while (current) {
        total_shelves++;
        float ratio = (float)current->current_count / current->max_capacity;// 当前货架使用率（转换为浮点数避免整数除法）
        char status[20];

        if (ratio > 0.8) {
            strcpy(status, "超载");
            has_overload = true;
            warning_count++;
        }
        else if (ratio > 0.6) {
            strcpy(status, "注意");
        }
        else {
            strcpy(status, "正常");
        }

        printf("%-8s %-8d %-8d %-3.0f%% %12s\n",
            current->shelf_id,
            current->current_count,
            current->max_capacity,
            ratio * 100,//使用率
            status);

        current = current->next;
    }

    // 显示统计信息和警告
    printf("\n============ 统计信息 ============\n");
    if (total_shelves == 0) {
        printf("\n当前没有货架数据！\n");
    }
    else {
        printf("货架总数: %d\n", total_shelves);
        printf("超载货架: %d (使用率>80%%)\n", warning_count);
        printf("正常货架: %d\n", total_shelves - warning_count);

        if (!has_overload) {
            printf("\n所有货架容量均在安全范围内(≤80%%)");
        }
        else {
            printf("\n[警告] 有%d个货架超过容量80%%，请及时处理！", warning_count);
        }
    }

    // 显示库存详情选项
    char shelf_id[20];
    do {
        printf("\n\n输入货架号查看详情（输入0返回）: ");
        scanf("%19s", shelf_id);
        clear_input_buffer();

        if (strcmp(shelf_id, "0") == 0) {
            break; // 输入0则退出循环返回
        }

        current = head;
        bool found = false;

        // 查找货架
        while (current) {
            if (strcmp(current->shelf_id, shelf_id) == 0) {
                found = true;
                clear_screen();
                printf("\n========= 货架 %s 库存详情 =========\n", shelf_id);
                printf("最大容量: %d\n", current->max_capacity);
                printf("当前存放: %d (%.0f%%)\n",
                    current->current_count,
                    (float)current->current_count / current->max_capacity * 100);
                printf("-------------------------------------\n");

                // 显示该货架上的快递
                ExpressNode* express_current = pickup_list;
                int package_count = 0;
                while (express_current) {
                    if (strcmp(express_current->data.shelf_id, shelf_id) == 0) {
                        package_count++;
                        printf("%d. %s | %s -> %s | %s\n",
                            package_count,
                            express_current->data.tracking_num,
                            express_current->data.sender_name,
                            express_current->data.receiver_name,
                            get_status_text(express_current->data.status));
                    }
                    express_current = express_current->next;
                }

                printf("\n共存放 %d 件快递", package_count);
                clear_input_buffer();

            }
            current = current->next;
        }
        if (!found) {
            printf("\n未找到货架 %s，请重新输入\n", shelf_id);
        }

    } while (true); // 循环直到输入0

    clear_input_buffer();
}

//-----------------------------------------------------------------------------------------
// 保存货架信息到文件
bool save_shelves_to_file(Shelf* head, const char* filename) {
    if (!filename || filename[0] == '\0') {
        fprintf(stderr, "错误：无效文件名\n");
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to save shelves");
        return false;
    }

    // 写入表头
    fprintf(fp, "shelf_id,max_capacity,current_count,pickup_list\n");

    Shelf* curr_shelf = head;
    while (curr_shelf) {
        // 写入货架基本信息
        fprintf(fp, "%s,%d,%d,",
            curr_shelf->shelf_id,
            curr_shelf->max_capacity,
            curr_shelf->current_count);

        // 写入关联的快递单号（分号分隔）
        ExpressNode* curr_express = curr_shelf->pickup_list;
        while (curr_express) {
            fprintf(fp, "%s", curr_express->data.tracking_num);
            curr_express = curr_express->next;
            if (curr_express) fprintf(fp, ";");
        }

        fprintf(fp, "\n"); // 换行
        curr_shelf = curr_shelf->next;
    }

    fclose(fp);
    return true;
}

// 从文件中获取货架信息
Shelf* load_shelves_from_file(const char* filename, ExpressNode* pickup_list) {
    if (!filename) return NULL;

    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    Shelf* head = NULL, * tail = NULL;
    char line[512]; // 假设一行最多512字节

    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char shelf_id[20], express_items[256];
        int max_cap, curr_cnt;

        // 解析CSV行
        if (sscanf(line, "%19[^,],%d,%d,%255[^\n]",
            shelf_id, &max_cap, &curr_cnt, express_items) < 3) {
            continue; // 解析失败则跳过
        }

        Shelf* shelf = create_shelf(shelf_id, max_cap);
        if (!shelf) continue;

        shelf->current_count = curr_cnt;

        // 解析快递单号并关联到货架
        char* token = strtok(express_items, ";");
        while (token) {
            ExpressNode* exp_node = find_express_by_tracking(pickup_list, token);
            if (exp_node) {
                ExpressNode* node = (ExpressNode*)malloc(sizeof(ExpressNode));
                if (node) {
                    node->data = exp_node->data;  // 复制快递数据
                    node->next = shelf->pickup_list;
                    shelf->pickup_list = node;
                    strncpy(exp_node->data.shelf_id, shelf->shelf_id, sizeof(exp_node->data.shelf_id) - 1);
                }
            }
            token = strtok(NULL, ";");
        }

        // 将货架加入链表
        if (!head) head = tail = shelf;
        else {
            tail->next = shelf;
            tail = shelf;
        }
    }

    fclose(fp);
    return head;
}

//根据快递单号查找特定的快递
Express* find_express_by_tracking(ExpressNode* head, const char* tracking_num) {
    while (head) {
        if (strcmp(head->data.tracking_num, tracking_num) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

// 获取状态文本
const char* get_status_text(PackageStatus status) {
    static const char* texts[] = {
        "待入库",       // STATUS_PENDING_IN
        "未寄出",       // STATUS_UNSENT
        "已寄出",       // STATUS_SENT
        "待取件",       // STATUS_UNPICKED
        "已取件",       // STATUS_PICKED
        "滞留件",       // STATUS_OVERDUE
        "丢失件",         // STATUS_LOST
        "损坏件",       // STATUS_DAMAGED
        "误领件",       // STATUS_MISPICKED
        "冒领件",	  // STATUS_WRONGPICKED
        "异常件",       // STATUS_ABNORMAL
        "异常已处理",   // STATUS_ABNORMAL_RESOLVED
        "拒收",        //STATUS_REJECTED
        "退回" ,      //STATUS_RETURNED
        "重新待取件 "   //STATUS_UNPICKED_AGAIN

    };
    return (status >= STATUS_UNSENT && status <= STATUS_UNPICKED_AGAIN)
        ? texts[status]
        : "未知状态";
}

//统计货架数量
int count_shelves(Shelf* shelves) {
    int count = 0;
    Shelf* current = shelves;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

//验证货架ID格式是否合法
bool is_valid_shelf_id(const char* shelf_id) {
    // 检查是否为NULL或空字符串
    if (!shelf_id || shelf_id[0] == '\0') {
        return false;
    }

    // 检查长度（示例要求：2-10个字符）
    size_t len = strlen(shelf_id);
    if (len < 2 || len > 10) {
        return false;
    }

    // 检查格式（示例：首字符必须为字母，其余为字母或数字）
    if (!isalpha(shelf_id[0])) {
        return false;
    }

    for (size_t i = 1; i < len; i++) {
        if (!isalnum(shelf_id[i])) {
            return false;
        }
    }

    return true;
}

// 根据运输方式获取货架前缀
char get_shelf_prefix_by_shipping(ShippingMethod method) {
    switch (method) {
    case SHIPPING_COLD_CHAIN: return 'C';
    case SHIPPING_BULK:       return 'B';
    case SHIPPING_NORMAL:     return 'N';
    default:                  return 'N';
    }
}

// 查找可用货架
Shelf* find_available_shelf(Shelf* shelves, char prefix) {
    Shelf* curr = shelves;
    while (curr) {
        // 检查货架类型前缀匹配且当前数量未达最大容量
        if (toupper(curr->shelf_id[0]) == toupper(prefix) &&
            curr->current_count < curr->max_capacity) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// 独立的分配货架函数
/*找到可用的货架后将快递加入该货架的快递链表中*/
int assign_to_shelf(Express* express, Shelf* shelf_list, char shelf_prefix, time_t storage_time) {
    // 查找可用货架
    Shelf* target = find_available_shelf(shelf_list, shelf_prefix);
    if (!target) {// 检查是否找到可用货架
        printf("快递 %s 分配失败：%c类型货架不存在或已满！\n",
            express->tracking_num, shelf_prefix);
        return 0;
    }

    // 生成取件码（格式：货架ID-序号，如 N01-001）
    int seq_num = target->current_count + 1;// 获取当前序号
    snprintf(express->pickup_code, sizeof(express->pickup_code),
        "%s-%03d", target->shelf_id, seq_num);

    // 创建快递节点并加入货架链表
    ExpressNode* new_node = (ExpressNode*)malloc(sizeof(ExpressNode));
    if (!new_node) {
        perror("内存分配失败");
        return 0;
    }
    //  将快递添加到货架链表头部
    new_node->data = *express;// 复制快递数据
    new_node->next = target->pickup_list;// 插入链表头部
    target->pickup_list = new_node;
    target->current_count++;

    // 更新快递信息
    strncpy(express->shelf_id, target->shelf_id, sizeof(express->shelf_id) - 1);
    express->shelf_id[sizeof(express->shelf_id) - 1] = '\0';// 确保字符串终止
    express->storage_time = storage_time;
    express->status = STATUS_UNPICKED;

    printf("|-- 快递 %s 已存入 %s 货架，取件码：%s\n",
        express->tracking_num, target->shelf_id, express->pickup_code);

    // 保存修改到文件
    if (!save_shelves_to_file(shelf_list, "shelves.txt")) {
        printf("\n\033[33m警告：分配成功但保存失败！\033[0m\n");
    }
    return 1;
}

//从货架上删除快递
/*将快递从货架的快递链表中删除*/
bool remove_express_from_shelf(Shelf** shelves, const char* shelf_id, const char* tracking_num) {
    if (!shelves || !shelf_id || !tracking_num) {
        printf("\n错误：无效参数！\n");
        return false;
    }

    // 查找目标货架
    Shelf* curr_shelf = *shelves;
    while (curr_shelf && strcmp(curr_shelf->shelf_id, shelf_id) != 0) {
        curr_shelf = curr_shelf->next;
    }

    if (!curr_shelf) {
        printf("\n错误：未找到货架 %s\n", shelf_id);
        return false;
    }

    // 查找并移除快递节点
    ExpressNode* prev = NULL;
    ExpressNode* curr = curr_shelf->pickup_list;
    bool found = false;

    while (curr) {
        if (strcmp(curr->data.tracking_num, tracking_num) == 0) {
            found = true;
            // 从链表中移除节点
            if (prev) prev->next = curr->next;
            else curr_shelf->pickup_list = curr->next;

            curr_shelf->current_count--;
            free(curr);
            printf("\n\033[32m成功：快递 %s 已从货架 %s 移除\033[0m\n",
                tracking_num, shelf_id);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) {
        printf("\n错误：货架 %s 上未找到快递 %s\n", shelf_id, tracking_num);
        return false;
    }

    // 保存修改到文件
    if (!save_shelves_to_file(*shelves, "shelves.txt")) {
        printf("\n\033[33m警告：移除成功但保存失败！\033[0m\n");
    }

    return true;
}

