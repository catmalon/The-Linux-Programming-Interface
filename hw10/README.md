# Producer & Consumer
![image](https://user-images.githubusercontent.com/75157669/149868358-05ac7694-4800-46d0-9fbd-0192940d5ab4.png)

### **./program [DataNum M] [TrxInterval(ms) R] [ConsumerNum N] [BuffNum B]** 
* DataNum: total number of transfer data
* TrxInterval: interval time between two data generation 
* ConsumerNum: Number of consumer
* BuffNum: Number of Buffer for temporary storage data

1. Producer 與 Consumer 皆以 process 來開發。
2. Producer 負責產生 M 個資料(例如：M=1000，資料內容”This is message 0”)，資料間隔 R millisecond (例如：R=300 ms，你會需要用到 usleep() 函式)，然後設法將資料交給所有 Consumer (例如：N=200)。
3. R 越小代表資料產生越快，每份資料都是文字資料，內含序號，如上例。
4. 你必須要設計適當的資料傳遞之方式，否則當 N 很大時，consumer 會來不及收到資料。
5. 系統只能有 B 個 buffers 來暫存 B 筆資料(例如：B=3)，第 B 筆資料， 會蓋到第 0 筆資料。所以資料必須被及時收到，否則會被蓋掉，如下圖依此類推：

![image](https://user-images.githubusercontent.com/75157669/149880482-7c20b048-5976-4d15-bdce-f68d5efcb684.png)

**限制：**
1. 每個 Data 內有 80 個 char(如: char message[80];)，且每個 message 都要內含序號(如：0、1、2、3…)，讓接收訊息的 consumer 知道此為第幾份資料。
2. 本作業優先使用Shared Memory + signal 的解決方法(若有更好解法不受此限)，consumer 才能及時讀取資料。Consumer 被動被通知去讀取資料，signal通知時訊息中必須帶資料序號(上課有教如何用 sigaction()加參數到 signal 中)。
3. 輸出樣式為全部 consumers 總共接收到資料量，並且計算出 loss rate，如下範例：以資料數量 M=1000，傳送速率 R=300，N=150，B=3 為例，輸出應為：
```
M＝1000 R=300 ms N=150
--------------------------------------------
Total messages: 150,000 (1000*150)
Sum of received messages by all consumers: X
Loss rate: 1 - (X/150000)
--------------------------------------------
```

![image](https://user-images.githubusercontent.com/75157669/149868321-261dc956-d8bc-4b98-96a3-c2f4b29da68b.png)
![image](https://user-images.githubusercontent.com/75157669/149868334-abc5f59c-168d-4144-bfc3-b66fde34cd81.png)
