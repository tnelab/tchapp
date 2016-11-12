#include "TchResourceHandler.h"
#include "cefclient/browser/test_runner.h"
namespace Tnelab {
	TchResourceHandler::ResourceRequestDelegate TchResourceHandler::user_resource_request_handle_;
	TchResourceHandler::TchResourceHandler() {
	}
	void TchResourceHandler::Cancel() {

	}
	bool TchResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback) {
		CEF_REQUIRE_IO_THREAD();

		if (TchResourceHandler::user_resource_request_handle_ == 0) return false;
		std::string url = request->GetURL();
		std::string schemes = url.substr(0, url.find(':'));
		int i = 0;
		schemes[i] = ::toupper(schemes[i]);
		while (schemes[i++])
			schemes[i] = toupper(schemes[i]);
		//if (schemes != "HTTP"&&schemes != "HTTPS") return false;
		//if (url.find("http://tchapp") != 0) return false;

		std::string datas = DumpRequestContents(url, schemes, request);
		int result = TchResourceHandler::user_resource_request_handle_(this,datas.c_str(), datas.length(),
			[](
				TchResourceHandler* handler,
				int status_code, const char* status_text, const char* mime_type, const char* header_map_str,
				const char* response_body_datas, int response_body_datas_size) {

			std::string header_str(header_map_str);
			size_t current_index = 0;
			size_t scan_index = 0;
			std::string key;
			std::string value;
			while (current_index < header_str.length()) {
				scan_index = header_str.find(':', current_index);
				key = header_str.substr(current_index, scan_index - current_index);
				current_index = scan_index + 2;
				scan_index = header_str.find_first_of("\r\n", current_index);
				value = header_str.substr(current_index, scan_index - current_index);
				handler->header_map_.insert(std::pair<const char*, const char*>(key.c_str(), value.c_str()));
				// printf("%s %s %lu\n", key.c_str(), value.c_str(), current_index);q
				if (scan_index == std::string::npos) {
					break;
				}
				current_index = scan_index + 1;
				if (current_index < header_str.length() &&
					header_str.at(current_index) == '\n') {
					++current_index;
				}
			}
			//std::ofstream fout("e:\\cef_out.txt",std::ios::app);
			//fout << header_map_str << ":"<< header_map.size()<<"\r\n";
			//fout.close();
			if (response_body_datas_size > 0) {
				handler->body_bytes_ = new char[response_body_datas_size];
				memcpy(handler->body_bytes_, response_body_datas, response_body_datas_size);
				handler->body_bytes_lenght_ = response_body_datas_size;
			}

			handler->status_code_ = status_code;
			handler->status_text_ = status_text;
			handler->mime_type_ = mime_type;
			return 0;
		});
		if(result == 0) callback->Continue();
		return result==0;
	}
	void TchResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
		int64& response_length,
		CefString& redirectUrl) {
		response->SetHeaderMap(this->header_map_);
		response->SetStatus(this->status_code_);
		response->SetMimeType(this->mime_type_);
		response->SetStatusText(this->status_text_);
		response_length = this->body_bytes_lenght_;
	}
	bool TchResourceHandler::ReadResponse(void* data_out,
		int bytes_to_read,
		int& bytes_read,
		CefRefPtr<CefCallback> callback) {
		bytes_read = 0;
		if (this->body_bytes_lenght_ < 1) return false;
		if (this->body_bytes_offset_ >= this->body_bytes_lenght_) return false;

		memcpy(data_out, this->body_bytes_ + this->body_bytes_offset_, bytes_to_read);
		bytes_read = bytes_to_read;
		body_bytes_offset_ += bytes_read;
		return true;		
	}
	bool TchResourceHandler::CanSetCookie(const CefCookie& cookie) {
		return true;
	}
	bool TchResourceHandler::CanGetCookie(const CefCookie& cookie) {
		return true;
	}

	std::string TchResourceHandler::DumpRequestContents(std::string url, std::string schemes, CefRefPtr<CefRequest> request) {
		std::stringstream ss;
		ss << std::string(request->GetMethod()) << " ";
		ss << static_cast<std::string>(url) << " ";

		ss << schemes << "/1.1\r\n";
		CefRequest::HeaderMap headerMap;
		request->GetHeaderMap(headerMap);
		if (headerMap.size() > 0) {
			CefRequest::HeaderMap::const_iterator it = headerMap.begin();
			for (; it != headerMap.end(); ++it) {
				ss << std::string((*it).first) << ": " << std::string((*it).second) << "\r\n";
			}
		}
		//std::ofstream fout("e:\\cef_out.txt", std::ios::app);
		//fout << ss.str() << ":" << headerMap.size() << "\r\n";
		//fout.close();

		CefRefPtr<CefPostData> postData = request->GetPostData();
		if (postData.get()) {
			ss << "\r\n";
			CefPostData::ElementVector elements;
			postData->GetElements(elements);
			if (elements.size() > 0) {
				CefRefPtr<CefPostDataElement> element;
				CefPostData::ElementVector::const_iterator it = elements.begin();
				for (; it != elements.end(); ++it) {
					element = (*it);
					if (element->GetType() == PDE_TYPE_BYTES) {
						if (element->GetBytesCount() == 0) {
						}
						else {
							// retrieve the data.
							size_t size = element->GetBytesCount();
							char* bytes = new char[size];
							element->GetBytes(size, bytes);
							ss << std::string(bytes, size);
							delete[] bytes;
						}
					}
					else if (element->GetType() == PDE_TYPE_FILE) {
						std::string name = std::string(element->GetFile());
						//size_t size = element->GetBytesCount();
						//char* bytes = new char[size];
						//element->GetBytes(size, bytes);
						//ss << std::string(bytes, size);
						//delete[] bytes;
					}
				}
			}
		}

		return ss.str();
	}
	void TchResourceHandler::SetResourceRequestDelegate(ResourceRequestDelegate delegate) {
		if (TchResourceHandler::user_resource_request_handle_ != 0) return;
		TchResourceHandler::user_resource_request_handle_ = delegate;
	}
}
