//
//  contentProcess.h
//  spiter
//
//  Created by chenqingzhu on 15/11/25.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef __spiter__contentProcess__
#define __spiter__contentProcess__

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iostream>

#include "publicPart.h"
using namespace std;
/*
对文本进行预处理，删标签
*/
class contentProcess{
public:
    //string content;
    
    contentProcess(){
    }
    ~contentProcess()
    {
    }
    // 删除<head ...> .........</head> 标签
    int delete_head_lable(string &content){
        //去除 <head ...> .........</head>内容
        int headBeginIndex = content.find("<head>");
        if(headBeginIndex == string::npos)
        {
            cout<<"The page has no header"<<endl;
            return -1;
        }
        int headEndIndex = content.find("</head>");
        if(headEndIndex == string::npos)
        {
            cout<<"The page has no header end."<<endl;
            return -1;
        }
        int nextHeadEndIndex = content.find("</head>",headEndIndex+5);
        if(nextHeadEndIndex == string::npos)
        {
            cout<<"The page has no header end."<<endl;
            return -1;
        }
        if(nextHeadEndIndex == string::npos)
        {
            content.erase(content.begin() + headBeginIndex,content.begin() + headEndIndex+7);
        }
        else
        {
            content.erase(content.begin() + headBeginIndex,content.begin() + nextHeadEndIndex+7);
        }
        
        return 0;
    }
    
