'use strict';
var Tch = {};
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
Tch.AddCaptionRect = function (rect) {
    let callback;
    let len = arguments.length;
    if (len === 1) {
        callback = function (result) { }
    }
    else {
        callback = arguments[1];
    }
    let invoke_body = JSON.stringify(rect);
    let request_body = { TchQuery: "AddCaptionRect", Arguments: invoke_body };
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
$(function () {
    if ($("*[data-tch-caption=true]").length > 0) {
        let caption_rect = {};
        caption_rect.X = $("*[data-tch-caption=true]").first().offset().left;
        caption_rect.Y = $("*[data-tch-caption=true]").first().offset().top;
        caption_rect.Width = $("*[data-tch-caption=true]").first().width();
        caption_rect.Height = $("*[data-tch-caption=true]").first().height();
        Tch.AddCaptionRect(caption_rect, function (result) { console.log(result); });
    }
    alert("hi");
});