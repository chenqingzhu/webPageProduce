//
//  publicPart.h
//  spiter
//
//  Created by chenqingzhu on 15/11/25.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef spiter_publicPart_h
#define spiter_publicPart_h


#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iostream>
#include <fstream>
#include <ctype.h>

using namespace std;
#define LEFTLABLE 1   //左标签
#define RIGHTLABLE 2 //右标签

//特征
typedef vector<double> Feature;

//标签结构体
struct lableMsg{
    string lableName;   //标签名
    int beginIndex;  // 标签开始位置
    int endIndex;   //标签结束位置
    int lableFlag;   //左右标签标识
    bool leafLableFlag; //是否是叶子标签
    lableMsg (string name="", int begin=0, int end=0,int flag=0,bool leafFlag=true) : lableName(name), beginIndex(begin),
    endIndex(end),lableFlag(flag),leafLableFlag(leafFlag)
    {
    }
};

//标签内容结构体
struct textMsg{
    string text;
    int beginIndex;
    int endIndex;
    textMsg(string tmsg="",int begin = -1, int end = -1):text(tmsg),beginIndex(begin),endIndex(end){}
};


//标签特征结构体
struct lableFeature{
    string lableName;                           // 标签名
    string lableContent;                        // 标签文本内容
    string lableLeftPartContent;                // 左标签内容
    int lableBeginIndex;                        // 标签文本开始位置
    int lableEndIndex;                          // 标签文本结束位置
    int lableId;                                // 标签ID  （从上至下依次递增）
    int lableContentLength;                     // 标签文本长度
    //int lableContentBeginOfTheFullContent;      //由于标签嵌套，提取feature时会修改content，这个参数用于全局定位,用语lableId排序
    int lableLeftPartLength;                    // 左标签长度
    int lableRightPartLength;                   // 右标签长度
    int lablePunctNumber;                       // 标签中标点符号的数量
    vector<string>  lableAttributeVector;       // 标签存在的属性
    int lableLevelNumber;                       // 标签层次树
    lableFeature(string name,string content, string leftPartContent,int beginIndex,int endIndex,int id,
                 int contentLength,int leftPartLength, int rightPartLength,int punchNumber,
                 vector<string> attributeVector, int levelNumber):lableName(name),
    lableContent(content),
    lableLeftPartContent(leftPartContent),lableBeginIndex(beginIndex),lableEndIndex(endIndex),lableId(id),lableContentLength(contentLength),
    lableLeftPartLength(leftPartLength),lableRightPartLength(rightPartLength),lablePunctNumber(punchNumber),
    lableAttributeVector(attributeVector),lableLevelNumber(levelNumber)
    {
    }
};




#endif
