'use strict';
(function () {
    window.Tch = {};
    //调用本机代码
    Tch.JsInvoke = function (invoke_obj, callback) {
        let invoke_body = invoke_obj;
        if (!(Object.prototype.toString.call(invoke_obj) === "[object String]")) invoke_body = JSON.stringify(invoke_body);
        let request_body = { TchQuery: "JsInvoke", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                alert(error_code + ":" + error_message);
            }
        });
    }
    //添加标题区域，用于移动窗口
    Tch.SetCaptionRect = function (rect) {
        let callback;
        let len = arguments.length;
        if (len === 1) {
            callback = function (result) { }
        }
        else {
            callback = arguments[1];
        }
        let invoke_body = JSON.stringify(rect);
        let request_body = { TchQuery: "SetCaptionRect", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }

    //关闭窗口
    Tch.CloseWindow = function () {
        let callback;
        let len = arguments.length;
        if (len === 0) {
            callback = function (result) { }
        }
        else {
            callback = arguments[0];
        }
        let invoke_body = "";
        let request_body = { TchQuery: "CloseWindow", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //最小化窗口
    Tch.MinimizeWindow = function () {
        let callback;
        let len = arguments.length;
        if (len === 0) {
            callback = function (result) { }
        }
        else {
            callback = arguments[0];
        }
        let invoke_body = "";
        let request_body = { TchQuery: "MinimizeWindow", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //最大化窗口
    Tch.MaximizingWindow = function () {
        let callback;
        let len = arguments.length;
        if (len === 0) {
            callback = function (result) { }
        }
        else {
            callback = arguments[0];
        }
        let invoke_body = "";
        let request_body = { TchQuery: "MaximizingWindow", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //恢复窗口
    Tch.RestoreWindow = function () {
        let callback;
        let len = arguments.length;
        if (len === 0) {
            callback = function (result) { }
        }
        else {
            callback = arguments[0];
        }
        let invoke_body = "";
        let request_body = { TchQuery: "RestoreWindow", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //隐藏窗口
    Tch.HideWindow = function () {
        let callback;
        let len = arguments.length;
        if (len === 0) {
            callback = function (result) { }
        }
        else {
            callback = arguments[0];
        }
        let invoke_body = "";
        let request_body = { TchQuery: "HideWindow", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //显示窗口
    Tch.ShowWindow = function () {
        let callback;
        let len = arguments.length;
        if (len === 0) {
            callback = function (result) { }
        }
        else {
            callback = arguments[0];
        }
        let invoke_body = "";
        let request_body = { TchQuery: "ShowWindow", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //设置边框大小
    Tch.SetWindowBorder = function (width) {
        let callback;
        let len = arguments.length;
        if (len == 2) {
            callback = arguments[1];
        }
        else {
            callback = function (result) { }
        }
        let obj = { Border: width };
        let invoke_body = JSON.stringify(obj);
        let request_body = { TchQuery: "SetWindowBorder", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }

    //设置窗口位置和大小
    Tch.SetWindowPos = function (x,y,width,height) {
        let callback;
        let len = arguments.length;
        if (len ==5) {
            callback = arguments[4];
        }
        else {
            callback = function (result) { }
        }
        let obj = { X:x,Y:y,Width: width,Height:height };
        let invoke_body = JSON.stringify(obj);
        let request_body = { TchQuery: "SetWindowPos", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }

    //获取窗口位置和大小
    Tch.GetWindowPos = function (callback) {
        let callback_;
        let len = arguments.length;
        if (len == 1) {
            callback_ = function (result) {
               callback(JSON.parse(result));
            }
        }
        else {
            callback_ = function (result) { }
        }
        let obj = {};
        let invoke_body = JSON.stringify(obj);
        let request_body = { TchQuery: "GetWindowPos", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback_(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }
    //获取TchAppDomainName
    Tch.GetTchAppDomainName = function (callback) {
        let callback_;
        let len = arguments.length;
        if (len == 1) {
            callback_ = callback;
        }
        else {
            callback_ = function (result) { }
        }
        let obj = {};
        let invoke_body = JSON.stringify(obj);
        let request_body = { TchQuery: "GetTchAppDomainName", Arguments: invoke_body };
        window.cefQuery({
            request: JSON.stringify(request_body),
            onSuccess: function (response) {
                callback_(response);
            },
            onFailure: function (error_code, error_message) {
                console.log(error_code + ":" + error_message);
            }
        });
    }

    //初始化
    Tch.Ready = function () {
        let set_caption_rect = function () {
            //处理标题栏,内部ID：tch_window_caption表示标题栏
            let caption_elm = document.getElementById("tch_window_caption");
            if (caption_elm != undefined) {
                let caption_rect = {};
                caption_rect.X = caption_elm.offsetLeft;
                caption_rect.Y = caption_elm.offsetTop;
                caption_rect.Width = caption_elm.offsetWidth;
                caption_rect.Height = caption_elm.offsetHeight;
                Tch.SetCaptionRect(caption_rect, function (result) { console.log(result); });
            }
        }
        set_caption_rect();
        window.addEventListener("resize", set_caption_rect);
        //处理window设置
        let html_settings_attr=document.getElementsByTagName("html")[0].attributes.getNamedItem("data-tch-window");
        if (html_settings_attr != undefined) {
            let window_settings = html_settings_attr.value;
            try {
                window_settings = eval("window_settings=" + window_settings);
            }
            catch (err) {
                alert("data-tch-window error:" + err);
            }
            //处理边框
            if (!isNaN(window_settings.Border) && window_settings.Border > 0) {
                Tch.SetWindowBorder(window_settings.Border);
            }
        }
    }

    let dom_ready_ = function () {
        document.removeEventListener("DOMContentLoaded", dom_ready_, false);
        Tch.Ready();
    }
    document.addEventListener("DOMContentLoaded", dom_ready_, false);
})();