#include <utime.h>
#include "tair_config.hpp"
#include "mmap_file.hpp"
#include "stringutil.h"

using namespace std;

namespace tair {

    static Configer  _config;

    /**
     * 构造函数
     */
    Configer::Configer()
    {
    }

    /**
     * 析构函数
     */
    Configer::~Configer()
    {
        for(STR_MAP_ITER it=m_configMap.begin(); it!=m_configMap.end(); ++it) {
            delete it->second;
        }
    }

    /**
     * 得到静态实例
     */
    Configer& Configer::getConfiger()
    {
        return _config;
    }

    /**
     * delete it->second;
     * 解析字符串
     */
    int Configer::parseValue(char *str, char *key, char *val)
    {
        char           *p, *p1, *name, *value;
    
        if (str == NULL)
            return -1;
    
        p = str;
        // 去前置空格
        while ((*p) == ' ' || (*p) == '\t' || (*p) == '\r' || (*p) == '\n') p++;
        p1 = p + strlen(p);
        // 去后置空格
        while(p1 > p) {
            p1 --;
            if (*p1 == ' ' || *p1 == '\t' || *p1 == '\r' || *p1 == '\n') continue;
            p1 ++;
            break;
        }
        (*p1) = '\0';
        // 是注释行或空行
        if (*p == '#' || *p == '\0') return -1;
        p1 = strchr(str, '=');
        if (p1 == NULL) return -2;
        name = p;
        value = p1 + 1;
        while ((*(p1 - 1)) == ' ') p1--;
        (*p1) = '\0';
    
        while ((*value) == ' ') value++;
        p = strchr(value, '#');
        if (p == NULL) p = value + strlen(value);
        while ((*(p - 1)) <= ' ') p--;
        (*p) = '\0';
        if (name[0] == '\0')
            return -2;
    
        strcpy(key, name);
        strcpy(val, value);
    
        return 0;
    }

