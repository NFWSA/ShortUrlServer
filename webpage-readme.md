我有一个短链接服务器，假定监听地址在 localhost:8080，共有以下几种api（用curl指令演示）：
+ 获取短链接
curl locahost:8080/add -d '{"url":"https://example.com/url"}'
成功响应为 {"code":0,"msg":"ok","data":"f9c53e9d7284"} 其中 f9c53e9d7284 即为短链接key
+ 删除短链接
curl locahost:8080/del -d '{"hash":"f9c53e9d7284"}'
或者
curl locahost:8080/del -d '{"url":"https://example.com/url"}'
成功找到并删除响应为 {"code":0,"msg":"ok"}
+ 获取hash对应连接
curl locahost:8080/get -d '{"hash":"f9c53e9d7284"}'
成功找到连接响应为 {"code":0,"msg":"ok","data":"https://example.com/url"}
+ 重定向
可以直接通过浏览器访问对应链接实现302重定向跳转，对应的连接为 locahost:8080/jump/f9c53e9d7284

根据这些信息，帮我写一个简单的纯静态界面，做一个简单的展示页面，可以输入url创建短链接并复制，可以输入hash查询链接，可以输入hash或url删除连接
