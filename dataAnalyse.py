import pandas as pd
import numpy as np
# import matplotlib
import matplotlib.pyplot as plt
# import csv
plt.rcParams['font.sans-serif'] = 'simhei'#用来显示中文标签
plt.rcParams['axes.unicode_minus']=False
 
#------------------------------------------------------
data = pd.read_csv("data.csv",header=None)
data = np.array(data) #转为列表
last = data.shape[0] - 1 #总行数减一，因为从0开始
# data = data0.loc[[last],:]
print(last)
print(data)
 
labels=['wrongPosition','goodPosition']
#绘图显示的标签
values=data[last,[0,1]]#获取最后一行，第0列和第1列的地址
print(values)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
# 分析坐姿数据
#plt.title("Proportion of correct and incorrect sitting posture",fontsize=15)
plt.title("一段时间内坐姿概览",fontsize=15)
plt.pie(values,labels=labels,autopct="%.1f%%")
# plt.show()
plt.savefig('./analyseResult/一段时间内坐姿概览.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠

#----------------------------------------------------------------------------
data1 = pd.read_csv("neck.csv",header=None)
data1 = np.array(data1)
last1 = data1.shape[0] - 1 
# data = data.loc[[last1],:]
print(data1)
print(last1)
 
labels1=['wrongNeck','goodNeck']
#绘图显示的标签
values1=data1[last1,[0,1]]#获取最后一行，第0列和第1列的地址
print(values1)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
#分析坐姿数据
#plt.title("Proportion of correct and incorrect neck posture",fontsize=15)
plt.title("一段时间内颈部位置姿势概览",fontsize=15)
plt.pie(values1,labels=labels1,autopct="%.1f%%")
# plt.show()
plt.savefig('./analyseResult/一段时间内颈部位置姿势概览.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠

#----------------------------------------------------------------------------
data2 = pd.read_csv("shoulder.csv",header=None)
data2 = np.array(data2)
last2 = data2.shape[0]  - 1
# data = data.loc[[last2],:]
print(data2)
print(last2)
 
labels2=['wrongShoulder','goodShoulder']
#绘图显示的标签
values2=data2[last2,[0,1]]#获取最后一行，第0列和第1列的地址
print(values2)
colors=['y','m','b']
explode=[0,0.1,0]
#旋转角度
#分析坐姿数据
#plt.title("Proportion of correct and incorrect shoulder posture",fontsize=15)
plt.title("一段时间内肩膀位置姿势概览",fontsize=15)
plt.pie(values2,labels=labels2,autopct="%.1f%%")
# plt.show()
plt.savefig('./analyseResult/一段时间内肩膀位置姿势概览.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠

#----------------------------------------------------------------------------
#绘制曲线图
data3 = pd.read_csv("total.csv",header=None)#不将第一行作为列名
data3 = np.array(data3)
n = data3.shape[0] #总行数
print(data3)
#取第一列,正误数组
list1= data3[:,0]
print(list1)
#取第三列作为时间数组
time = data3[:,2]
print(time)
#设置标题
plt.title('坐姿随时间变化1表示正确，0表示错误')
#画出折线图
plt.xlabel('Time')
plt.ylabel('Change')
plt.plot(time,list1)
# plt.show()
plt.savefig('./analyseResult/坐姿随时间变化图.jpg', bbox_inches='tight', dpi=450)
plt.close()#防止图片重叠