    // 删除html标签，由于有些页面包含多个<html> <head></head> <body> </body> </html> ，需要除去非收尾的html标签内容
    int delete_another_html_head_lable(string & content)
    {
        int findPostIndex = content.find("<body");
        while(1)
        {
            int scriptBeginIndex =  content.find("<head ",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("</head>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+7] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
            }
        }
        return 1;
    }
    
    // 删除script标签
    int delete_script_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<script",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int bigscriptEndIndex = content.find("</SCRIPT>",scriptBeginIndex);
            int scriptEndIndex = content.find("</script>",scriptBeginIndex);
            if(scriptEndIndex == string::npos && bigscriptEndIndex == string::npos)
            {
                break;
            }
            if(scriptEndIndex != string::npos && bigscriptEndIndex != string::npos)
            {
                scriptEndIndex = min(scriptEndIndex,bigscriptEndIndex);
            }
            else if(scriptEndIndex == string::npos)
            {
                scriptEndIndex = bigscriptEndIndex;
            }
        
            
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+9] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+10);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
            }
        }
        
        findPostIndex = 0;
        while(1)
        {
            int scriptBeginIndex =  content.find("<SCRIPT",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int bigscriptEndIndex = content.find("</SCRIPT>",scriptBeginIndex);
            int scriptEndIndex = content.find("</script>",scriptBeginIndex);
            if(scriptEndIndex == string::npos && bigscriptEndIndex == string::npos)
            {
                break;
            }
            if(scriptEndIndex != string::npos && bigscriptEndIndex != string::npos)
            {
                scriptEndIndex = min(scriptEndIndex,bigscriptEndIndex);
            }
            else if(scriptEndIndex == string::npos)
            {
                scriptEndIndex = bigscriptEndIndex;
            }
            
            
            /*
            int scriptEndIndex = content.find("</SCRIPT>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            */
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+9] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+10);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
            }
        }
        return 0;
    }
    
    // 删除style标签
    int delete_style_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<style",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("</style>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+8] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
            }
        }
        findPostIndex = 0;
        while(1)
        {
            int scriptBeginIndex =  content.find("<STYLE",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("</STYLE>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+8] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
            }
        }
        
        return 0;
    }
    
    // 删除<!---   -->标签
    int delete_explain_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<!-",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("->",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+2] == '\n')
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+3);
            }
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+2);
            }
        }
        return 0;
    }
    
    // 删除 ul 标签
    int delete_ul_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        stack<string> divUlStack;
        stack<int> indexUlStack;
        int divUlBeginIndex = content.find("<ul",findPostIndex);
        if(divUlBeginIndex == string::npos)
        {
            cout<<"page dont have <ul> \n";
        }else
        {
            divUlStack.push("<ul");
            indexUlStack.push(divUlBeginIndex);
            findPostIndex = divUlBeginIndex +3;
            int i=0;
            int total_len = content.size();
            while(1)
            {
                int total_len = content.size();
                divUlBeginIndex = content.find("<ul",findPostIndex);
                if(divUlBeginIndex == string::npos)
                {
                    if(divUlStack.empty()){
                        break; //处理完毕
                    }
                    else
                    {
                        int divUlEndIndex  = content.rfind("</ul>");
                        int firstUlIndex = indexUlStack.top();
                        while(!indexUlStack.empty())
                        {
                            firstUlIndex = indexUlStack.top();
                            indexUlStack.pop();
                            divUlStack.pop();
                        }
                        /*
                         cout<<"\n清除最早一个:\n";
                         cout<<content.substr(firstUlIndex,divUlEndIndex+6)<<endl;
                         cout<<"   和最后一个div标签：  "<<endl;
                         cout<<"清除最早一个"<<firstUlIndex<<"   和最后一个div标签：  "<<divUlEndIndex<<endl;
                         */
                        content.erase(content.begin() + firstUlIndex,content.begin()+divUlEndIndex+5);
                        break;
                    }
                }
                int divUlEndIndex = content.find("</ul>",findPostIndex);
                if(divUlEndIndex == string::npos)
                {
                    int tmp_end = content.find(">",divUlBeginIndex);
                    if(tmp_end == string::npos)
                        content.erase(divUlBeginIndex,tmp_end - divUlBeginIndex +1);
                    else
                        content.erase(content.begin() + divUlBeginIndex,content.end());
                    cout<<"this DOM tree is not perfect, because ul is not perfect\n";
                    break;
                    
                }
                else{
                    if(divUlBeginIndex < divUlEndIndex)
                    {
                        divUlStack.push("<ul");
                        indexUlStack.push(divUlBeginIndex);
                        findPostIndex = divUlBeginIndex+3;
                    }
                    else
                    {
                        //int indexUlStackSize = indexUlStack.size();
                        //int divUlStackSize = divUlStack.size();
                        if(indexUlStack.empty())
                        {
                            content.erase(content.begin() + divUlEndIndex ,content.begin() + divUlEndIndex +5);
                            continue;
                        }
                        findPostIndex = indexUlStack.top();
                        indexUlStack.pop();
                        divUlStack.pop();
                        content.erase(content.begin()+findPostIndex,content.begin()+divUlEndIndex+5);
                    }
                }
            }
        }
        
        return 0;
    }
    
    // 删除li标签
    int delete_li_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int scriptBeginIndex =  content.find("<li",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int scriptEndIndex = content.find("</li>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+5] == '\n')
            {
                // cout<<"li 5  等于换行符\n";
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+6);
            }
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+5);
            }
        }
        
        return 0;
    }
    
    // 删除 a 标签  修改a标签嵌套的bug
    int delete_a_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        stack<string> divUlStack;
        stack<int> indexUlStack;
        int divUlBeginIndex = content.find("<a",findPostIndex);
        if(divUlBeginIndex == string::npos)
        {
            cout<<"page dont have <a> \n";
        }else
        {
            divUlStack.push("<a");
            indexUlStack.push(divUlBeginIndex);
            findPostIndex = divUlBeginIndex +2;
            int i=0;
            int total_len = content.size();
            while(1)
            {
                int total_len = content.size();
                divUlBeginIndex = content.find("<a",findPostIndex);
                if(divUlBeginIndex == string::npos)
                {
                    if(divUlStack.empty()){
                        break; //处理完毕
                    }
                    else
                    {
                        int divUlEndIndex  = content.rfind("</a");
                        int firstUlIndex = indexUlStack.top();
                        while(!indexUlStack.empty())
                        {
                            firstUlIndex = indexUlStack.top();
                            indexUlStack.pop();
                            divUlStack.pop();
                        }
                        /*
                         cout<<"\n清除最早一个:\n";
                         cout<<content.substr(firstUlIndex,divUlEndIndex+6)<<endl;
                         cout<<"   和最后一个div标签：  "<<endl;
                         cout<<"清除最早一个"<<firstUlIndex<<"   和最后一个div标签：  "<<divUlEndIndex<<endl;
                         */
                        while(divUlEndIndex < content.size() && content[divUlEndIndex] != '>')
                            divUlEndIndex++;
                        
                        content.erase(content.begin() + firstUlIndex,content.begin()+divUlEndIndex+1);
                        break;
                    }
                }
                int divUlEndIndex = content.find("</a",findPostIndex);
                
                if(divUlEndIndex == string::npos)
                {
                    int tmp_end = content.find(">",divUlBeginIndex);
                    
                    if(tmp_end == string::npos)
                        content.erase(divUlBeginIndex,tmp_end - divUlBeginIndex +1);
                    else
                        content.erase(content.begin() + divUlBeginIndex,content.end());
                    cout<<"this DOM tree is not perfect, because a lable is not perfect\n";
                    break;
                    
                }
                
                while((content[divUlEndIndex+3] != '>' && content[divUlEndIndex+3] != ' '))
                {
                    divUlEndIndex = content.find("</a",divUlEndIndex +2);
                    if(divUlEndIndex == string::npos)
                    {
                        break;
                    }
                }
                
                if(divUlEndIndex == string::npos)
                {
                    int tmp_end = content.find(">",divUlBeginIndex);
                    if(tmp_end == string::npos)
                        content.erase(divUlBeginIndex,tmp_end - divUlBeginIndex +1);
                    else
                        content.erase(content.begin() + divUlBeginIndex,content.end());
                    cout<<"this DOM tree is not perfect, because a lable is not perfect\n";
                    break;
                    
                }
                else{
                    if(divUlBeginIndex < divUlEndIndex)
                    {
                        divUlStack.push("<a");
                        indexUlStack.push(divUlBeginIndex);
                        findPostIndex = divUlBeginIndex+2;
                    }
                    else
                    {
                        if(indexUlStack.empty())
                        {
                            int tmp = divUlEndIndex;
                            while(divUlEndIndex < content.size() && content[divUlEndIndex] != '>')
                                divUlEndIndex++;
                            content.erase(content.begin() + tmp ,content.begin() + divUlEndIndex+1);
                            continue;
                        }
                        findPostIndex = indexUlStack.top();
                        indexUlStack.pop();
                        divUlStack.pop();
                        while(divUlEndIndex < content.size() && content[divUlEndIndex] != '>')
                            divUlEndIndex++;
                        content.erase(content.begin()+findPostIndex,content.begin()+divUlEndIndex+1);
                    }
                }
            }
        }
        
        return 0;
    }
    
    // 删除a标签
    int delete_a_lable_old(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<a",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int scriptEndIndex = content.find("</a",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
                break;
            //bool flag = false;
            while((content[scriptEndIndex+3] != '>' || content[scriptEndIndex+3] != ' '))
            {
                scriptEndIndex = content.find("</a",scriptEndIndex +2);
                if(scriptEndIndex == string::npos)
                {
                   break;
                }
            }
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            
            while(scriptEndIndex < content.size() && content[scriptEndIndex] != '>')
                scriptEndIndex++;
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+1] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+2);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+1);
            }
        }
        findPostIndex = content.find("<body");
        while(1)
        {
            int scriptBeginIndex =  content.find("<A",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int scriptEndIndex = content.find("</A>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+4] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+5);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+4);
            }
        }
        
        return 0;
    }
    
    // 删除ins标签
    int delete_ins_lable(string &content){
        int findPostIndex = content.find("<body");
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<ins",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int scriptEndIndex = content.find("</ins>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+6] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+6);
            }
        }
        return 0;
    }
    
    // 删除form标签
    int delete_form_lable(string &content){
        int findPostIndex = content.find("<body");
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<form",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("</form>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+7] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
            }
        }
        return 0;
    }
    
    // 删除noScript标签
    int delete_noScript_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        
        while(1)
        {
            int scriptBeginIndex =  content.find("<noScript",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("</noScript>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+11] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+12);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+11);
            }
        }
        return 0;
    }
    
    // 删除iframe标签
    int delete_iframe_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int scriptBeginIndex =  content.find("<iframe",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            int scriptEndIndex = content.find("</iframe>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            findPostIndex= scriptBeginIndex;
            if(content[scriptEndIndex+9] == '\n')
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+10);
            else
            {
                content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
            }
        }
        return 0;
    }
    
    // 删除table标签
    int delete_table_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        stack<string> divUlStack;
        stack<int> indexUlStack;
        int divUlBeginIndex = content.find("<table",findPostIndex);
        if(divUlBeginIndex == string::npos)
        {
            cout<<"page dont have <table> \n";
        }else
        {
            divUlStack.push("<table");
            indexUlStack.push(divUlBeginIndex);
            findPostIndex = divUlBeginIndex +5;
            int i=0;
            while(1)
            {
                divUlBeginIndex = content.find("<table",findPostIndex);
                if(divUlBeginIndex == string::npos)
                {
                    if(divUlStack.empty()){
                        break; //处理完毕
                    }
                    else
                    {
                        int divUlEndIndex  = content.rfind("</table>");
                        int firstUlIndex = indexUlStack.top();
                        while(!indexUlStack.empty())
                        {
                            firstUlIndex = indexUlStack.top();
                            indexUlStack.pop();
                            divUlStack.pop();
                        }
                        /*
                         cout<<"\n清除最早一个:\n";
                         cout<<content.substr(firstUlIndex,divUlEndIndex+6)<<endl;
                         cout<<"   和最后一个div标签：  "<<endl;
                         cout<<"清除最早一个"<<firstUlIndex<<"   和最后一个div标签：  "<<divUlEndIndex<<endl;
                         */
                        content.erase(content.begin() + firstUlIndex,content.begin()+divUlEndIndex+8);
                        break;
                    }
                }
                int divUlEndIndex = content.find("</table>",findPostIndex);
                if(divUlEndIndex == string::npos)
                {
                    int tmp_end = content.find(">",divUlBeginIndex);
                    if(tmp_end == string::npos)
                        content.erase(divUlBeginIndex,tmp_end - divUlBeginIndex +1);
                    else
                        content.erase(content.begin() + divUlBeginIndex,content.end());
                    
                    break;//处理完毕
                    
                }
                else{
                    if(divUlBeginIndex < divUlEndIndex)
                    {
                        divUlStack.push("<table");
                        indexUlStack.push(divUlBeginIndex);
                        findPostIndex = divUlBeginIndex+35;
                    }
                    else
                    {
                        findPostIndex = indexUlStack.top();
                        indexUlStack.pop();
                        divUlStack.pop();
                        content.erase(content.begin()+findPostIndex,content.begin()+divUlEndIndex+8);
                    }
                }
                //if(i%10 == 0)
                //    cout<<"处理ul:  findPostIndex: "<<findPostIndex<<"   totallencontent :"<<content.size()<<endl;
            }
        }
        
        return 0;
    }
    
    // 删除font标签,内容保留（font只是改变文本格式）
    int delete_font_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int scriptBeginIndex =  content.find("<font",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int tmp_end = content.find(">",scriptBeginIndex + 3);
            content.erase(scriptBeginIndex,tmp_end - scriptBeginIndex +1);
            
            int scriptEndIndex = content.find("</font>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            content.erase(scriptEndIndex,7);
            /*
             string inner_content = content.substr(scriptBeginIndex,scriptEndIndex-scriptBeginIndex +5);
             if(inner_content.find("href") == string::npos)
             {
             findPostIndex= scriptBeginIndex;
             if(content[scriptEndIndex+7] == '\n')
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
             else
             {
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
             }
             }
             else
             findPostIndex= scriptEndIndex +5;
             */
        }
        return 0;
    }
    
    // 删除span标签,内容保留（span只是改变文本格式）
    int delete_span_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int scriptBeginIndex =  content.find("<span",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int tmp_end = content.find(">",scriptBeginIndex + 3);
            content.erase(scriptBeginIndex,tmp_end - scriptBeginIndex +1);
            
            int scriptEndIndex = content.find("</span",scriptBeginIndex);
            int scriptEndIndex_big = content.find("</SPAN",scriptBeginIndex);
            if(scriptEndIndex == string::npos && scriptEndIndex_big == string::npos)
            {
                break;
            }
            if(scriptEndIndex != string::npos && scriptEndIndex_big != string::npos)
            {
                scriptEndIndex = min(scriptEndIndex_big,scriptEndIndex);
            }
            else if(scriptEndIndex == string::npos && scriptEndIndex_big != string::npos)
            {
                scriptEndIndex = scriptEndIndex_big;
            }
            int tmp_index =scriptEndIndex;
            while(tmp_index <= content.size() && content[tmp_index] != '>')
                tmp_index ++;
            content.erase(scriptEndIndex,tmp_index - scriptEndIndex +1);
            /*
             string inner_content = content.substr(scriptBeginIndex,scriptEndIndex-scriptBeginIndex +5);
             if(inner_content.find("href") == string::npos)
             {
             findPostIndex= scriptBeginIndex;
             if(content[scriptEndIndex+7] == '\n')
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
             else
             {
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
             }
             }
             else
             findPostIndex= scriptEndIndex +5;
             */
        }
        return 0;
    }
    
    
    // 删除strong标签,内容保留（span只是改变文本格式）   增加删除 <strong> ....  <STRONG>  之类的大小写标签。。。。。
    int delete_strong_lable(string &content){
        int findPostIndex =0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int scriptBeginIndex =  content.find("<strong",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            int tmp_end = content.find(">",scriptBeginIndex + 3);
            content.erase(scriptBeginIndex,tmp_end - scriptBeginIndex +1);
            
            int scriptEndIndex = content.find("</strong",scriptBeginIndex);
            int scriptEndIndex_big = content.find("</STRONG",scriptBeginIndex);
            if(scriptEndIndex == string::npos && scriptEndIndex_big == string::npos)
            {
                break;
            }
            if(scriptEndIndex != string::npos && scriptEndIndex_big != string::npos)
            {
                scriptEndIndex = min(scriptEndIndex_big,scriptEndIndex);
            }
            else if(scriptEndIndex == string::npos && scriptEndIndex_big != string::npos)
            {
                scriptEndIndex = scriptEndIndex_big;
            }
            int tmp_index =scriptEndIndex;
            while(tmp_index <= content.size() && content[tmp_index] != '>')
                tmp_index ++;
            content.erase(scriptEndIndex,tmp_index - scriptEndIndex +1);
            /*
             string inner_content = content.substr(scriptBeginIndex,scriptEndIndex-scriptBeginIndex +5);
             if(inner_content.find("href") == string::npos)
             {
             findPostIndex= scriptBeginIndex;
             if(content[scriptEndIndex+7] == '\n')
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
             else
             {
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
             }
             }
             else
             findPostIndex= scriptEndIndex +5;
             */
        }
        return 0;
    }

    
    // 删除b标签,内容保留（b只是改变文本格式）
    int delete_b_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int scriptBeginIndex =  content.find("<b>",findPostIndex);
            if(scriptBeginIndex == string::npos)
            {
                break;
            }
            
            //int tmp_end = content.find(">",scriptBeginIndex + 3);
            content.erase(scriptBeginIndex,3);
            
            int scriptEndIndex = content.find("</b>",scriptBeginIndex);
            if(scriptEndIndex == string::npos)
            {
                break;
            }
            content.erase(scriptEndIndex,4);
            findPostIndex = scriptEndIndex;
            /*
             string inner_content = content.substr(scriptBeginIndex,scriptEndIndex-scriptBeginIndex +5);
             if(inner_content.find("href") == string::npos)
             {
             findPostIndex= scriptBeginIndex;
             if(content[scriptEndIndex+7] == '\n')
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
             else
             {
             content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
             }
             }
             else
             findPostIndex= scriptEndIndex +5;
             */
        }
        return 0;
    }
    
    // 删除img标签
    int delete_img_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int imgBeginIndex = content.find("<img",findPostIndex);
            if(imgBeginIndex == string::npos)
            {
                break;
            }
            int imgEndIndex = content.find(">",imgBeginIndex+3);
            if(imgEndIndex == string::npos)
            {
                return -1;
            }
            findPostIndex = imgBeginIndex;
            content.erase(content.begin() + imgBeginIndex,content.begin()+imgEndIndex+1);
        }
        findPostIndex = 0;
        while(1)
        {
            int imgBeginIndex = content.find("<IMG",findPostIndex);
            if(imgBeginIndex == string::npos)
            {
                break;
            }
            int imgEndIndex = content.find(">",imgBeginIndex+3);
            if(imgEndIndex == string::npos)
            {
                return -1;
            }
            findPostIndex = imgBeginIndex;
            content.erase(content.begin() + imgBeginIndex,content.begin()+imgEndIndex+1);
        }
        
        return 0;
    }
    
    // 删除div标签
    int delete_div_lable(string &content){
        int findPostIndex = content.find("<body");
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        stack<string> divStringStack;
        stack<int> divIndexStack;
        int divBeginIndex_tmp = content.find("<div",findPostIndex);
        if(divBeginIndex_tmp == string::npos)
        {
            cout<< "page has no div part\n";
            return 1;
        }
        else
        {
            divStringStack.push("<div");
            divIndexStack.push(divBeginIndex_tmp);
            findPostIndex = divBeginIndex_tmp+3;
            int i=0;
            while(1)
            {
                i++;
                int divBeginIndex_tmp = content.find("<div",findPostIndex);
                if(divBeginIndex_tmp == string::npos)
                {
                    if(divStringStack.empty())
                    {
                        cout<<"last content has no div part\n";
                        break;
                    }
                    else
                    {
                        
                        int lastDivEndIndex = content.rfind("</div>");
                        int tmp_div_index = divIndexStack.top();
                        while(!divIndexStack.empty())
                        {
                            tmp_div_index = divIndexStack.top();
                            divIndexStack.pop();
                            divStringStack.pop();
                        }
                        cout<<"\n清除最早一个:\n";
                        cout<<content.substr(tmp_div_index,lastDivEndIndex+6)<<endl;
                        cout<<"   和最后一个div标签：  "<<endl;
                        cout<<"清除最早一个"<<tmp_div_index<<"   和最后一个div标签：  "<<lastDivEndIndex<<endl;
                        
                        content.erase(content.begin()+tmp_div_index,content.begin() +lastDivEndIndex+6);
                        break;
                    }
                }
                int divEndindex_tmp = content.find("</div>",findPostIndex);
                if(divEndindex_tmp == string::npos)
                {
                    if(divStringStack.size() !=0)
                    {
                        cout<<"divStringStack is not empty but ";
                    }
                    cout<<"last content has no </div> part\n";
                    break;
                }
                if(divBeginIndex_tmp < divEndindex_tmp)
                {
                    divStringStack.push("<div");
                    divIndexStack.push(divBeginIndex_tmp);
                    findPostIndex = divBeginIndex_tmp+3;
                    //continue;
                }
                else
                {
                    int topindex = divIndexStack.top();
                    divIndexStack.pop();
                    divStringStack.pop();
                    findPostIndex = topindex;
                    content.erase(content.begin()+topindex,content.begin()+divEndindex_tmp + 6);
                }
                if(i%10== 0)
                {
                    cout<<"处理div:  findPostIndex: "<<findPostIndex<<"   totallencontent :"<<content.size()<<endl;
                }
            }
        }
        
        return 0;
    }
    
    // 删除meta标签
    int delete_meta_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int imgBeginIndex = content.find("<meta",findPostIndex);
            if(imgBeginIndex == string::npos)
            {
                break;
            }
            int imgEndIndex = content.find("/>",imgBeginIndex+4);
            if(imgEndIndex == string::npos)
            {
                return -1;
            }
            findPostIndex = imgBeginIndex;
            content.erase(content.begin() + imgBeginIndex,content.begin()+imgEndIndex+2);
        }
        return 0;
    }
    
    //删除\r结束符
    int delete_end_lable(string &content)
    {
        if (content.size() == 0) {
            return -1;
        }
        for(int i=0;i<content.size();)
        {
            if(content[i] == '\r')
            {
                content.erase(content.begin() + i);
            }
            else
                i++;
        }
        return 0;
    }
    
    //删除空格行
    int delete_space_line_lable(string & content)
    {
        if (content.size() == 0) {
            return -1;
        }
        for(int i = 0; i<content.size();)
        {
            if(content[i] == '\n')
            {
                int j=i+1;
                while(content[j] == '\n')
                    j++;
                //cout<<"i="<<i<<"   j="<<j<<endl;
                content.erase(i+1,j-i-1);
                i++;
            }
            else
                i++;
        }
        return 0;
    }
    
    //删除空行和删除每行开始的空格符
    int delete_space_line_new(string &content)
    {
        int i=0,j=0;
        string new_content("");
        while(i!=content.size())
        {
            while(i!=content.size() && content[i]!='\n')
            {
                i++;
            }
            if(i== content.size())
                break;
            string lines = content.substr(j,i-j);
            for(int m=0;m<lines.size();m++)
            {
                if(lines[m]!= ' ' )
                {
                    new_content += lines.substr(m) +'\n';
                    break;
                }
            }
            j = i+1;
            i++;
        }
        content = new_content;
        return 0;
    }
    
    //删除<br /> 标签
    int delete_br_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
            return 0;
        while(1){
            int brBeginIndex = content.find("<br",findPostIndex);
            if(brBeginIndex == string::npos){
                break;
            }
            int brEndIndex = content.find(">",brBeginIndex +2);
            if(brEndIndex == string::npos)
                break;
            findPostIndex = brBeginIndex;
            content.erase(content.begin() + brBeginIndex, content.begin() + brEndIndex +1);
        }
        return 0;
    }
    
    //删除<wbr /> 标签
    int delete_wbr_lable(string &content){
        int findPostIndex =0;
        if(findPostIndex == string::npos)
            return 0;
        while(1){
            int brBeginIndex = content.find("<wbr",findPostIndex);
            if(brBeginIndex == string::npos){
                break;
            }
            int brEndIndex = content.find(">",brBeginIndex +2);
            if(brEndIndex == string::npos)
                break;
            findPostIndex = brBeginIndex;
            content.erase(content.begin() + brBeginIndex, content.begin() + brEndIndex +1);
        }
        return 0;
    }
    
    //删除<hr /> 标签
    int delete_hr_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
            return 0;
        while(1){
            int brBeginIndex = content.find("<hr",findPostIndex);
            if(brBeginIndex == string::npos){
                break;
            }
            int brEndIndex = content.find(">",brBeginIndex +2);
            if(brEndIndex == string::npos)
                break;
            findPostIndex = brBeginIndex;
            content.erase(content.begin() + brBeginIndex, content.begin() + brEndIndex +1);
        }
        return 0;
    }
    
    // 删除input标签
    int delete_input_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int imgBeginIndex = content.find("<input",findPostIndex);
            if(imgBeginIndex == string::npos)
            {
                break;
            }
            int imgEndIndex = content.find(">",imgBeginIndex+5);
            if(imgEndIndex == string::npos)
            {
                return -1;
            }
            findPostIndex = imgBeginIndex;
            content.erase(content.begin() + imgBeginIndex,content.begin()+imgEndIndex+1);
        }
        return 0;
    }
    
    // 删除link标签
    int delete_link_lable(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int imgBeginIndex = content.find("<link",findPostIndex);
            if(imgBeginIndex == string::npos)
            {
                break;
            }
            int imgEndIndex = content.find(">",imgBeginIndex+4);
            if(imgEndIndex == string::npos)
            {
                return -1;
            }
            findPostIndex = imgBeginIndex;
            content.erase(content.begin() + imgBeginIndex,content.begin()+imgEndIndex+1);
        }
        return 0;
    }
    
    // 删除tab键
    int delete_tab_key(string &content){
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        while(1)
        {
            int imgBeginIndex = content.find('\t',findPostIndex);
            if(imgBeginIndex == string::npos)
            {
                break;
            }
            content.erase(imgBeginIndex,1);
            findPostIndex = imgBeginIndex;
        }
        return 0;
    }
    //删除tab键
    
    void printMsg(struct lableMsg msg)
    {
        cout<<"lableName: "<<msg.lableName<<"  " ;
        cout<<"lableFlag: " <<( (msg.lableFlag == 0) ? "error" : (msg.lableFlag == LEFTLABLE?  "leftLable" : "rightLable") )<<"  ";
        cout<<"beginIndex: "<<msg.beginIndex <<"  endIndex: " << msg.endIndex<<endl;
    }
    
    
    //标签名统一转化为小写字符
    string string_uptolower(string name)
    {
        for(int i=0;i<name.size();++i)
            name[i] = tolower(name[i]);
        return name;
    }
    
    //从content中提取左右标签
    lableMsg getLableMsg(string &content,int findPostIndex)
    {
        int rightIndex, spaceIndex;
        int leftIndex = content.find("<",findPostIndex);
        if(leftIndex == string::npos)
        {
            struct lableMsg msg;
            return msg;
        }
        if(content[leftIndex+1] == '/')  //右标签
        {
            rightIndex = content.find(">",leftIndex+1);
            if(rightIndex == string::npos)
            {
                /*
                 cout<<"------------------------------------\n";
                 cout<<content.size()<<endl;
                 cout<<leftIndex<<endl;
                 cout<<content.substr(leftIndex-10)<<endl;
                 cout<<"------------------------------------\n";
                 */
                content.erase(content.begin() + leftIndex,content.end());
                struct lableMsg msg;
                return msg;
            }
            
            //  return lableMsg
            
            string lableName = content.substr(leftIndex+2,rightIndex-leftIndex-2);
            struct lableMsg msg(string_uptolower( lableName),leftIndex,rightIndex,RIGHTLABLE);
            return msg;
        }
        else
        {
            rightIndex = content.find(">",leftIndex +1);
            if(rightIndex == string::npos)
            {
                /*
                 cout<<"------------------------------------\n";
                 cout<<content.size()<<endl;
                 cout<<leftIndex<<endl;
                 cout<<content.substr(leftIndex-10)<<endl;
                 cout<<"------------------------------------\n";
                 */
                content.erase(content.begin() + leftIndex,content.end());
                struct lableMsg msg;
                return msg;
            }
            if(content[rightIndex-1] == '/')   //单个不配对标签  eg：<input .....  />  --->直接删除
            {
                content.erase(content.begin() + leftIndex,content.begin() + rightIndex + 1);
                return getLableMsg(content, findPostIndex);
            }
            string tmpcontent = content.substr(leftIndex+1,rightIndex - leftIndex-1);
            spaceIndex = tmpcontent.find(" ");
            string lableName;
            if(spaceIndex != string::npos)
                lableName = tmpcontent.substr(0, spaceIndex);
            else
                lableName = tmpcontent;
            struct lableMsg msg(string_uptolower( lableName),leftIndex,rightIndex,LEFTLABLE);
            return msg;
        }
    }
    
    //判断标签内容是否为空
    bool  inner_content_is_empty(string str)
    {
        int len = str.size();
        if(len <= 1)
        {
            return true;
        }
        
        for(int i=0; i<len ; i++)
        {
            //if(str[i] == '\r')
            //  cout<<"change line\n";
            if(str[i] ==' ' || str[i] == '\t' || str[i] == '\r' || str[i] =='\n' || str[i] =='-' || str[i] == '|'|| str[i] == '['|| str[i] == ']')
                continue;
            else
                return false;
        }
        return true;
    }
    
    //修复html  补全不对称的标签，删除不对称（多余的）的右标签， 但是如果左标签丢失呢？ 如果右标签丢失，则添加左标签
    int repair_html_lable(string &content)
    {
        int findPostIndex = content.find("<html");
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        findPostIndex = 0;
        stack<lableMsg> lableStack;
        struct lableMsg msg= getLableMsg(content,findPostIndex);
        if(msg.lableFlag == 0)
        {
            cout<<"no lable # delete_empty_lable function"<<endl;
            return 0;
        }
        while(msg.lableFlag == RIGHTLABLE){
            findPostIndex = msg.endIndex+1;
            msg= getLableMsg(content,findPostIndex);
        }
        findPostIndex = msg.endIndex +1;
        lableStack.push(msg);
        while(1)
        {
            msg = getLableMsg(content, findPostIndex);
            if(msg.lableFlag == 0)
            {
                cout<< "findPostIndex: "<< findPostIndex<<endl;
                cout<< "content size: : "<< content.size()<<endl;
                break;
                
            }
            // break;
            //cout<<"msg: \n";
            //printMsg(msg);
            if(msg.lableFlag == LEFTLABLE)
            {
                lableStack.push(msg);
                findPostIndex = msg.endIndex+1;
            }
            else
            {
                if(lableStack.empty())
                {
                    cout<<msg.lableName<<endl;
                    cout<<msg.endIndex << "   " << content.size()<<endl;
                    cout<<"empty stack\n";
                    break;
                }
                struct lableMsg  topMsg = lableStack.top();
                
                if(topMsg.lableName == msg.lableName && topMsg.lableFlag == LEFTLABLE)
                {
                    
                    findPostIndex = msg.endIndex;
                    lableStack.pop();
                    //cout<<"get pair lable: ";
                    //printMsg(msg);
                }
                else
                {
                    findPostIndex = msg.beginIndex+1;
                    content.erase(content.begin()+msg.beginIndex ,content.begin() + msg.endIndex+1);
                    //cout<<content<<endl;
                    cout<< "remove missmatch lable\n";
                    printMsg(msg);
                    //lableStack.push(msg);
                }
            }
        }
        //cout<<content<<endl;
        int m=0;
        
        while(!lableStack.empty())
        {
            cout<<m++<<"  ";
            printMsg(lableStack.top());
            //cout<<content.substr(lableStack.top().beginIndex, lableStack.top().endIndex);
            struct lableMsg topMsg = lableStack.top();
            content += "</" + topMsg.lableName + ">";
            lableStack.pop();
        }
        
        cout<<"repaire html success"<<endl;
        return 1;
    }
    
    //提取文本内容
    int get_page_text(string &content)
    {
        int findPostIndex = content.find("<body");
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        findPostIndex = 0;
        stack<lableMsg> lableStack;
        struct lableMsg msg= getLableMsg(content,findPostIndex);
        if(msg.lableFlag == 0)
        {
            cout<<"no lable # delete_empty_lable function"<<endl;
            return 0;
        }
        while(msg.lableFlag == RIGHTLABLE){
            findPostIndex = msg.endIndex+1;
            msg= getLableMsg(content,findPostIndex);
        }
        findPostIndex = msg.endIndex +1;
        lableStack.push(msg);
        while(1)
        {
            msg = getLableMsg(content, findPostIndex);
            if(msg.lableFlag == 0)
                break;
            if(msg.lableFlag == LEFTLABLE)
            {
                lableStack.push(msg);
                findPostIndex = msg.endIndex+1;
            }
            else
            {
                if(lableStack.empty())
                {
                    cout<<"empty stack\n";
                }
                struct lableMsg  topMsg = lableStack.top();
                
                if(topMsg.lableName == msg.lableName && topMsg.lableFlag == LEFTLABLE)
                {
                    //cout<<"topMsg :\n";
                    //printMsg(topMsg);
                    findPostIndex = topMsg.beginIndex;
                    string content_inner_lable = content.substr(topMsg.endIndex+1, msg.beginIndex -  topMsg.endIndex -1);
                    /*
                     cout<<"content size :  "<< content_inner_lable.size()<<endl;
                     cout<<"content:8888"<< content_inner_lable<<"888888"<<endl;
                     string tmps("");
                     */
                    int lenght =  content_inner_lable.size();
                    /*
                     for(int i=0;i<content_inner_lable.size(); i++)
                     {
                     if(content_inner_lable[i] == '\t' ||content_inner_lable[i] == ' ')
                     lenght --;
                     }
                     */
                    if(lenght< 18)
                    {
                        /*
                         int eEnd = msg.endIndex+1;
                         while(content[eEnd] == ' '  || content[eEnd] == '\t' || content[eEnd] == '\n' )
                         {
                         eEnd ++;
                         }
                         content.erase(content.begin() + topMsg.beginIndex,content.begin() + eEnd);
                         */
                        content.erase(content.begin() + topMsg.beginIndex,content.begin() + msg.endIndex +1);
                    }
                    else
                        
                    {
                        /*
                         int eEnd = msg.endIndex+1;
                         while(content[eEnd] == ' '  || content[eEnd] == '\t' || content[eEnd] == '\n' )
                         {
                         eEnd ++;
                         }
                         content.erase(content.begin() + msg.beginIndex , content.begin() + eEnd );
                         
                         eEnd = topMsg.endIndex+1;
                         while(content[eEnd] == ' ' || content[eEnd] == '\t' || content[eEnd] == '\n')
                         eEnd++;
                         content.erase(content.begin() + topMsg.beginIndex, content.begin() + eEnd);
                         */
                        content.erase(content.begin() + msg.beginIndex , content.begin() + msg.endIndex +1 );
                        content.erase(content.begin() + topMsg.beginIndex, content.begin() + topMsg.endIndex+1);
                    }
                    lableStack.pop();
                }
                else
                {
                    findPostIndex = msg.endIndex +1;
                    lableStack.push(msg);
                    //findPostIndex = msg.beginIndex;
                    //content.erase(content.begin()+msg.beginIndex ,content.begin() + msg.endIndex+1);
                    
                }
            }
        }
        
        int m=0;
        while(!lableStack.empty())
        {
            cout<<m++<<"  ";
            printMsg(lableStack.top());
            lableStack.pop();
        }
        return 1;
        
    }
    
    int delete_empty_label(string &content)
    {
        int findPostIndex = 0;
        if(findPostIndex == string::npos)
        {
            return 0;
        }
        findPostIndex += 6;
        stack<lableMsg> lableStack;
        struct lableMsg msg= getLableMsg(content,findPostIndex);
        if(msg.lableFlag == 0)
        {
            cout<<"no lable # delete_empty_lable function"<<endl;
            return 0;
        }
        while(msg.lableFlag == RIGHTLABLE){
            findPostIndex = msg.endIndex+1;
            msg= getLableMsg(content,findPostIndex);
            cout<< "while-->:  ";
            printMsg(msg);
        }
        findPostIndex = msg.endIndex+1;
        lableStack.push(msg);
        msg= getLableMsg(content,findPostIndex);
        while(1)
        {
            if(msg.lableFlag == 0)
                break;
            if(msg.lableFlag == LEFTLABLE)
            {
                lableStack.push(msg);
                findPostIndex = msg.endIndex+1;
            }
            else
            {
                struct lableMsg  topMsg = lableStack.top();
                if(topMsg.lableName == msg.lableName && topMsg.lableFlag == LEFTLABLE)
                {
                    string content_inner_lable = content.substr(topMsg.endIndex+1, msg.beginIndex -  topMsg.endIndex -1);
                    bool isEmpty = inner_content_is_empty(content_inner_lable);
                    if(isEmpty)
                    {
                        findPostIndex = topMsg.beginIndex;
                        content.erase(content.begin() + topMsg.beginIndex, content.begin() + msg.endIndex+1);
                        lableStack.pop();
                        //cout<<"delete lable " << msg.lableName<<endl;
                    }
                    else
                    {
                        findPostIndex = msg.endIndex +1;
                        lableStack.push(msg);
                        //cout<<"push lable 1" << msg.lableName<<endl;
                    }
                    
                }
                else
                {
                    lableStack.push(msg);
                    findPostIndex = msg.endIndex+1;
                    //cout<<"push lable 2 " << msg.lableName<<endl;
                }
            }
            msg= getLableMsg(content,findPostIndex);
            //printMsg(msg);
        }
        return 1;
        
    }
    
    // 转义字符删除，在什么情况下需要删除呢
    int delete_nbsp_lable(string &content)
    {
        
        while(1){
            int findpost = content.find("&nbsp");
            if(findpost == string::npos)
                break;
            if(content[findpost+5] == ';')
                content.erase(content.begin() + findpost ,content.begin() + findpost + 6);
            else
                content.erase(content.begin() + findpost ,content.begin() + findpost + 5);
        }
        return 0;
    }
    
    int delete_gt_lable(string &content)
    {
        
        while(1){
            int findpost = content.find("&gt");
            if(findpost == string::npos)
                break;
            content.erase(content.begin() + findpost ,content.begin() + findpost + 3);
        }
        return 0;
    }
    
    int delete_lt_lable(string &content)
    {
        
        while(1){
            int findpost = content.find("&lt");
            if(findpost == string::npos)
                break;
            content.erase(content.begin() + findpost ,content.begin() + findpost + 3);
        }
        return 0;
    }
    
    int delete_quot_lable(string &content)
    {
        
        while(1){
            int findpost = content.find("&quot");
            if(findpost == string::npos)
                break;
            content.erase(content.begin() + findpost ,content.begin() + findpost + 5);
        }
        return 0;
    }
    
    int delete_amp_lable(string &content)
    {
        
        while(1){
            int findpost = content.find("&amp");
            if(findpost == string::npos)
                break;
            content.erase(content.begin() + findpost ,content.begin() + findpost + 4);
        }
        return 0;
    }
    
    int delete_copy_lable(string &content)
    {
        
        while(1){
            int findpost = content.find("&copy");
            if(findpost == string::npos)
                break;
            content.erase(content.begin() + findpost ,content.begin() + findpost + 5);
        }
        return 0;
    }
    
    
    
    //处理内容之前需要先将接收到的内容进行标准html格式处理，因为在接受数据的过程中可能会出现格式错误
    //去除 <head ...> .........</head>内容
    //去除 <script ...> .........</script>内容
    //去除 <style ...> .........</style>内容
    //去除 <!-- .... -->注释内容
    //去除 <ul >.....</ul>内容,
    //去除 <li ...> .........</li>内容
    //去除 <a ...> .........</a>内容
    //去除 <ins ...> .........</ins>内容
    //去除 <form ...> .........</form>内容
    //去除 <noScript ...> .........</noScript>内容
    //去除 <div ...> .........</div>内容
    //去除 <img ... />内容
    //去除 <hr .. />内容
    //去除 <br ... />内容
    //去除 <div ...> .........</div>内容
    //去除iframe标签
    
    int parse_content( string &content)
    {
        cout<<"开始处理文本......"<<endl;
        
        //去除html的响应报文内容
        int  htmlIndex = content.find("<html");
        cout<<"contentsize: "<<content.size()<<endl;
        cout<<"htmlIndex:"<< htmlIndex<<endl;
        content.erase(content.begin(),content.begin() + htmlIndex );
        cout<<"contentsize: "<<content.size()<<endl;
        cout<<"content:"<<content.substr(0,100)<<endl;
        cout<<"去除html的响应报文内容 ok"<<endl;
        
        //去除 <head ...> .........</head>内容
        delete_head_lable(content);
        cout<<"<去除 <head ...> .........</head>内容 完成"<<endl;
        
        int bodyBeginIndex = content.find("<body");
        if(bodyBeginIndex == string::npos)
        {
            cout<<"The page has no body"<<endl;
            return -1;
        }
        
        //去除 <script ...> .........</script>内容
        delete_script_lable(content);
        cout<<"<去除 <script ...> .........</script>内容 完成"<<endl;
        
        //去除 <style ...> .........</style>内容
        delete_style_lable(content);
        cout<<"<去除 <style ...> .........</style>内容 完成"<<endl;
        
        //去除 <!-- .... -->注释内容
        delete_explain_lable(content);
        cout<<"<去除 <!-- .... -->内容 完成"<<endl;
        
        //去除ul表情
        delete_ul_lable(content);
        cout<<"<去除 <ul ...> .........</ul>内容 完成"<<endl;
        
        //去除 <li ...> .........</li>内容
        delete_li_lable(content);
        cout<<"<去除 <li ...> .........</li>内容 完成"<<endl;
        
        //去除img标签
        delete_img_lable(content);
        cout<<"<去除 <img  ....  />内容 完成"<<endl;
        
        //去除 <a ...> .........</a>内容
        //delete_a_lable(content);
        delete_a_lable(content);
        
        cout<<"<去除 <a ...> .........</a>内容 完成"<<endl;
        
        //去除 <ins ...> .........</ins>内容
        delete_ins_lable(content);
        cout<<"<去除 <ins ...> .........</ins>内容 完成"<<endl;
        
        //去除 <form ...> .........</form>内容
        delete_form_lable(content);
        cout<<"<去除 <form ...> .........</form>内容 完成"<<endl;
        
        
        //去除 <noScript ...> .........</noScript>内容
        delete_noScript_lable(content);
        cout<<"<去除 <noScript ...> .........</noScript>内容 完成"<<endl;
        
        //去除 <iframe ...> .........</iframe>内容
        delete_iframe_lable(content);
        cout<<"<去除 <iframe ...> .........</iframe>内容 完成"<<endl;
        
        //去除 <span ...> .........</span>内容
        delete_span_lable(content);
        //cout<<"<去除 <span ...> .........</span>内容 完成"<<endl;
        
        // 删除不是文字跳转链接的span标签
        //delete_span_lable(content);
        
        //去除meta标签
        delete_meta_lable(content);
        cout<<"<去除 <meta  ....  />内容 完成"<<endl;
        
        //去除link标签
        delete_link_lable(content);
        cout<<"<去除 <link   ....  /> 内容 完成"<<endl;
        
        // 去除<br />标签
        delete_br_lable(content);
        cout<<"<去除 <br /> 内容 完成"<<endl;
        
        // 去除<hr />标签
        delete_hr_lable(content);
        cout<<"<去除 <hr /> 内容 完成"<<endl;
        
        delete_table_lable(content);
        
        delete_input_lable(content);
        
        delete_font_lable(content);
        
        //删除空标签
        delete_empty_label(content);
        cout<<"<去除empty lable  内容 完成"<<endl;
        
        //  删除strong标签项，不删内容
        delete_strong_lable(content);
        
        //删除\r结束符
        delete_end_lable(content);
        delete_space_line_lable(content);
        delete_nbsp_lable(content);
        delete_gt_lable(content);
        delete_copy_lable(content);
        delete_tab_key(content);
        delete_space_line_new(content);
        
        
        return 1;
    }
    
    
    //精简预处理，防止过度处理而影响结果
    int parse_content_avoid_over_delete( string &content)
    {
        cout<<"开始处理文本......"<<endl;
        //去除 <!-- .... -->注释内容
        delete_explain_lable(content);
        cout<<"<去除 <!-- .... -->内容 完成"<<endl;
        //去除html的响应报文内容
        int  htmlIndex = content.find("<html");
        cout<<"contentsize: "<<content.size()<<endl;
        cout<<"htmlIndex:"<< htmlIndex<<endl;
        content.erase(content.begin(),content.begin() + htmlIndex );
        cout<<"contentsize: "<<content.size()<<endl;
        cout<<"content:"<<content.substr(0,100)<<endl;
        cout<<"去除html的响应报文内容 ok"<<endl;
        
        delete_another_html_head_lable(content);
        
        //去除 <head ...> .........</head>内容
        delete_head_lable(content);
        
        cout<<"<去除 <head ...> .........</head>内容 完成"<<endl;
        //cout<< content<<endl;
        int bodyBeginIndex = content.find("<body");
        if(bodyBeginIndex == string::npos)
        {
            cout<<"The page has no body"<<endl;
            return -1;
        }
        
        //去除 <script ...> .........</script>内容
        delete_script_lable(content);
        cout<<"<去除 <script ...> .........</script>内容 完成"<<endl;

        //去除 <style ...> .........</style>内容
        delete_style_lable(content);
        cout<<"<去除 <style ...> .........</style>内容 完成"<<endl;
        
        //去除 <noScript ...> .........</noScript>内容
        delete_noScript_lable(content);
        cout<<"<去除 <noScript ...> .........</noScript>内容 完成"<<endl;
        
        
        //去除 <!-- .... -->注释内容
        delete_explain_lable(content);
        cout<<"<去除 <!-- .... -->内容 完成"<<endl;
        
        //去除ul表情
        //delete_ul_lable(content);
        cout<<"<去除 <ul ...> .........</ul>内容 完成"<<endl;
        
        //去除 <a ...> .........</a>内容
        //delete_a_lable(content);
        //delete_a_lable(content);
        //cout<<"<去除 <a ...> .........</a>内容 完成"<<endl;
        
        //去除 <span ...> .........</span>内容
        delete_span_lable(content);
        cout<<"<去除 <span ...> .........</span>内容 完成"<<endl;
        //  删除strong标签项，不删内容
        delete_strong_lable(content);
        
        // 去除<br />标签
        delete_br_lable(content);
        
        // 去除<wbr />标签
        delete_wbr_lable(content);
        cout<<"<去除 <br /> 内容 完成"<<endl;
        
        delete_nbsp_lable(content);
        
        delete_font_lable(content);
        
        //去除img标签
        delete_img_lable(content);
        cout<<"<去除 <img  ....  />内容 完成"<<endl;
        
        delete_b_lable(content);
        
        //修复标签
        repair_html_lable(content);
        
        return 1;
        
        
        //去除 <li ...> .........</li>内容
        delete_li_lable(content);
        cout<<"<去除 <li ...> .........</li>内容 完成"<<endl;
        
        //去除img标签
        delete_img_lable(content);
        cout<<"<去除 <img  ....  />内容 完成"<<endl;
        
        //去除 <ins ...> .........</ins>内容
        delete_ins_lable(content);
        cout<<"<去除 <ins ...> .........</ins>内容 完成"<<endl;
        
        
        
        //去除 <form ...> .........</form>内容
        delete_form_lable(content);
        cout<<"<去除 <form ...> .........</form>内容 完成"<<endl;
        
        
        
        
        //去除 <iframe ...> .........</iframe>内容
        delete_iframe_lable(content);
        cout<<"<去除 <iframe ...> .........</iframe>内容 完成"<<endl;
        
        //去除 <span ...> .........</span>内容
        delete_span_lable(content);
        //cout<<"<去除 <span ...> .........</span>内容 完成"<<endl;
        
        // 删除不是文字跳转链接的span标签
        //delete_span_lable(content);
        
        //去除meta标签
        delete_meta_lable(content);
        cout<<"<去除 <meta  ....  />内容 完成"<<endl;
        
        //去除link标签
        delete_link_lable(content);
        cout<<"<去除 <link   ....  /> 内容 完成"<<endl;
        
        // 去除<hr />标签
        delete_hr_lable(content);
        cout<<"<去除 <hr /> 内容 完成"<<endl;
        
        delete_table_lable(content);
        
        delete_input_lable(content);
        
        delete_font_lable(content);
        
        //删除空标签
        delete_empty_label(content);
        cout<<"<去除empty lable  内容 完成"<<endl;
        
        
        
        //  删除strong标签项，不删内容
        delete_strong_lable(content);
        
        //删除\r结束符
        delete_end_lable(content);
        delete_space_line_lable(content);
        delete_nbsp_lable(content);
        delete_gt_lable(content);
        delete_copy_lable(content);
        delete_tab_key(content);
        delete_space_line_new(content);
        
        return 1;
    }
    
};

#endif /* defined(__spiter__contentProcess__) */
