#include "task_uart.h"

/**
 * This example shows how to use the UART driver to handle special UART events.
 *
 * It also reads data from UART0 directly, and echoes it to console.
 *
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: on
 * - Pin assignment: TxD (default), RxD (default)
 */

#define PATTERN_CHAR_NUM    (3)         //关键字检索重复次数
#define BUF_SIZE (1024)                 //buf大小
#define RD_BUF_SIZE (BUF_SIZE)          //读取字符大小
static QueueHandle_t uart0_queue;       //串口队列
static const char* TAG = "UART0";

void TASK_UART(void* param)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);   //设置ESP32 log打印等级

    // Configure parameters of an UART driver,      配置串口驱动函数
    // communication pins and install the driver    通信引脚并安装驱动程序
    uart_config_t uart_config = {
        .baud_rate = 115200,                        //设置波特率
        .data_bits = UART_DATA_8_BITS,              //设置数据位
        .parity = UART_PARITY_DISABLE,              //设置奇偶校验 不使能校验
        .stop_bits = UART_STOP_BITS_1,              //一个停止位
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      //不使能硬件流控制
        .source_clk = UART_SCLK_APB,                //时钟源的选择
    };
    //Install UART driver, and get the queue.       //安装串口驱动 串口通道 接收buf 发送buf 事件队列 分配中断的标志
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(UART_NUM_0, &uart_config);    //串口参数配置

    esp_log_level_set(TAG, ESP_LOG_INFO);           //设置ESP32 log打印等级

    //Set UART pins (using UART0 default pins ie no changes.)   //设置串口引脚 串口号 发送引脚 接收引脚 rts引脚 cts引脚
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Set uart pattern detect function.             //设置串口参数自检功能
    uart_enable_pattern_det_baud_intr(UART_NUM_0, '+', PATTERN_CHAR_NUM, 11, 0, 0);

    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(UART_NUM_0, 20);      //重置模式队列长度，最多记录20个模式位置。

    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*)malloc(RD_BUF_SIZE); //动态申请内存

    while (1)
    {
        //Waiting for UART event.                   等待串口事件队列
        if (xQueueReceive(uart0_queue, (void*)&event, (TickType_t)portMAX_DELAY))
        {
            switch (event.type)
            {
                /*我们最好快速处理数据事件，会有更多的数据事件比其他类型
                的事件。如果我们在数据事件上花费太多时间，队列可能会充满。*/
            case UART_DATA:         // UART接收数据的事件
            {
                ESP_LOGI(TAG, "recv(%d)", event.size);
                uart_read_bytes(UART_NUM_0, dtmp, event.size, portMAX_DELAY);  //获取数据
                uart_write_bytes(UART_NUM_0, (const char*)dtmp, event.size);  //发送数据
            }break;
            case UART_PATTERN_DET:  //UART 输入样式检测事件
            {
                uart_get_buffered_data_len(UART_NUM_0, &buffered_size);    //UART获取RX环缓冲区缓存的数据长度
                int pos = uart_pattern_pop_pos(UART_NUM_0);                //返回在缓冲区中最近的检测到的模式位置
                ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                if (pos == -1) {
                    //曾经有一个UART_PATTERN_DET事件，但是模式位置队列已满，所以它不能
                    //记录位置。我们应该设置更大的队列大小。
                    //例如，我们直接刷新rx缓冲区。
                    uart_flush_input(UART_NUM_0);  //清除输入缓冲区，丢弃所有环缓冲区中的数据      
                }
                else {
                    uart_read_bytes(UART_NUM_0, dtmp, pos, 100 / portTICK_PERIOD_MS);              //读取数据
                    uint8_t pat[PATTERN_CHAR_NUM + 1];
                    memset(pat, 0, sizeof(pat));    //清除缓存           
                    uart_read_bytes(UART_NUM_0, pat, PATTERN_CHAR_NUM, 100 / portTICK_PERIOD_MS);   //读取数据
                    ESP_LOGI(TAG, "read data: %s", dtmp);
                    ESP_LOGI(TAG, "read pat : %s", pat);
                }
            }break;
            case UART_FIFO_OVF:     // 检测到硬件 FIFO 溢出事件 
            {
                ESP_LOGI(TAG, "hw fifo overflow");
                //如果fifo溢出发生，你应该考虑为你的应用程序添加流量控制。
                //ISR已经重置了rx FIFO，例如，我们直接刷新rx缓冲区来读取更多的数据。
                uart_flush_input(UART_NUM_0);   //清除输入缓冲区，丢弃所有环缓冲区中的数据
                xQueueReset(uart0_queue);       //重置一个队列到它原来的空状态。返回值是现在过时，并且总是设置为pdPASS。
            }break;
            case UART_BUFFER_FULL:  // UART RX 缓冲器满事件
            {
                ESP_LOGI(TAG, "ring buffer full");
                // 如果缓冲区满了，你应该考虑增加你的缓冲区大小
                // 举个例子，我们这里直接刷新 rx 缓冲区，以便读取更多数据。uart_flush_input(UART_NUM_0);
                xQueueReset(uart0_queue);       //重置一个队列到它原来的空状态
            }break;
            case UART_BREAK:        //UART 中断事件
            {
                ESP_LOGI(TAG, "uart rx break");
            }break;
            case UART_PARITY_ERR:   //UART奇偶校验错误事件
                ESP_LOGI(TAG, "uart parity error");
                break;
            case UART_FRAME_ERR:    //UART 帧错误事件
                ESP_LOGI(TAG, "uart frame error");
                break;
            default:                            //其他
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
            bzero(dtmp, RD_BUF_SIZE);               //清空动态申请的队列
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    free(dtmp);                                     //释放内存
    dtmp = NULL;                                    //清除内存指针
    vTaskDelete(NULL);                              //删除任务
}
