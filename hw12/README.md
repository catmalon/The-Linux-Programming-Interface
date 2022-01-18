# Linux Drivers
## 簡介
Linux 將 driver 分為三種型態，分別是字元、區塊和網路設備，本章將以最常用的字元裝置當作例子。一般而言字元裝置可當作一般檔案存取，包含基本的open、close、I/O control、read 和 write。在 driver 的基本架構中，我們首先向
系統註冊一個 driver，再向系統註冊我們所提供的 open、close、read 和 write 的服務即可。我們將這幾項服務列成 event 來看，並且一步步引導大家來實踐這些event。
* Initial module
  * 當 driver 被載入之後第一個被呼叫的函式，類似一般 C 語言中的 main function，在此 function 中向系統註冊為字元 device 和所提供的服務
* Open device
  * 當我們的 device 被 fopen 之類的函式開啟時所執行的對應處理函式
* Close device
  * 使用者程式關閉我們的 device 時執行的對應處理函式
--------------------------------------------------------
* I/O control
  * 使用者可透過 ioctl 命令設定 device 的一些參數
* Read device
  * 當程式從我們的 device 讀取資料時對應的處理函式
* Write device
  * 當程式對我們的 device 寫入資料時對應的處理函式
* Remove module
  * 當 driver 被移除時所執行的處理函式，必須對系統取消註冊 device
### demo1 Basic
![make](https://user-images.githubusercontent.com/75157669/149881741-78d84d7a-7718-4c2a-80dc-24d6338a5118.jpg)
![insmod](https://user-images.githubusercontent.com/75157669/149882427-ebb3cc06-a3cc-4924-aa61-28e9d2cb8cef.jpg)

* Install driver:
`insmod demo1.ko`

* List driver:
`lsmod`

* Remove driver:
`rmmod demo1`

### demo2 Add Initial Module & Remove Module
status from `/var/log/syslog`

![demo start   demo remove](https://user-images.githubusercontent.com/75157669/149882093-c8f69d82-0299-44a1-ac10-327eae51a57f.jpg)
![init func   exit func](https://user-images.githubusercontent.com/75157669/149882360-b8454da1-0b72-4f72-9daa-b1f1cb0281bc.jpg)


### demo Add open、close、I/O control、read & write