    /* 是段名 */
    char *Configer::isSectionName(char *str) {
        if (str == NULL || strlen(str) <= 2 || (*str) != '[') 
            return NULL;
            
        char *p = str + strlen(str);
        while ((*(p-1)) == ' ' || (*(p-1)) == '\t' || (*(p-1)) == '\r' || (*(p-1)) == '\n') p--;
        if (*(p-1) != ']') return NULL;
        *(p-1) = '\0';
        
        p = str + 1;
        while(*p) {
            if ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || (*p == '_')) {
            } else {
                return NULL;
            }
            p ++;
        }
        return (str+1);
    }
    
    /**
     * 加载文件
     */
    int Configer::load(const char *filename)
    {
        FILE           *fp;
        char            key[128], value[4096], data[4096];
        int             ret, line = 0;
        
        if ((fp = fopen(filename, "rb")) == NULL) {
            TBSYS_LOG(ERROR, "不能打开配置文件: %s", filename);
            return EXIT_FAILURE;
        }
        
        STR_STR_MAP *m = NULL;
        while (fgets(data, 4096, fp)) {
            line ++;
            char *sName = isSectionName(data);
            // 是段名
            if (sName != NULL) {
                STR_MAP_ITER it = m_configMap.find(sName);
                if (it == m_configMap.end()) {
                    m = new STR_STR_MAP();
                    m_configMap.insert(STR_MAP::value_type(/*CStringUtil::strToLower(sName)*/sName, m));
                } else {
                    m = it->second;
                }
                continue;
            }
            ret = parseValue(data, key, value);
            if (ret == -2) {
                TBSYS_LOG(ERROR, "解析错误, Line: %d, %s", line, data);
                fclose(fp);
                return EXIT_FAILURE;
            }
            if (ret < 0) {
                continue;
            }
            if (m == NULL) {
                TBSYS_LOG(ERROR, "没在配置section, Line: %d, %s", line, data);
                fclose(fp);
                return EXIT_FAILURE;
            }            
            //CStringUtil::strToLower(key);
            STR_STR_MAP_ITER it1 = m->find(key);
            if (it1 != m->end()) {
                it1->second += '\0';
                it1->second += value;
            } else {
                m->insert(STR_STR_MAP::value_type(key, value));
            }
        }
        fclose(fp);
        return EXIT_SUCCESS;
    }

    /**
     * 取一个string
     */
    const char *Configer::getString(const char *section, const string& key, const char *d)
    {
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            return d;
         }
        STR_STR_MAP_ITER it1 = it->second->find(key);
        if (it1 == it->second->end()) {
            return d;
        }
        return it1->second.c_str();
    }

    void Configer::setString(const char* section, const char* key, const char* value) { 
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            STR_STR_MAP* m = new STR_STR_MAP();
            m_configMap.insert(STR_MAP::value_type(section, m));
            it = m_configMap.find(section);
        }
        STR_STR_MAP_ITER it1 = it->second->find(key);
        if (it1 == it->second->end()) {
            it->second->insert(STR_STR_MAP::value_type(key, value));
            return;
        }
        else {
            it1->second += '\0';
            it1->second += value;
        }
        return ;
    }

    
    /**
     * 取一string列表
     */
    vector<const char*> Configer::getStringList(const char *section, const string& key) {
        vector<const char*> ret;
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            return ret;
        }
        STR_STR_MAP_ITER it1 = it->second->find(key);
        if (it1 == it->second->end()) {
            return ret;
        }
        const char *data = it1->second.data();
        const char *p = data;
        for(int i=0; i<(int)it1->second.size(); i++) {
            if (data[i] == '\0') {
                ret.push_back(p);
                p = data+i+1;
            }
        }
        ret.push_back(p);
        return ret;
    }

    /**
     * 取一整型
     */
    int Configer::getInt(const char *section, const string& key, int d)
    {
        const char *str = getString(section, key);
        return tbsys::CStringUtil::strToInt(str, d);
    }
    
    /**
     * 取一int list
     */
    vector<int> Configer::getIntList(const char *section, const string& key) {
        vector<int> ret;
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            return ret;
        }
        STR_STR_MAP_ITER it1 = it->second->find(key);
        if (it1 == it->second->end()) {
            return ret;
        }
        const char *data = it1->second.data();
        const char *p = data;
        for(int i=0; i<(int)it1->second.size(); i++) {
            if (data[i] == '\0') {
                ret.push_back(atoi(p));
                p = data+i+1;
            }
        }
        ret.push_back(atoi(p));
        return ret;
    }
    
    // 取一section下所有的key
    int Configer::getSectionKey(const char *section, vector<string> &keys)
    {
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            return 0;
        }
        STR_STR_MAP_ITER it1;
        for(it1=it->second->begin(); it1!=it->second->end(); ++it1) {
            keys.push_back(it1->first);
        }
        return (int)keys.size();
    }
             
    // 得到所有section的名字
    int Configer::getSectionName(vector<string> &sections)
    {
        STR_MAP_ITER it;
        for(it=m_configMap.begin(); it!=m_configMap.end(); ++it)
        {
            sections.push_back(it->first);
        }
        return (int)sections.size();
    }

    // toString
    string Configer::toString()
    {
        string result;
        STR_MAP_ITER it;
        STR_STR_MAP_ITER it1;
        for(it=m_configMap.begin(); it!=m_configMap.end(); ++it) {
            result += "[" + it->first + "]\n";
            for(it1=it->second->begin(); it1!=it->second->end(); ++it1) {
                string s = it1->second.c_str();
                result += "    " + it1->first + " = " + s + "\n";
                if (s.size() != it1->second.size()) {
                    char *data = (char*)it1->second.data();
                    char *p = NULL;
                    for(int i=0; i<(int)it1->second.size(); i++) {
                        if (data[i] != '\0') continue;
                        if (p) result += "    " + it1->first + " = " + p + "\n";
                        p = data+i+1;
                    }
                    if (p) result += "    " + it1->first + " = " + p + "\n";
                }
            }
        }
        result += "\n";    
        return result;
    }

    void Configer::removeSectionKey(const char* section, const char* key) {
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            return;
        }
        STR_STR_MAP_ITER it1;
        it1 = it->second->find(key);
        if (it1 == it->second->end()) {
            return;
        }

        it->second->erase(it1);
    }

    void Configer::removeSection(const char* section) {
        STR_MAP_ITER it = m_configMap.find(section);
        if (it == m_configMap.end()) {
            return;
        }
        delete it->second;
        m_configMap.erase(it);
    }

    bool Configer::backup_and_write_file(const char *file_name, const char *data, int size, int modified_time) {
      bool ret = false;
      if(data == NULL)
        return ret;
      if(1) {
        file_mapper mmap_file;
        mmap_file.open_file(file_name);
        char *tdata = (char *) mmap_file.get_data();
        int tsize = mmap_file.get_size();
        //same, not need to write
        if(tdata != NULL && tsize == size && memcmp(tdata, data, size) == 0) {
          log_info("%s not changed, set modify time  to %d", file_name,
                   modified_time);
          if(modified_time > 0) {
            struct utimbuf times;
            times.actime = modified_time;
            times.modtime = modified_time;
            utime(file_name, &times);
          }
          return true;
        }
      }
      //back up first
      char backup_file_name[256];
      time_t t;
      time(&t);
      struct tm *tm =::localtime((const time_t *) &t);
      snprintf(backup_file_name, 256, "%s.%04d%02d%02d%02d%02d%02d.backup",
               file_name, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
               tm->tm_hour, tm->tm_min, tm->tm_sec);
      rename(file_name, backup_file_name);
      int fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0644);
      if(write(fd, data, size) == size) {
        ret = true;
      }
      close(fd);
      if(ret == false) {
        rename(backup_file_name, file_name);
      }
      else if(modified_time > 0) {
        struct utimbuf times;
        times.actime = modified_time;
        times.modtime = modified_time;
        if(utime(file_name, &times)) {
          log_warn("can not change modified timestamp: %s", file_name);
        }
      }
      if(ret) {
        log_info("back up file  %s to  %s, new file modify time: %d",
                 file_name, backup_file_name, modified_time);
      }
      return ret;
    }

}
///////////////

