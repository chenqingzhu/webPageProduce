//
//  AGENSCluster.h
//  spiter
/*
 凝聚的层次聚类：自底向上的策略，首先将每个对象作为一个簇，然后合并这些原子簇为更大的簇，直到所有的对象都在同一个簇中，或者满足终止条件。
 
 AGNES算法
 AGNES(Agglomerative Nesting) 是凝聚的层次聚类算法，如果簇C1中的一个对象和簇C2中的一个对象之间的距离是所有属于不同簇的对象间欧式距离中最小的，C1和C2可能被合并。这是一种单连接方法，其每个簇可以被簇中的所有对象代表，两个簇之间的相似度由这两个簇中距离最近的数据点对的相似度来确定。
 
 
 */

//  Created by chenqingzhu on 15/12/7.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef __spiter__AGENSCluster__
#define __spiter__AGENSCluster__

#include <stdio.h>
#include "publicPart.h"

//存储每两篇文档距离信息的结构体
struct DistanceInfo{
    int leftPoint;      //标签索引
    int rightPoint;     //标签索引
    double distance;    //文档间的欧式距离
};

class AGENSCluster{
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
    vector<DistanceInfo> m_allDistance; //存储所有标签特征向量之间的距离信息
    
public:
    AGENSCluster(vector<Feature> fv,bool need_init_2_one = true):initFeatureVector(fv),dataNum(0)
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
        dataNum = initFeatureVector.size();
        //是否需要将特征向量进行归一化
        if(need_init_2_one)
            process_feature_vector();
        else
            featureVector = initFeatureVector;
    }
    
    ~AGENSCluster()
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
    
    //根据簇中心向量的标签文本平均长度来确定文本簇，目前长度+标点符号数的大小来定义为特征簇（待优化）
    int get_page_text_cluster_k()
    {
        int tmp_k = 0;
        int tmp_len = kCenter[0][1] + kCenter[0][4];
        
        for(int i=1;i<K; i++)
        {
            if(kCenter[i][1] < kCenter[i][4])
                continue;
            
            if(kCenter[i][1] + kCenter[i][4] > tmp_len)
            {
                tmp_len = kCenter[i][1] + kCenter[i][4] ;
                tmp_k = i;
            }
        }
        return tmp_k+1;
    }
    //自顶向小层次聚类的主函数
    void AGENS(int max_k = 5)
    {
        if(featureVector.size() < max_k)
        {
            throw 2;
        }
        
        K = max_k;
        cluster.clear();
        kCenter.clear();
        cout<<"featureVector size: "<<featureVector.size() <<endl;
        computeAllFeatureDistance();
        cout<< "m_allDistance size: "<< m_allDistance.size()<<endl;
        if(K == 1)
        {
            cluster.push_back(featureVector);
            update_cluster_center();
            SSE_error = get_SSE(cluster);
            return ;
        }
        
        if(dataNum == K)
        {
            vector<Feature> t_vect;
            t_vect.push_back(featureVector[0]);
            cluster.push_back(t_vect);
            kCenter.push_back(featureVector[0]);
            for(int i=1;i<dataNum;i++)
            {
                t_vect[0] = featureVector[i];
                cluster.push_back(t_vect);
                kCenter.push_back(featureVector[i]);
            }
            SSE_error = get_SSE(cluster);
            return ;
        }
        
        //存储各个类中文档的索引号
        vector< set<int> > t_cluster;
        
        //先把一对节点放入一个类中
        set<int> t_set;
        t_set.insert(m_allDistance[0].leftPoint);
        t_set.insert(m_allDistance[0].rightPoint);
        t_cluster.push_back(t_set);
        
        int  featureNum = 2;   //已经聚类的特征数量
        
        for(int t_index = 1; t_index < m_allDistance.size(); t_index ++)
        {
            if(featureNum  == dataNum && t_cluster.size() == K)
                break;
            
            int leftClusterIndex = -1;   //左点是否已经在某聚类集合中标记
            int rightClusterIndex = -1;  //右点是否已经在某聚类集合中标记
            
            //搜索左点是否已经在某聚类集合中标记
            for(int t_clusterIndex = 0; t_clusterIndex < t_cluster.size(); t_clusterIndex++)
            {
                if(t_cluster[t_clusterIndex].find(m_allDistance[t_index].leftPoint) != t_cluster[t_clusterIndex].end())
                {
                    leftClusterIndex = t_clusterIndex;
                    break;
                }
            }
            
            //搜索右点是否已经在某聚类集合中标记
            for(int t_clusterIndex = 0; t_clusterIndex < t_cluster.size(); t_clusterIndex++)
            {
                if(t_cluster[t_clusterIndex].find(m_allDistance[t_index].rightPoint) != t_cluster[t_clusterIndex].end())
                {
                    rightClusterIndex = t_clusterIndex;
                    break;
                }
            }
            
            //对左右点索引值进行判断，共有五种情况：
            
            //第一种：左右文档均不在当前的分类簇集合中
            if(leftClusterIndex == -1 && rightClusterIndex == -1)
            {
                //若还未聚类的特征数已经与还未分配特征的聚类数目相等，为保证每个类至少有一个标签，将右文档归为下一类
                if(dataNum - featureNum == K - t_cluster.size())
                {
                    t_set.clear();
                    //t_set.insert(m_allDistance[t_index].leftPoint);
                    t_set.insert(m_allDistance[t_index].rightPoint);
                    t_cluster.push_back(t_set);
                    featureNum ++;
                }
                else
                {
                    t_set.clear();
                    t_set.insert(m_allDistance[t_index].leftPoint);
                    t_set.insert(m_allDistance[t_index].rightPoint);
                    t_cluster.push_back(t_set);
                    featureNum += 2;
                }
            }
            
            //第二种：左标签在，右标签不在
            else if (leftClusterIndex != -1 && rightClusterIndex == -1)
            {
                //若还未聚类的特征数已经与还未分配特征的聚类数目相等，为保证每个类至少有一个标签，将右文档归为下一类
                if(dataNum - featureNum == K - t_cluster.size())
                {
                    t_set.clear();
                    //t_set.insert(m_allDistance[t_index].leftPoint);
                    t_set.insert(m_allDistance[t_index].rightPoint);
                    t_cluster.push_back(t_set);
                    featureNum ++;
                }
                //若还未达到聚类数目，则将右文档划分为一个新类
                else if(t_cluster.size() != K)
                {
                    t_set.clear();
                    t_set.insert(m_allDistance[t_index].rightPoint);
                    t_cluster.push_back(t_set);
                    featureNum ++;
                }
                else{
                    t_cluster[leftClusterIndex].insert(m_allDistance[t_index].rightPoint);
                    featureNum ++;
                }
            }
            
            //第三种：左标签不在已有分类簇中，右标签在
            else if(leftClusterIndex == -1 && rightClusterIndex != -1)
            {
                //若还未聚类的特征数已经与还未分配特征的聚类数目相等，为保证每个类至少有一个标签，将左文档归为下一类
                if(dataNum - featureNum == K - t_cluster.size())
                {
                    t_set.clear();
                    t_set.insert(m_allDistance[t_index].leftPoint);
                    //t_set.insert(m_allDistance[t_index].rightPoint);
                    t_cluster.push_back(t_set);
                    featureNum ++;
                }
                //若还未达到聚类数目，则将左文档划分为一个新类
                else if(t_cluster.size() != K)
                {
                    t_set.clear();
                    t_set.insert(m_allDistance[t_index].leftPoint);
                    t_cluster.push_back(t_set);
                    featureNum ++;
                }
                else{
                    t_cluster[rightClusterIndex].insert(m_allDistance[t_index].leftPoint);
                    featureNum ++;
                }
            }
            
            //第四种：左右标签不在同一个分类簇中
            else if(leftClusterIndex != -1 && rightClusterIndex != -1 && leftClusterIndex != rightClusterIndex)
            {
                //如果未聚类的标签数和还未分配标签的聚类数目相等，为了保证每个簇中有一个标签特征向量，则重新开始循环
                 if(dataNum - featureNum == K - t_cluster.size())
                 {
                     continue;
                 }
                //若还未达到聚类数目，则跳过此循环
                else if(t_cluster.size() <= K)
                {
                    continue;
                }
                //对于一般情况，合并两个集合，统一放入左标签所在的集合中
                else
                {
                    t_cluster[leftClusterIndex].insert(t_cluster[rightClusterIndex].begin(),t_cluster[rightClusterIndex].end());
                    t_cluster.erase(t_cluster.begin() + rightClusterIndex);
                }
            }
            //左右文档在同一个簇内
            else
            {
                continue;
            }
        }
        
        //将t_cluster分类的结果整理到聚类簇中
        
        for(int i=0;i<t_cluster.size();i++)
        {
            set<int>::iterator iter = t_cluster[i].begin();
            vector<Feature> t_feature;
            for(; iter != t_cluster[i].end(); iter++)
            {
                t_feature.push_back(featureVector[*iter]);
            }
            cluster.push_back(t_feature);
        }
        
        cout<<"t_cluster size: "<< t_cluster.size()<<endl;
        cout<<"K: "<<K<<endl;
        update_cluster_center();
        SSE_error = get_SSE(cluster);
    }
    
private:
    
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
            kCenter.push_back(tmpf);
            //kCenter[i] = tmpf;
        }
    }
    
    //计算每两标签向量的距离并排序,生成文档距离结构体
    void computeAllFeatureDistance()
    {
        m_allDistance.clear();
        
        for(int leftIndex=0;leftIndex < dataNum -1; leftIndex++)
        {
            for(int rightIndex = leftIndex +1 ; rightIndex < dataNum; rightIndex ++)
            {
                DistanceInfo t_dis;
                t_dis.leftPoint = leftIndex;
                t_dis.rightPoint = rightIndex;
                t_dis.distance = get_dist_XY(featureVector[leftIndex], featureVector[rightIndex]);
                m_allDistance.push_back(t_dis);
            }
        }
        sort(m_allDistance.begin(), m_allDistance.end(), this->sortDistanceInfo);
        
        return ;
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
    
    //用于sort排序的小于号重载
    static bool sortDistanceInfo(const DistanceInfo d1,const DistanceInfo d2)
    {
        return d1.distance < d2.distance;
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
};

#endif /* defined(__spiter__AGENSCluster__) */
