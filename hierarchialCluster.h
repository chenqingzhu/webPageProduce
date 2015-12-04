//
//  hierarchialCluster.h
//  spiter
////  层次聚类
//  Created by chenqingzhu on 15/12/3.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef __spiter__hierarchialCluster__
#define __spiter__hierarchialCluster__

#include <stdio.h>
#include "publicPart.h"
#include "kmeansCluster.h"
/*
 （三）二分K-均值（bisecting k-means)聚类算法
 顾名思义，二分K-均值聚类算法就是每次对数据集（子数据集）采取k=2的k-均值聚类划分，子数据集的选取则有一定的准则。二分K-均值聚类算法首先将所有点作为一个簇，第一步是然后将该簇一分为二，之后的迭代是：在所有簇中根据SSE选择一个簇继续进行二分K-均值划分，直到得到用户指定的簇数目为止。根据SSE选取继续划分簇的准则有如下两种:
 
 (1)选择哪一个簇进行划分取决于对”其划分是否可以最大程度降低SSE的值。这需要将每个簇都进行二分划分，然后计算该簇二分后的簇SSE之和并计算其与二分前簇SSE之差（当然SSE必须下降），最后选取差值最大的那个簇进行二分。
 
 该方案下的二分k-均值算法的伪代码形式如下：
 
 ***************************************************************
 
 将所有数据点看成一个簇
 
 当簇数目小于k时
 
 对每一个簇
 
 计算总误差
 
 在给定的簇上面进行k-均值聚类（k=2）
 
 计算将该簇一分为二后的总误差
 
 选择使得误差最小的那个簇进行划分操作
 
 ***************************************************************
 
 (2)另一种做法是所有簇中选择SSE最大的簇进行划分，直到簇数目达到用户指定的数目为止，算法过程与（1）相似，区别仅在于每次选取簇中SSE最大的簇。
 
*/

//二分K-均值（bisecting k-means)聚类算法
class two_split_K_Means{
private:
    int dataNum; //数据集中纪录条数
    int dimNum;  //纪录维数
    double SSE_error; //聚类平均方差
    vector<Feature> featureVector;  //聚类归一化数据
    vector<Feature> initFeatureVector;  //聚类初始数据
    int K;  //聚类数目
    vector<Feature> kCenter; // 初始化聚类中心
    vector< vector<Feature> > cluster; //聚类簇信息
    int max_k; //最大的迭代K值
    
public:
    two_split_K_Means(vector<Feature> fv,int k = 6):initFeatureVector(fv),max_k(k),dataNum(0),SSE_error(-1)
    {
        K = 2;
        //初始化
        if(max_k<=0 || initFeatureVector.size() == 0)
        {
            
            throw 1;
        }
        if(initFeatureVector.size() <= 2)
            throw 3;
        /*
         if(featureVector.size() < K)
         {
         throw 2;
         }
         */
        dimNum = initFeatureVector[0].size();
        
        //process_feature_vector();
        process_feature_vector();
        
    }
    
    two_split_K_Means()
    {
    }
    
    //获取聚类中心
    vector<Feature> get_kCenter(){
        return kCenter;
    }
    
    //获取每个簇的标签ID
    vector< vector<int> > get_lableId_from_cluster()
    {
        vector< vector<int> > ret(K,vector<int>());
        for(int i=0;i<K;i++)
        {
            for(int j=0;j<cluster[i].size(); j++)
            {
                ret[i].push_back(cluster[i][j][0]);
            }
        }
        return ret;
    }
    
    //获取分类簇信息
    vector< vector<Feature> > get_cluster()
    {
        return cluster;
    }
    
    //获取聚类簇数
    int get_K(){
        return K;
    }
    //打印聚类质心和簇结果
    void print_kmeans_cluster(int numk)
    {
        if(numk > K)
        {
            cout<<"输入的簇类大于分类数K"<<endl;
            return;
        }
        vector<Feature> numKCluster = cluster[numk-1];
        cout<<"--------------第 "<< numk <<" 簇------------------------------"<<endl;
        cout<<"第 "<< numk <<" 簇质心: ";
        for(int i=0;i<dimNum-1;i++)
            cout<<kCenter[numk-1][i]<<", ";
        cout<<kCenter[numk-1][dimNum-1]<<" )"<<endl;
        cout<<"第 "<< numk <<" 簇 簇内共 "<< numKCluster.size()<<" 个样本， 分别如下：" <<endl;
        for(int i=0;i<numKCluster.size(); i++)
        {
            cout<<"第 "<<i+1<<" 个样本：(";
            for(int j=0;j<dimNum-1;j++)
            {
                cout<< numKCluster[i][j] <<", ";
            }
            cout<<numKCluster[i][dimNum -1]<<" )"<<endl;
        }
        cout<<"-------------------------------------------------------"<<endl;
    }
    
    //打印质心
    void print_center()
    {
        for(int i=0;i<K; i++)
        {
            cout<<i<<"质心: ( ";
            for(int j=0;j<kCenter[i].size(); j++)
            {
                cout<<kCenter[i][j]<<", ";
            }
            cout<<")"<<endl;
        }
    }
    
    //根据簇中心向量的标签文本平均长度来确定文本簇，目前长度最长的来定义为特征簇（待优化）
    int get_page_text_cluster_k()
    {
        int tmp_k = 0;
        int tmp_len = kCenter[0][1];
        
        for(int i=1;i<K; i++)
        {
            if(kCenter[i][1] > tmp_len)
            {
                tmp_len = kCenter[i][1];
                tmp_k = i;
            }
        }
        return tmp_k+1;
    }

    void print_every_cluster_vector_num()
    {
        for(int i=0;i<K;i++)
        {
            cout<<"第"<<i<<"簇内标本数量："<< cluster[i].size()<<endl;
        }
        
    }
    
