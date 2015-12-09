//
//  kmeansCluster.h
//  spiter
//
//  Created by chenqingzhu on 15/11/19.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef spiter_kmeansCluster_h
#define spiter_kmeansCluster_h
#include <string>

#include <iostream>

#include <stdlib.h>
#include "publicPart.h"
using namespace std;
#define MAX_K 20


class kmeansCluster{
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
    kmeansCluster(vector<Feature> fv,bool need_init_2_one = true):initFeatureVector(fv),dataNum(0),SSE_error(-1)
    {
        K = 2;
        //初始化
        if(initFeatureVector.size() == 0)
        {
            throw 1;
        }
        if(initFeatureVector.size() < 2)
            throw 3;
        /*
        if(featureVector.size() < K)
        {
            throw 2;
        }
        */
        dimNum = initFeatureVector[0].size();
        
        //是否需要将特征向量进行归一化
        if(need_init_2_one)
            process_feature_vector();
        else
            featureVector = initFeatureVector;
    }
    
    ~kmeansCluster()
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
    
    //获取聚类簇的误差SSE
    int get_SSE_error(){
        return SSE_error;
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
    
    //Kmeans 聚类算法实现  将数据聚合为need_k类  随机获取初始聚类中心
    void kmeans_function(int need_k = 3)
    {
    
        K = need_k;// 单次聚类就默认选择聚成三类
        if(featureVector.size() < K)
        {
            throw 2;
            return;
        }
        
        vector< vector<Feature> > tmpCluster(K,vector<Feature>());
        int lable = 0;
        get_random_k_center();
        //get_k_center_by_kmeans_plus_plus();
        cout<<"初始化质心: "<<endl;
        print_center();
        
        for(int i=0;i<featureVector.size(); i++)
        {
            lable = cluster_of_feature(featureVector[i]);
            tmpCluster[lable].push_back(featureVector[i]);
        }
        cluster = tmpCluster;
        SSE_error = get_SSE(cluster);
        update_cluster_center();
        double old_sse = -1;
        double new_sse = SSE_error;
        int interal_time = 1;
        while( (new_sse - old_sse) >= 0.1 || (new_sse - old_sse) <= -0.1 )
        {
            cout<<"第 "<< interal_time++<<" 迭代开始"<<endl;
            cluster = tmpCluster;
            SSE_error = new_sse;
            print_center();
            update_cluster_center();
            
            old_sse = new_sse;
            
            //vector< vector<Feature> > tmpCluster(K,vector<Feature>());
            for(int i=0;i<K;i++)
                tmpCluster[i].clear();
            for(int i=0;i<featureVector.size(); i++)
            {
                lable = cluster_of_feature(featureVector[i]);
                tmpCluster[lable].push_back(featureVector[i]);
            }
            new_sse = get_SSE(tmpCluster);
        }
        get_SSE(cluster);
        
        cout<<"聚类结束，均方误差："<<SSE_error<<endl;
    }
    
    //Kmeans 聚类算法实现  将数据聚合为need_k类  运用kmeans＋＋ 获取初始聚类中心
    void kmeans_plus_plus_function(int need_k = 3)
    {
        
        K = need_k;// 单次聚类就默认选择聚成三类
        if(featureVector.size() < K)
        {
            throw 2;
            return;
        }
        vector< vector<Feature> > tmpCluster(K,vector<Feature>());
        int lable = 0;
        //get_random_k_center();
        get_k_center_by_kmeans_plus_plus();
        //cout<<"--------------------------初始化质心: "<<endl;
        //print_center();
        
        for(int i=0;i<featureVector.size(); i++)
        {
            lable = cluster_of_feature(featureVector[i]);
            tmpCluster[lable].push_back(featureVector[i]);
        }
        cluster = tmpCluster;
        SSE_error = get_SSE(cluster);
        update_cluster_center();
        double old_sse = -1;
        double new_sse = SSE_error;
        int interal_time = 1;
        //cout<<"SSE_error: "<<SSE_error<<endl;
        while( (new_sse - old_sse) >= 0.1 || (new_sse - old_sse) <= -0.1 )
        {
            //cout<<"第 "<< interal_time++<<" 迭代开始"<<endl;
            cluster = tmpCluster;
            SSE_error = new_sse;
            //print_center();
            update_cluster_center();
            
            old_sse = new_sse;
            //vector< vector<Feature> > tmpCluster(K,vector<Feature>());
            for(int i=0;i<K;i++)
                tmpCluster[i].clear();
            for(int i=0;i<featureVector.size(); i++)
            {
                lable = cluster_of_feature(featureVector[i]);
                tmpCluster[lable].push_back(featureVector[i]);
            }
            new_sse = get_SSE(tmpCluster);
            //cout<<"SSE_error: "<<new_sse<<endl;
        }
        get_SSE(cluster);
        
        //cout<<"--------------------------聚类结束，均方误差："<<SSE_error<<endl;
    }
    
    //Kmeans
    void kmeans_function_for_iternal(int internal_k = 3)
    {
        K = internal_k;
        vector< vector<Feature> > tmpCluster(K,vector<Feature>());
        int lable = 0;
        //get_random_k_center();
        get_k_center_by_kmeans_plus_plus();
        cout<<"初始化质心: "<<endl;
        print_center();
        
        for(int i=0;i<featureVector.size(); i++)
        {
            lable = cluster_of_feature(featureVector[i]);
            tmpCluster[lable].push_back(featureVector[i]);
        }
        cluster = tmpCluster;
        SSE_error = get_SSE(cluster);
        update_cluster_center();
        double old_sse = -1;
        double new_sse = SSE_error;
        int interal_time = 1;
        while( (new_sse - old_sse) >= 0.1 || (new_sse - old_sse) <= -0.1)
        {
            //cout<<"第 "<< interal_time++<<" 迭代开始"<<endl;
            cluster = tmpCluster;
            SSE_error = new_sse;
            //print_center();
            update_cluster_center();
            
            old_sse = new_sse;
            
            //vector< vector<Feature> > tmpCluster(K,vector<Feature>());
            for(int i=0;i<K;i++)
                tmpCluster[i].clear();
            for(int i=0;i<featureVector.size(); i++)
            {
                lable = cluster_of_feature(featureVector[i]);
                tmpCluster[lable].push_back(featureVector[i]);
            }
            new_sse = get_SSE(tmpCluster);
        }
        get_SSE(cluster);
        cout<<"聚类结束，K值:"<<K;
        cout<<"    均方误差："<<SSE_error<<endl;
    }
    
    
    
    //迭代K值，选SSE最小的K值,采用kmeans＋＋算法
    
    void kmeans_with_select_k_function(int max_k_internal = 6)
    {
        max_k = max_k_internal;
        int internal_k = 2;
        vector<double> SSE_vect;
        kmeans_plus_plus_function(internal_k);
        
        vector<Feature> last_kCenter = kCenter;
        vector< vector<Feature> > last_cluster = cluster;
        int last_k = internal_k;
        double last_SSE = SSE_error;
        internal_k = 3;
        SSE_vect.push_back(SSE_error);
        while(internal_k <= max_k){
            if(internal_k > featureVector.size())
                break;
            kmeans_plus_plus_function(internal_k);
            if (SSE_error <= last_SSE)
            {
                last_SSE = SSE_error;
                last_k = internal_k;
                last_cluster = cluster;
                last_kCenter = kCenter;
            }
            SSE_vect.push_back(SSE_error);
            internal_k ++;
        }
        
        SSE_error = last_SSE;
        K = last_k;
        kCenter = last_kCenter;
        cluster = last_cluster;
        
        for(int i=0;i<SSE_vect.size(); i++)
            cout<<"K = "<<i+2<<"     SSE_error = "<<SSE_vect[i]<<endl;
        cout<<"最终聚类中心数量选择为："<< K <<endl;
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
    
private:
    //随即获取初始化的聚类中心
    void get_random_k_center()
    {
        srand((unsigned int) time(NULL) + 20000 * K);
        vector<int> tmpvect;
        for(int i=0;i<featureVector.size();i++)
            tmpvect.push_back(i);
        kCenter.clear();
        for(int i=0;i<K;i++)
        {
            int select = rand() %(tmpvect.size());
            kCenter.push_back( featureVector[ tmpvect[ select ] ]);
            tmpvect.erase(tmpvect.begin() + select);
        }
    }
    
    void get_k_center_by_kmeans_plus_plus()
    {
        srand((unsigned int)time(NULL) + 20000*K);
        //set<int> had_select_id;
        vector<int> tmpvect;
        for(int i=0;i<featureVector.size();i++)
            tmpvect.push_back(i);
        kCenter.clear();
        int select = rand() %(tmpvect.size());
        kCenter.push_back(featureVector[ tmpvect[ select ] ]);
        Feature tmp_center = featureVector[ tmpvect[ select ] ];
        tmpvect.erase(tmpvect.begin() + select);
        for(int i=1; i<K; ++i)
        {
            double max_distance = get_dist_XY(tmp_center, featureVector[ tmpvect[ 0 ] ]);
            select = 0;
            for(int j=1; j<tmpvect.size(); j++)
            {
                double tmp_distance = get_dist_XY(tmp_center, featureVector[ tmpvect[ j ] ]);
                if(tmp_distance > max_distance )
                {
                    select = j;
                    max_distance = tmp_distance;
                }
            }
            kCenter.push_back(featureVector[ tmpvect[ select ] ]);
            Feature tmp_center = featureVector[ tmpvect[ select ] ];
            tmpvect.erase(tmpvect.begin() + select);
        }
    }

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
    
    //根据质心决定当前元祖属于哪个簇
    int cluster_of_feature(const Feature & fea)
    {
        double dist = get_dist_XY(kCenter[0],fea);
        int ret = 0;
        for(int i=1;i<K;i++)
        {
            double tmp = get_dist_XY(kCenter[i],fea);
            if(tmp < dist)
            {
                dist = tmp;
                ret = i;
            }
        }
        return ret;
    }

    //更新簇的均值（质心)
    void update_cluster_center()
    {
        for(int i=0;i<K; i++)
        {
            vector<Feature> iCluster = cluster[i];
            Feature tmpf(dimNum,0.0);
            for(int j=0; j<iCluster.size(); j++)
            {
                for(int m=1;m<dimNum;m++)
                {
                    tmpf[m] += iCluster[j][m];
                }
            }
            int num = iCluster.size();
            for(int ii=1;ii<dimNum;ii++)
            {
                tmpf[ii] /= (double)num;
            }
            kCenter[i] = tmpf;
        }
    }
    
    //获取聚类簇集的SSE值（平方误差）
    double get_SSE(vector< vector<Feature> > cluster)
    {
        double ret = 0.0;
        for(int i=0;i<K;i++)
        {
            vector<Feature> iCluster = cluster[i];
            for(int j=0;j<iCluster.size();j++)
            {
                ret += get_dist_XY(iCluster[j],kCenter[i]);
            }
        }
        return ret;
    }
    

    //对特征向量进行处理：各个参数的权重及诡归一化调整。
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

#endif
