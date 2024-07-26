/*
 * @Author       : mark
 * @Date         : 2020-06-27
 * @copyleft Apache 2.0
 */ 
#include "httpresponse.h"

using namespace std;

const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

const unordered_map<int, string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

const vector<string> HttpResponse::RESOURCES={
    "/index.html",
    "/register.html",
    "/upload.html"
};
// unordered_map<string,int*> RESOURCE_MAP={};

// void HttpResponse::init_resouce(){
//      for(auto i:RESOURCES){
//         int fd=open((srcDir_ + i).data(),O_RDONLY);
//         int size = lseek(fd, 0, SEEK_END); 
//         int* ptr=(int *)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);
//         RESOURCE_MAP[i]=ptr;
//     }
// };

HttpResponse::HttpResponse() {
    code_ = -1;
    path_ = srcDir_ = "";
    isKeepAlive_ = false;
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
    init_resouce();
};

HttpResponse::~HttpResponse() {
    UnmapFile();
}

void HttpResponse::Init(const string& srcDir, string& path, bool isKeepAlive, int code){
    assert(srcDir != "");           //他不是  空字符串则继续想下走
    if(mmFile_) { UnmapFile(); }
    //如果指针不为空，则对他进行释放            内存映射
    code_ = code;       //状态码
    isKeepAlive_ = isKeepAlive;
    path_ = path;
    srcDir_ = srcDir;
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };    //表示文件状态信息的
    
}

void HttpResponse::MakeResponse(Buffer& buff) {
    /* 判断请求的资源文件 */
    //查看资源的额状态，保存到   mmFileStat小于0就是调用失败了   判断是不是目录，就返回失败
    if(stat((srcDir_ + path_).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)) {
        code_ = 404;
    }
    //判断权限的话，  如果没有权限，则返回   403
    else if(!(mmFileStat_.st_mode & S_IROTH)) {
        code_ = 403;
    }
    //默认code是  -1
    else if(code_ == -1) { 
        code_ = 200; 
    }
    // LOG_INFO("*******************************正在生成响应信息");
    ErrorHtml_();
    AddStateLine_(buff);
    AddHeader_(buff);
    // LOG_INFO("*******************************正在生成响应信息2");
    AddContent_(buff);
    // LOG_INFO("*******************************正在生成响应信息3");
}

char* HttpResponse::File() {
    return mmFile_;
}

size_t HttpResponse::FileLen() const {
    return mmFileStat_.st_size;
}

void HttpResponse::ErrorHtml_() {
    if(CODE_PATH.count(code_) == 1) {
        path_ = CODE_PATH.find(code_)->second;
        stat((srcDir_ + path_).data(), &mmFileStat_);
    }
}

void HttpResponse::AddStateLine_(Buffer& buff) {
    string status;
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    }
    else {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buff.Append("HTTP/1.1 " + to_string(code_) + " " + status + "\r\n");
}

void HttpResponse::AddHeader_(Buffer& buff) {
    buff.Append("Connection: ");
    if(isKeepAlive_) {
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6, timeout=120\r\n");
    } else{
        buff.Append("close\r\n");
    }
    buff.Append("Content-type: " + GetFileType_() + "\r\n");
}

void HttpResponse::AddContent_(Buffer& buff) {
    // LOG_INFO("addcontent path %s", (path_).data());
    // if(RESOURCE_MAP.find(path_)!=RESOURCE_MAP.end()){
    //     mmFile_=(char*)RESOURCE_MAP[path_];
    //     // LOG_INFO(mmFile_);
    //     LOG_INFO("初始化资源路径命中file path %s", (srcDir_ + path_).data());
    // }
    // else{
    int srcFd = open((srcDir_ + path_).data(), O_RDONLY);
    if(srcFd < 0) { 
        ErrorContent(buff, "File NotFound!");
        return; 
    }

    /* 将文件映射到内存提高文件的访问速度 
        MAP_PRIVATE 建立一个写入时拷贝的私有映射*/
    LOG_DEBUG("file path %s", (srcDir_ + path_).data());
    int* mmRet = (int*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if(*mmRet == -1) {
        ErrorContent(buff, "File NotFound!");
        return; 
    }
    mmFile_ = (char*)mmRet;
    close(srcFd);
    // }
    buff.Append("Content-length: " + to_string(mmFileStat_.st_size) + "\r\n\r\n");
    
}

void HttpResponse::UnmapFile() {
    if(mmFile_) {
        //取消映射
        // 在使用 munmap 取消映射之后，之前通过 mmFile_ 访问的内存区域将不再有效，
        //避免在取消映射后继续使用 mmFile_。
        // 取消映射后，需要确保不再需要访问映射的文件内容，否则可能导致未定义的行为或程序崩溃。
        // munmap 的调用通常发生在文件处理完成或者不再需要文件内容映射时，以释放系统资源并确保程
        //序的稳定性和效率。
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}

string HttpResponse::GetFileType_() {
    /* 判断文件类型 */
    string::size_type idx = path_.find_last_of('.');
    if(idx == string::npos) {
        return "text/plain";
    }
    string suffix = path_.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

void HttpResponse::ErrorContent(Buffer& buff, string message) 
{
    string body;
    string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    } else {
        status = "Bad Request";
    }
    body += to_string(code_) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";
    buff.Append("Content-length: " + to_string(body.size()) + "\r\n\r\n");
    buff.Append(body);
}
