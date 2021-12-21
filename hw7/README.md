# Chapter 22: SIGNALS
### 22-3 Compare sigwaitinfo() with sigsuspend()

#### 1.  [Usage] ./sigwaitinfo num-sigs

![image](https://user-images.githubusercontent.com/75157669/144007824-54edd492-0ca2-4f06-a516-3e3e79bff8b3.png)
* Time cost of sigwaitinfo is: 6.5 secs

#### 2. [Usage] ./sigsuspend num-sigs

![image](https://user-images.githubusercontent.com/75157669/144008481-0b5b89b4-5441-4a2b-8867-76face9e99f0.png)
* Time cost of sigsuspend is: 18.3 secs

### 22-4 Implement the System V functions
```
sigset(), sighold(), sigrelse(), sigignore(), and sigpause()
```
![image](https://user-images.githubusercontent.com/75157669/144008745-8ef6f008-6fdf-4845-bdf7-4a92e08f75db.png)
