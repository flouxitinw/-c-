#include "cainiao.h"


// 辅助函数实现
void pause_program() {
    printf("\n按任意键继续...");
    fflush(stdout);

    // 完全清空输入缓冲区
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // 等待新输入
    //getchar();
}

void clear_screen() {
    system("cls || clear");
}
// 缓冲区清理
void clear_input_buffer() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

