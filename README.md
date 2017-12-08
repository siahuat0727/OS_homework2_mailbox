# OS_homework2_mailbox

嗯……跑起來算順利，應該沒什麼大問題，有問題歡迎留言告訴我 ^.*

作業說明有誤，Insert module的部分：
```
sudo insmod mailbox.ko num_entry_max=2 (大小可改)
```

測試時可執行:
```
sudo ./master -q apple -d dirs -s 2 (數量可改)
```
