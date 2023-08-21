
**liblantern.so** 是一个 Android 5.x ~ 11 按需加载ttf/ttc/otf的拓展库，它用以缓解32位APP虚拟内存紧张的问题。

## 背景
目前32位APP常见虚拟内存治理方案：
| 方案 | 使用范围 | 节省大小 | 开源方 | 链接 |
| :----: | :----: | :----: | :----: |:----: |
| 线程栈空间减半 | Android 4.x及以上 | 一个线程可节省0.5M | 腾讯 | [matrix](https://github.com/Tencent/matrix) |
| 释放webview预占用内存 | Android 5.x及以上 | 100 ～ 130M左右 | 腾讯 | [matrix](https://github.com/Tencent/matrix) |
| 堆内存释放  | Android 5.x ～ 7.x | 128M ～ 512M左右 | 腾讯 | [matrix](https://github.com/Tencent/matrix) |
| 堆内存压缩  | Android 8.x及以上 | 动态压缩 | 阿里 | [Patrons](https://github.com/alibaba/Patrons) |

## 原理
Android 5.x ～ 11 zygote 进程一股脑会加载诸多字体库，所占系统虚拟内存 30M ～ 140M左右，
对于32位APP来说非常浪费系统虚拟内存，本方案按照“以新换旧，按需加载”的原则自研字体库扩展加载方案，以达到Android 12 按需分配的效果。
