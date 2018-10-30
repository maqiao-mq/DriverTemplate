这个目录下是块设备的写法。
块设备有两种写法，一种是把bio变成request，然后放到request_queue里面（一般是电梯算法），等到一定时间再将request的数据写到块设备上.  
另一种是直接处理bio，数据一来直接写块设备，比如内存盘。  
这个模板通过<span style="color:red">宏USE_QUEUE</span>来控制使用哪种工作方式。
默认开启了USE_QUEUE