    void twoSplitKmeansFunction(int max_kcenter = 10)
    {
        max_k = max_kcenter;
        SSE_error = split_2_cluster_by_kmeans(featureVector, kCenter, cluster);
        K = 2;
        cout<<"总共的特征向量数量："<<featureVector.size()<<endl;
        vector<Feature> ret2Center;
        vector< vector<Feature> > ret2Cluster;
        print_every_cluster_vector_num();
        double ret_sse_error;
        int step=1;
        while(K < max_k)
        {
            cout<<"－－－－－－－－－－第"<<step<<"次分裂开始"<<endl;
            //print_center();
            int tmp_select_k_cluster_2_split = -1;
            vector<Feature> tmp_2Center;
            vector< vector<Feature> > tmp_2Cluster;
            double tmp_see_error = SSE_error;
            cout<<"SSE_error123: "<<SSE_error<<endl;
            int i =0;
            for(;i< K;i++)
            {
                if(cluster[i].size() < 2)
                    continue;
                ret_sse_error = split_2_cluster_by_kmeans(cluster[i], tmp_2Center, tmp_2Cluster);
                double err = calculate_SSE(cluster, i);
                cout<<"split 簇数："<<i <<" －－－－－－－－－－－－－－－－－－－－－  with error: "<< ret_sse_error + err<<endl;
                if(ret_sse_error + err < tmp_see_error)
                {
                    tmp_see_error = ret_sse_error + err;
                    break;
                }
            }
            if(i == K)
            {
                cout<<"第"<<step<<" 不能再分裂，退出了"<<endl;
                break;
            }
            cout<<"first split\n";
            tmp_select_k_cluster_2_split = i;
            i++;
            for(;i<K;i++)
            {
                if(cluster[i].size() < 2)
                    continue;
                ret_sse_error = split_2_cluster_by_kmeans(cluster[i], ret2Center, ret2Cluster);
                double err = calculate_SSE(cluster, i);
                cout<<"split 簇数："<<i <<" －－－－－  with error: "<< ret_sse_error + err<<"   fea 大小："<< cluster[i].size() <<endl;

                if(ret_sse_error + err < tmp_see_error)
                {
                    tmp_see_error = ret_sse_error + err;
                    tmp_select_k_cluster_2_split = i;
                    tmp_2Center = ret2Center;
                    tmp_2Cluster = ret2Cluster;
                }
            }
            cout<<"选择第"<<tmp_select_k_cluster_2_split <<"簇进行分裂"<<endl;
            //print_center();
            kCenter[tmp_select_k_cluster_2_split] = tmp_2Center[0];
            kCenter.push_back(tmp_2Center[1]);
            cluster[tmp_select_k_cluster_2_split] = tmp_2Cluster[0];
            cluster.push_back(tmp_2Cluster[1]);
            SSE_error = tmp_see_error;
            K++;
            print_every_cluster_vector_num();
            //print_center();
           cout<<"－－－－－－－－－－第"<<step++<<"次分裂完成"<<endl;
        }
        cout<<"全部聚类数："<<K<<endl;
    }
    
    
    
private:
    
    //计算两个元组间的欧几里距离
    double get_dist_XY(const Feature &f1, const Feature &f2)
    {
        double ret = 0.0;
        for(int i=1;i< dimNum; i++)
        {
            ret += (f1[i] - f2[i]) * (f1[i] - f2[i]);
        }
        return sqrt(ret);
    }
    
    //获取聚类簇集的SSE值（平方误差）
    double calculate_SSE(vector< vector<Feature> > cluster, int without_k)
    {
        double ret = 0.0;
        for(int i=0;i<K;i++)
        {
            if(i == without_k)
                continue;
            vector<Feature> iCluster = cluster[i];
            for(int j=0;j<iCluster.size();j++)
            {
                ret += get_dist_XY(iCluster[j],kCenter[i]);
            }
        }
        return ret;
    }
    //运用kmeans算法降fea特征向量聚成两簇，返回SSE_error
    double split_2_cluster_by_kmeans(vector<Feature> fea,vector<Feature> &ret2Center,vector< vector<Feature> > &ret2Cluster)
    {
        kmeansCluster kmeans(fea,false);
        kmeans.kmeans_plus_plus_function(2);
        ret2Center = kmeans.get_kCenter();
        ret2Cluster = kmeans.get_cluster();
        return kmeans.get_SSE_error();
    }

    //对特征向量进行处理：各个参数的权重及归一化调整。
    void process_feature_vector()
    {
        Feature max_feature = initFeatureVector[0];
        Feature min_feature = initFeatureVector[0];
        
        double weights[] = {1.0,50.0,10.0,10.0,20.0,30.0};
        Feature feature_weights(weights,weights +6);
        for(int i=1;i< initFeatureVector.size(); i++)
        {
            for(int j=1;j<dimNum;j++)
            {
                max_feature[j] = max(max_feature[j],initFeatureVector[i][j]);
                min_feature[j] = min(min_feature[i],initFeatureVector[i][j]);
            }
        }
        Feature between(dimNum,0);
        for(int j=1;j<dimNum;j++)
            between[j] = (max_feature[j] - min_feature[j]) == 0 ? 1:(max_feature[j] - min_feature[j] );
        featureVector  = initFeatureVector;
        for(int i=0;i<initFeatureVector.size(); i++)
        {
            for(int j=1;j<dimNum;j++)
            {
                featureVector[i][j] = (featureVector[i][j] - min_feature[j])/between[j] * feature_weights[j];
            }
        }
        
        return;
    }
    
    
    
};
#endif /* defined(__spiter__hierarchialCluster__) */
