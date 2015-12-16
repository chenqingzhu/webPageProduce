//
//  testUrlClass.h
//  spiter
//
//  Created by chenqingzhu on 15/12/14.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef spiter_testUrlClass_h
#define spiter_testUrlClass_h

#include <stdlib.h>
#include "contentProcessClass.h"

extern int save_content2file(string file_path_name,string content);
class testUrlClass{
public:
    testUrlClass(){}
    
    ~testUrlClass(){}
    
    void get_ifeng_test_url()
    {
        string url("http://news.ifeng.com/");
        vector<string> vecturl;
        vecturl.push_back("http://news.ifeng.com/");
        vecturl.push_back("http://finance.ifeng.com/");
        vecturl.push_back("http://ent.ifeng.com/");
        vecturl.push_back("http://sports.ifeng.com/");
        vecturl.push_back("http://fashion.ifeng.com/");
        vecturl.push_back("http://fashion.ifeng.com/");
        vecturl.push_back("http://auto.ifeng.com/");
        vecturl.push_back("http://cd.house.ifeng.com/");
        vecturl.push_back("http://tech.ifeng.com/");
        vecturl.push_back("http://blog.ifeng.com/");
        vecturl.push_back("http://games.ifeng.com/");
        vecturl.push_back("http://cp.ifeng.com/?aid=44");
        vecturl.push_back("http://yc.ifeng.com/?_bookch=top");
        vecturl.push_back("http://zhongyi.ifeng.com/");
        vecturl.push_back("http://audi-future.ifeng.com/");
        
        
        for(int i=0;i<vecturl.size(); i++)
        {
            getWebPageClass gwp;
            string urlStringToFile = gwp.get_url_from_web_page_by_url(vecturl[i]);
            //save_content2file(, <#string content#>)
            stringstream ss;
            ss <<"/Users/pc/UrlFile/ifeng_url_"<<i<<".txt";
            save_content2file(ss.str(), urlStringToFile);
        }
    }
    
    void get_xinhua_test_url()
    {
        string url("http://news.ifeng.com/");
        vector<string> vecturl;
        vecturl.push_back("http://www.news.cn/politics/");
        vecturl.push_back("http://www.news.cn/politics/leaders/index.htm");
        vecturl.push_back("http://www.news.cn/local/index.htm");
        vecturl.push_back("http://www.xinhuanet.com/politics/rs.htm");
        vecturl.push_back("http://www.news.cn/legal/index.htm");
        vecturl.push_back("http://www.xinhuanet.com/politics/xhll.htm");
        vecturl.push_back("http://www.news.cn/world/index.htm");
        vecturl.push_back("http://www.news.cn/fortune/");
        vecturl.push_back("http://www.news.cn/energy/index.htm");
        vecturl.push_back("http://travel.news.cn/");
        vecturl.push_back("http://www.news.cn/sports/");
        vecturl.push_back("http://www.news.cn/health/");
        vecturl.push_back("http://xuan.news.cn/");
        vecturl.push_back("http://www.news.cn/photo/zhuanti/bt/index.htm");
        //vecturl.push_back("http://audi-future.ifeng.com/");
        
        
        for(int i=0;i<vecturl.size(); i++)
        {
            getWebPageClass gwp;
            string urlStringToFile = gwp.get_url_from_web_page_by_url(vecturl[i]);
            //save_content2file(, <#string content#>)
            stringstream ss;
            ss <<"/Users/pc/UrlFile/xinhua_url_"<<i<<".txt";
            save_content2file(ss.str(), urlStringToFile);
        }
    }
    
    
    void get_toutiao_test_url()
    {

        vector<string> vecturl;
        vecturl.push_back("http://mini.eastday.com/?360dh");
        
        for(int i=0;i<vecturl.size(); i++)
        {
            getWebPageClass gwp;
            string urlStringToFile = gwp.get_url_from_web_page_by_url(vecturl[i]);
            //save_content2file(, <#string content#>)
            stringstream ss;
            ss <<"/Users/pc/UrlFile/toutiao_url_"<<i<<".txt";
            save_content2file(ss.str(), urlStringToFile);
        }
    }
    
    void get_weilaiwang_test_url()
    {
        vector<string> vecturl;
        vecturl.push_back("http://mil.k618.cn/");
        vecturl.push_back("http://e.k618.cn/");
        vecturl.push_back("http://news.k618.cn/yl_37061/");
        vecturl.push_back("http://keji.k618.cn/");
        vecturl.push_back("http://jjh.k618.cn/");
        vecturl.push_back("http://kids.k618.cn/");
        vecturl.push_back("http://baby.k618.cn/");
        vecturl.push_back("http://guoxue.k618.cn/");
        vecturl.push_back("http://jz.k618.cn/");
        vecturl.push_back("http://shsj.k618.cn/");
        for(int i=0;i<vecturl.size(); i++)
        {
            getWebPageClass gwp;
            string urlStringToFile = gwp.get_url_from_web_page_by_url(vecturl[i]);
            //save_content2file(, <#string content#>)
            stringstream ss;
            ss <<"/Users/pc/UrlFile/weilaiwang_url_"<<i<<".txt";
            save_content2file(ss.str(), urlStringToFile);
        }
    }
    
    void get_huanqiu_test_url()
    {
        vector<string> vecturl;
        vecturl.push_back("http://world.huanqiu.com/");
        vecturl.push_back("http://mil.huanqiu.com/");
        vecturl.push_back("http://china.huanqiu.com/");
        vecturl.push_back("http://taiwan.huanqiu.com/");
        
        vecturl.push_back("http://society.huanqiu.com/");
        vecturl.push_back("http://finance.huanqiu.com/");
        vecturl.push_back("http://tech.huanqiu.com/");
        vecturl.push_back("http://tech.huanqiu.com/digi/");
        vecturl.push_back("http://ent.huanqiu.com/");
        vecturl.push_back("http://fashion.huanqiu.com/");
        vecturl.push_back("http://sports.huanqiu.com/");
        vecturl.push_back("http://women.huanqiu.com/");
        vecturl.push_back("http://hope.huanqiu.com/");
        
        for(int i=0;i<vecturl.size(); i++)
        {
            getWebPageClass gwp;
            string urlStringToFile = gwp.get_url_from_web_page_by_url(vecturl[i]);
            //save_content2file(, <#string content#>)
            stringstream ss;
            ss <<"/Users/pc/UrlFile/huanqiu_url_"<<i<<".txt";
            save_content2file(ss.str(), urlStringToFile);
        }
    }
};


#endif
