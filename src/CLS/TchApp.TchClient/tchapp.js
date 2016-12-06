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
//添加标题区域的排除区域，用于在标题栏中放置按钮
Tch.AddCaptionClipRect = function (rect) {
    let callback;
    let len = arguments.length;
    if (len === 1) {
        callback = function (result) { }
    }
    else {
        callback = arguments[1];
    }
    let invoke_body = JSON.stringify(rect);
    let request_body = { TchQuery: "AddCaptionClipRect", Arguments: invoke_body };
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
        let caption_elm = $("*[data-tch-caption=true]").first();
        setTimeout(function () {
            let caption_rect = {};
            caption_rect.X = caption_elm.offset().left;
            caption_rect.Y = caption_elm.offset().top;
            caption_rect.Width = caption_elm.width();
            caption_rect.Height = caption_elm.height();
            Tch.SetCaptionRect(caption_rect, function (result) { console.log(result); });
        }, 100);
        $(caption_elm).find("*[data-tch-caption=false]").each(function () {
            let clip_elm = $(this);
            setTimeout(function () {
                let caption_rect = {};
                caption_rect.X = clip_elm.offset().left;
                caption_rect.Y = clip_elm.offset().top;
                caption_rect.Width = clip_elm.width();
                caption_rect.Height = clip_elm.height();
                Tch.AddCaptionClipRect(caption_rect, function (result) { console.log(result); });
            }, 100);
        });
    }
});