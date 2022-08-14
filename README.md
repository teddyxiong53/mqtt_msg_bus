# mqtt_msg_bus

实现一个基于mqtt的消息总线。

用来替代dbus做进程间通信。

mqtt broker就用mosquitto。

通信部分，其实mosquitto大部分都处理掉了。

我需要做的就是topic的设计。

这个需要认真梳理一下。形成自己的一套方法。



对于系统消息总线这个场景，我觉得topic尽量简单。

只要get、set、notify。

具体的消息内容放到json格式的payload里去解析得了。

那么topic就这么设计：

```
msgbus/get
msgbus/set
msgbus/notify
```

每一个使用mqtt msg bus进行通信的，都是相当于mosquitto的 client。

假设现在有2个进程audioservice和homeapp，需要通过mqtt msg bus进行通信。

```
audioservice
	接受homeapp的play/pause/input_settings控制命令。
	把playing/paused/volume这些状态通知给homeapp。
	那么就是要订阅get和set。
	要发布notify。
homeapp
	想audioservice发出控制命令。查询状态。
	接受audioservice的通知。
	那么就是要定义notify。
	要发布get和set。
	
```

先就基于这个简单的进行实现看看。

编译环境，就简单点。

把这个代码下载下来。

https://github.com/eclipse/mosquitto

编译并install。

有这些库文件：

```

├── libmosquittopp.so -> libmosquittopp.so.1
├── libmosquittopp.so.1
├── libmosquitto.so -> libmosquitto.so.1
├── libmosquitto.so.1
└── pkgconfig
    ├── libmosquitto.pc
    └── libmosquittopp.pc
```

这些头文件：

```
├── mosquitto_broker.h
├── mosquitto.h
├── mosquitto_plugin.h
├── mosquittopp.h
└── mqtt_protocol.h
```

现在写了一个基本的架子。

运行发现一点问题。

因为homeapp和audioservice都订阅了同样的topic。

所以他们自己发出的东西，自己也会收到。

当然可以在处理消息的时候过滤掉。

但是最好还是不要收到。

把进程的名字作为topic的一部分，会不会有改善？

例如这样：

```
msgbus/homeapp/get
```

这样似乎绑定还是太死板了。

其实不管mqtt。

从audioservice和homeapp的业务逻辑本身来看。

audioservice还是作为server的。

homeapp还是作为client的。

这样改造一下是不是好一点：

```
msgbus/server/get
msgbus/server/set
msgbus/client/notify

上面server和client，表示消息的目的地。
从server进行get/set
向client进行notify。
```

为了双向通信。虽然有几个实际不用，但是也可以加上：

```
server向client进行get/set
msgbus/client/get
msgbus/client/set

```

基本上这些就可以进行基本的通信了。



