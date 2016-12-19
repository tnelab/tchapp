// Called due to cefQuery execution in binding.html.
#include "TchWindowApi.h"
#include "TchQueryHandler.h"
#include "cefclient/browser/main_context.h"
#include "cefclient/browser/root_window.h"
using namespace client;
namespace Tnelab {
	//资源处理委托
	TchQueryHandler::JsInvokeDelegate TchQueryHandler::user_js_invoke_handle_ = 0;

	TchQueryHandler::TchQueryHandler() { InitQueryProcessorMap_(); }
	TchQueryHandler::~TchQueryHandler() {
		query_processor_map_.clear();
	}
	bool TchQueryHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback){
		CEF_REQUIRE_UI_THREAD();
		CefRefPtr<CefValue> value = CefParseJSON(request, JSON_PARSER_RFC);
		if (!(value.get() && value->GetType() == VTYPE_DICTIONARY)) return false;
		auto request_dict = value->GetDictionary();
		auto tch_query = request_dict->GetString("TchQuery");
		auto it = query_processor_map_.find(tch_query);
		if (it == query_processor_map_.end()) return false;
		return query_processor_map_[tch_query](request_dict, frame, query_id, request, persistent, callback);
	}
	//设置用户Js调用委托
	void TchQueryHandler::SetJsInvokeDelegate(JsInvokeDelegate delegate) {
		user_js_invoke_handle_ = delegate;
	}

	void TchQueryHandler::InitQueryProcessorMap_() {
		query_processor_map_["JsInvoke"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->JsInvokeProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["SetCaptionRect"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->SetCaptionRectProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["CloseWindow"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->CloseWindowProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["MinimizeWindow"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->MinimizeWindowProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["MaximizingWindow"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->MaximizingWindowProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["RestoreWindow"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->RestoreWindowProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["HideWindow"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->HideWindowProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["ShowWindow"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->ShowWindowProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["SetWindowBorder"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->SetWindowBorderProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["SetWindowPos"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->SetWindowPosProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
		query_processor_map_["GetWindowPos"] = [this](CefRefPtr<CefDictionaryValue> request_dic, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
			return this->GetWindowPosProcessor_(request_dic, frame, query_id, request, persistent, callback);
		};
	}

	//TchQueryProcessor List
	//处理JsInvoke
	bool TchQueryHandler::JsInvokeProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		if (user_js_invoke_handle_ == 0) {
			callback->Failure(-1, "JsInvokeDelegate is null");
			return true;
		}
		auto invoke_json = request_dict->GetString("Arguments");
		auto result = user_js_invoke_handle_(invoke_json.c_str());
		callback->Success(result);
		return true;
	}
	//处理SetCaptionRect
	bool TchQueryHandler::SetCaptionRectProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		auto invoke_json = request_dict->GetString("Arguments");
		auto json_val=CefParseJSON(invoke_json, JSON_PARSER_RFC);
		auto json_dic = json_val->GetDictionary();
		CefRect rect;
		rect.x=json_dic->GetDouble("X");
		rect.y = json_dic->GetDouble("Y");
		rect.width = json_dic->GetDouble("Width");
		rect.height = json_dic->GetDouble("Height");
		scoped_refptr<client::RootWindow> root_window =
			client::RootWindow::GetForBrowser(frame->GetBrowser()->GetIdentifier());
		auto ptr_tch_window_settings = TchWindowApi::GetSettings(reinterpret_cast<unsigned long>(root_window->GetWindowHandle()));
		ptr_tch_window_settings->CaptionRect = rect;
		callback->Success("true");
		return true;
	}
	//处理CloseWindow
	bool TchQueryHandler::CloseWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		TchWindowApi::CloseWindow(frame);
		callback->Success("true");
		return true;
	}
	//处理MinimizeWindow
	bool TchQueryHandler::MinimizeWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		TchWindowApi::MinimizeWindow(frame);
		callback->Success("true");
		return true;
	}
	//处理MaximizingWindow
	bool TchQueryHandler::MaximizingWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		TchWindowApi::MaximizingWindow(frame);
		callback->Success("true");
		return true;
	}
	//处理RestoreWindow
	bool TchQueryHandler::RestoreWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		TchWindowApi::RestoreWindow(frame);
		callback->Success("true");
		return true;
	}
	//处理HideWindow
	bool TchQueryHandler::HideWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		TchWindowApi::HideWindow(frame);
		callback->Success("true");
		return true;
	}
	//处理ShowWindow
	bool TchQueryHandler::ShowWindowProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		TchWindowApi::ShowWindow(frame);
		callback->Success("true");
		return true;
	}
	//处理SetWindowBorder
	bool TchQueryHandler::SetWindowBorderProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		auto invoke_json = request_dict->GetString("Arguments");
		auto json_val = CefParseJSON(invoke_json, JSON_PARSER_RFC);
		auto json_dic = json_val->GetDictionary();
		scoped_refptr<client::RootWindow> root_window =
			client::RootWindow::GetForBrowser(frame->GetBrowser()->GetIdentifier());
		auto ptr_tch_window_settings = TchWindowApi::GetSettings(reinterpret_cast<unsigned long>(root_window->GetWindowHandle()));
		ptr_tch_window_settings->WindowBorderWidth= json_dic->GetInt("Border");
		callback->Success("true");
		return true;
	}
	//处理SetWindowPos
	bool TchQueryHandler::SetWindowPosProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		auto invoke_json = request_dict->GetString("Arguments");
		auto json_val = CefParseJSON(invoke_json, JSON_PARSER_RFC);
		auto json_dic = json_val->GetDictionary();
		TchWindowApi::SetWindowPos(frame,json_dic->GetInt("X"), json_dic->GetInt("Y"), json_dic->GetInt("Width"), json_dic->GetInt("Height"));
		callback->Success("true");
		return true;
	}
	//处理GetWindowPos
	bool TchQueryHandler::GetWindowPosProcessor_(CefRefPtr<CefDictionaryValue> request_dict, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
		CefRect rect;
		TchWindowApi::GetWindowPos(frame,rect);
		CefRefPtr<CefDictionaryValue> dictionary = CefDictionaryValue::Create();
		dictionary->SetInt("X", rect.x);
		dictionary->SetInt("Y", rect.y);
		dictionary->SetInt("Width", rect.width);
		dictionary->SetInt("Height", rect.height);
		CefRefPtr<CefValue> value = CefValue::Create();
		value->SetDictionary(dictionary);
		
		callback->Success(CefWriteJSON(value, JSON_WRITER_DEFAULT));
		return true;
	}

}