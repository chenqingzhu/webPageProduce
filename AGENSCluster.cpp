//
//  AGENSCluster.cpp
//  spiter
/*
 凝聚的层次聚类：自底向上的策略，首先将每个对象作为一个簇，然后合并这些原子簇为更大的簇，直到所有的对象都在同一个簇中，或者满足终止条件。
 
 AGNES算法
 AGNES(Agglomerative Nesting) 是凝聚的层次聚类算法，如果簇C1中的一个对象和簇C2中的一个对象之间的距离是所有属于不同簇的对象间欧式距离中最小的，C1和C2可能被合并。这是一种单连接方法，其每个簇可以被簇中的所有对象代表，两个簇之间的相似度由这两个簇中距离最近的数据点对的相似度来确定。
*/
//  Created by chenqingzhu on 15/12/7.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#include "AGENSCluster.h"
